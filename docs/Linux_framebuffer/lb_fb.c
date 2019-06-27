#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/kd.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "lb_types.h"
#include "lb_fb.h"
#include "lb_graphics.h"

struct fb_var_screeninfo var_info;
struct fb_fix_screeninfo fix_info;
long int screensize;
int fd_fb = 0;
int fd_tty;

/* Extern variables in types.h */

extern PICTURE_T  ty_pic_shadow;
extern CONSOLE_T  *ty_C;
extern S_INT_16_T ty_width, ty_height;
extern S_INT_8_T  ty_scale_x, ty_scale_y;
extern U_INT_8_T  ty_renderoptions;
extern S_INT_8_T  ty_fb_initialized;
extern char       *ty_fb_main; 


void lb_fb_close(void)
{
  if (!ty_fb_initialized)
    printf("Warning: lb_fb_close() --> Closing a framebuffer which hasn't been initialized\r\n");
   
  munmap(ty_fb_main, screensize);
  close(fd_fb);
  lb_fb_setmode_text();
  ty_fb_initialized=FALSE;
       
  if (lb_fb_use_shadow() )
    lb_gr_release_picture(&ty_pic_shadow);
  printf("lb_fb_close() --> Closing framebuffer\r\n");
}

void lb_fb_get_res(const char *path_fb, S_INT_16_T *width, S_INT_16_T *height)
{
  fd_fb  = open(path_fb, O_RDONLY);

  if (ioctl(fd_fb, FBIOGET_VSCREENINFO, &var_info) == -1)
    {
      printf("Error: lb_fb_get_res() --> Error reading variable information: %s\r\n",strerror(errno));
      exit(1);
    }

  *width=var_info.xres;
  *height=var_info.yres;
}

S_INT_8_T lb_fb_use_shadow(void)
{
  if ( (N_BITS_R==8) && (N_BITS_G==8) && (N_BITS_B==8) && (N_BITS_K==8) && (ty_scale_x==1) && (ty_scale_y==1) ) return FALSE;
  return TRUE;
}

void lb_fb_open(const char *path_fb, const char *path_tty, S_INT_16_T scale_x, S_INT_16_T scale_y, U_INT_8_T renderoptions)
{
  /* This redirects SIGINT (control-C to function lb_ft_exit() */
  signal(SIGINT,lb_fb_exit);
  /*************************************************************/
  if (ty_fb_initialized)
      lb_fb_exit_msg("Error: lb_fb_open() --> Framebuffer is already open\r\n");
  ty_fb_initialized=TRUE;
  ty_renderoptions = renderoptions;
   
  fd_tty = open(path_tty, O_RDWR);
  fd_fb  = open(path_fb, O_RDWR);
  if (fd_fb == -1)
    {
      printf("Error: lb_fb_open() --> cannot open framebuffer device: %s\r\n",strerror(errno));
      exit(1);
    }

  /* Get fixed screen information */
  if (ioctl(fd_fb, FBIOGET_FSCREENINFO, &fix_info) == -1)
    {
      printf("Error: lb_fb_open() --> Error reading fixed information: %s\r\n",strerror(errno));
      exit(2);
    }
  
  /* Get variable screen information */
  if (ioctl(fd_fb, FBIOGET_VSCREENINFO, &var_info) == -1)
    {
      printf("Error: lb_fb_open() --> Error reading variable information: %s\r\n",strerror(errno));
      exit(3);
    }
  /* Currently, only 32 bpp are supported */
  if (var_info.bits_per_pixel!=32)
    {
      printf("Error: lb_fb_open() --> currently, only 32 bpp are supported");
      exit(4);
    }
  /* Warning: this is NOT the purpose of vmode !
     I'm deliberately using it to store the bytes per pixel in order to pre-compute 
     the offset for the putpixel function to avoid calculating bpp/8 [or bpp>>3] every
     time a pixel is drawn. */
  //var_info.vmode = var_info.vmode>>3;
  if ( (var_info.xoffset!=0) || (var_info.yoffset!=0) )
    {
      printf("Error: lb_fb_open() --> Strangely, one of the offsets isn't 0. Investigate! xoffset=%d, yoffset=%d\r\n",
	     var_info.xoffset, var_info.yoffset);
      exit(1);
    }
    
  ty_width=var_info.xres/scale_x;
  ty_height=var_info.yres/scale_y;
  ty_scale_x=scale_x;
  ty_scale_y=scale_y;
  if ( lb_fb_use_shadow() )
    {
      ty_pic_shadow.w=ty_width;
      ty_pic_shadow.h=ty_height;
      lb_gr_create_picture(&ty_pic_shadow,lb_gr_12RGB(COLOR_BLACK));
    }

  /* Figure out the size of the screen in bytes */
  screensize = var_info.xres*var_info.yres*(var_info.bits_per_pixel>>3);

  // Map the device to memory
  ty_fb_main = (char *)mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_fb, 0);
  if (ty_fb_main==MAP_FAILED)
    {
      printf("Error: lb_fb_open() --> Memory mapping failed\t\n");
      exit(1);
    }
    
  if ((char)*ty_fb_main == -1)
    {
      printf("Error: lb_fb_open() --> failed to map framebuffer device to memory: %s\r\n",strerror(errno));
      exit(4);
    }

  if (ty_renderoptions & RENDEROPTIONS_GRAPHICS_ONLY)
    lb_fb_setmode_graphics();
}

/* This function assumes the file descriptor fd_tty has been initialized */
void lb_fb_setmode_graphics(void)
{
  /* Disable console updates to the fb*/
  if(ioctl(fd_tty, KDSETMODE, KD_GRAPHICS) == -1)
   {
    printf("Error: lb_fb_setmode_graphics() --> Failed to set graphics mode\r\n");
    exit(1);
   }
}

/* This function assumes the file descriptor fd_tty has been initialized */
void lb_fb_setmode_text(void)
{
  if(ioctl(fd_tty, KDSETMODE, KD_TEXT) == -1)
      printf("Warning: lb_fb_setmode_text --> Failed to set text mode\r\n");
}

void lb_fb_exit_msg(const char *format, ...)
{
  va_list args;
  int size;
  char *buffer;

  lb_fb_close();

  va_start(args,format);

  /* We want this function to support an arbitrary size. */
  size=vsnprintf(NULL, 0, format, args);
  printf("Size=[%d]\n",size);
  buffer=malloc(size+1);
  if (buffer==NULL)
    {
      printf("Error: lb_fb_exit() --> malloc failed");
      exit(1);
    }
  /* This seems to be a bug */
  va_end(args);
  va_start(args,format);
  vsprintf(buffer, format, args);
  printf("\r\n%s\n\n",buffer);
  va_end(args);
  free(buffer);
  printf("Closing program\r\n");
  exit(1);
}

void lb_fb_exit(int code)
{
  lb_fb_close();
  exit(code);
}


void lb_fb_flush(void)
{
  fflush(NULL);
}

void lb_fb_parse_finfo(void)
{
  struct fb_fix_screeninfo finfo;

  /* Get fixed screen information */
  if (ioctl(fd_fb, FBIOGET_FSCREENINFO, &finfo) == -1)
    lb_fb_exit_msg("Error: lb_fb_parse_finfo(void) --> %s\r\n",strerror(errno));
     
  printf("*****************************************\r\n");
  printf("Parsing fix_info\r\n");
  printf("finfo.id = %s\r\n", finfo.id);
  printf("finfo.smem_len = %ud\r\n", finfo.smem_len);
  printf("finfo.type = %d\r\n", finfo.type);
  printf("finfo.type_aux = %d\r\n", finfo.type_aux);
  printf("finfo.visual = %d\r\n", finfo.visual);
  printf("finfo.xpanstep = %d\r\n", finfo.xpanstep);
  printf("finfo.ypanstep = %d\r\n", finfo.ypanstep);
  printf("finfo.ywrapstep = %d\r\n", finfo.ywrapstep);
  printf("finfo.line_length = %d\r\n", finfo.line_length);
  printf("finfo.mmio_start = %ld\r\n", finfo.mmio_start);
  printf("finfo.mmio_len = %d\r\n", finfo.mmio_len);
  printf("finfo.accel = %d\r\n", finfo.accel);
  printf("f_info.capabilities = %d\r\n", finfo.capabilities);
}

void lb_fb_parse_vinfo(void)
{
  struct fb_var_screeninfo vinfo;

  /* Get variable screen information */
  if (ioctl(fd_fb, FBIOGET_VSCREENINFO, &vinfo) == -1)
    lb_fb_exit_msg("Error: lb_fb_parse_vinfo() --> %s\r\n",strerror(errno));
     
  printf("*****************************************\r\n");
  printf("vinfo.xres = %d\r\n", vinfo.xres);
  printf("vinfo.yres = %d\r\n", vinfo.yres);
  printf("vinfo.xres_virtual = %d\r\n", vinfo.xres_virtual);
  printf("vinfo.yres_virtual = %d\r\n", vinfo.yres_virtual);
  printf("vinfo.xoffset = %d\r\n", vinfo.xoffset);
  printf("vinfo.yoffset = %d\r\n", vinfo.yoffset);
  printf("vinfo.bits_per_pixel = %d\r\n", vinfo.bits_per_pixel);
  printf("vinfo.grayscale = %d\r\n", vinfo.grayscale);
  printf("vinfo.bitfield.red [offset = %d, length = %d, MSB = %d]\r\n",
	 vinfo.red.offset, vinfo.red.length, vinfo.red.msb_right);
  printf("vinfo.bitfield.green [offset = %d, length = %d, MSB = %d]\r\n",
	 vinfo.green.offset, vinfo.green.length, vinfo.green.msb_right);
  printf("vinfo.bitfield.blue [offset = %d, length = %d, MSB = %d]\r\n",
	 vinfo.blue.offset, vinfo.blue.length, vinfo.blue.msb_right);
  printf("vinfo.bitfield.transp [offset = %d, length = %d, MSB = %d]\r\n",
	 vinfo.transp.offset, vinfo.transp.length, vinfo.transp.msb_right);
  printf("vinfo.nonstd = %d\r\n", vinfo.nonstd);
  printf("vinfo.activate = %d\r\n", vinfo.activate);
  printf("vinfo.height = %d\r\n", vinfo.height);
  printf("vinfo.width = %d\r\n", vinfo.width);
  printf("vinfo.accel_flags = %d\r\n", vinfo.accel_flags);
  printf("vinfo.pixclock = %d\r\n", vinfo.pixclock);
  printf("vinfo.left_margin = %d\r\n", vinfo.left_margin);
  printf("vinfo.right_margin = %d\r\n", vinfo.right_margin);
  printf("vinfo.upper_margin = %d\r\n", vinfo.upper_margin);
  printf("vinfo.lower_margin = %d\r\n", vinfo.lower_margin);
  printf("vinfo.hsync_len = %d\r\n", vinfo.hsync_len);
  printf("vinfo.vsync_len = %d\r\n", vinfo.vsync_len);
  printf("vinfo.sync = %d\r\n",vinfo.sync);
  printf("vinfo.vmode = %d\r\n",vinfo.vmode);
}

void lb_fb_draw_pixel(char *fb, S_INT_16_T x, S_INT_16_T y, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b, U_INT_8_T t)
{
  char *pos;
 
  if ( (x<0) || (x>=var_info.xres) ) return;
  if ( (y<0) || (y>=var_info.yres) ) return;

  /* This is the simplified formula for the initial position.  A more general function would consider arbitrary 
     values for xoffset or yoffset.  The thing is, I have not yet seen them different than zero.  
     I've put an exit() with an error message should either xoffset or yoffset are not null */
  
  //location = (x+var_info.xoffset)*(var_info.bits_per_pixel>>3) + (y+var_info.yoffset)*fix_info.line_length;
  switch (var_info.bits_per_pixel)
    {
    case 16:
      //unsigned short int t = r<<11 | g << 5 | b;
      //*((unsigned short int*)(fb_main + location)) = t;
      printf("Error: lb_fb_setpixel() --> unsupported framebuffer color mode\r\n");
      lb_fb_exit(1);
      break;
    case 24:
      //*(fb_main + location)     = b;
      //*(fb_main + location + 1) = g;
      //*(fb_main + location + 2) = r;
      // pos = fb + (x*4 + y*fix_info.line_length);
      //*(pos) = b;
      //pos++;
      //*(pos) = g;
      //pos++;
      //*(pos) = r;
      //pos++;
      //*(pos) = t;
      //msync(fb,var_info.yres*fix_info.line_length,MS_ASYNC);

      printf("Error: lb_fb_setpixel() --> unsupported framebuffer color mode\r\n");
      break;
    case 32:
      pos = fb + (x*4 + y*fix_info.line_length);
      *(pos) = b;
      pos++;
      *(pos) = g;
      pos++;
      *(pos) = r;
      pos++;
      *(pos) = t;
      msync(fb,var_info.yres*fix_info.line_length,MS_ASYNC);

      //      unsigned char tr, tg, tb, tt;
      //lb_fb_get_pixel(fb, x, y, &tr, &tg, &tb, &tt);

      //printf("RGBT W: %03d  %03d  %03d  %03d R: %03d %03d %03d %03d\r\n",r,g,b,t,tr,tg,tb,tt);
      break;
    default:
      printf("Error: lb_fb_setpixel() --> unsupported framebuffer color mode\r\n");
      lb_fb_exit(1);
      break;
    }
}

void lb_fb_get_pixel(char *fb, S_INT_16_T x, S_INT_16_T y, U_INT_8_T *r, U_INT_8_T *g, U_INT_8_T *b, U_INT_8_T *t)
{
  char *pos;

  if ( (x<0) || (x>=var_info.xres) ) return;
  if ( (y<0) || (y>=var_info.yres) ) return;

  /* This is the simplified formula for the initial position.  A more general function would consider arbitrary 
     values for xoffset or yoffset.  The thing is, I have not yet seen them different than zero.  
     I've put an exit() with an error message should either xoffset or yoffset are not null */
  
  //location = (x+var_info.xoffset)*(var_info.bits_per_pixel>>3) + (y+var_info.yoffset)*fix_info.line_length;
  switch (var_info.bits_per_pixel)
    {
    case 16:
      //unsigned short int t = r<<11 | g << 5 | b;
      //*((unsigned short int*)(fb_main + location)) = t;
      printf("Error: lb_fb_setpixel() --> unsupported framebuffer color mode\r\n");
      lb_fb_exit(1);
      break;
    case 24:
      //*(fb_main + location)     = b;
      //*(fb_main + location + 1) = g;
      //*(fb_main + location + 2) = r;  
      printf("Error: lb_fb_setpixel() --> unsupported framebuffer color mode\r\n");
      lb_fb_exit(1);
      break;
    case 32:
      pos = fb + (x*4 + y*fix_info.line_length);
      *b=*(pos);
      pos++;
      *g=*(pos);
      pos++;
      *r=*(pos);
      pos++;
      *t=*(pos);

      break;
    default:
      printf("Error: lb_fb_setpixel() --> unsupported framebuffer color mode\r\n");
      lb_fb_exit(1);
      break;
    }
}

void lb_fb_line_h(char *fb, S_INT_16_T y0, S_INT_16_T x0, S_INT_16_T x1, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b, U_INT_8_T t)
{
  S_INT_16_T x_min, x_max;
  char *pos, *pos_final;

  if ( (y0<0) || (y0>=var_info.yres) ) return;
  
  if (x0<=x1)
    {
      x_min=x0;
      x_max=x1;
    }
  else
   {
      x_min=x1;
      x_max=x0;
    }
  
  if (x_min<0)
    x_min=0;

  if (x_max>var_info.xres-1)
    x_max=var_info.xres-1;

  pos       = fb + 4*x_min  + y0*fix_info.line_length;
  pos_final = fb + 4*x_max  + y0*fix_info.line_length;  //*var_info.vmode;

  while (pos<=pos_final)
    {
     *(pos) = b;
      pos++;
      *(pos) = g;
      pos++;
      *(pos) = r;
      pos++;
      *(pos) = t;
      pos++;
    }
}

void lb_fb_line_v(char *fb,  S_INT_16_T x0, S_INT_16_T y0, S_INT_16_T y1, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b, U_INT_8_T t)
{
  S_INT_16_T y_min, y_max;
  char *pos, *pos_final;

  if ( (x0<0) || (x0>=var_info.xres) ) return;
  
  if (y0<=y1)
    {
      y_min=y0;
      y_max=y1;
    }
  else
   {
      y_min=y1;
      y_max=y0;
    }
  
  if (y_min<0)
    y_min=0;

  if (y_max>var_info.yres-1)
    y_max=var_info.yres-1;
  
  pos       = fb + x0*4 + y_min*fix_info.line_length;
  pos_final = fb + x0*4 + y_max*fix_info.line_length;
  while (pos<=pos_final)
    {
      *(pos) = b;
      pos++;
      *(pos) = g;
      pos++;
      *(pos) = r;
      pos++;
      *(pos) = t;
      pos+=fix_info.line_length-3;
    }
}

void lb_fb_clear(char *fb, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b, U_INT_8_T t)
{
  S_INT_16_T y;
  char *pos, *pos_final;
  
  for (y=0; y<ty_height*ty_scale_y; y++)
    {
      pos       = fb + y*fix_info.line_length;
      pos_final = fb + (y+1)*fix_info.line_length  ;  //*var_info.vmode;
   
      while (pos<pos_final)
	{
	  *(pos) = b;
	  pos++;
	  *(pos) = g;
	  pos++;
	  *(pos) = r;
	  pos++;
	  *(pos) = t;
	  pos++;
	}
    } 
}


void lb_fb_rectangle(char *fb,  S_INT_16_T x0, S_INT_16_T y0, S_INT_16_T x1, S_INT_16_T y1, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b, U_INT_8_T t)
{
  S_INT_16_T x_min, x_max, y_min, y_max, y;
  char *pos, *pos_final;

  if (x0<=x1)
    {
      x_min=x0;
      x_max=x1;
    }
  else
    {
      x_min=x1;
      x_max=x0;
    }
  
  if (x_min<0)
    x_min=0;

  if (x_max>var_info.xres-1)
    x_max=var_info.xres-1;

  if (y0<=y1)
    {
      y_min=y0;
      y_max=y1;
    }
  else
   {
      y_min=y1;
      y_max=y0;
    }
  
  if (y_min<0)
    y_min=0;

  if (y_max>var_info.yres-1)
    y_max=var_info.yres-1;

  for (y=y_min; y<=y_max; y++)
    {
      pos       = fb + x_min*4 + y*fix_info.line_length;
      pos_final = fb + x_max*4 + y*fix_info.line_length;

      while (pos<=pos_final)
	{
	  *(pos) = b;
	  pos++;
	  *(pos) = g;
	  pos++;
	  *(pos) = r;
	  pos++;
	  *(pos) = t;
	  pos++;
	}
    }
}

void lb_fb_draw_triangle_fill_i(char *fb, POINT_2D_INT_T P0, POINT_2D_INT_T P1, POINT_2D_INT_T P2,
				 U_INT_8_T r, U_INT_8_T g, U_INT_8_T b, U_INT_8_T t)
{
  // http://www-users.mat.uni.torun.pl/~wrona/3d_tutor/tri_fillers.html
  POINT_2D_INT_T A, B, C, S, E, P_temp;
  S_INT_16_T x_start, x_end;
  S_INT_16_T ab_dx, ab_dy, ab_err, ab_e2, ab_sx, ab_sy;
  S_INT_16_T ac_dx, ac_dy, ac_err, ac_e2, ac_sx, ac_sy;
  S_INT_16_T bc_dx, bc_dy, bc_err, bc_e2, bc_sx, bc_sy;

  A=P0;
  B=P1;
  C=P2;
  
  if (B.y>C.y)
    {
      /* swap B and C */
      P_temp=B;
      B=C;
      C=P_temp;
    }

  if (A.y>B.y)
    {
      /* swap A and B */
      P_temp=A;
      A=B;
      B=P_temp;
    }

  if (B.y>C.y)
    {
      /* swap B and C */
      P_temp=B;
      B=C;
      C=P_temp;
    }

  /* Initialization for Bressenham AB */
  ab_dx= abs(B.x-A.x);
  ab_dy= abs(B.y-A.y);

  if (A.x<B.x)
    ab_sx=1;
  else
    ab_sx=-1;

  if (A.y<B.y)
    ab_sy=1;
  else
    ab_sy=-1;

  if (ab_dx>ab_dy)
    ab_err=ab_dx/2;
  else
    ab_err=-ab_dy/2;


  /* Initialization for Bressenham AC */
  ac_dx= abs(C.x-A.x);
  ac_dy= abs(C.y-A.y);

  if (A.x<C.x)
    ac_sx=1;
  else
    ac_sx=-1;

  if (A.y<C.y)
    ac_sy=1;
  else
    ac_sy=-1;

  if (ac_dx>ac_dy)
    ac_err=ac_dx/2;
  else
    ac_err=-ac_dy/2;

  /* Initialization for Bressenham BC*/
  bc_dx= abs(C.x-B.x);
  bc_dy= abs(C.y-B.y);

  if (B.x<C.x)
    bc_sx=1;
  else
    bc_sx=-1;

  if (B.y<C.y)
    bc_sy=1;
  else
    bc_sy=-1;

  if (bc_dx>bc_dy)
    bc_err=bc_dx/2;
  else
    bc_err=-bc_dy/2;

  
  //lb_ft_printf(ty_C, "A = (POINT_2D_INT_T){%d,%d};\r\n",A.x,A.y); 
  //lb_ft_printf(ty_C, "B = (POINT_2D_INT_T){%d,%d};\r\n",B.x,B.y); 
  //lb_ft_printf(ty_C, "C = (POINT_2D_INT_T){%d,%d};\r\n",C.x,C.y); 
  
  S=A;
  E=A;

  /* Include first vertice.  The following pixels depend on changes in the 'y' direction */
  lb_fb_draw_pixel(fb, A.x, A.y , r, g, b, t);
  do
    {
      ab_e2 = ab_err;
      if (ab_e2 >-ab_dx)
	{
	  ab_err -= ab_dy;
	  S.x    += ab_sx;
	}

      if (ab_e2 < ab_dy)
	{
	  ab_err += ab_dx;
	  S.y += ab_sy;
	  /* An increment in 'y' has ocurred.  We "nest" the drawing of the second line (AC) in between. */
	  /***************************/
	  do 
	    {
	      //lb_ft_printf(ty_C, "E.y=%d\r\n",E.y); 
	      ac_e2 = ac_err;

	      if (ac_e2 >-ac_dx)
		{
		  ac_err -= ac_dy;
		  E.x    += ac_sx;
		}

	      if (ac_e2 < ac_dy)
		{
		  ac_err += ac_dx;
		  E.y += ac_sy;
		  //lb_ft_printf(ty_C, "1) S.x=%d S.y=%d E.x=%d E.y=%d, delta=%d\r\n", S.x, S.y, E.x, E.y,S.x-E.x);
		  if (S.x!=E.x)
		     lb_fb_draw_pixel(fb, E.x, E.y , r, g, b, t);
 
		  //lb_gr_delay(500);

		  /* An increment in 'y' has ocurred.  This is where the scan line should be drawn. */
		  if (E.x>=S.x)
		    {
		      x_start=S.x;
		      x_end=E.x;
		    }
		  else
		    {
		      x_start=E.x;
		      x_end=S.x;
		    }
		  x_start++;
		  if ( (x_end-x_start) >1)
		    {
		      x_end--;
		      lb_fb_line_h(fb, E.y, x_start, x_end, r, g, b, t);
		    }
		}
	    } while (E.y!=S.y);
	  /***************************/
	}
      lb_fb_draw_pixel(fb, S.x, S.y , r, g, b, t);
      //lb_gr_delay(500);
    } while ((S.x!=B.x) || (S.y!=B.y));


  do
    {
      bc_e2 = bc_err;
      if (bc_e2 >-bc_dx)
	{
	  bc_err -= bc_dy;
	  S.x    += bc_sx;
	}

      if (bc_e2 < bc_dy)
	{
	  bc_err += bc_dx;
	  S.y += bc_sy;
	  /* An increment in 'y' has ocurred.  We "nest" the drawing of the second line (BC) in between. */
	  /***************************/
	  do 
	    {
	      //lb_ft_printf(ty_C, "E.y=%d\r\n",E.y); 
	      ac_e2 = ac_err;

	      if (ac_e2 >-ac_dx)
		{
		  ac_err -= ac_dy;
		  E.x    += ac_sx;
		}

	      if (ac_e2 < ac_dy)
		{
		  ac_err += ac_dx;
		  E.y += ac_sy;
		  //lb_ft_printf(ty_C, "1) S.x=%d S.y=%d E.x=%d E.y=%d, delta=%d\r\n", S.x, S.y, E.x, E.y,S.x-E.x);
		  if (S.x!=E.x)
		    lb_fb_draw_pixel(fb, E.x, E.y , r, g, b, t);

		  //lb_gr_delay(500);

		  /* An increment in 'y' has ocurred.  This is where the scan line should be drawn. */
		  if (E.x>=S.x)
		    {
		      x_start=S.x;
		      x_end=E.x;
		    }
		  else
		    {
		      x_start=E.x;
		      x_end=S.x;
		    }

		  x_start++;
		  if ( (x_end-x_start) >1)
		    {
		      x_end--;
		      lb_fb_line_h(fb, E.y, x_start, x_end, r, g, b, t);
		    }
		  lb_fb_line_h(fb, E.y, x_start, x_end, r, g, b, t);

		}
	    } while (E.y!=S.y);
	  /***************************/
	}
      lb_fb_draw_pixel(fb, S.x, S.y , r, g, b, t);

      //lb_gr_delay(500);
    } while ((S.x!=C.x) || (S.y!=C.y));
  
  //lb_gr_draw_pixel(NULL, A.x, A.y, lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_YELLOW), copymode);
  //lb_gr_draw_pixel(NULL, B.x, B.y, lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_YELLOW), copymode);
  //lb_gr_draw_pixel(NULL, C.x, C.y, lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_YELLOW), copymode);
}



#ifdef NADA

int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
    {
      ungetc(ch, stdin);
      return 1;
    }

  return 0;
}


struct termios tp, save;
int c;   
static struct termios oldt, newt;

/*tcgetattr gets the parameters of the current terminal
  STDIN_FILENO will tell tcgetattr that it should write the settings
  of stdin to oldt*/
tcgetattr( STDIN_FILENO, &oldt);
/*now the settings will be copied*/
newt = oldt;

/*ICANON normally takes care that one line at a time will be processed
  that means it will return if it sees a "\n" or an EOF or an EOL*/
newt.c_lflag &= ~(ICANON);          

/*Those new settings will be set to STDIN
  TCSANOW tells tcsetattr to change attributes immediately. */
tcsetattr( STDIN_FILENO, TCSANOW, &newt);



/*restore the old settings*/
tcsetattr( STDIN_FILENO, TCSANOW, &oldt);


#endif

