#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <float.h>
#include "lb_geometry.h"
#include "lb_algebra.h"
#include "lb_graphics.h"
#include "lb_real.h"
#include "lb_types.h"
#include "lb_numer.h"
#include "lb_fonts.h"

#define TEXTUREMODE_SOFTWARE

/* Pending:
   Implement Copymode weighted

   Look up instances of lb_gr_BMPfile_getsize, and make sure the return value is handled correctly:
   0 --> success
   1 --> failure 
*/

/* Global Variables.  They're declared "extern" in lb_x11.c */
extern CONSOLE_T *ty_C;
extern SCREEN_T ty_screen;

SDL_Renderer *renderer;
SDL_Window *window;
SDL_Texture *texture;

int pitch;
  	   

#define TEXTUREMODE_SOFTWARE

void lb_gr_SDL_init(const char *title, Uint32 flags, S_INT_16_T width, S_INT_16_T height, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b)
{
  SDL_Init(SDL_INIT_VIDEO);

  if ( (width==0) && (height==0) )
    {
      SDL_Rect rect;

      if (!SDL_GetDisplayUsableBounds(0, &rect))
	{
	  /* Some of the usable area will be taken by the title bar, whose size may be set or even change 
	     dynamically as per the user's settings and theme. I could not find how to read these and, likely, such 
	     a method does not exist covering all platforms.  The values below look nicely with the Linux Mint's default
	     settings using xfce". */
	  ty_screen.w = (U_INT_16_T)rect.w-6;  // 6: makes up for a 3 pixels border width on each side of the screen
	  ty_screen.h = (U_INT_16_T)rect.h-28; // 6*2 + 3*2 + 10 font size
	  printf("Detected usable area: width = %d, height = %d\r\n", rect.w, rect.h);
	}
      else
	{
	  printf("Error: lb_gr_SLD_init() --> Tried detecting maximum screen size but failed.\r\n");
	  exit(EXIT_FAILURE);
	}
    }
      else
    {	  
      ty_screen.w = width;
      ty_screen.h = height;
    }
  
  window = SDL_CreateWindow(title, 0, 0, ty_screen.w, ty_screen.h, 0);

  printf("Native window Format = %s\r\n",SDL_GetPixelFormatName(SDL_GetWindowPixelFormat(window)));

  if (SDL_GetWindowPixelFormat(window) != SDL_PIXELFORMAT_RGB888 )
    printf("Warning: Native window Format = %s\r\n",SDL_GetPixelFormatName(SDL_GetWindowPixelFormat(window)));
  
  /* The renderer is created as "Software" since it is actually faster both in the Raspberry Pi as well as in a Desktop */
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
  //SDL_SetRenderDrawColor(renderer, r, g, b, 255);
  //SDL_RenderClear(renderer);
  //SDL_RenderPresent(renderer);

#ifdef TEXTUREMODE_SOFTWARE
  ty_screen.dat=malloc(ty_screen.w*ty_screen.h*4);
#endif

#ifdef TEXTUREMODE_SOFTWARE
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, ty_screen.w, ty_screen.h);
  SDL_SetTextureBlendMode(texture,SDL_BLENDMODE_NONE);
#else
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, ty_screen.w, ty_screen.h);
  SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);
#endif
  lb_gr_fb_rectangle(&ty_screen, 0, 0, ty_screen.w, ty_screen.h, r, g, b);
  lb_gr_refresh();
}
  
void lb_gr_SDL_close()
{
  free(ty_screen.dat);
}

void lb_gr_refresh()
{
#ifdef TEXTUREMODE_SOFTWARE
  SDL_UpdateTexture(texture, NULL, &ty_screen.dat[0], 4*ty_screen.w); //Copy entire array only once
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
#else
  SDL_UpdateTexture(texture, NULL, &ty_screen.dat[0], pitch); //Copy entire array only once
  SDL_UnlockTexture(texture);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
  SDL_LockTexture(texture, NULL, (void**)&ty_screen.dat, &pitch);
#endif     
}

PIXEL_T lb_gr_12RGB(U_INT_16_T number)
{
  PIXEL_T pixel;
  U_INT_16_T temp_r, temp_g, temp_b, temp_a;

  temp_r = number       & 0x000F;
  temp_g = (number>>4)  & 0x000F;
  temp_b = (number>>8)  & 0x000F;
  temp_a = (number>>12) & 0x000F;

  temp_r=temp_r*MAX_R/15;
  temp_g=temp_g*MAX_G/15;
  temp_b=temp_b*MAX_B/15;
  temp_a=temp_a*MAX_B/15;

  pixel.r=temp_r;
  pixel.g=temp_g;
  pixel.b=temp_b;
  pixel.a=temp_a;
  
  return pixel;
}

S_INT_8_T lb_gr_assert_dimensions_line2d_i(LINE_2D_INT_T *L)
{
  if ( ((*L).items <= 0) || ((*L).items > LINE_MAX_ITEMS) )
    return 0;
  return 1;
}

S_INT_8_T lb_gr_assert_dimensions_line2d_f(LINE_2D_FLOAT_T *L)
{
  if ( ((*L).items <= 0) || ((*L).items > LINE_MAX_ITEMS) )
    return 0;
  return 1;
}

S_INT_8_T lb_gr_assert_dimensions_line3d_f(LINE_3D_FLOAT_T *L)
{
  if ( ((*L).items <= 0) || ((*L).items > VECTOR_MAX_ITEMS) )
    return 0;
  return 1;
}

S_INT_8_T lb_gr_assert_dimensions_picture(PICTURE_T *Pic)
{
  if (Pic==NULL)
    {
      if ( (ty_screen.w <= 0) || (ty_screen.w > PICTURE_MAX_WIDTH) ||
	   (ty_screen.h <= 0) || (ty_screen.h > PICTURE_MAX_HEIGHT) )
	{
	  printf("Error: lb_gr_assert_dimensions_picture() --> ty_screen.w=%i, ty_screen.h=%i\r\n",
		 ty_screen.w, ty_screen.h); 
	  return 0;
	}
    }
  else
    if ( ((*Pic).w <= 0) || ((*Pic).w > PICTURE_MAX_WIDTH) ||
	 ((*Pic).h <= 0) || ((*Pic).h > PICTURE_MAX_HEIGHT) )
      {
	printf("Error: lb_gr_assert_dimensions_picture() --> Pic.w=%i, Pic.h=%i\r\n",(*Pic).w, (*Pic).h); 
	return 0;
      }
  return 1;
}

void lb_gr_bitmap_rotate(PICTURE_T *pic_src, PICTURE_T *pic_dst, FLOAT_T angle, PIXEL_T default_color)
{
  MATRIX_R_T M1_r, M1_g, M1_b, M2;
  MATRIX_S_INT_8_T M_counter;
  S_INT_16_T i, j, i_rot, j_rot, i_adj, j_adj, x_lowest, x_highest, y_lowest, y_highest;
  FLOAT_T  x_rot, y_rot, distance;
  FLOAT_T x01, y01, x10, y10, x11, y11;
  ERR_T error;
  const FLOAT_T radius = 0.95;
  
  /* New limits detection */

  /* New limits detection */
  lb_re_rotate((*pic_src).w, 0.0, -angle, &x01, &y01);
  lb_re_rotate(0.0, (*pic_src).h, -angle, &x10, &y10);
  lb_re_rotate((*pic_src).w, (*pic_src).h, -angle, &x11, &y11);
  x_lowest=0.0;
  x_highest=0.0;
  y_lowest=0.0;
  y_highest=0.0;

  if(x01<x_lowest)
    x_lowest=x01;
  if(x10<x_lowest)
    x_lowest=x10;
  if(x11<x_lowest)
    x_lowest=x11;

  if(x01>x_highest)
    x_highest=x01;
  if(x10>x_highest)
    x_highest=x10;
  if(x11>x_highest)
    x_highest=x11;

  if(y01<y_lowest)
    y_lowest=y01;
  if(y10<y_lowest)
    y_lowest=y10;
  if(y11<y_lowest)
    y_lowest=y11;

  if(y01>y_highest)
    y_highest=y01;
  if(y10>y_highest)
    y_highest=y10;
  if(y11>y_highest)
    y_highest=y11;

  /* If destination picture had already been created, destroy it */ 
  if(((*pic_dst).w==0) || ((*pic_dst).h==0))
    lb_gr_release_picture(pic_dst);
       
  (*pic_dst).w=ceil(x_highest)-floor(x_lowest);
  (*pic_dst).h=ceil(y_highest)-floor(y_lowest);
  lb_gr_create_picture(pic_dst,default_color);

  M1_r.cols=(*pic_dst).w+1;
  M1_r.rows=(*pic_dst).h+1;
  M1_g.cols=(*pic_dst).w+1;
  M1_g.rows=(*pic_dst).h+1;
  M1_b.cols=(*pic_dst).w+1;
  M1_b.rows=(*pic_dst).h+1;
  

  M2.cols=(*pic_dst).w+1;
  M2.rows=(*pic_dst).h+1;
  M_counter.cols=(*pic_dst).w+1;
  M_counter.rows=(*pic_dst).h+1;

  lb_al_create_matrix_r(&M1_r);
  lb_al_create_matrix_r(&M1_g);
  lb_al_create_matrix_r(&M1_b);
  lb_al_create_matrix_r(&M2);
 
  lb_al_create_matrix_si8(&M_counter);
  
  FLOAT_T sin_val, cos_val;
  sin_val=sin(-angle);
  cos_val=cos(-angle);

  for(i=0;i<(*pic_src).h;i++)
    for(j=0;j<(*pic_src).w;j++)
      {
	x_rot = -x_lowest + ((FLOAT_T)j+0.5)*cos_val - ((FLOAT_T)i+0.5)*sin_val;  
	y_rot = -y_lowest + ((FLOAT_T)j+0.5)*sin_val + ((FLOAT_T)i+0.5)*cos_val;

	j_rot = (S_INT_16_T)x_rot;
	i_rot = (S_INT_16_T)y_rot;
	//lb_ft_printf(ty_C, "angle=%f, x_lowest=%d, y_lowest=%d, j_rot=%d/%d, i_rot=%d/%d\r\n",
	//       angle, x_lowest, y_lowest,j_rot,(*pic_dst).w,i_rot,(*pic_dst).h);
	
	if (lb_re_frac(x_rot)>=0.5)
	  j_adj=j_rot+1;
	else
	  j_adj=j_rot-1;

	if (lb_re_frac(y_rot)>=0.5)
	  i_adj=i_rot+1;
	else
	  i_adj=i_rot-1;

	/* Check distance self pixel */
	if(M_counter.array[i_rot][j_rot]!=-1)
	  {	   
	    distance=lb_re_sqrt(lb_re_sqr((FLOAT_T)j_rot+0.5-x_rot,&error)+lb_re_sqr((FLOAT_T)i_rot+0.5 - y_rot,&error),&error);
	    if(distance<=radius)
	      {
		if(lb_re_equal(distance,0.0))
		  {
		    M1_r.array[i_rot][j_rot]=(*pic_src).pic[i][j].r;
		    M1_g.array[i_rot][j_rot]=(*pic_src).pic[i][j].g;
		    M1_b.array[i_rot][j_rot]=(*pic_src).pic[i][j].b;
		    M_counter.array[i_rot][j_rot]=-1; /* A flag indicating the distance was zero */
		  }
		else
		  {
		    M1_r.array[i_rot][j_rot]+=(*pic_src).pic[i][j].r/distance;
		    M1_g.array[i_rot][j_rot]+=(*pic_src).pic[i][j].g/distance;
		    M1_b.array[i_rot][j_rot]+=(*pic_src).pic[i][j].b/distance;
		    M2.array[i_rot][j_rot]+=1.0/distance;
		    M_counter.array[i_rot][j_rot]++;
		  }
	      }
	  }

	/* Check distance to j_adj,i adjacent pixel */
	if ((0<=j_adj) && (j_adj<M1_r.cols))
	  if(M_counter.array[i_rot][j_adj]!=-1)
	    {	   
	      distance=lb_re_sqrt(lb_re_sqr((FLOAT_T)j_adj+0.5-x_rot,&error)+lb_re_sqr((FLOAT_T)i_rot+0.5-y_rot,&error),&error);
	      if(distance<=radius)
		{
		  if(lb_re_equal(distance,0.0))
		    {
		      M1_r.array[i_rot][j_adj]=(*pic_src).pic[i][j].r;
		      M1_g.array[i_rot][j_adj]=(*pic_src).pic[i][j].g;
		      M1_b.array[i_rot][j_adj]=(*pic_src).pic[i][j].b;
		      M_counter.array[i_rot][j_adj]=-1; /* A flag indicating the distance was zero */
		    }
		  else
		    {
		      M1_r.array[i_rot][j_adj]+=(*pic_src).pic[i][j].r/distance;
		      M1_g.array[i_rot][j_adj]+=(*pic_src).pic[i][j].g/distance;
		      M1_b.array[i_rot][j_adj]+=(*pic_src).pic[i][j].b/distance;
		      M2.array[i_rot][j_adj]+=1.0/distance;
		      M_counter.array[i_rot][j_adj]++;
		    }
		}
	    }

	/* Check distance to i_adj,j adjacent pixel */
	if ((0<=i_adj) && (i_adj<M1_r.rows))
	  if(M_counter.array[i_adj][j_rot]!=-1)
	    {	   
	      distance=lb_re_sqrt(lb_re_sqr((FLOAT_T)j_rot+0.5-x_rot,&error)+lb_re_sqr((FLOAT_T)i_adj+0.5-y_rot,&error),&error);
	      if(distance<=radius)
		{
		  if(lb_re_equal(distance,0.0))
		    {
		      M1_r.array[i_adj][j_rot]=(*pic_src).pic[i][j].r;
		      M1_g.array[i_adj][j_rot]=(*pic_src).pic[i][j].g;
		      M1_b.array[i_adj][j_rot]=(*pic_src).pic[i][j].b;
		      M_counter.array[i_adj][j_rot]=-1; /* A flag indicating the distance was zero */
		    }
		  else
		    {
		      M1_r.array[i_adj][j_rot]+=(*pic_src).pic[i][j].r/distance;
		      M1_g.array[i_adj][j_rot]+=(*pic_src).pic[i][j].g/distance;
		      M1_b.array[i_adj][j_rot]+=(*pic_src).pic[i][j].b/distance;
		      M2.array[i_adj][j_rot]+=1.0/distance;
		      M_counter.array[i_adj][j_rot]++;
		    }
		}
	    }

	/* Check distance to i_adj,j_adj adjacent pixel */
	if ((0<=i_adj) && (i_adj<M1_r.rows) && (0<=j_adj) && (j_adj<M1_r.cols))
	  if(M_counter.array[i_adj][j_adj]!=-1)
	    {	   
	      distance=lb_re_sqrt(lb_re_sqr((FLOAT_T)j_adj+0.5-x_rot,&error)+lb_re_sqr((FLOAT_T)i_adj+0.5-y_rot,&error),&error);
	      if(distance<=radius)
		{
		  if(lb_re_equal(distance,0.0))
		    {
		      M1_r.array[i_adj][j_adj]=(*pic_src).pic[i][j].r;
		      M1_g.array[i_adj][j_adj]=(*pic_src).pic[i][j].g;
		      M1_b.array[i_adj][j_adj]=(*pic_src).pic[i][j].b;
		      M_counter.array[i_adj][j_adj]=-1; /* A flag indicating the distance was zero */
		    }
		  else
		    {
		      M1_r.array[i_adj][j_adj]+=(*pic_src).pic[i][j].r/distance;
		      M1_g.array[i_adj][j_adj]+=(*pic_src).pic[i][j].g/distance;
		      M1_b.array[i_adj][j_adj]+=(*pic_src).pic[i][j].b/distance;
		      M2.array[i_adj][j_adj]+=1.0/distance;
		      M_counter.array[i_adj][j_adj]++;
		    }
		}
	    }
      }

  i=0;
  for(j=0;j<(*pic_src).w;j++)
    {
      x_rot = -x_lowest + ((FLOAT_T)j+0.5)*cos_val - ((FLOAT_T)i+0.5)*sin_val;  
      y_rot = -y_lowest + ((FLOAT_T)j+0.5)*sin_val + ((FLOAT_T)i+0.5)*cos_val;

      j_rot = (S_INT_16_T)x_rot;
      i_rot = (S_INT_16_T)y_rot;

      M1_r.array[i_rot][j_rot]=0;
      M1_g.array[i_rot][j_rot]=MAX_G;
      M1_b.array[i_rot][j_rot]=0;
      M_counter.array[i_rot][j_rot]=-1;
    }
  
  for(i=0;i<(*pic_dst).h;i++)
    for(j=0;j<(*pic_dst).w;j++)
      {
	if (M_counter.array[i][j]!=0)
	  {
	    if(M_counter.array[i][j]==-1)
	      {
		(*pic_dst).pic[i][j].r=round(M1_r.array[i][j]);
		(*pic_dst).pic[i][j].g=round(M1_g.array[i][j]);
		(*pic_dst).pic[i][j].b=round(M1_b.array[i][j]);
	      }
	    else
	      {
		(*pic_dst).pic[i][j].r=round(M1_r.array[i][j]/M2.array[i][j]);
		(*pic_dst).pic[i][j].g=round(M1_g.array[i][j]/M2.array[i][j]);
		(*pic_dst).pic[i][j].b=round(M1_b.array[i][j]/M2.array[i][j]);
	      }
	  }
	else
	  {
	    //lb_ft_printf(ty_C, "denom=%f\r\n",M2.array[i][j]);
	    (*pic_dst).pic[i][j].r=round(default_color.r);
	    (*pic_dst).pic[i][j].g=round(default_color.g);
	    (*pic_dst).pic[i][j].b=round(default_color.b);
	  }
      }
  
  lb_al_release_matrix_r(&M1_r);
  lb_al_release_matrix_r(&M1_g);
  lb_al_release_matrix_r(&M1_b);
  lb_al_release_matrix_r(&M2);
  lb_al_release_matrix_si8(&M_counter);
}

void lb_gr_bitmap_rotate_sampling(PICTURE_T *pic_src, PICTURE_T *pic_dst, FLOAT_T angle, U_INT_8_T n_samples, PIXEL_T default_color)
{
  MATRIX_R_T M_r, M_g, M_b, M_k;
  S_INT_16_T i, j, i_rot, j_rot;
  FLOAT_T x01, y01, x10, y10, x11, y11, ir, jr,
    x_lowest, x_highest,
    y_lowest, y_highest, max_r, max_g, max_b, max_k;
  
  /* New limits detection */
  lb_re_rotate((*pic_src).w, 0.0, -angle, &x01, &y01);
  lb_re_rotate(0.0, (*pic_src).h, -angle, &x10, &y10);
  lb_re_rotate((*pic_src).w, (*pic_src).h, -angle, &x11, &y11);
  x_lowest=0.0;
  x_highest=0.0;
  y_lowest=0.0;
  y_highest=0.0;

  if(x01<x_lowest)
    x_lowest=x01;
  if(x10<x_lowest)
    x_lowest=x10;
  if(x11<x_lowest)
    x_lowest=x11;

  if(x01>x_highest)
    x_highest=x01;
  if(x10>x_highest)
    x_highest=x10;
  if(x11>x_highest)
    x_highest=x11;

  if(y01<y_lowest)
    y_lowest=y01;
  if(y10<y_lowest)
    y_lowest=y10;
  if(y11<y_lowest)
    y_lowest=y11;

  if(y01>y_highest)
    y_highest=y01;
  if(y10>y_highest)
    y_highest=y10;
  if(y11>y_highest)
    y_highest=y11;

  /* If destination picture had already been created, destroy it */ 
  if(((*pic_dst).w==0) || ((*pic_dst).h==0))
    lb_gr_release_picture(pic_dst);
       
  (*pic_dst).w=ceil(x_highest)-floor(x_lowest);
  (*pic_dst).h=ceil(y_highest)-floor(y_lowest);
  lb_gr_create_picture(pic_dst,default_color);

  M_r.cols=(*pic_dst).w;
  M_r.rows=(*pic_dst).h;
  lb_al_create_matrix_r(&M_r);

  M_g.cols=(*pic_dst).w;
  M_g.rows=(*pic_dst).h;
  lb_al_create_matrix_r(&M_g);

  M_b.cols=(*pic_dst).w;
  M_b.rows=(*pic_dst).h;
  lb_al_create_matrix_r(&M_b);

  M_k.cols=(*pic_dst).w;
  M_k.rows=(*pic_dst).h;
  lb_al_create_matrix_r(&M_k);

  U_INT_16_T limit_i, limit_j;
  FLOAT_T factor, sin_val, cos_val;
  //jr=1.0/(2*n_samples)+(FLOAT_T)j/n_samples;
  //ir=1.0/(2*n_samples)+(FLOAT_T)i/n_samples;
  factor=1.0/n_samples;
  sin_val=-sin(angle);
  cos_val=cos(angle);
  limit_i=(*pic_src).h*n_samples;
  limit_j=(*pic_src).w*n_samples;
  for(i=0;i<limit_i;i++)
    for(j=0;j<limit_j;j++)
      {
	jr=factor*(0.5+(FLOAT_T)j);
	ir=factor*(0.5+(FLOAT_T)i);
	j_rot=-x_lowest+jr*cos_val-ir*sin_val;
	i_rot=-y_lowest+ir*cos_val+jr*sin_val;
	if ((i_rot>=(*pic_dst).h) || (j_rot>=(*pic_dst).w))
	  {
	    printf("Error: lb_gr_rotate_bitmap() --> Out of boundaries\r\n");
	    exit(EXIT_FAILURE);
	  }
	else
	  {
	    M_r.array[i_rot][j_rot]+=(FLOAT_T)(*pic_src).pic[(U_INT_16_T)ir][(U_INT_16_T)jr].r;
	    M_g.array[i_rot][j_rot]+=(FLOAT_T)(*pic_src).pic[(U_INT_16_T)ir][(U_INT_16_T)jr].g;
	    M_b.array[i_rot][j_rot]+=(FLOAT_T)(*pic_src).pic[(U_INT_16_T)ir][(U_INT_16_T)jr].b;
	    M_k.array[i_rot][j_rot]+=(FLOAT_T)(*pic_src).pic[(U_INT_16_T)ir][(U_INT_16_T)jr].a;
	  }
      }
  max_r=0;
  max_g=0;
  max_b=0;
  max_k=0;
  for(i=0;i<(*pic_dst).h;i++)
    for(j=0;j<(*pic_dst).w;j++)
      {
	//lb_ft_printf(ty_C, "max_rgb= %f %f %f\r\n",max_r, max_g, max_b);
	if (M_r.array[i][j]>max_r)
	  max_r=M_r.array[i][j];
	if (M_g.array[i][j]>max_g)
	  max_g=M_g.array[i][j];
	if (M_b.array[i][j]>max_b)
	  max_b=M_b.array[i][j];
	if (M_k.array[i][j]>max_k)
	  max_k=M_k.array[i][j];
      }

  for(i=0;i<(*pic_dst).h;i++)
    for(j=0;j<(*pic_dst).w;j++)
      {
	if(M_r.array[i][j]<MAX_R*n_samples*n_samples)
	  (*pic_dst).pic[i][j].r=M_r.array[i][j]/(n_samples*n_samples);
	else
	  (*pic_dst).pic[i][j].r=MAX_R*M_r.array[i][j]/max_r;

	if(M_g.array[i][j]<MAX_G*n_samples*n_samples)
	  (*pic_dst).pic[i][j].g=M_g.array[i][j]/(n_samples*n_samples);
	else
	  (*pic_dst).pic[i][j].g=MAX_G*M_g.array[i][j]/max_g;

	if(M_b.array[i][j]<MAX_B*n_samples*n_samples)
	  (*pic_dst).pic[i][j].b=M_b.array[i][j]/(n_samples*n_samples);
	else
	  (*pic_dst).pic[i][j].b=MAX_B*M_b.array[i][j]/max_b;
      }
  lb_al_release_matrix_r(&M_r);
  lb_al_release_matrix_r(&M_g);
  lb_al_release_matrix_r(&M_b);
  lb_al_release_matrix_r(&M_k);
}

void lb_gr_BMPfile_getsize(const char *filename, S_INT_16_T *width, S_INT_16_T *height)
{
  FILE *fd;
  fd = fopen(filename, "rb");
  if (fd == NULL)
    {
      lb_ft_printf(ty_C, "Error: lb_gr_BMPfile_getsize() --> fopen failed\n");
      return; 
    }

  unsigned char header[54];

  // Read header
  fread(header, sizeof(unsigned char), 54, fd);

  // Capture dimensions
  *width = *(int*)&header[18];
  *height = *(int*)&header[22];
  fclose(fd);
}

void lb_gr_BMPfile_load_to_pic(const char *filename, PICTURE_T *Pic, U_INT_8_T alpha)
{
  static int width, height;
  int i,j, padding,widthnew;
  long offset;

  FILE *fd;
  fd = fopen(filename, "rb");
  if (fd == NULL)
    {
      printf("Error: lb_gr_BMPfile_load_to_pic() --> fopen failed\n");
      exit(EXIT_FAILURE);
    }	
    
  unsigned char header[54];
  
  // Read header

  fread(header, sizeof(unsigned char), 54, fd);
  
  // Capture dimensions
  width = *(int*)&header[18];
  height = *(int*)&header[22];

  padding = 0;

  // Calculate padding
  while (((width*3+padding) % 4) != 0)
    padding++;
    
  // Compute new width, which includes padding
  widthnew = width * 3 + padding;
  
  // Allocate temporary memory to read widthnew size of data
  unsigned char* data = (unsigned char *)malloc(widthnew * sizeof (unsigned char));

  offset=*(int*)&header[10];
  fseek(fd, offset, SEEK_SET);
  // Read row by row of data and remove padded data.
  for (i=0; i<height; i++)
    {
      // Read widthnew length of data
      fread(data, sizeof(unsigned char), widthnew, fd);

      // Retain width length of data, and swizzle RB component.
      // BMP stores in BGR format, my usecase needs RGB format
      for (j=0; j<width; j++)
	{
	  (*Pic).pic[height-i-1][j].r = data[3*j + 2]*MAX_B/255;
	  (*Pic).pic[height-i-1][j].g = data[3*j + 1]*MAX_G/255;
	  (*Pic).pic[height-i-1][j].b = data[3*j + 0]*MAX_R/255;
	}
    }
  free(data);
  fclose(fd);
}

  
void lb_gr_BMPfile_load_to_matrix(const char *filename, MATRIX_R_T *R, MATRIX_R_T *G, MATRIX_R_T *B)
{
  static int width, height;
  int i,j, padding, widthnew;
  long offset;

  FILE *fd;
  fd = fopen(filename, "rb");
  if (fd == NULL)
    {
      printf("Error: lb_gr_BMPfile_load_to_pic() --> fopen failed\n");
      exit(EXIT_FAILURE);
    }	
      
  unsigned char header[54];
  
  // Read header

  fread(header, sizeof(unsigned char), 54, fd);
  
  // Capture dimensions
  width = *(int*)&header[18];
  height = *(int*)&header[22];

  if ( ((R!=NULL) && (width !=(*R).cols)) || ((G!=NULL) && (width !=(*G).cols)) || ((B!=NULL) && (width !=(*B).cols)) ||
       ((R!=NULL) && (height!=(*R).rows)) || ((G!=NULL) && (height!=(*G).rows)) || ((B!=NULL) && (height!=(*B).rows)) )       
    {
      printf("Error: lb_gr_BMPfile_load_to_matrix() --> dimension mismatch\r\n");
      printf("\tBMP:\twidth=%d\theight=%d\t\r\n",width,height);
      if (R!=NULL)
	printf("\tR:\tcols=%d\trows=%d\t\r\n",(*R).cols,(*R).rows);
      if (G!=NULL)
	printf("\tG:\tcols=%d\trows=%d\t\r\n",(*G).cols,(*G).rows);
      if (B!=NULL)
	printf("\tR:\tcols=%d\trows=%d\t\r\n",(*B).cols,(*B).rows);
      exit(EXIT_FAILURE);
    }

  padding = 0;

  // Calculate padding
  while (((width*3+padding) % 4) != 0)
    padding++;
      
  // Compute new width, which includes padding
  widthnew = width * 3 + padding;
  
  // Allocate temporary memory to read widthnew size of data
  unsigned char* data = (unsigned char *)malloc(widthnew * sizeof (unsigned char));

  offset=*(int*)&header[10];
  fseek(fd, offset, SEEK_SET);
  // Read row by row of data and remove padded data.
  for (i=0; i<height; i++)
    {
      // Read widthnew length of data
      fread(data, sizeof(unsigned char), widthnew, fd);

      // Retain width length of data, and swizzle RB component.
      // BMP stores in BGR format, my usecase needs RGB format
      for (j=0; j<width; j++)
	{
	  if (R!=NULL)
	    (*R).array[i][j] = (FLOAT_T)data[3*j + 2]/255.0;
	  if (G!=NULL)
	    (*G).array[i][j] = (FLOAT_T)data[3*j + 1]/255.0;
	  if (B!=NULL)
	    (*B).array[i][j] = (FLOAT_T)data[3*j + 0]/255.0;
	}
    }
  free(data);
  fclose(fd);
}

void lb_gr_BMPfile_save(const char *filename, PICTURE_T *Pic)
{
  FILE * outfile;
  unsigned int headers[13];
  int extrabytes;
  int paddedsize;
  int x, y, n, width, height;
  int r, g, b;
  U_INT_32_T dat_offset;

  if (Pic==NULL)
    {
      width  = ty_screen.w;  // biWidth
      height = ty_screen.h; // biHeight
    }
  else
    {
      width  = (*Pic).w;  // biWidth
      height = (*Pic).h;  // biHeight
    }
  
  extrabytes = 4 - ((width*3) % 4);                 // How many bytes of padding to add to each
  // horizontal line - the size of which must
  // be a multiple of 4 bytes.
  if (extrabytes == 4)
    extrabytes = 0;

  paddedsize = (width*3 + extrabytes)*height;

  /* Headers. Note that the "BM" identifier in bytes 0 and 1 is NOT included in these "headers". */

  headers[0]  = paddedsize + 54;      // bfSize (whole file size)
  headers[1]  = 0;                    // bfReserved (both)
  headers[2]  = 54;                   // bfOffbits
  headers[3]  = 40;                   // biSize
  headers[4]  = width;                // biWidth
  headers[5]  = height;               // biHeight

  // Would have biPlanes and biBitCount in position 6, but they're shorts.
  // It's easier to write them out separately (see below) than pretend
  // they're a single int, especially with endian issues...

  headers[7]  = 0;                    // biCompression
  headers[8]  = paddedsize;           // biSizeImage
  headers[9]  = 0;                    // biXPelsPerMeter
  headers[10] = 0;                    // biYPelsPerMeter
  headers[11] = 0;                    // biClrUsed
  headers[12] = 0;                    // biClrImportant

  outfile = fopen(filename, "wb");

  /* Headers.  When printing ints and shorts, we write out 1 character at a time to avoid endian issues. */
  fprintf(outfile, "BM");
  for (n=0; n<=5; n++)
    {
      fprintf(outfile, "%c", headers[n] & 0x000000FF);
      fprintf(outfile, "%c", (headers[n] & 0x0000FF00) >> 8);
      fprintf(outfile, "%c", (headers[n] & 0x00FF0000) >> 16);
      fprintf(outfile, "%c", (headers[n] & (unsigned int) 0xFF000000) >> 24);
    }

  // These next 4 characters are for the biPlanes and biBitCount fields.

  fprintf(outfile, "%c", 1);
  fprintf(outfile, "%c", 0);
  fprintf(outfile, "%c", 24);
  fprintf(outfile, "%c", 0);

  for (n = 7; n <= 12; n++)
    {
      fprintf(outfile, "%c", headers[n] & 0x000000FF);
      fprintf(outfile, "%c", (headers[n] & 0x0000FF00) >> 8);
      fprintf(outfile, "%c", (headers[n] & 0x00FF0000) >> 16);
      fprintf(outfile, "%c", (headers[n] & (unsigned int) 0xFF000000) >> 24);
    }

  /* Writting the data */
  if (Pic==NULL)
    for (y=height-1; y>=0; y--)     /* BMP image format is written from bottom to top */
      {
	for (x=0; x<width; x++)
	  {
	    dat_offset=4*(ty_screen.w*y+x);
	    b = *((U_INT_8_T*)ty_screen.dat + dat_offset    );
	    g = *((U_INT_8_T*)ty_screen.dat + dat_offset + 1); 
	    r = *((U_INT_8_T*)ty_screen.dat + dat_offset + 2);

	    fprintf(outfile, "%c", b);
	    fprintf(outfile, "%c", g);
	    fprintf(outfile, "%c", r);
	  }
	if (extrabytes)      /* Padding */
	  {
	    for (n=1; n <= extrabytes; n++)
	      fprintf(outfile, "%c", 0);
	  }
      }
  else
    for (y=height-1; y>=0; y--)     /* BMP image format is written from bottom to top */
      {
	for (x=0; x<width; x++)
	  {
	    r = round(FACTOR_N_TO_8_R*(*Pic).pic[y][x].r);
	    g = round(FACTOR_N_TO_8_R*(*Pic).pic[y][x].g);
	    b = round(FACTOR_N_TO_8_R*(*Pic).pic[y][x].b);

	    fprintf(outfile, "%c", b);
	    fprintf(outfile, "%c", g);
	    fprintf(outfile, "%c", r);
	  }
	if (extrabytes)      /* Padding */
	  {
	    for (n = 1; n <= extrabytes; n++)
	      fprintf(outfile, "%c", 0);
	  }
      }
  fclose(outfile);
}

void lb_gr_delay(S_INT_32_T delay)
{
  clock_t stamp, delta;
  stamp=clock();
  delta=round(0.001*delay*CLOCKS_PER_SEC);
  while ( clock()<=(stamp+delta) )
    {
      ;
    }
}
   
S_INT_8_T lb_gr_JPGfile_save(const char *filename, PICTURE_T *Pic, U_INT_8_T quality)
{
  FILE* outfile;
  S_INT_16_T width, height;
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr       jerr;
  JSAMPROW row_pointer;          /* pointer to a single row */
  U_INT_16_T j;
  unsigned  char *buffer;
  if(quality>100)
    {
      printf("Error: lb_gr_JPGfile_save() --> quality>100\r\n");
      exit(EXIT_FAILURE);
    }
  
  outfile=fopen(filename, "wb");
  if (!outfile)
    {
      printf("Error: lb_gr_JPGfile_save() --> Unable to create file %s", filename);
      exit(EXIT_FAILURE);
    }
  
  if (Pic==NULL)
    {
      width=ty_screen.w;
      height=ty_screen.h;
    }
  else
    {
      width =(*Pic).w;
      height=(*Pic).h;
    }
  
  buffer=malloc(3*sizeof(unsigned char)*width);
    
  if (buffer==NULL)
    {
      printf("Error: lb_gr_JPGfile_save() --> could not allocate memory\r\n");
      exit(EXIT_FAILURE);
    }	
     
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  jpeg_stdio_dest(&cinfo, outfile);

  cinfo.image_width      = width;
  cinfo.image_height     = height;
  cinfo.input_components = 3;
  cinfo.in_color_space   = JCS_RGB;
  jpeg_set_defaults(&cinfo);
  /*set the quality [0..100]  */
  jpeg_set_quality (&cinfo, quality, TRUE);
  jpeg_start_compress(&cinfo, TRUE);
 
  if (Pic==NULL)
    while (cinfo.next_scanline < cinfo.image_height)
      {
	for(j=0;j<width;j++)
	  {
	    //	    buffer[(unsigned int)3*j]   = (unsigned char)ty_pic_shadow.pic[cinfo.next_scanline][j].r*FACTOR_N_TO_8_R;
	    //buffer[(unsigned int)3*j+1] = (unsigned char)ty_pic_shadow.pic[cinfo.next_scanline][j].g*FACTOR_N_TO_8_G;
	    //buffer[(unsigned int)3*j+2] = (unsigned char)ty_pic_shadow.pic[cinfo.next_scanline][j].b*FACTOR_N_TO_8_B;
	  }
	row_pointer= (JSAMPROW) buffer;
	jpeg_write_scanlines(&cinfo, &row_pointer, 1);
      }
  else
    while (cinfo.next_scanline < cinfo.image_height)
      {
	for(j=0;j<width;j++)
	  {
	    buffer[(unsigned int)3*j]   = (unsigned char)(*Pic).pic[cinfo.next_scanline][j].r*FACTOR_N_TO_8_R;
	    buffer[(unsigned int)3*j+1] = (unsigned char)(*Pic).pic[cinfo.next_scanline][j].g*FACTOR_N_TO_8_G;
	    buffer[(unsigned int)3*j+2] = (unsigned char)(*Pic).pic[cinfo.next_scanline][j].b*FACTOR_N_TO_8_B;
	  }
	row_pointer= (JSAMPROW) buffer;
	jpeg_write_scanlines(&cinfo, &row_pointer, 1);
      }
  jpeg_finish_compress(&cinfo);
  
  free(buffer);
  fclose(outfile);
  return 1;
}


void lb_gr_clear_picture(PICTURE_T *Pic, PIXEL_T default_color)
{
  S_INT_16_T i, j;
  if (Pic==NULL)
    lb_gr_fb_rectangle(&ty_screen, 0, 0, ty_screen.w, ty_screen.h,
		       FACTOR_N_TO_8_R*default_color.r, FACTOR_N_TO_8_G*default_color.g, FACTOR_N_TO_8_B*default_color.b);
  else
    {
      if (!lb_gr_assert_dimensions_picture(Pic))
	{
	  printf("Error: lb_gr_clear_picture() --> invalid dimension\r\n");
          exit(EXIT_FAILURE);
	}
      
      for (i=0;i<(*Pic).h;i++)
	for (j=0;j<(*Pic).w;j++)
	  {
	    (*Pic).pic[i][j].r=default_color.r;
	    (*Pic).pic[i][j].g=default_color.g;
	    (*Pic).pic[i][j].b=default_color.b;
	    (*Pic).pic[i][j].a=default_color.a;
	  }
    }
}

/* Checks if a point us at the left, on, or at the right of a line.  I*/
S_INT_16_T lb_gr_check_left_i(POINT_2D_INT_T P0, POINT_2D_INT_T P1, POINT_2D_INT_T P)
{
  S_INT_32_T value;
  value=(P.y-P0.y)*(P1.x-P0.x)-(P.x-P0.x)*(P1.y-P0.y);
  if(value>0) return 1;
  if(value<0) return -1;
  return 0;
}

/* Checks if a point us at the left, on, or at the right of a line.  I*/
S_INT_16_T lb_gr_check_left_f(POINT_2D_FLOAT_T P0, POINT_2D_FLOAT_T P1, POINT_2D_FLOAT_T P)
{
  FLOAT_T value;
  value=(P.y-P0.y)*(P1.x-P0.x)-(P.x-P0.x)*(P1.y-P0.y);
  if (lb_re_equal(value, 0.0))
    return 0;
  if(value>0) return 1;
  if(value<0) return -1;
  return 0;
}

void lb_gr_create_line2d_i(LINE_2D_INT_T *L)
{
  S_INT_16_T j;
  if (!lb_gr_assert_dimensions_line2d_i(L))
    {
      printf("Error: lb_gr_line_create_i() --> invalid dimension\r\n");
      exit(EXIT_FAILURE);
    }
  
  (*L).array=malloc((*L).items*sizeof(POINT_2D_INT_T));
  if ((*L).array==NULL)
    {
      printf("Error: lb_gr_line_create_i() --> Out of memory\r\n");
      exit(EXIT_FAILURE);
    }
  
  for(j=0;j<(*L).items;j++)
    {
      (*L).array[j].x=0;
      (*L).array[j].y=0;
      (*L).array[j].flag=TRUE;
    }
}

void lb_gr_create_line2d_f(LINE_2D_FLOAT_T *L)
{
  S_INT_16_T j;

  if (!lb_gr_assert_dimensions_line2d_f(L))
    {
      printf("Error: lb_gr_create_line_f() --> invalid dimension\r\n");
      exit(EXIT_FAILURE);
    }
  
  (*L).array=malloc((*L).items*sizeof(POINT_2D_FLOAT_T));
  if ((*L).array==NULL)
    {
      printf("Error: lb_gr_create_line_f() --> Out of memory\r\n");
      exit(EXIT_FAILURE);
    }
  
  for(j=0;j<(*L).items;j++)
    {
      (*L).array[j].x=0.0;
      (*L).array[j].y=0.0;
      (*L).array[j].flag=TRUE;
    }
}

void lb_gr_create_line3d_f(LINE_3D_FLOAT_T *L)
{
  if (!lb_gr_assert_dimensions_line3d_f(L))
    {
      printf("Error: lb_gr_create_line3d_f() --> invalid dimension\r\n");
      exit(EXIT_FAILURE);
    }
  
  (*L).array=malloc((*L).items*sizeof(LINE_3D_FLOAT_T));

  if ((*L).array==NULL)
    {
      printf("Error: lb_gr_create_line3d_f() --> out of memory\r\n");
      exit(EXIT_FAILURE);
    }	    
}

void lb_gr_create_picture(PICTURE_T *Pic, PIXEL_T default_color)
{
  S_INT_16_T i, j;

  if (!lb_gr_assert_dimensions_picture(Pic))
    {
      fflush(stdout);
      printf("Error: lb_gr_create_picture() --> invalid dimension\r\n");
      exit(EXIT_FAILURE);
    }

  (*Pic).pic=malloc((*Pic).h*sizeof(PIXEL_T*));
  
  if ((*Pic).pic==NULL)
    {
      printf("Error: lb_gr_create_picture() --> Out of memory\r\n");
      exit(EXIT_FAILURE);
    }
  
  for (i=0;i<(*Pic).h;i++)
    { 
      (*Pic).pic[i]=malloc((*Pic).w*sizeof(PIXEL_T));
      if ((*Pic).pic[i]== NULL)
	{
	  printf("Error: lb_gr_create_picture() --> Out of memory\r\n");
	  exit(EXIT_FAILURE);
	}
    }

  for (i=0;i<(*Pic).h;i++)
    for (j=0;j<(*Pic).w;j++)
      {
	(*Pic).pic[i][j].r=default_color.r;
	(*Pic).pic[i][j].g=default_color.g;
	(*Pic).pic[i][j].b=default_color.b;
 	(*Pic).pic[i][j].a=default_color.a;
      }
}


void lb_gr_reset_zbuffer(MATRIX_R_T *Z)
{
  S_INT_16_T i, j;
 
  if (!lb_al_assert_dimensions_matrix_r(Z))
    {
      lb_ft_printf(ty_C, "Error: lb_gr_reset_zbuffer() --> invalid dimension\r\n");
    }
  for (i=0;i<(*Z).rows;i++)
    for (j=0;j<(*Z).cols;j++)
      (*Z).array[i][j]=-1e20;
}


void       lb_gr_create_zbuffer(PICTURE_T *Pic, MATRIX_R_T *Z)
{
  
  if (!lb_gr_assert_dimensions_picture(Pic))
    {
      printf("Error: lb_gr_create_zbuffer() --> invalid dimension\r\n");
      exit(EXIT_FAILURE);
    }	
  
  if (Pic==NULL)
    {
      (*Z).rows=ty_screen.h;
      (*Z).cols=ty_screen.w;
    }
  else
    {
      (*Z).rows=(*Pic).h;
      (*Z).cols=(*Pic).w;
    }
  lb_al_create_matrix_r(Z);
  lb_gr_reset_zbuffer(Z);
}

void       lb_gr_plot_zbuffer_line_1(PICTURE_T *Pic, VIEWPORT_3D_T vp3d, FLOAT_T Rot[3][3], MATRIX_R_T *Z,
				     POINT_3D_FLOAT_T P0, POINT_3D_FLOAT_T P1, PIXEL_T color, COPYMODE_T copymode)
{
  FLOAT_T P0_x, P0_y, P0_hz, P1_x, P1_y, P1_hz, hz, m_hz;
  FLOAT_T _x0, _y0, _hz0, _x1, _y1, _hz1, delta, m, xr, yr;
  S_INT_16_T k;
  S_INT_8_T flag_octant_1;

  lb_gr_project_3d(vp3d, Rot, P0, &P0_x, &P0_y, &P0_hz);
  lb_gr_project_3d(vp3d, Rot, P1, &P1_x, &P1_y, &P1_hz);

  if (lb_re_equal(P1_x,P0_x) && lb_re_equal(P1_y,P0_y))
    {
      lb_gr_draw_pixel(Pic, P0_x, P0_y, color, copymode);
      return;
    }

  /* The following section is just the standard floating-point version of the line-drawing routine */ 
  /* If slope m<=1.0, we treat y as a function of x, otherwise x is a function of y */

  if(fabs(P1_y-P0_y)<=fabs(P1_x-P0_x))
    {
      flag_octant_1=TRUE;
      if(P1_x>P0_x)
	{
	  _x1=P1_x;
	  _y1=P1_y;
	  _hz1=P1_hz;
	  _x0=P0_x;
	  _y0=P0_y;
	  _hz0=P0_hz;
	}
      else
	{
	  _x1=P0_x;
	  _y1=P0_y;
	  _hz1=P0_hz;
	  _x0=P1_x;
	  _y0=P1_y;
	  _hz0=P1_hz;
	}
      m=(_y1-_y0)/(_x1-_x0);
      delta=_x1-_x0;
    }			 
  else
    {
      flag_octant_1=FALSE;
      if(P1_y>P0_y)
	{
	  _y1=P1_y;
	  _x1=P1_x;
	  _hz1=P1_hz;
	  _x0=P0_x;
	  _y0=P0_y;
	  _hz0=P0_hz;
      	}
      else
      	{
	  _y1=P0_y;
	  _x1=P0_x;
	  _hz1=P0_hz;
	  
	  _y0=P1_y;
	  _x0=P1_x;
	  _hz0=P1_hz;
	}
      m=(_x1-_x0)/(_y1-_y0);
      delta=_y1-_y0;
    }
  m_hz=(_hz1-_hz0)/delta;
  lb_ft_printf(ty_C, "delta=%f\r\n",delta);
  for (k=0;k<delta;k++)
    {
      if (flag_octant_1)
	{
	  xr=_x0+k;
	  yr=_y0+m*k;
	}
      else
	{
	  xr=_x0+m*k;
	  yr=_y0+k;
	}
      hz=_hz0+m_hz*k;


      /* If a NULL pointer is passed, we "read" the framebuffer's color from a shadow register */
      if (Pic==NULL)
	{
	  //	  if ( (xr<0) || (xr>=ty_pic_shadow.w) || (yr<0) || (yr>=ty_pic_shadow.h) )
	    return;
	}
      else
	{
	  if ( (xr<0) || (xr>=(*Pic).w) || (yr<0) || (yr>=(*Pic).h) )
	    return;
	}

      if (hz>(*Z).array[(S_INT_16_T)yr][(S_INT_16_T)xr])
	{
	  (*Z).array[(S_INT_16_T)yr][(S_INT_16_T)xr]=hz;
	  lb_gr_draw_pixel(Pic, xr, yr, color, copymode);
	}
    }
}

void       lb_gr_plot_zbuffer_dot(PICTURE_T *Pic, VIEWPORT_3D_T vp3d, FLOAT_T Rot[3][3], MATRIX_R_T *Z,
				  POINT_3D_FLOAT_T P, PIXEL_T color, COPYMODE_T copymode)
{
  FLOAT_T P_x, P_y, hz;

  lb_gr_project_3d(vp3d, Rot, P, &P_x, &P_y, &hz);
  if (hz>(*Z).array[(S_INT_16_T)P_y][(S_INT_16_T)P_x])
    {
      (*Z).array[(S_INT_16_T)P_y][(S_INT_16_T)P_x]=hz;
      lb_gr_draw_pixel(Pic, P_x, P_y, color, copymode);
    }
}


void       lb_gr_plot_zbuffer_triangle(PICTURE_T *Pic, VIEWPORT_3D_T vp3d, FLOAT_T Rot[3][3], MATRIX_R_T *Z,
				       POINT_3D_FLOAT_T PA, POINT_3D_FLOAT_T PB, POINT_3D_FLOAT_T PC,
				       PIXEL_T color, COPYMODE_T copymode)
{
  FLOAT_T VAB[3], VAC[3], Vperp[3];
  FLOAT_T PA_x, PA_y, PA_hz, PB_x, PB_y, PB_hz, PC_x, PC_y, PC_hz;
  POINT_2D_INT_T A, B, C, S, E, P_temp;
  S_INT_16_T i, x_start, x_end;
  S_INT_16_T ab_dx, ab_dy, ab_err, ab_e2, ab_sx, ab_sy;
  S_INT_16_T ac_dx, ac_dy, ac_err, ac_e2, ac_sx, ac_sy;
  S_INT_16_T bc_dx, bc_dy, bc_err, bc_e2, bc_sx, bc_sy;

  /* First, check if the points are collinear.  If they do, do not draw as a triangle. */ 

  VAB[0]=PB.x-PA.x;
  VAB[1]=PB.y-PA.y;
  VAB[2]=PB.z-PA.z;

  VAC[0]=PC.x-PA.x;
  VAC[1]=PC.y-PA.y;
  VAC[2]=PC.z-PA.z;

  lb_al_cross_product_vector3_r(VAB,VAC,Vperp);
  if (lb_re_equal(Vperp[0]*Vperp[0] + Vperp[1]*Vperp[1] + Vperp[2]*Vperp[2],0.0))
    {
      lb_ft_printf(ty_C, "Warning: lb_gr_plot_zbuffer_triangle() --> 3d collinear points\r\n");
      return;
    }
  
  lb_gr_project_3d(vp3d, Rot, PA, &PA_x, &PA_y, &PA_hz);
  lb_gr_project_3d(vp3d, Rot, PB, &PB_x, &PB_y, &PB_hz);
  lb_gr_project_3d(vp3d, Rot, PC, &PC_x, &PC_y, &PC_hz);

  A.x=PA_x;
  A.y=PA_y;

  B.x=PB_x;
  B.y=PB_y;

  C.x=PC_x;
  C.y=PC_y;

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

  
  S=A;
  E=A;

  /* Include first vertice.  The following pixels depend on changes in the 'y' direction */

  lb_gr_plot_zbuffer_pixel(Pic, Z, A.x, A.y,
			   PA_x, PA_y, PB_x, PB_y, PC_x, PC_y,
			   PA_hz, PB_hz, PC_hz,
			   color, copymode);
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
		    lb_gr_plot_zbuffer_pixel(Pic, Z, E.x, E.y,
					     PA_x, PA_y, PB_x, PB_y, PC_x, PC_y,
					     PA_hz, PB_hz, PC_hz,
					     color, copymode);

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
		  i=x_start+1;
		  while (i<x_end)
		    {
		      lb_gr_plot_zbuffer_pixel(Pic, Z, i, E.y,
					       PA_x, PA_y, PB_x, PB_y, PC_x, PC_y,
					       PA_hz, PB_hz, PC_hz,
					       color, copymode);
		      i++;
		    }
		}
	    } while (E.y!=S.y);
	  /***************************/
	}
      lb_gr_plot_zbuffer_pixel(Pic, Z, S.x, S.y,
			       PA_x, PA_y, PB_x, PB_y, PC_x, PC_y,
			       PA_hz, PB_hz, PC_hz,
			       color, copymode);

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
		    lb_gr_plot_zbuffer_pixel(Pic, Z, E.x, E.y,
					     PA_x, PA_y, PB_x, PB_y, PC_x, PC_y,
					     PA_hz, PB_hz, PC_hz,
					     color, copymode);

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
		  i=x_start+1;
		  while (i<x_end)
		    {
		      lb_gr_plot_zbuffer_pixel(Pic, Z, i, E.y,
					       PA_x, PA_y, PB_x, PB_y, PC_x, PC_y,
					       PA_hz, PB_hz, PC_hz,
					       color, copymode);
		      i++;
		    }
		}
	    } while (E.y!=S.y);
	  /***************************/
	}
      lb_gr_plot_zbuffer_pixel(Pic, Z, S.x, S.y,
			       PA_x, PA_y, PB_x, PB_y, PC_x, PC_y,
			       PA_hz, PB_hz, PC_hz,
			       color, copymode);

      //lb_gr_delay(500);
    } while ((S.x!=C.x) || (S.y!=C.y));
}


void       lb_gr_plot_zbuffer_pixel(PICTURE_T *Pic,  MATRIX_R_T *Z, FLOAT_T xp, FLOAT_T yp,
				    FLOAT_T PA_x, FLOAT_T PA_y, FLOAT_T PB_x, FLOAT_T PB_y, FLOAT_T PC_x, FLOAT_T PC_y,
				    FLOAT_T PA_hz, FLOAT_T PB_hz, FLOAT_T PC_hz,
				    PIXEL_T color, COPYMODE_T copymode)
{
  FLOAT_T M[2][2], det_dividend,  det_divisor, pp0, pp1, hz;

  if ( (xp<0) || (yp<0) || (xp>(*Z).cols) || (yp>(*Z).rows) )
    return;
  
  M[0][0]=PB_x-PA_x;   M[0][1]=PC_x-PA_x;
  M[1][0]=PB_y-PA_y;   M[1][1]=PC_y-PA_y;
  det_divisor=lb_al_determinant_matrix22_r(M);
  //det_divisor=0;

  if (!lb_re_equal(det_divisor,0.0))
    {
      M[0][0]=xp-PA_x;   // M[0][1]=PCx-PAx;
      M[1][0]=yp-PA_y;   // M[1][1]=PCy-PAy;
      det_dividend=lb_al_determinant_matrix22_r(M);
      pp0=det_dividend/det_divisor;

      M[0][0]=PB_x-PA_x;   M[0][1]=xp-PA_x;
      M[1][0]=PB_y-PA_y;   M[1][1]=yp-PA_y;
      det_dividend=lb_al_determinant_matrix22_r(M);
      pp1=det_dividend/det_divisor;
      //lb_ft_printf(ty_C, "pp0 = %f,  pp1=%f\r\n", pp0, pp1);

      //if (pp0<0.0) pp0=0.0;
      //if (pp0>1.0) pp0=1.0;

      // if (pp1<0.0) pp1=0.0;
      // if (pp1>1.0) pp1=1.0;
      
      hz=PA_hz+pp0*(PB_hz-PA_hz)+pp1*(PC_hz-PA_hz);
      /* Compare hz with the current value in the Z-buffer */
      if ( (xp<0) || (xp>=(*Z).cols) || (yp<0) || (yp>=(*Z).rows) )
	{
	  printf("Error: lb_gr_plot_zbuffer_pixel() --> out of boundaries\r\n");
	  printf("       Columns: %d  xp=%f\r\n",(*Z).cols,xp);
	  printf("       Rows:    %d  yp=%f\r\n",(*Z).cols,yp);
	  exit(EXIT_FAILURE);
	}
      
      if (hz>(*Z).array[(S_INT_16_T)yp][(S_INT_16_T)xp])
	{
	  (*Z).array[(S_INT_16_T)yp][(S_INT_16_T)xp]=hz;
	  lb_gr_draw_pixel(Pic, xp, yp, color, copymode);
	}   
    }
  else  /* All 3 points are part of the same line */
    {
      lb_ft_printf(ty_C, "Collinear projected triangle.  det_divisor=%f, equal=%d\r\n",det_divisor*1e6,lb_re_equal(det_divisor,0.0)); 
      /* We first determine if the line is in the first or second octant */
      FLOAT_T max_delta_x, max_delta_y, temp, hz_AB, hz_AC, hz_BC;

      hz_AB=-1e20;
      hz_AC=-1e20;
      hz_BC=-1e20;
      
      max_delta_x = fabs(PA_x-PB_x);
      temp = fabs(PA_x-PC_x);
      if (temp>max_delta_x) max_delta_x=temp;

      max_delta_y = fabs(PA_y-PB_y);
      temp = fabs(PA_y-PC_y);
      if (temp>max_delta_y) max_delta_y=temp;

      
      /* First octant */
      if (max_delta_x>=max_delta_y) 
	{
	  /* See if xp lies in between A and B and interpolate height */ 	  
	  if (!lb_re_equal(PB_x,PA_x))
	    {
	      pp0=(xp-PA_x)/(PB_x-PA_x);
	      if ( (pp0>=0) && (pp0<=1.0) )
		hz_AB = PA_hz + pp0*(PB_hz-PA_hz);
	    }
	  else
	    hz_AB = PA_hz; 
	  lb_ft_printf(ty_C, "hz_AB=%f\r\n",hz_AB);
	   
	  /* See if xp lies in between A and C and interpolate height */ 	  
	  if (!lb_re_equal(PC_x,PA_x))
	    {
	      pp0=(xp-PA_x)/(PC_x-PA_x);
	      if ( (pp0>=0) && (pp0<=1.0) )
		hz_AC = PA_hz + pp0*(PC_hz-PA_hz);
	    }
	  else
	    hz_AC = PA_hz; 
	  lb_ft_printf(ty_C, "hz_AC=%f\r\n",hz_AC);
	   
	  /* See if xp lies in between B and C and interpolate height */ 	  
	  if (!lb_re_equal(PC_x,PB_x))
	    {
	      pp0=(xp-PB_x)/(PC_x-PB_x);
	      if ( (pp0>=0) && (pp0<=1.0) )
		hz_BC = PB_hz + pp0*(PC_hz-PB_hz);
	    }
	  else
	    hz_BC = PB_hz;
	  lb_ft_printf(ty_C, "hz_BC=%f\r\n",hz_BC);
	   
	}
      else   /* Second octant */
    	{
	  if (!lb_re_equal(PB_y,PA_y))
	    {
	      pp0=(yp-PA_y)/(PB_y-PA_y);
	      if ( (pp0>=0) && (pp0<=1.0) )
		hz_AB = PA_hz + pp0*(PB_hz-PA_hz);
	    }
	  else
	    hz_AB = PA_hz; 
	  lb_ft_printf(ty_C, "hz_AB=%f\r\n",hz_AB);

	  /* See if xp lies in between A and C and interpolate height */ 	  
	  if (!lb_re_equal(PC_y,PA_y))
	    {
	      pp0=(yp-PA_y)/(PC_y-PA_y);
	      if ( (pp0>=0) && (pp0<=1.0) )
		hz_AC = PA_hz + pp0*(PC_hz-PA_hz);
	    }
	  else
	    hz_AC = PA_hz; 
	  lb_ft_printf(ty_C, "hz_AC=%f\r\n",hz_AC);

	  /* See if xp lies in between B and C and interpolate height */ 	  
	  if (!lb_re_equal(PC_y,PB_y))
	    {
	      pp0=(yp-PB_y)/(PC_y-PB_y);
	      if ( (pp0>=0) && (pp0<=1.0) )
		hz_BC = PB_hz + pp0*(PC_hz-PB_hz);
	    }
	  else
	    hz_BC = PB_hz; 
	}
      if (hz_AB>=hz_AC)
	hz=hz_AB;
      else
	hz=hz_AC;
      if (hz_BC>hz)
	hz=hz_BC;
      lb_ft_printf(ty_C, "hz_BC=%f\r\n",hz_BC);

      lb_ft_printf(ty_C, "hz=%f\r\n",hz);
      if (hz>(*Z).array[(S_INT_16_T)yp][(S_INT_16_T)xp])
	{
	  (*Z).array[(S_INT_16_T)yp][(S_INT_16_T)xp]=hz;
	  lb_gr_draw_pixel(Pic, xp, yp, color, copymode);
	}
    }
}



void lb_gr_draw_circle(PICTURE_T *Pic, S_INT_16_T xc, S_INT_16_T yc, S_INT_16_T radius, PIXEL_T color, COPYMODE_T copymode) 
{
  S_INT_16_T x = radius;
  S_INT_16_T y = 0;
  S_INT_16_T decisionOver2 = 1-x;

  while (x >= y)
    {
      lb_gr_draw_pixel(Pic, xc+x, yc-y, color, copymode); /* Oct 1 */
      lb_gr_draw_pixel(Pic, xc+y, yc-x, color, copymode); /* Oct 2 */
      
      if (x != 0)
        {
	  lb_gr_draw_pixel(Pic, xc-x, yc+y, color, copymode); /* Oct 5 */
	  lb_gr_draw_pixel(Pic, xc+y, yc+x, color, copymode); /* Oct 7 */
     	}
      
      if (y != 0)
        {
	  lb_gr_draw_pixel(Pic, xc-y, yc-x, color, copymode); /* Oct 3 */
	  lb_gr_draw_pixel(Pic, xc+x, yc+y, color, copymode); /* Oct 8 */
	}
      
      if (x != 0 && y != 0)
        {
	  lb_gr_draw_pixel(Pic, xc-x,yc-y, color, copymode); /* Oct 4 */
	  lb_gr_draw_pixel(Pic, xc-y,yc+x, color, copymode); /* Oct 6 */
  	}
      y++;
      if (decisionOver2<=0)
	{
	  decisionOver2+= 2*y+1;
	}
      else
	{
	  x--;
	  decisionOver2+=2*(y-x)+1;
        }
    }
}

void lb_gr_draw_circle_antialiasing(PICTURE_T *Pic, FLOAT_T xc, FLOAT_T yc, FLOAT_T radius, FLOAT_T w, PIXEL_T color, COPYMODE_T copymode)
{
  S_INT_16_T x, y, x_min, x_max, x_min_bk, y_min, y_max;
  FLOAT_T  distance, w2;
  ERR_T error;
  PIXEL_T pix_main;

  w2=0.5*w;
  x=round(radius);
  y=0;
  
  pix_main=color;
  
  y_min=0;
  y_max=ceil(radius+w2);

  for(y=y_min;y<=y_max;y++)
    {
      x_min=floor(lb_re_sqrt((radius-w2)*(radius-w2)-1.0*y*y, &error));
      x_min_bk=x_min;
      if (y>x_min)
	x_min=y;
      x_max=ceil(lb_re_sqrt((radius+w2)*(radius+w2)-1.0*y*y, &error));
      for(x=x_min;x<=x_max;x++)
	{
	  if( ((x_min_bk+1)<x) && (x<(x_max-1))) /* For circles with a larger w, this speeds up the calculation */
	    pix_main.a=MAX_A;
	  else
	    {
	      distance=fabs(lb_re_sqrt(x*x+y*y,&error)-radius);
	      if(distance<w2)
		pix_main.a=MAX_A;
	      else if(w2-distance<=1.0)
		pix_main.a=round(MAX_A*(1.0-fabs(w2-distance))); 
	    }
	 
	  lb_gr_draw_pixel(Pic, xc+x, yc-y, pix_main, copymode); /* Oct 1 */
	  if (x!=y)
	    lb_gr_draw_pixel(Pic, xc+y, yc-x, pix_main, copymode); /* Oct 2 */

	  if (x != 0)
	    {
	      lb_gr_draw_pixel(Pic, xc-x, yc+y, pix_main, copymode); /* Oct 5 */
	      lb_gr_draw_pixel(Pic, xc+y, yc+x, pix_main, copymode); /* Oct 7 */
	    }
	  
	  if ((y != 0) && (x!=y))
	    {
	      lb_gr_draw_pixel(Pic, xc-y, yc-x, pix_main, copymode); /* Oct 3 */
	      lb_gr_draw_pixel(Pic, xc+x, yc+y, pix_main, copymode); /* Oct 8 */
	    }
      
	  if ((x!=0) && (y!=0))
	    {
	      lb_gr_draw_pixel(Pic, xc-x,yc-y, pix_main, copymode); /* Oct 4 */
	      if (x!=y)
		lb_gr_draw_pixel(Pic, xc-y,yc+x, pix_main, copymode); /* Oct 6 */
	    }
	    
	  // lb_gr_picture_render(Pic, renderer, 0, 0, 8, RENDERMODE_BOX);
	}
    }
}

void lb_gr_draw_circle_antialiasing2(PICTURE_T *Pic, S_INT_16_T xc, S_INT_16_T yc, S_INT_16_T radius, PIXEL_T color, COPYMODE_T copymode) 
{
  S_INT_16_T x, y, sign, decisionOver2;
  FLOAT_T distance, dmax;
  ERR_T error;
  PIXEL_T pix_main, pix_adj;
  
  x=radius;
  y=0;
  decisionOver2 = 1-x;

  pix_main=color;
  pix_main.a=MAX_A;
  pix_adj=color;
  dmax=0.0;
  while (x >= y)
    {
      distance=(FLOAT_T)radius - lb_re_sqrt(x*x+y*y,&error);

      if (distance>=0.0)
	sign=1;
      else
	sign=-1;

      distance=(FLOAT_T)radius - lb_re_sqrt((x+sign)*(x+sign)+y*y,&error);
      if (fabs(distance)>dmax)
	dmax=fabs(distance);
      
      if (fabs(distance)<=1.0)
	pix_adj.a=round(MAX_A*(1.0-fabs(distance)/1.0)); /* try out other values! */ 
      else
	pix_adj.a=0;

      lb_gr_draw_pixel(Pic, xc+x, yc-y, pix_main, copymode); /* Oct 1 */
      lb_gr_draw_pixel(Pic, xc+y, yc-x, pix_main, copymode); /* Oct 2 */

      lb_gr_draw_pixel(Pic, xc+(x+sign), yc-y, pix_adj, copymode); /* Oct 1 */
      lb_gr_draw_pixel(Pic, xc+y, yc-(x+sign), pix_adj, copymode); /* Oct 2 */

      if (x!=0)
        {
	  lb_gr_draw_pixel(Pic, xc-x, yc+y, pix_main, copymode); /* Oct 5 */
	  lb_gr_draw_pixel(Pic, xc+y, yc+x, pix_main, copymode); /* Oct 7 */

	  lb_gr_draw_pixel(Pic, xc-(x+sign), yc+y, pix_adj, copymode); /* Oct 5 */
	  lb_gr_draw_pixel(Pic, xc+y, yc+(x+sign), pix_adj, copymode); /* Oct 7 */
	}
      
      if (y!=0)
        {
	  lb_gr_draw_pixel(Pic, xc-y, yc-x, pix_main, copymode); /* Oct 3 */
	  lb_gr_draw_pixel(Pic, xc+x, yc+y, pix_main, copymode); /* Oct 8 */

	  lb_gr_draw_pixel(Pic, xc-y, yc-(x+sign), pix_adj, copymode); /* Oct 3 */
	  lb_gr_draw_pixel(Pic, xc+(x+sign), yc+y, pix_adj, copymode); /* Oct 8 */
	}
      
      if ((x!= 0) && (y!=0))
        {
	  lb_gr_draw_pixel(Pic, xc-x,yc-y, pix_main, copymode); /* Oct 4 */
	  lb_gr_draw_pixel(Pic, xc-y,yc+x, pix_main, copymode); /* Oct 6 */

	  lb_gr_draw_pixel(Pic, xc-(x+sign),yc-y, pix_adj, copymode); /* Oct 4 */
	  lb_gr_draw_pixel(Pic, xc-y,yc+(x+sign), pix_adj, copymode); /* Oct 6 */
	}
      y++;
      if (decisionOver2<=0)
	{
	  decisionOver2+= 2*y+1;
	}
      else
	{
	  x--;
	  decisionOver2+=2*(y-x)+1;
        }
    }
}

void lb_gr_draw_circle_antialiasing3(PICTURE_T *Pic, S_INT_16_T xc, S_INT_16_T yc, S_INT_16_T radius, PIXEL_T color, COPYMODE_T copymode) 
{
  S_INT_16_T x, y, decisionOver2;
  FLOAT_T distance;
  ERR_T error;
  PIXEL_T pix_main, pix_left, pix_right;
  
  x=radius;
  y=0;
  decisionOver2 = 1-x;

  pix_main=color;
  pix_main.a=MAX_A;
  pix_right=color;
  pix_left=color;
  while (x >= y)
    {
      distance=(FLOAT_T)radius - lb_re_sqrt((x+0.7071)*(x+0.7071)+y*y,&error);

      if (fabs(distance)<1.0)
	pix_right.a=round(MAX_A*(1.0-lb_re_frac(fabs(distance))));
      else
	pix_right.a=0;

      distance=(FLOAT_T)radius - lb_re_sqrt((x-0.7071)*(x-0.7071)+y*y,&error);
      
      if (fabs(distance)<1.0)
	pix_left.a=round(MAX_A*(1.0-lb_re_frac(fabs(distance))));
      //	pix_left.a=15-10.0*lb_re_frac(fabs(distance));
      else
	pix_left.a=0;
      	
      lb_gr_draw_pixel(Pic, xc+x, yc-y, pix_main, copymode); /* Oct 1 */
      lb_gr_draw_pixel(Pic, xc+y, yc-x, pix_main, copymode); /* Oct 2 */

      lb_gr_draw_pixel(Pic, xc+(x+1), yc-y, pix_right, copymode); /* Oct 1 */
      lb_gr_draw_pixel(Pic, xc+y, yc-(x+1), pix_right, copymode); /* Oct 2 */

      lb_gr_draw_pixel(Pic, xc+(x-1), yc-y, pix_left, copymode); /* Oct 1 */
      lb_gr_draw_pixel(Pic, xc+y, yc-(x-1), pix_left, copymode); /* Oct 2 */

      
      if (x != 0)
        {
	  lb_gr_draw_pixel(Pic, xc-x, yc+y, pix_main, copymode); /* Oct 5 */
	  lb_gr_draw_pixel(Pic, xc+y, yc+x, pix_main, copymode); /* Oct 7 */

	  lb_gr_draw_pixel(Pic, xc-(x+1), yc+y, pix_right, copymode); /* Oct 5 */
	  lb_gr_draw_pixel(Pic, xc+y, yc+(x+1), pix_right, copymode); /* Oct 7 */

	  lb_gr_draw_pixel(Pic, xc-(x-1), yc+y, pix_left, copymode); /* Oct 5 */
	  lb_gr_draw_pixel(Pic, xc+y, yc+(x-1), pix_left, copymode); /* Oct 7 */
	}
      
      if (y != 0)
        {
	  lb_gr_draw_pixel(Pic, xc-y, yc-x, pix_main, copymode); /* Oct 3 */
	  lb_gr_draw_pixel(Pic, xc+x, yc+y, pix_main, copymode); /* Oct 8 */

	  lb_gr_draw_pixel(Pic, xc-y, yc-(x+1), pix_right, copymode); /* Oct 3 */
	  lb_gr_draw_pixel(Pic, xc+(x+1), yc+y, pix_right, copymode); /* Oct 8 */

	  lb_gr_draw_pixel(Pic, xc-y, yc-(x-1), pix_left, copymode); /* Oct 3 */
	  lb_gr_draw_pixel(Pic, xc+(x-1), yc+y, pix_left, copymode); /* Oct 8 */
	}
      
      if (x != 0 && y != 0)
        {
	  lb_gr_draw_pixel(Pic, xc-x,yc-y, pix_main, copymode); /* Oct 4 */
	  lb_gr_draw_pixel(Pic, xc-y,yc+x, pix_main, copymode); /* Oct 6 */

	  lb_gr_draw_pixel(Pic, xc-(x+1),yc-y, pix_right, copymode); /* Oct 4 */
	  lb_gr_draw_pixel(Pic, xc-y,yc+(x+1), pix_right, copymode); /* Oct 6 */

	  lb_gr_draw_pixel(Pic, xc-(x-1),yc-y, pix_left, copymode); /* Oct 4 */
	  lb_gr_draw_pixel(Pic, xc-y,yc+(x-1), pix_left, copymode); /* Oct 6 */
	}
      y++;
      if (decisionOver2<=0)
	{
	  decisionOver2+= 2*y+1;
	}
      else
	{
	  x--;
	  decisionOver2+=2*(y-x)+1;
        }
    }
}

void lb_gr_draw_circle_antialiasing_simple(PICTURE_T *Pic, FLOAT_T xc, FLOAT_T yc, FLOAT_T radius, FLOAT_T w, PIXEL_T color, COPYMODE_T copymode)
{
  S_INT_16_T x, y, x_min, x_max, y_min, y_max;
  FLOAT_T  distance, w2;
  ERR_T error;
  PIXEL_T pix_main;

  w2=0.5*w;
  pix_main=color;

  x_min=floor(-radius-w2);
  x_max=ceil(radius+w2);

  y_min=floor(-radius-w2);
  y_max=ceil(radius+w2);

  /* This is the simple version of the algorithm for academic or refernce purposes */
  for(y=y_min;y<=y_max;y++)
    for(x=x_min;x<=x_max;x++)
      {
	pix_main.a=0;
	distance=fabs(lb_re_sqrt(x*x+y*y,&error)-radius);
	if(distance<w2)
	  pix_main.a=MAX_A;
	else if(fabs(w2-distance)<=1.0)
	  pix_main.a=round(MAX_A*(1.0-fabs(w2-distance))); 
	if (pix_main.a!=0)
	  lb_gr_draw_pixel(Pic, xc+x, yc-y, pix_main, copymode); 
      }
}

void lb_gr_draw_circle_arc(PICTURE_T *Pic, S_INT_16_T xc, S_INT_16_T yc, S_INT_16_T radius, FLOAT_T a1, FLOAT_T a2, PIXEL_T color, COPYMODE_T copymode) 
{
  S_INT_16_T x = radius;
  S_INT_16_T y = 0;
  S_INT_16_T decisionOver2 = 1-x;
  S_INT_16_T oct_x_min[8], oct_x_max[8], oct_y_min[8], oct_y_max[8];
  S_INT_16_T i, octant_a1, octant_a2;
  FLOAT_T main_angle;

  if (a1>=a2) return;

  octant_a1=(int)(4.0*a1/M_PI);
  octant_a2=(int)(4.0*a2/M_PI);

  for(i=0;i<8;i++)
    {
      if ((octant_a1<=i) && (i<=octant_a2)) /* This will allow dots */
	{
	  oct_x_min[i]=0;
	  oct_x_max[i]=radius;
	  oct_y_min[i]=0;
	  oct_y_max[i]=radius;
	}
      else /* This will void dots */
	{
	  oct_x_min[i]=radius;
	  oct_x_max[i]=0;
	  oct_y_min[i]=radius;
	  oct_y_max[i]=0;
	}

      if (i==octant_a1)
	{
	  main_angle=0.25*M_PI*(4*a1/M_PI-trunc(4*a1/M_PI));
	  if (!(i % 2)) /* Odd octants */
	    {
	      oct_x_max[octant_a1]=round(radius*cos(main_angle));
	      oct_y_min[octant_a1]=round(radius*sin(main_angle));
	    }
	  else /* Even octants */
	    {
	      oct_x_min[octant_a1]=round(radius*cos(0.25*M_PI - main_angle));
	      oct_y_max[octant_a1]=round(radius*sin(0.25*M_PI - main_angle));
	    }
	}

      if (i==octant_a2)
	{
	  main_angle=0.25*M_PI*(4*a2/M_PI-trunc(4*a2/M_PI));
	  if (!(i % 2)) /* Odd octants */
	    {
	      oct_x_min[octant_a2]=round(radius*cos(main_angle));
	      oct_y_max[octant_a2]=round(radius*sin(main_angle));
	    }
	  else /* Even octants */
	    {
	      oct_x_max[octant_a2]=round(radius*cos(0.25*M_PI - main_angle));
	      oct_y_min[octant_a2]=round(radius*sin(0.25*M_PI - main_angle));
	    }
	}
    }
  while (x >= y)
    {
      if ((oct_x_min[0]<=x) && (x<=oct_x_max[0]) && (oct_y_min[0]<=y) && (y<=oct_y_max[0]))
	lb_gr_draw_pixel(Pic, xc + x, yc - y, color, copymode); /* Oct 1 */ 
      if ((oct_x_min[1]<=x) && (x<=oct_x_max[1]) && (oct_y_min[1]<=y) && (y<=oct_y_max[1]))
	lb_gr_draw_pixel(Pic, xc + y, yc - x, color, copymode); /* Oct. 2 */
      if (x != 0)
        {
	  if ((oct_x_min[4]<=x) && (x<=oct_x_max[4]) && (oct_y_min[4]<=y) && (y<=oct_y_max[4])) 
	    lb_gr_draw_pixel(Pic, xc - x, yc + y, color, copymode); /* Oct. 5 */ 

	  if ((oct_x_min[6]<=x) && (x<=oct_x_max[6]) && (oct_y_min[6]<=y) && (y<=oct_y_max[6])) 
	    lb_gr_draw_pixel(Pic, xc + y, yc + x, color, copymode); /* Oct. 7 */ 
     	}
      if (y != 0)
        {
	  if ((oct_x_min[2]<=x) && (x<=oct_x_max[2]) && (oct_y_min[2]<=y) && (y<=oct_y_max[2])) 
	    lb_gr_draw_pixel(Pic, xc - y, yc - x  , color, copymode); /* Oct.3 */ 

	  if ((oct_x_min[7]<=x) && (x<=oct_x_max[7]) && (oct_y_min[7]<=y) && (y<=oct_y_max[7])) 
	    lb_gr_draw_pixel(Pic, xc + x, yc + y , color, copymode); /* Octant 8 */ 
	}
      if (x != 0 && y != 0)
        {
	  if ((oct_x_min[3]<=x) && (x<=oct_x_max[3]) && (oct_y_min[3]<=y) && (y<=oct_y_max[3])) 
	    lb_gr_draw_pixel(Pic, xc - x, yc - y , color, copymode); /* Oct. 4 */

	  if ((oct_x_min[5]<=x) && (x<=oct_x_max[5]) && (oct_y_min[5]<=y) && (y<=oct_y_max[5])) 
	    lb_gr_draw_pixel(Pic, xc - y, yc + x , color, copymode); /* Oct. 6 */ 
  	}
      y++;
      if (decisionOver2<=0)
	{
	  decisionOver2+= 2*y+1;
	}
      else
	{
	  x--;
	  decisionOver2+=2*(y-x)+1;
        }
    }
}

void lb_gr_draw_circle_filled_antialiasing(PICTURE_T *Pic, S_INT_16_T xc, S_INT_16_T yc, S_INT_16_T radius, PIXEL_T color, COPYMODE_T copymode) 
{
  S_INT_16_T x = radius;
  S_INT_16_T y = 0;
  S_INT_16_T limit=radius*0.70710678118654752440084436210485;
  ERR_T error;
  FLOAT_T distance;
  PIXEL_T pixel;

  pixel=color;
  for(y=yc-limit; y<=yc+limit; y++)
    for(x=xc-limit; x<=xc+limit; x++)
      {
	pixel.a=MAX_A;
	lb_gr_draw_pixel(Pic, x, y , pixel, copymode);  
      }
  for(x=0; x<=limit; x++)
    for(y=limit+1; y<=radius; y++)
      {
	if(x*x+y*y < radius*radius)
	  pixel.a=MAX_A;
	else
	  {
	    pixel.a=0;
	    distance=fabs(lb_re_sqrt(x*x+y*y,&error)-radius);
	    if(distance<=1.0)
	      pixel.a=round(MAX_A*(1.0-distance)); 
	  }
	lb_gr_draw_pixel(Pic, xc + x, yc + y , pixel, copymode);  
	lb_gr_draw_pixel(Pic, xc + y, yc + x , pixel, copymode);  
	lb_gr_draw_pixel(Pic, xc + x, yc - y , pixel, copymode);  
	lb_gr_draw_pixel(Pic, xc - y, yc + x , pixel, copymode);  
	if (x!=0)
	  {
	    lb_gr_draw_pixel(Pic, xc - x, yc + y , pixel, copymode);  
	    lb_gr_draw_pixel(Pic, xc + y, yc - x , pixel, copymode);  
	    lb_gr_draw_pixel(Pic, xc - x, yc - y , pixel, copymode);  
	    lb_gr_draw_pixel(Pic, xc - y, yc - x , pixel, copymode);  
	  }
      }
}

void lb_gr_draw_circle_filled_antialiasing_f(PICTURE_T *Pic, FLOAT_T xc, FLOAT_T yc, FLOAT_T radius, PIXEL_T color, COPYMODE_T copymode) 
{
  FLOAT_T x = radius;
  FLOAT_T y = 0;
  FLOAT_T limit=radius*0.70710678118654752440084436210485;
  ERR_T error;
  FLOAT_T distance;
  PIXEL_T pixel;

  pixel=color;
  for(y=yc-limit; y<=yc+limit; y++)
    for(x=xc-limit; x<=xc+limit; x++)
      lb_gr_draw_pixel(Pic, round(x), round(y) , color, copymode);  

  for(x=0; x<=limit; x++)
    for(y=limit; y<=radius; y++)
      {
	if(x*x+y*y <= radius*radius)
	  pixel.a=MAX_A;
	else
	  {
	    pixel.a=0;
	    distance=fabs(lb_re_sqrt(x*x+y*y,&error)-radius);
	    if(distance<=1.0)
	      pixel.a=round(MAX_A*(1.0-distance)); 
	  }
	
	lb_gr_draw_pixel(Pic, round(xc + x), round(yc + y), pixel, copymode);  
	lb_gr_draw_pixel(Pic, round(xc + y), round(yc + x), pixel, copymode);  
	lb_gr_draw_pixel(Pic, round(xc + x), round(yc - y), pixel, copymode);  
	lb_gr_draw_pixel(Pic, round(xc - y), round(yc + x), pixel, copymode);  
	if (x!=0)
	  {
	    lb_gr_draw_pixel(Pic, round(xc - x), round(yc + y), pixel, copymode);  
	    lb_gr_draw_pixel(Pic, round(xc + y), round(yc - x), pixel, copymode);  
	    lb_gr_draw_pixel(Pic, round(xc - x), round(yc - y), pixel, copymode);  
	    lb_gr_draw_pixel(Pic, round(xc - y), round(yc - x), pixel, copymode);  
	  }
      }
}

void lb_gr_draw_circle_filled_fast(PICTURE_T *Pic, S_INT_16_T xc, S_INT_16_T yc, S_INT_16_T radius, PIXEL_T color, COPYMODE_T copymode) 
{
  S_INT_16_T x = radius;
  S_INT_16_T y = 0;
  S_INT_16_T limit=radius*0.70710678118654752440084436210485;
  
  for(x=0; x<=limit; x++)
    for(y=limit+1; y<=radius; y++)
      if(x*x+y*y <= radius*radius)
	{
	  lb_gr_draw_pixel(Pic, xc + x, yc + y , color, copymode);  
	  lb_gr_draw_pixel(Pic, xc + y, yc + x , color, copymode);  
	  lb_gr_draw_pixel(Pic, xc + x, yc - y , color, copymode);  
	  lb_gr_draw_pixel(Pic, xc - y, yc + x , color, copymode);  
	  if (x!=0)
	    {
	      lb_gr_draw_pixel(Pic, xc - x, yc + y , color, copymode);  
	      lb_gr_draw_pixel(Pic, xc + y, yc - x , color, copymode);  
	      lb_gr_draw_pixel(Pic, xc - x, yc - y , color, copymode);  
	      lb_gr_draw_pixel(Pic, xc - y, yc - x , color, copymode);  
	    }
	}
  for(y=yc-limit; y<=yc+limit; y++)
    for(x=xc-limit; x<=xc+limit; x++)
      lb_gr_draw_pixel(Pic, x, y , color, copymode);  
}

void lb_gr_draw_circle_filled_slow(PICTURE_T *Pic, S_INT_16_T xc, S_INT_16_T yc, S_INT_16_T radius, PIXEL_T color, COPYMODE_T copymode) 
{
  S_INT_16_T x = radius;
  S_INT_16_T y = 0;
  
  for(y=-radius; y<=radius; y++)
    for(x=-radius; x<=radius; x++)
      if(x*x+y*y <= radius*radius)
	lb_gr_draw_pixel(Pic, xc+x, yc+y, color, copymode); 
}

void lb_gr_draw_ellipse(PICTURE_T *Pic, S_INT_32_T xc, S_INT_32_T yc, S_INT_32_T a, S_INT_32_T b, PIXEL_T color, COPYMODE_T copymode)
{
  S_INT_32_T  x, y, x_change, y_change, ellipse_error, two_a_square, two_b_square, stop_x, stop_y;

  if ((a==0) || (b==0))
    {
      printf("Error: lb_gr_draw_ellipse() --> an axis is zero\r\n");
      exit(EXIT_FAILURE);
    }
  
  two_a_square=2*a*a;
  two_b_square=2*b*b;
 
  x=a;
  y=0;

  x_change=b*b*(1-2*a);
  y_change=a*a;
  ellipse_error=0;
  stop_x=two_b_square*a;
  stop_y=0;
 
  while (stop_x>=stop_y)
    {
      lb_gr_draw_pixel(Pic, xc + x, yc - y, color, copymode);
      lb_gr_draw_pixel(Pic, xc - x, yc - y, color, copymode);
      lb_gr_draw_pixel(Pic, xc - x, yc + y, color, copymode);
      lb_gr_draw_pixel(Pic, xc + x, yc + y, color, copymode);
      y++;
      stop_y+=two_a_square;
      ellipse_error+=y_change;
      y_change+=two_a_square;
      if ((2*ellipse_error + x_change)>0)
	{
	  x--;
	  stop_x-=two_b_square;
	  ellipse_error+=x_change;
	  x_change+=two_b_square;
	}
    }

  x=0;
  y=b;

  x_change=b*b;
  y_change=a*a*(1-2*b);
  ellipse_error=0;
  stop_x=0;
  stop_y=two_a_square*b;
 
  while (stop_x<=stop_y)
    {
      lb_gr_draw_pixel(Pic, xc + x, yc - y, color, copymode);
      lb_gr_draw_pixel(Pic, xc - x, yc - y, color, copymode);
      lb_gr_draw_pixel(Pic, xc - x, yc + y, color, copymode);
      lb_gr_draw_pixel(Pic, xc + x, yc + y, color, copymode);
      x++;
      stop_x+=two_b_square;
      ellipse_error+=x_change;
      x_change+=two_b_square;
      if ((2*ellipse_error + y_change)>0)
	{
	  y--;
	  stop_y-=two_a_square;
	  ellipse_error+=y_change;
	  y_change+=two_a_square;
	}
    }
}


void lb_gr_draw_ellipse_rotated_antialiasing(PICTURE_T *Pic, FLOAT_T xc, FLOAT_T yc, FLOAT_T a, FLOAT_T b, FLOAT_T angle,
					     FLOAT_T w, S_INT_16_T n_q1, PIXEL_T color, COPYMODE_T copymode, LINEMODE_T linemode)
{
  FLOAT_T tetha, xr, yr, xrr, yrr, cos_val, sin_val;
  LINE_2D_FLOAT_T Pol;
  S_INT_16_T k;
  // FLOAT length;
  // length=lb_ge_arclength_ellipse(a,b);

  Pol.items=4*(n_q1-1)+1;
  //lb_ft_printf(ty_C, "Pol items = %d\r\n",Pol.items);
  lb_gr_create_line2d_f(&Pol);
  
  for(k=0;k<n_q1;k++)
    {
      //tetha=lb_ge_angle_from_length_ellipse_q1(a,b,(FLOAT_T)k*length/(n_q1-1),length);
      tetha=M_PI_2*k/(n_q1-1);
      //lb_ft_printf(ty_C, "tetha=%f\r\n",tetha);
      xr=a*cos(tetha);
      yr=b*sin(tetha);
      cos_val=cos(angle);
      sin_val=sin(angle);

      /* Q1 */
      //lb_ft_printf(ty_C, "Q1 k=%d\r\n",k);
      xrr=xr*cos_val-yr*sin_val;
      yrr=xr*sin_val+yr*cos_val;
      Pol.array[k].x=xc+xrr;
      Pol.array[k].y=yc-yrr;
    
      /* Q2 */
      if (k!=n_q1-1)
	{
	  //lb_ft_printf(ty_C, "Q2 k=%d\r\n",2*(n_q1-1)-k);
	  xrr=-xr*cos_val-yr*sin_val;
	  yrr=-xr*sin_val+yr*cos_val;
	  Pol.array[2*(n_q1-1)-k].x=xc+xrr;
	  Pol.array[2*(n_q1-1)-k].y=yc-yrr;
	}
      
      /* Q3 */
      if (k!=0)
	{
	  //lb_ft_printf(ty_C, "Q3 k=%d\r\n",2*(n_q1-1)+k);
	  xrr=-xr*cos_val+yr*sin_val;
	  yrr=-xr*sin_val-yr*cos_val;
	  Pol.array[2*(n_q1-1)+k].x=xc+xrr;
	  Pol.array[2*(n_q1-1)+k].y=yc-yrr;
	}
      /* Q4 */
      if ((k!=n_q1-1))
	{
	  //lb_ft_printf(ty_C, "Q4 k=%d\r\n",4*(n_q1-1)-k);
	  xrr=xr*cos_val+yr*sin_val;
	  yrr=xr*sin_val-yr*cos_val;
	  Pol.array[4*(n_q1-1)-k].x=xc+xrr;
	  Pol.array[4*(n_q1-1)-k].y=yc-yrr;
	}
    }
  lb_gr_draw_polygon_f(Pic, &Pol, w, color, copymode, linemode);
  lb_gr_release_line2d_f(&Pol);
}





#ifdef NADA
void lb_gr_draw_ellipse_rotated(PICTURE_T *Pic, S_INT_16_T xc, S_INT_16_T yc, S_INT_16_T a, S_INT_16_T b, FLOAT_T angle, PIXEL_T color, COPYMODE_T copymode)
{
  S_INT_32_T  xp, yp, x_change, y_change, ellipse_error, two_a_square, two_b_square, stop_x, stop_y;
  FLOAT_T xrot, yrot;
  ERR_T error;
  
  if ((a==0) || (b==0))
    {
      printf("Error: lb_gr_draw_ellipse_rotated() --> an axis is zero\r\n");
      exit(EXIT_FAILURE);
    }
  
  two_a_square=2*a*a;
  two_b_square=2*b*b;
 
  xp=a;
  yp=0;

  x_change=b*b*(1-2*a);
  y_change=a*a;
  ellipse_error=0;
  stop_x=two_b_square*a;
  stop_y=0;
 
  while (stop_x>=stop_y)
    {
      //lb_re_rotate(lb_re_sqrt(b*b-yp*yp,&error)*(FLOAT_T)a/b,
      //	   lb_re_sqrt(a*a-xp*xp,&error)*(FLOAT_T)b/a, angle,
      //	   &xrot, &yrot);
      lb_re_rotate(xp, -yp, -angle, &xrot, &yrot);
      lb_gr_draw_pixel(Pic, xc + round(xrot), yc + round(yrot), color, copymode);

      lb_re_rotate(-xp, -yp, -angle, &xrot, &yrot);
      lb_gr_draw_pixel(Pic, xc + round(xrot), yc + round(yrot), color, copymode);

      lb_re_rotate(-xp, yp, -angle, &xrot, &yrot);
      lb_gr_draw_pixel(Pic, xc + round(xrot), yc + round(yrot), color, copymode);

      lb_re_rotate(xp, yp, -angle, &xrot, &yrot);
      lb_gr_draw_pixel(Pic, xc + round(xrot), yc + round(yrot), color, copymode);
      yp++;
      stop_y+=two_a_square;
      ellipse_error+=y_change;
      y_change+=two_a_square;
      if ((2*ellipse_error + x_change)>0)
	{
	  xp--;
	  stop_x-=two_b_square;
	  ellipse_error+=x_change;
	  x_change+=two_b_square;
	}
    }

  xp=0;
  yp=b;

  x_change=b*b;
  y_change=a*a*(1-2*b);
  ellipse_error=0;
  stop_x=0;
  stop_y=two_a_square*b;
 
  while (stop_x<=stop_y)
    {
      lb_re_rotate(xp, -yp, -angle, &xrot, &yrot);
      lb_gr_draw_pixel(Pic, xc + round(xrot), yc + round(yrot), color, copymode);

      lb_re_rotate(-xp, -yp, -angle, &xrot, &yrot);
      lb_gr_draw_pixel(Pic, xc + round(xrot), yc + round(yrot), color, copymode);

      lb_re_rotate(-xp, yp, -angle, &xrot, &yrot);
      lb_gr_draw_pixel(Pic, xc + round(xrot), yc + round(yrot), color, copymode);

      lb_re_rotate(xp, yp, -angle, &xrot, &yrot);
      lb_gr_draw_pixel(Pic, xc + round(xrot), yc + round(yrot), color, copymode);
      xp++;
      stop_x+=two_b_square;
      ellipse_error+=x_change;
      x_change+=two_b_square;
      if ((2*ellipse_error + y_change)>0)
	{
	  yp--;
	  stop_y-=two_a_square;
	  ellipse_error+=y_change;
	  y_change+=two_a_square;
	}
    }
}

void lb_gr_draw_ellipse_rotated(PICTURE_T *Pic, S_INT_16_T xc, S_INT_16_T yc, S_INT_16_T a, S_INT_16_T b, FLOAT_T angle, PIXEL_T color, COPYMODE_T copymode)
{
  S_INT_32_T  xp, yp, x_change, y_change, ellipse_error, two_a_square, two_b_square, stop_x, stop_y;
  FLOAT_T xr, yr, xrot, yrot;
  ERR_T error;
  
  if ((a==0) || (b==0))
    {
      printf("Error: lb_gr_draw_ellipse_rotated() --> an axis is zero\r\n");
      exit(EXIT_FAILURE);
    }
  
  two_a_square=2*a*a;
  two_b_square=2*b*b;
 
  xp=a;
  yp=0;

  x_change=b*b*(1-2*a);
  y_change=a*a;
  ellipse_error=0;
  stop_x=two_b_square*a;
  stop_y=0;

  
  while (stop_x>=stop_y)
    {
      xr=lb_re_sqrt(b*b-yp*yp,&error)*(FLOAT_T)a/b;
      yr=lb_re_sqrt(a*a-xr*xr,&error)*(FLOAT_T)b/a;
      lb_ft_printf(ty_C, "xp=%02.2f, xr=%02.2f\r\n",xp, xr);
      //lb_re_rotate(lb_re_sqrt(b*b-yp*yp,&error)*(FLOAT_T)a/b,
      //	   lb_re_sqrt(a*a-xp*xp,&error)*(FLOAT_T)b/a, angle,
      //	   &xrot, &yrot);
      lb_re_rotate(xr, -yr, -angle, &xrot, &yrot);
      lb_gr_draw_pixel(Pic, xc + round(xrot), yc + round(yrot), color, copymode);

      lb_re_rotate(-xr, -yr, -angle, &xrot, &yrot);
      lb_gr_draw_pixel(Pic, xc + round(xrot), yc + round(yrot), color, copymode);

      lb_re_rotate(-xr, yr, -angle, &xrot, &yrot);
      lb_gr_draw_pixel(Pic, xc + round(xrot), yc + round(yrot), color, copymode);

      lb_re_rotate(xr, yr, -angle, &xrot, &yrot);
      lb_gr_draw_pixel(Pic, xc + round(xrot), yc + round(yrot), color, copymode);
      yp++;
      stop_y+=two_a_square;
      ellipse_error+=y_change;
      y_change+=two_a_square;
      if ((2*ellipse_error + x_change)>0)
	{
	  xp--;
	  stop_x-=two_b_square;
	  ellipse_error+=x_change;
	  x_change+=two_b_square;
	}
    }

  xp=0;
  yp=b;

  x_change=b*b;
  y_change=a*a*(1-2*b);
  ellipse_error=0;
  stop_x=0;
  stop_y=two_a_square*b;
 
  while (stop_x<=stop_y)
    {
      xr=lb_re_sqrt(b*b-yp*yp,&error)*(FLOAT_T)a/b;
      yr=lb_re_sqrt(a*a-xr*xr,&error)*(FLOAT_T)b/a;

      lb_re_rotate(xr, -yr, -angle, &xrot, &yrot);
      lb_gr_draw_pixel(Pic, xc + round(xrot), yc + round(yrot), color, copymode);

      lb_re_rotate(-xr, -yr, -angle, &xrot, &yrot);
      lb_gr_draw_pixel(Pic, xc + round(xrot), yc + round(yrot), color, copymode);

      lb_re_rotate(-xr, yr, -angle, &xrot, &yrot);
      lb_gr_draw_pixel(Pic, xc + round(xrot), yc + round(yrot), color, copymode);

      lb_re_rotate(xr, yr, -angle, &xrot, &yrot);
      lb_gr_draw_pixel(Pic, xc + round(xrot), yc + round(yrot), color, copymode);
      xp++;
      stop_x+=two_b_square;
      ellipse_error+=x_change;
      x_change+=two_b_square;
      if ((2*ellipse_error + y_change)>0)
	{
	  yp--;
	  stop_y-=two_a_square;
	  ellipse_error+=y_change;
	  y_change+=two_a_square;
	}
    }
}


#endif

void  lb_gr_draw_ellipse_antialiasing2(PICTURE_T *Pic, S_INT_16_T xc, S_INT_16_T yc, S_INT_16_T a, S_INT_16_T b, PIXEL_T color, COPYMODE_T copymode)
{
  S_INT_32_T  xp, yp, x_change, sign, y_change, ellipse_error, two_a_square, two_b_square, stop_x, stop_y;
  FLOAT_T temp, dist_min, xr, yr;
  PIXEL_T pix_main, pix_adj;
  ERR_T error;
  
  pix_main=color;
  pix_main.a=MAX_A;
  pix_adj=color;
  
  two_a_square=2*a*a;
  two_b_square=2*b*b;
 
  xp=a;
  yp=0;

  x_change=b*b*(1-2*a);
  y_change=a*a;
  ellipse_error=0;
  stop_x=two_b_square*a;
  stop_y=0;

  /* An xp, yp obtained by following the Bressenham algorith is just the closest integer approximation to 
     the actual ellipse.  
     The algorithm iterates xp and yp repeatedly.
     Now, given an integer P(xp,yp), we'll assume yr=yp but will calculate what the actual xr would be.
     Then, we'll compare this xr to xp-1 and xp+1 (the adjacent points to the left and right) and will
     color one of them (or both, depending of the thickness(). */
  
  while (stop_x>=stop_y)
    {
      xr=lb_re_sqrt(b*b-yp*yp,&error)*(FLOAT_T)a/b;

      temp=xr-(FLOAT_T)xp;
      if(temp<0.0)
	sign=-1;
      else
	sign=1;
      dist_min=fabs(temp);
      pix_adj.a=round(MAX_A*(2.0*dist_min)); 
      
      lb_gr_draw_pixel(Pic, xc + xp, yc - yp, pix_main, copymode);
      lb_gr_draw_pixel(Pic, xc - xp, yc - yp, pix_main, copymode);
      lb_gr_draw_pixel(Pic, xc - xp, yc + yp, pix_main, copymode);
      lb_gr_draw_pixel(Pic, xc + xp, yc + yp, pix_main, copymode);

      /* Draw anti-aliased closest pixel per scan row */
      lb_gr_draw_pixel(Pic, xc + (xp+sign), yc - yp, pix_adj, copymode);
      lb_gr_draw_pixel(Pic, xc - (xp+sign), yc - yp, pix_adj, copymode);
      lb_gr_draw_pixel(Pic, xc - (xp+sign), yc + yp, pix_adj, copymode);
      lb_gr_draw_pixel(Pic, xc + (xp+sign), yc + yp, pix_adj, copymode);

      
      yp++;
      stop_y+=two_a_square;
      ellipse_error+=y_change;
      y_change+=two_a_square;
      if ((2*ellipse_error + x_change)>0)
	{
	  xp--;
	  stop_x-=two_b_square;
	  ellipse_error+=x_change;
	  x_change+=two_b_square;
	}
    }

  xp=0;
  yp=b;

  x_change=b*b;
  y_change=a*a*(1-2*b);
  ellipse_error=0;
  stop_x=0;
  stop_y=two_a_square*b;
 
  while (stop_x<=stop_y)
    {
      yr=lb_re_sqrt(a*a-xp*xp,&error)*(FLOAT_T)b/a;

      temp=yr-(FLOAT_T)yp;
      if(temp<0.0)
	sign=-1;
      else
	sign=1;
      dist_min=fabs(temp);
      pix_adj.a=round(MAX_A*(2.0*dist_min)); 

      lb_gr_draw_pixel(Pic, xc + xp, yc - yp, pix_main, copymode);
      lb_gr_draw_pixel(Pic, xc - xp, yc - yp, pix_main, copymode);
      lb_gr_draw_pixel(Pic, xc - xp, yc + yp, pix_main, copymode);
      lb_gr_draw_pixel(Pic, xc + xp, yc + yp, pix_main, copymode);

      lb_gr_draw_pixel(Pic, xc + xp, yc - (yp+sign), pix_adj, copymode);
      lb_gr_draw_pixel(Pic, xc - xp, yc - (yp+sign), pix_adj, copymode);
      lb_gr_draw_pixel(Pic, xc - xp, yc + (yp+sign), pix_adj, copymode);
      lb_gr_draw_pixel(Pic, xc + xp, yc + (yp+sign), pix_adj, copymode);

      /* Draw anti-aliased closest pixel per scan column */

      xp++;
      stop_x+=two_b_square;
      ellipse_error+=x_change;
      x_change+=two_b_square;
      if ((2*ellipse_error + y_change)>0)
	{
	  yp--;
	  stop_y-=two_a_square;
	  ellipse_error+=y_change;
	  y_change+=two_a_square;
	}
    }
}


void  lb_gr_draw_ellipse_antialiasing3(PICTURE_T *Pic, S_INT_16_T xc, S_INT_16_T yc, S_INT_16_T a, S_INT_16_T b, PIXEL_T color, COPYMODE_T copymode)
{
  S_INT_32_T  xp, yp, x_change, y_change, ellipse_error, two_a_square, two_b_square, stop_x, stop_y;
  FLOAT_T temp, xr, yr;
  PIXEL_T pix_main, pix_left, pix_right;
  ERR_T error;
  
  pix_main=color;
  pix_main.a=MAX_A;
  pix_left=color;
  pix_right=color;
  
  two_a_square=2*a*a;
  two_b_square=2*b*b;
 
  xp=a;
  yp=0;

  x_change=b*b*(1-2*a);
  y_change=a*a;
  ellipse_error=0;
  stop_x=two_b_square*a;
  stop_y=0;

  /* An xp, yp obtained by following the Bressenham algorith is just the closest integer approximation to 
     the actual ellipse.  
     The algorithm iterates xp and yp repeatedly.
     Now, given an integer P(xp,yp), we'll assume yr=yp but will calculate what the actual xr would be.
     Then, we'll compare this xr to xp-1 and xp+1 (the adjacent points to the left and right) and will
     color one of them (or both, depending of the thickness(). */
  
  while (stop_x>=stop_y)
    {
      xr=lb_re_sqrt(b*b-yp*yp,&error)*(FLOAT_T)a/b;
      temp=xr-(FLOAT_T)xp;

      pix_right.a=MAX_A*(0.5+temp);
      pix_left.a=MAX_A*(1.0-0.5-temp);
      
      lb_gr_draw_pixel(Pic, xc + xp, yc - yp, pix_main, copymode);
      lb_gr_draw_pixel(Pic, xc - xp, yc - yp, pix_main, copymode);
      lb_gr_draw_pixel(Pic, xc - xp, yc + yp, pix_main, copymode);
      lb_gr_draw_pixel(Pic, xc + xp, yc + yp, pix_main, copymode);

      /* Draw anti-aliased closest pixel per scan row */
      lb_gr_draw_pixel(Pic, xc + (xp-1), yc - yp, pix_left, copymode);
      lb_gr_draw_pixel(Pic, xc - (xp-1), yc - yp, pix_left, copymode);
      lb_gr_draw_pixel(Pic, xc - (xp-1), yc + yp, pix_left, copymode);
      lb_gr_draw_pixel(Pic, xc + (xp-1), yc + yp, pix_left, copymode);

      lb_gr_draw_pixel(Pic, xc + (xp+1), yc - yp, pix_right, copymode);
      lb_gr_draw_pixel(Pic, xc - (xp+1), yc - yp, pix_right, copymode);
      lb_gr_draw_pixel(Pic, xc - (xp+1), yc + yp, pix_right, copymode);
      lb_gr_draw_pixel(Pic, xc + (xp+1), yc + yp, pix_right, copymode);

      yp++;
      stop_y+=two_a_square;
      ellipse_error+=y_change;
      y_change+=two_a_square;
      if ((2*ellipse_error + x_change)>0)
	{
	  xp--;
	  stop_x-=two_b_square;
	  ellipse_error+=x_change;
	  x_change+=two_b_square;
	}
    }

  xp=0;
  yp=b;

  x_change=b*b;
  y_change=a*a*(1-2*b);
  ellipse_error=0;
  stop_x=0;
  stop_y=two_a_square*b;
 
  while (stop_x<=stop_y)
    {
      yr=lb_re_sqrt(a*a-xp*xp,&error)*(FLOAT_T)b/a;
      temp=yr-(FLOAT_T)yp;

      /* Actually,, in this section "left" means "down" and "right" means "up" 
	 We keep the same names because the code is simetrical */
      pix_right.a=MAX_A*(0.5+temp);
      pix_left.a=MAX_A*(1.0-0.5-temp);
       
      lb_gr_draw_pixel(Pic, xc + xp, yc - yp, pix_main, copymode);
      lb_gr_draw_pixel(Pic, xc - xp, yc - yp, pix_main, copymode);
      lb_gr_draw_pixel(Pic, xc - xp, yc + yp, pix_main, copymode);
      lb_gr_draw_pixel(Pic, xc + xp, yc + yp, pix_main, copymode);

      /* Draw anti-aliased closest pixel per scan column */
      lb_gr_draw_pixel(Pic, xc + xp, yc - (yp-1), pix_left, copymode);
      lb_gr_draw_pixel(Pic, xc - xp, yc - (yp-1), pix_left, copymode);
      lb_gr_draw_pixel(Pic, xc - xp, yc + (yp-1), pix_left, copymode);
      lb_gr_draw_pixel(Pic, xc + xp, yc + (yp-1), pix_left, copymode);

      lb_gr_draw_pixel(Pic, xc + xp, yc - (yp+1), pix_right, copymode);
      lb_gr_draw_pixel(Pic, xc - xp, yc - (yp+1), pix_right, copymode);
      lb_gr_draw_pixel(Pic, xc - xp, yc + (yp+1), pix_right, copymode);
      lb_gr_draw_pixel(Pic, xc + xp, yc + (yp+1), pix_right, copymode);

      
      /* Draw anti-aliased closest pixel per scan column */

      xp++;
      stop_x+=two_b_square;
      ellipse_error+=x_change;
      x_change+=two_b_square;
      if ((2*ellipse_error + y_change)>0)
	{
	  yp--;
	  stop_y-=two_a_square;
	  ellipse_error+=y_change;
	  y_change+=two_a_square;
	}
    }
}


void       lb_gr_implicit_2d(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, MATRIX_R_T *M, FLOAT_T w, PIXEL_T color,
			     COPYMODE_T copymode, LINEMODE_T linemode)
{
  S_INT_8_T f_00_10, f_10_11, f_01_11, f_00_01;
  S_INT_16_T i, j;
  FLOAT_T x0, y0, x1, y1, root_00_10, root_10_11, root_01_11, root_00_01;
  FLOAT_T P00_10_x, P00_10_y, P10_11_x, P10_11_y, P01_11_x, P01_11_y, P00_01_x, P00_01_y;
  
  
  for(i=0;i<(*M).rows-1;i++)
    {
      /* Condition A1: A zero x0 exists between  z[j,i] and z[j+1,i] 
	 A2: z[j,i] == z[j+1,i] */

      x0 = vp2d.xr_min;  // This comes from: x0 = vp.xr_min + 0*j*(vp.xr_max-vp.xr_min)/((*M).cols-1);
      f_00_01= lb_nu_find_zero(y0,(*M).array[i][0], y1,(*M).array[i+1][0],   &root_00_01);
      // This comes from: f_00_01= lb_nu_find_zero(y0,(*M).array[i][j], y1,(*M).array[i+1][j],   &root_00_01);
      if (f_00_01==1)  lb_gr_project_2d(vp2d, x0         , root_00_01 , &P00_01_x, &P00_01_y);
	  
      for(j=0;j<(*M).cols-1;j++)
	{
	  x1 = vp2d.xr_min + (j+1)*(vp2d.xr_max-vp2d.xr_min)/((*M).cols-1);
	  y1 = vp2d.yr_min + (i+1)*(vp2d.yr_max-vp2d.yr_min)/((*M).rows-1);
	  y0 = vp2d.yr_min +     i*(vp2d.yr_max-vp2d.yr_min)/((*M).rows-1);
	  
	  f_00_10= lb_nu_find_zero(x0,(*M).array[i]    [j], x1,(*M).array[  i][j+1], &root_00_10);
	  f_01_11= lb_nu_find_zero(x0,(*M).array[i+1]  [j], x1,(*M).array[i+1][j+1], &root_01_11);
	  f_10_11= lb_nu_find_zero(y0,(*M).array[i]  [j+1], y1,(*M).array[i+1][j+1], &root_10_11);

	  /* We draw the lines */
	  if (f_00_10==1)  lb_gr_project_2d(vp2d, root_00_10 , y0         , &P00_10_x, &P00_10_y);
	  if (f_10_11==1)  lb_gr_project_2d(vp2d, x1         , root_10_11 , &P10_11_x, &P10_11_y);
	  if (f_01_11==1)  lb_gr_project_2d(vp2d, root_01_11 , y1         , &P01_11_x, &P01_11_y);
	     
	  /* Case 1 */
	  if ( (f_00_10==1) && (f_00_01==1) )
	    lb_gr_draw_line(Pic, P00_10_x, P00_10_y, P00_01_x, P00_01_y, w, color, copymode, linemode);

	  /* Case 2 */
	  if ( (f_00_10==1) && (f_10_11==1) )
	    lb_gr_draw_line(Pic, P00_10_x, P00_10_y, P10_11_x, P10_11_y, w, color, copymode, linemode);

	  /* Case 3 */
	  if ( (f_10_11==1) && (f_01_11==1) )
	    lb_gr_draw_line(Pic, P10_11_x, P10_11_y, P01_11_x, P01_11_y, w, color, copymode, linemode);

	  /* Case 4 */
	  if ( (f_00_01==1) && (f_01_11==1) )
	    lb_gr_draw_line(Pic, P00_01_x, P00_01_y, P01_11_x, P01_11_y, w, color, copymode, linemode);

	  /* Case 5 */
	  if ( (f_00_01==1) && (f_10_11==1) )
	    lb_gr_draw_line(Pic, P00_01_x, P00_01_y, P10_11_x, P10_11_y, w, color, copymode, linemode);

	  /* Case 6 */
	  if ( (f_00_10==1) && (f_01_11==1) )
	    lb_gr_draw_line(Pic, P00_10_x, P00_10_y, P01_11_x, P01_11_y, w, color, copymode, linemode);
  	
	  /* We reuse an intercept and a flag */
	  x0 = x1;
	  f_00_01 = f_10_11;
	  root_00_01 = root_10_11;
	  P00_01_x = P10_11_x;
	  P00_01_y = P10_11_y; 
	}
    }
}

void lb_gr_draw_line(PICTURE_T *Pic, FLOAT_T x0, FLOAT_T y0, FLOAT_T x1, FLOAT_T y1, FLOAT_T w, PIXEL_T color, COPYMODE_T copymode, LINEMODE_T linemode)
{
  S_INT_16_T w_int;
  w_int=round(w);

  switch(linemode)
    {
    case LINEMODE_SOLID:
      if (w_int==1)
	lb_gr_draw_line1(Pic, round(x0), round(y0), round(x1), round(y1),
			 color, copymode);
      else
	if (w_int==2)
	  lb_gr_draw_line2(Pic, round(x0), round(y0), round(x1), round(y1),
			   color, copymode);
	else
	  if (w_int==3)
	    lb_gr_draw_line3(Pic, round(x0), round(y0), round(x1), round(y1),
			     color, copymode);
	  else
	    {
	      printf("Error: lb_gr_draw_line --> Valid width as pixelated line are [1..3]\r\n");
	      exit(EXIT_FAILURE);
	    }    
      break;
    case LINEMODE_FILTERED:
      /* When using anti-aliasing techniques, the copymode becomes irrelevant and to be COPYMODE_BLEND */
      if (w_int==2)
	lb_gr_draw_line_antialiasing2(Pic, round(x0), round(y0), round(x1), round(y1),
				      color, copymode);
    
      else
	if (w_int==3)
	  lb_gr_draw_line_antialiasing3(Pic, round(x0), round(y0), round(x1), round(y1),
					color, copymode);
	else
	  lb_gr_draw_line_antialiasing(Pic, x0, y0, x1, y1, w, color, copymode);
      break;
    default:
      printf("Error: lb_gr_draw_line --> Invalid LINEMODE\r\n");
      exit(EXIT_FAILURE);
      break;
    } /* switch */
}

void lb_gr_draw_histogram(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, VECTOR_R_T *bins,
			  PIXEL_T color_border, PIXEL_T color_background, PIXEL_T color_bar_border, PIXEL_T color_bar)
{
  S_INT_16_T k;
  FLOAT_T xp1_f, yp1_f, xp2_f, yp2_f;
  S_INT_16_T xp1, yp1, xp2, yp2;

  lb_gr_draw_rectangle_bar(Pic,  vp2d.xp_min, vp2d.yp_min, vp2d.xp_max, vp2d.yp_max, 2,
			   color_border, color_background,  COPYMODE_COPY);
  for(k=0;k<(*bins).items;k++)
    {
      lb_gr_project_2d(vp2d, vp2d.xr_min +     k*(vp2d.xr_max-vp2d.xr_min)/(*bins).items, 0,                &xp1_f, &yp1_f);
      lb_gr_project_2d(vp2d, vp2d.xr_min + (k+1)*(vp2d.xr_max-vp2d.xr_min)/(*bins).items, (*bins).array[k], &xp2_f, &yp2_f);

      
      xp1=round(xp1_f);
      yp1=round(yp1_f);
      xp2=round(xp2_f);
      yp2=round(yp2_f);
      
      lb_gr_draw_rectangle_bar(Pic, xp1, yp1, xp2, yp2, 1, color_bar_border, color_bar,  COPYMODE_COPY);
    }
}


void lb_gr_draw_line1(PICTURE_T *Pic, S_INT_16_T x0, S_INT_16_T y0, S_INT_16_T x1, S_INT_16_T y1, PIXEL_T color, COPYMODE_T copymode)  
{
  S_INT_16_T dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
  S_INT_16_T dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
  S_INT_16_T err = (dx>dy ? dx : -dy)/2, e2, steps, i;
 
  if (dx>=dy)
    steps=dx;
  else
    steps=dy;
  
  for (i=0;i<=steps;i++)
    {
      lb_gr_draw_pixel(Pic, x0, y0 , color, copymode);  
      e2 = err;
      if (e2 >-dx) { err -= dy; x0 += sx; }
      if (e2 < dy) { err += dx; y0 += sy; }
    } 
}

#ifdef BACKUP
void lb_gr_draw_line1(PICTURE_T *Pic, S_INT_16_T x0, S_INT_16_T y0, S_INT_16_T x1, S_INT_16_T y1, PIXEL_T color, COPYMODE_T copymode)  
{
  S_INT_16_T dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
  S_INT_16_T dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
  S_INT_16_T err = (dx>dy ? dx : -dy)/2, e2;
 
  lb_gr_draw_pixel(Pic, x0, y0 , color, copymode);  

  do
    {
      e2 = err;
      if (e2 >-dx) { err -= dy; x0 += sx; }
      if (e2 < dy) { err += dx; y0 += sy; }
      lb_gr_draw_pixel(Pic, x0, y0 , color, copymode);  
    } while ((x0!=x1) || (y0!=y1));
 
}
#endif

void lb_gr_draw_line1_f(PICTURE_T *Pic, FLOAT_T x0, FLOAT_T y0, FLOAT_T x1, FLOAT_T y1, PIXEL_T color, COPYMODE_T copymode)
{
  FLOAT_T _x0, _x1, _y0, _y1, xr, yr, dx, dy;

  if (lb_re_equal(x1,x0) && lb_re_equal(y1,y0))
    {
      lb_gr_draw_pixel(Pic, x0, y0, color, copymode);
      return;
    }
   
  /* If slope m<=1.0, we treat y as a function of x, otherwise x is a function of y */

  if(fabs(y1-y0)<=fabs(x1-x0))
    {
      if(x1>=x0)
	{
	  _x1=x1;
	  _y1=y1;
	  _x0=x0;
	  _y0=y0;
	}
      else
	{
	  _x1=x0;
	  _y1=y0;
	  _x0=x1;
	  _y0=y1;
	}
      dy=(_y1-_y0)/(_x1-_x0);
      xr=_x0;
      yr=_y0;
      while (xr<=_x1)
	{
	  lb_gr_draw_pixel(Pic, round(xr), round(yr), color, copymode);
	  xr++;
	  yr+=dy;
	}
    }
  else
    {
      if(y1>=y0)
	{
	  _y1=y1;
	  _x1=x1;
	  _x0=x0;
	  _y0=y0;
      	}
      else
      	{
	  _y1=y0;
	  _x1=x0;
	  _y0=y1;
	  _x0=x1;
	}
      dx=(_x1-_x0)/(_y1-_y0);
      xr=_x0;
      yr=_y0;
      while (yr<=_y1)
	{
	  lb_gr_draw_pixel(Pic, round(xr), round(yr), color, copymode);
	  xr+=dx;
	  yr++;
	}
    }
}

void lb_gr_draw_line2(PICTURE_T *Pic, S_INT_16_T _x0, S_INT_16_T _y0, S_INT_16_T _x1, S_INT_16_T _y1, PIXEL_T color, COPYMODE_T copymode)  
{
  {
    S_INT_16_T x0, y0, x1, y1, i, i_max, sign;
    S_INT_8_T flag_mirror;
    FLOAT_T y_real, x_real, m;

    if ((_x1==_x0) && (_y1==_y0))
      {
	lb_gr_draw_pixel(Pic, _x0, _y0, color, copymode);
	return;
      }
     
    if (abs(_y1-_y0)<=abs(_x1-_x0))
      {
	x0=_x0;
	y0=_y0;
	x1=_x1;
	y1=_y1;
	flag_mirror=FALSE;
      }
    else
      {
	x0=_y0;
	y0=_x0;
	x1=_y1;
	y1=_x1;
	flag_mirror=TRUE;
      }
  
    if (x1>=x0)
      sign=1;
    else
      sign=-1;

    i_max=0.5+fabs(x1-x0);
    m=(FLOAT_T)(y1-y0)/(FLOAT_T)(x1-x0);

    for(i=0;i<=i_max;i++)
      {
	x_real=(FLOAT_T)x0+(FLOAT_T)i*sign; /* Adding 0.5 and truncating effectively rounds to the nearest integer */
	y_real=y0+(FLOAT_T)i*sign*m;

	if (!flag_mirror)
	  {
	    lb_gr_draw_pixel(Pic, x_real+0.5, y_real+0.5, color, copymode);

	    if (y_real>0.5)
	      lb_gr_draw_pixel(Pic, x_real+0.5, y_real+1.5, color, copymode);
	    else
	      lb_gr_draw_pixel(Pic, x_real+0.5, y_real-0.5, color, copymode);
	  }
	else
	  {
	    lb_gr_draw_pixel(Pic, y_real+0.5, x_real+0.5, color, copymode);

	    if (y_real>0.5)
	      lb_gr_draw_pixel(Pic, y_real+1.5, x_real+0.5, color, copymode);
	    else
	      lb_gr_draw_pixel(Pic, y_real-0.5, x_real+0.5, color, copymode);
	  }
      }
  }
}

void lb_gr_draw_line3(PICTURE_T *Pic, S_INT_16_T _x0, S_INT_16_T _y0, S_INT_16_T _x1, S_INT_16_T _y1, PIXEL_T color, COPYMODE_T copymode)  
{
  {
    S_INT_16_T x0, y0, x1, y1, i, i_max, sign;
    S_INT_8_T flag_mirror;
    FLOAT_T m;
    S_INT_16_T x_int, y_int;

    if ((_x1==_x0) && (_y1==_y0))
      {
	lb_gr_draw_pixel(Pic, _x0, _y0, color, copymode);
	return;
      }
    
    if (abs(_y1-_y0)<=abs(_x1-_x0))
      {
	x0=_x0;
	y0=_y0;
	x1=_x1;
	y1=_y1;
	flag_mirror=FALSE;
      }
    else
      {
	x0=_y0;
	y0=_x0;
	x1=_y1;
	y1=_x1;
	flag_mirror=TRUE;
      }
  
    if (x1>=x0)
      sign=1;
    else
      sign=-1;

    if (x1!=x0)
      {
	i_max=round(fabs(x1-x0));
	m=(FLOAT_T)(y1-y0)/(FLOAT_T)(x1-x0);

	for(i=0;i<=i_max;i++)
	  {
	    x_int=round((FLOAT_T)x0+(FLOAT_T)i*sign); 
	    y_int=round((FLOAT_T)y0+(FLOAT_T)i*sign*m);

	    if (!flag_mirror)
	      {
		lb_gr_draw_pixel(Pic, x_int, y_int,   color, copymode);
		lb_gr_draw_pixel(Pic, x_int, y_int+1, color, copymode);
		lb_gr_draw_pixel(Pic, x_int, y_int-1, color, copymode);
	      }
	    else
	      {
		lb_gr_draw_pixel(Pic, y_int,   x_int, color, copymode);
		lb_gr_draw_pixel(Pic, y_int+1, x_int, color, copymode);
		lb_gr_draw_pixel(Pic, y_int-1, x_int, color, copymode);
	      }
	  }
      }
  }
}

void lb_gr_draw_line_antialiasing(PICTURE_T *Pic, FLOAT_T _xr0, FLOAT_T _yr0, FLOAT_T _xr1, FLOAT_T _yr1, FLOAT_T w, PIXEL_T color, COPYMODE_T copymode)  
{
  S_INT_16_T i_a, i_b, j_a, j_b, i, j, sign, xi, yi;
  S_INT_8_T flag_mirror;
  FLOAT_T xr0, yr0, xr1, yr1,  m, denom, temp, distance, csc_tetha, dx_sqr, dy_sqr;
  PIXEL_T temp_color;
  ERR_T error;

  temp_color=color;
 
  if (fabs(_yr1-_yr0)<=fabs(_xr1-_xr0))
    {
      xr0=_xr0;
      yr0=_yr0;
      xr1=_xr1;
      yr1=_yr1;
      flag_mirror=FALSE;
    }
  else
    {
      xr0=_yr0;
      yr0=_xr0;
      xr1=_yr1;
      yr1=_xr1;
      flag_mirror=TRUE;
    }

  if (xr1>=xr0)
    sign=1;
  else
    sign=-1;

  dx_sqr=(xr1-xr0)*(xr1-xr0);
  dy_sqr=(yr1-yr0)*(yr1-yr0);
  denom=lb_re_sqrt(dx_sqr+dy_sqr,&error);
  if (denom>1.0)
    {
      csc_tetha=denom/(xr1-xr0);
      m=(FLOAT_T)(yr1-yr0)/(FLOAT_T)(xr1-xr0);
    }
  else
    {
      m=0.0;
      csc_tetha=1.0;
    }
  i_a=floor(-(0.5+0.5*w));
  i_b=ceil(0.5+fabs(xr1-xr0)+0.5*w);
  j_a=floor(-(0.5+0.5*fabs(w*csc_tetha)));
  j_b=ceil(0.5+fabs(0.5*w*csc_tetha));
  for(i=i_a;i<=i_b;i++)
    {
      xi=0.5+xr0+(FLOAT_T)i*sign; /* Adding 0.5 and truncating effectively rounds to the nearest integer */
      for (j=j_a;j<=j_b;j++)
	{
     	  yi=0.5+yr0+i*sign*m+j;
	  distance=-1.0; /* implicit flag */

	  /* if the area is far enough (0.5*w ; length-0.5*w) from the edges */
	  
	  if(denom<0.5)
	    distance=0.5*w-lb_re_sqrt(lb_re_sqr((FLOAT_T)xi-0.5*(xr0+xr1),&error)+lb_re_sqr((FLOAT_T)yi-0.5*(yr0+yr1),&error),&error);
	  else
	    if ( (0.5*w<i) && (i<(fabs(xr1-xr0)-0.5*w)) )
	      distance=0.5*w-fabs((FLOAT_T)xi*(yr1-yr0)-(FLOAT_T)yi*(xr1-xr0)+xr1*yr0-yr1*xr0)/denom;
	    else
	      if (lb_re_sqr(xi-0.5*(xr0+xr1),&error)+lb_re_sqr(yi-0.5*(yr0+yr1),&error) <=
		  0.25*(w*w+dx_sqr+dy_sqr) )
		distance=0.5*w-fabs(xi*(yr1-yr0)-yi*(xr1-xr0)+xr1*yr0-yr1*xr0)/denom;
	      else
		{
		  temp = 0.5*w - lb_re_sqrt(lb_re_sqr(xi-xr0,&error)+lb_re_sqr(yi-yr0,&error),&error);
		  if (temp>0.0)
		    distance=temp;
		  temp = 0.5*w - lb_re_sqrt(lb_re_sqr(xi-xr1,&error)+lb_re_sqr(yi-yr1,&error),&error);
		  if (temp>0.0)
		    distance=temp;	
		}
	  if (distance>0.0)
	    temp_color.a=round(MAX_A*lb_re_max(lb_re_min(1.0,distance),lb_re_frac(distance)));
	  else
	    temp_color.a=0;
	  
	  if (!flag_mirror)
	    lb_gr_draw_pixel(Pic, xi, yi, temp_color, copymode);
	  else
	    lb_gr_draw_pixel(Pic, yi, xi, temp_color, copymode);

	}
    }
}

void lb_gr_draw_line_antialiasing2(PICTURE_T *Pic, S_INT_16_T _x0, S_INT_16_T _y0, S_INT_16_T _x1, S_INT_16_T _y1, PIXEL_T color, COPYMODE_T copymode)  
{
  S_INT_16_T x0, y0, x1, y1, i, i_max, sign;
  S_INT_8_T flag_mirror;
  PIXEL_T temp_color;
  FLOAT_T y_real, x_real, m, distance;

  temp_color=color;

  if (abs(_y1-_y0)<=abs(_x1-_x0))
    {
      x0=_x0;
      y0=_y0;
      x1=_x1;
      y1=_y1;
      flag_mirror=FALSE;
    }
  else
    {
      x0=_y0;
      y0=_x0;
      x1=_y1;
      y1=_x1;
      flag_mirror=TRUE;
    }

  if(x1==x0)
    {
      lb_gr_draw_pixel(Pic, round(x0), round(y0), temp_color, copymode);
      return;
    }
  
  if (x1>=x0)
    sign=1;
  else
    sign=-1;

  i_max=0.5+fabs(x1-x0);
  m=(FLOAT_T)(y1-y0)/(FLOAT_T)(x1-x0);

  for(i=0;i<=i_max;i++)
    {
      x_real=(FLOAT_T)x0+(FLOAT_T)i*sign; /* Adding 0.5 and truncating effectively rounds to the nearest integer */
      y_real=y0+(FLOAT_T)i*sign*m;

      temp_color.a=MAX_A;
      if (!flag_mirror)
	lb_gr_draw_pixel(Pic, round(x_real),round(y_real), temp_color, copymode);
      else
	lb_gr_draw_pixel(Pic, round(y_real), round(x_real), temp_color, copymode);

      distance=lb_re_frac(y_real)-0.5;
      if (distance<0) /* pixel below */
	{
	  temp_color.a=MAX_A*(1.0+2.0*distance);
	  if (!flag_mirror)
	    lb_gr_draw_pixel(Pic, round(x_real), round(y_real)+1, temp_color, copymode);
	  else
	    lb_gr_draw_pixel(Pic, round(y_real)+1, round(x_real), temp_color, copymode);
	}
      else /* pixel above */
	{
	  temp_color.a=MAX_A*(1.0-2.0*distance);
	  if (!flag_mirror)
	    lb_gr_draw_pixel(Pic, round(x_real), round(y_real)-1, temp_color, copymode);
	  else
	    lb_gr_draw_pixel(Pic, round(y_real)-1, round(x_real), temp_color, copymode);
	}
    }
}


void lb_gr_draw_line_antialiasing2_f(PICTURE_T *Pic, FLOAT_T _x0, FLOAT_T _y0, FLOAT_T _x1, FLOAT_T _y1, PIXEL_T color, COPYMODE_T copymode)  
{
  FLOAT_T x0, y0, x1, y1, y_real, x_real, m, distance;
  S_INT_16_T i, i_max, sign;
  S_INT_8_T flag_mirror;
  PIXEL_T temp_color;
 
  temp_color=color;

  if (fabs(_y1-_y0)<=fabs(_x1-_x0))
    {
      x0=_x0;
      y0=_y0;
      x1=_x1;
      y1=_y1;
      flag_mirror=FALSE;
    }
  else
    {
      x0=_y0;
      y0=_x0;
      x1=_y1;
      y1=_x1;
      flag_mirror=TRUE;
    }

  /* Draw a point if  x1=x0 && y1=y0  */
  if(lb_re_equal(x1,x0))
    {
      lb_gr_draw_pixel(Pic, round(x0), round(y0), temp_color, copymode);
      return;
    }
  if (x1>=x0)
    sign=1;
  else
    sign=-1;

  i_max=0.5+fabs(x1-x0);

  m=(y1-y0)/(x1-x0);

  for(i=0;i<=i_max;i++)
    {
      x_real=x0 + i*sign; 
      y_real=y0 + i*sign*m;

      temp_color.a=MAX_A;
      if (!flag_mirror)
	lb_gr_draw_pixel(Pic, round(x_real), round(y_real), temp_color, copymode);
      else
	lb_gr_draw_pixel(Pic, round(y_real), round(x_real), temp_color, copymode);

      distance=lb_re_frac(y_real)-0.5;
      if (distance<0) /* pixel below */
	{
	  temp_color.a=MAX_A*(1.0+2.0*distance);
	  if (!flag_mirror)
	    lb_gr_draw_pixel(Pic, round(x_real), round(y_real)+1, temp_color, copymode);
	  else
	    lb_gr_draw_pixel(Pic, round(y_real)+1, round(x_real), temp_color, copymode);
	}
      else /* pixel above */
	{
	  temp_color.a=MAX_A*(1.0-2.0*distance);
	  if (!flag_mirror)
	    lb_gr_draw_pixel(Pic, round(x_real), round(y_real)-1, temp_color, copymode);
	  else
	    lb_gr_draw_pixel(Pic, round(y_real)-1, round(x_real), temp_color, copymode);
	}
    }
}

void lb_gr_draw_line_antialiasing3(PICTURE_T *Pic, S_INT_16_T _x0, S_INT_16_T _y0, S_INT_16_T _x1, S_INT_16_T _y1, PIXEL_T color, COPYMODE_T copymode)  
{
  S_INT_16_T x0, y0, x1, y1, i, i_max, sign;
  S_INT_8_T flag_mirror;
  PIXEL_T temp_color;
  FLOAT_T y_real, x_real, m;

  temp_color=color;

  if (abs(_y1-_y0)<=abs(_x1-_x0))
    {
      x0=_x0;
      y0=_y0;
      x1=_x1;
      y1=_y1;
      flag_mirror=FALSE;
    }
  else
    {
      x0=_y0;
      y0=_x0;
      x1=_y1;
      y1=_x1;
      flag_mirror=TRUE;
    }

  /* Draw a point if  x1=x0 && y1=y0  */
  if(x1==x0)
    {
      temp_color.a = MAX_A;
      lb_gr_draw_pixel(Pic, x0, y0, temp_color, copymode);
      temp_color.a = MAX_A>>1;
      lb_gr_draw_pixel(Pic, x0-1, y0,   temp_color, copymode);
      lb_gr_draw_pixel(Pic, x0+1, y0,   temp_color, copymode);
      lb_gr_draw_pixel(Pic, x0,   y0-1, temp_color, copymode);
      lb_gr_draw_pixel(Pic, x0,   y0+1, temp_color, copymode);
      temp_color.a = MAX_A>>2;
      lb_gr_draw_pixel(Pic, x0-1, y0-1,  temp_color, copymode);
      lb_gr_draw_pixel(Pic, x0-1, y0+1,  temp_color, copymode);
      lb_gr_draw_pixel(Pic, x0+1, y0-1,  temp_color, copymode);
      lb_gr_draw_pixel(Pic, x0+1, y0+1,  temp_color, copymode);
      return;
    }

  if (x1>=x0)
    sign=1;
  else
    sign=-1;

  i_max=0.5+fabs(x1-x0);

  m=(FLOAT_T)(y1-y0)/(FLOAT_T)(x1-x0);

  for(i=0;i<=i_max;i++)
    {
      x_real=(FLOAT_T)x0+(FLOAT_T)i*sign; /* Adding 0.5 and truncating effectively rounds to the nearest integer */
      y_real=y0+(FLOAT_T)i*sign*m;
      
      temp_color.a=MAX_A;
	
      if (!flag_mirror)
	{
	  /* Middle */
      	  lb_gr_draw_pixel(Pic, x_real, y_real, temp_color, copymode);
	  //lb_ft_printf(ty_C, "x_real=%f, y_real=%f, temp_float=%f \r\n",x_real,y_real,temp_float);

	  temp_color.a=round(MAX_A*(1-lb_re_frac(y_real)));
	  lb_gr_draw_pixel(Pic, x_real, y_real-1, temp_color, copymode);

	  temp_color.a=round(MAX_A*(lb_re_frac(y_real)));
      	  lb_gr_draw_pixel(Pic, x_real, y_real+1, temp_color, copymode);
	}
      else
	{
	  lb_gr_draw_pixel(Pic, y_real, x_real, temp_color, copymode);

	  temp_color.a=round(MAX_A*(1-lb_re_frac(y_real)));
	  lb_gr_draw_pixel(Pic, y_real-1, x_real, temp_color, copymode);

	  temp_color.a=round(MAX_A*(lb_re_frac(y_real)));
	  lb_gr_draw_pixel(Pic, y_real+1, x_real, temp_color, copymode);
	}
    }
}

void lb_gr_draw_line_antialiasing3_f(PICTURE_T *Pic, FLOAT_T _x0, FLOAT_T _y0, FLOAT_T _x1, FLOAT_T _y1, PIXEL_T color, COPYMODE_T copymode)  
{
  FLOAT_T x0, y0, x1, y1;
  S_INT_16_T i, i_max, sign;
  S_INT_8_T flag_mirror;
  PIXEL_T temp_color;
  FLOAT_T y_real, x_real, m;

  temp_color=color;

  if (fabs(_y1-_y0)<=fabs(_x1-_x0))
    {
      x0=_x0;
      y0=_y0;
      x1=_x1;
      y1=_y1;
      flag_mirror=FALSE;
    }
  else
    {
      x0=_y0;
      y0=_x0;
      x1=_y1;
      y1=_x1;
      flag_mirror=TRUE;
    }

  /* Draw a point if  x1=x0 && y1=y0  */
  if(lb_re_equal(x1,x0))
    {
      temp_color.a = MAX_A;
      lb_gr_draw_pixel(Pic, round(x0), round(y0), temp_color, copymode);
      temp_color.a = MAX_A>>1;
      lb_gr_draw_pixel(Pic, round(x0)-1, round(y0),   temp_color, copymode);
      lb_gr_draw_pixel(Pic, round(x0)+1, round(y0),   temp_color, copymode);
      lb_gr_draw_pixel(Pic, round(x0),   round(y0)-1, temp_color, copymode);
      lb_gr_draw_pixel(Pic, round(x0),   round(y0)+1, temp_color, copymode);
      temp_color.a = MAX_A>>2;
      lb_gr_draw_pixel(Pic, round(x0)-1, round(y0)-1,   temp_color, copymode);
      lb_gr_draw_pixel(Pic, round(x0)-1, round(y0)+1,   temp_color, copymode);
      lb_gr_draw_pixel(Pic, round(x0)+1, round(y0)-1,   temp_color, copymode);
      lb_gr_draw_pixel(Pic, round(x0)+1, round(y0)+1,   temp_color, copymode);
      return;
    }
  
  if (x1>=x0)
    sign=1;
  else
    sign=-1;

  i_max=round(fabs(x1-x0));
  m=(y1-y0)/(x1-x0);

  for(i=0;i<=i_max;i++)
    {
      x_real=x0+i*sign; /* Adding 0.5 and truncating effectively rounds to the nearest integer */
      y_real=y0+i*sign*m;
      
      temp_color.a=MAX_A;
	
      if (!flag_mirror)
	{
	  /* Middle */
      	  lb_gr_draw_pixel(Pic, x_real, y_real, temp_color, copymode);
	  //lb_ft_printf(ty_C, "x_real=%f, y_real=%f, temp_float=%f \r\n",x_real,y_real,temp_float);

	  temp_color.a=round(MAX_A*(1-lb_re_frac(y_real)));
	  lb_gr_draw_pixel(Pic, x_real, y_real-1, temp_color, copymode);

	  temp_color.a=round(MAX_A*(lb_re_frac(y_real)));
      	  lb_gr_draw_pixel(Pic, x_real, y_real+1, temp_color, copymode);
	}
      else
	{
	  lb_gr_draw_pixel(Pic, y_real, x_real, temp_color, copymode);

	  temp_color.a=round(MAX_A*(1-lb_re_frac(y_real)));
	  lb_gr_draw_pixel(Pic, y_real-1, x_real, temp_color, copymode);

	  temp_color.a=round(MAX_A*(lb_re_frac(y_real)));
	  lb_gr_draw_pixel(Pic, y_real+1, x_real, temp_color, copymode);
	}
    }
}


void lb_gr_fb_rectangle(SCREEN_T *screen, S_INT_16_T x0, S_INT_16_T y0, S_INT_16_T x1, S_INT_16_T y1, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b)
{
  S_INT_16_T x_min, x_max, y_min, y_max, y;
  unsigned long l_color;
  unsigned int *pos, *pos_a, *pos_b;

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

  if (x_max>(*screen).w-1)
    x_max=(*screen).w-1;

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

  if (y_max>(*screen).h-1)
    y_max=(*screen).h-1;

  l_color= 0xFF000000 | r<<16 | g<<8 |  b;
  for (y=y_min; y<=y_max; y++)
    {
      pos_a = (unsigned int*)(*screen).dat + (*screen).w*y + x_min;
      pos_b = (unsigned int*)(*screen).dat + (*screen).w*y + x_max;;
      for (pos=pos_a; pos<=pos_b; pos++)
      	*pos = l_color;
    }
}


void  lb_gr_fb_rectangle_copymode(SCREEN_T *screen, S_INT_16_T x0, S_INT_16_T y0, S_INT_16_T x1, S_INT_16_T y1, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b, U_INT_8_T a, COPYMODE_T copymode)
{
  S_INT_16_T x_min, x_max, y_min, y_max, x, y;

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

  if (x_max>(*screen).w-1)
    x_max=(*screen).w-1;

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

  if (y_max>(*screen).h-1)
    y_max=(*screen).h-1;

  for (y=y_min; y<=y_max; y++)
    for (x=x_min; x<=x_max; x++)
      lb_gr_fb_setpixel_ARGB_copymode(screen, x, y, r, g, b, a, copymode);
}



void _lb_gr_fb_setpixel_ARGB(SCREEN_T *screen, S_INT_16_T x, S_INT_16_T y, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b, U_INT_8_T a)
{
  /* Sets a pixel in the screen buffer with no blending and no boundary checks. */
  /* Can corrupt the memory if misused. */
  
  U_INT_32_T dat_offset;

#define METHOD_1
#ifdef METHOD_1
  /* Performance: 5.02 fps @ 1920x1080 on Raspberry Pi */
  dat_offset=4*((*screen).w*y+x);
  *((unsigned char*)(*screen).dat + dat_offset)     = b;
  *((unsigned char*)(*screen).dat + dat_offset + 1) = g; 
  *((unsigned char*)(*screen).dat + dat_offset + 2) = r;
  *((unsigned char*)(*screen).dat + dat_offset + 3) = a;
#else
  /* Performance:  6.56 fps @ 1920x1080 on Raspberry Pi */
  dat_offset=(*screen).w*y + x;
  *((unsigned int*)(*screen).dat + dat_offset) =  a<<24 | r<<16 | g<<8 |  b ;
#endif
  return;
}


void lb_gr_fb_setpixel_ARGB(SCREEN_T *screen, S_INT_16_T x, S_INT_16_T y, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b, U_INT_8_T a)
{
  if ( (x<0) || (y<0) || (x>(*screen).w) || (y>(*screen).h) )
    return;
  _lb_gr_fb_setpixel_ARGB(screen, x, y, r, g, b, a);
}


void _lb_gr_fb_setpixel_ARGB_copymode(SCREEN_T *screen, S_INT_16_T x, S_INT_16_T y, U_INT_8_T src_r, U_INT_8_T src_g, U_INT_8_T src_b, U_INT_8_T src_a, COPYMODE_T copymode)
{
  U_INT_32_T dat_offset;

  if (!copymode) 
    {
      dat_offset = (*screen).w*y + x;
      *((U_INT_32_T*)(*screen).dat + dat_offset) =  src_a<<24 | src_r<<16 | src_g<<8 |  src_b ;
      return;
    }

  COPYMODE_T temp_copymode;
  temp_copymode = copymode & 0b11111; /* The mode is defined in the 5-lower bits of the 16-bit parameter. */

  if( temp_copymode==COPYMODE_COPY )
    {

      dat_offset = (*screen).w*y + x;
      *((U_INT_32_T*)(*screen).dat + dat_offset) =  src_a<<24 | src_r<<16 | src_g<<8 |  src_b ;
      return;
    }
  
  U_INT_8_T dst_r, dst_g, dst_b, dst_a;
  
  dat_offset=4*((*screen).w*y+x);
  dst_b = *((unsigned char*)(*screen).dat + dat_offset);
  dst_g = *((unsigned char*)(*screen).dat + dat_offset + 1); 
  dst_r = *((unsigned char*)(*screen).dat + dat_offset + 2);
  dst_a = *((unsigned char*)(*screen).dat + dat_offset + 3);

  switch (temp_copymode)
    { 
    case COPYMODE_AND: /* Performs logical AND between SRC and DST. Alpha remains unchanged. */
      dst_r &= src_r;
      dst_g &= src_g;
      dst_b &= src_b;

      dat_offset = (*screen).w*y + x;
      *((U_INT_32_T*)(*screen).dat + dat_offset) =  dst_a<<24 | dst_r<<16 | dst_g<<8 | dst_b ;
      return;
    case COPYMODE_OR: /* Performs logical OR between SRC and DST. Alpha remains unchanged. */
      dst_r |= src_r;
      dst_g |= src_g;
      dst_b |= src_b;

      dat_offset = (*screen).w*y + x;
      *((U_INT_32_T*)(*screen).dat + dat_offset) =  dst_a<<24 | dst_r<<16 | dst_g<<8 | dst_b ;
      return;
    case COPYMODE_XOR: /* Performs logical XOR between SRC and DST. Alpha remains unchanged. */
      dst_r ^= src_r;
      dst_g ^= src_g;
      dst_b ^= src_b;

      dat_offset = (*screen).w*y + x;
      *((U_INT_32_T*)(*screen).dat + dat_offset) =  dst_a<<24 | dst_r<<16 | dst_g<<8 | dst_b ;
      return;
    case COPYMODE_NOT_SRC: /* Makes DST the logical NOT of SRC. Alpha remains unchanged. */
      dst_r = ~src_r;
      dst_g = ~src_g;
      dst_b = ~src_b;

      dat_offset = (*screen).w*y + x;
      *((U_INT_32_T*)(*screen).dat + dat_offset) =  dst_a<<24 | dst_r<<16 | dst_g<<8 | dst_b ;
      return;
    case COPYMODE_NOT_DST: /* Performs a logical NOT on DST (regardless of SRC). Alpha remains unchanged. */
      dst_r = ~dst_r;
      dst_g = ~dst_g;
      dst_b = ~dst_b;

      dat_offset = (*screen).w*y + x;
      *((U_INT_32_T*)(*screen).dat + dat_offset) =  dst_a<<24 | dst_r<<16 | dst_g<<8 | dst_b ;
      return;
    case COPYMODE_ADD: /* Sums each one of the color components but caps the value at the maximum if exceeded. */
      {
	S_INT_16_T temp; /* Sets the alpha as the average of SRC and DST */
	      
	temp=dst_r + src_r;  
	if (temp>0xFF)
	  temp=0xFF;
	dst_r=temp;

	temp=dst_g + src_g;  
	if (temp>0xFF)
	  temp=0xFF;
	dst_g=temp;

	temp=dst_b + src_b;  
	if (temp>0xFF)
	  temp=0xFF;
	dst_b=temp;

	temp=(src_a + dst_a) >> 1;
	dst_a=temp;

	dat_offset = (*screen).w*y + x;
	*((U_INT_32_T*)(*screen).dat + dat_offset) =  dst_a<<24 | dst_r<<16 | dst_g<<8 | dst_b ;
	return;
      }
    case COPYMODE_SRC_MINUS_DST: /* Substracts each one of the color components of SRC minus DST.  Caps at zero. */
      {
	S_INT_16_T temp;           /* Sets the alpha as the average of SRC and DST */
	      
	temp = src_r - dst_r;
	if (temp<0)
	  temp=0;
	dst_r=temp;

	temp = src_g - dst_g;
	if (temp<0)
	  temp=0;
	dst_g=temp;

	temp = src_b - dst_b;
	if (temp<0)
	  temp=0;
	dst_b=temp;

	temp=(src_a + dst_a) >> 1;
	dst_a=temp;

	dat_offset = (*screen).w*y + x;
	*((U_INT_32_T*)(*screen).dat + dat_offset) =  dst_a<<24 | dst_r<<16 | dst_g<<8 | dst_b ;
      return;
      }
    case COPYMODE_DST_MINUS_SRC: /* Substracts each one of the color components of DST minus SRC.  Caps at zero. */
      {
	S_INT_16_T temp;           /* Sets the alpha as the average of SRC and DST */
	
	temp=dst_r - src_r;      
	if (temp<0)
	  temp=0;
	dst_r=temp;

	temp=dst_g - src_g;      
	if (temp<0)
	  temp=0;
	dst_g=temp;

	temp=dst_b - src_b;      
	if (temp<0)
	  temp=0;
	dst_b=temp;

	temp=(src_a + dst_a) >> 1;
	dst_a=temp;
	
	dat_offset = (*screen).w*y + x;
	*((U_INT_32_T*)(*screen).dat + dat_offset) =  dst_a<<24 | dst_r<<16 | dst_g<<8 | dst_b ;
	return;
      }
    case COPYMODE_AVG: /* Averages each one of the components of SRC and DST as well as the alpha. */
      dst_r = ((S_INT_16_T)src_r + (S_INT_16_T)dst_r) >> 1;
      dst_g = ((S_INT_16_T)src_g + (S_INT_16_T)dst_g) >> 1;
      dst_b = ((S_INT_16_T)src_b + (S_INT_16_T)dst_b) >> 1;
      dst_a = ((S_INT_16_T)src_a + (S_INT_16_T)dst_a) >> 1;

      dat_offset = (*screen).w*y + x;
      *((U_INT_32_T*)(*screen).dat + dat_offset) =  dst_a<<24 | dst_r<<16 | dst_g<<8 | dst_b ;
      return;
    case COPYMODE_BLEND:
      dst_r = (U_INT_32_T)((0xFF-src_a)*dst_r + src_a*src_r)>>8;
      dst_g = (U_INT_32_T)((0xFF-src_a)*dst_g + src_a*src_g)>>8;
      dst_b = (U_INT_32_T)((0xFF-src_a)*dst_b + src_a*src_b)>>8;
      //      dst_a = (U_INT_32_T)((0xFF-src_a)*dst_a + src_a*src_a)>>8;

      dat_offset = (*screen).w*y + x;
      //      *((U_INT_32_T*)(*screen).dat + dat_offset) =  dst_a<<24 | dst_r<<16 | dst_g<<8 | dst_b ;
            *((U_INT_32_T*)(*screen).dat + dat_offset) =  dst_r<<16 | dst_g<<8 | dst_b ;
      return;
    case COPYMODE_MULTIPLY:
      dst_r = (U_INT_32_T)dst_r*src_r>>8;
      dst_g = (U_INT_32_T)dst_g*src_g>>8;
      dst_b = (U_INT_32_T)dst_b*src_b>>8;

      dat_offset = (*screen).w*y + x;
      *((U_INT_32_T*)(*screen).dat + dat_offset) =  dst_a<<24 | dst_r<<16 | dst_g<<8 | dst_b ;
      return;
    case COPYMODE_SRC_DIV_DST:
      {
	U_INT_8_T temp;
	      
	if (dst_r!=0)
	  temp=(U_INT_32_T)(0xFF*src_r/dst_r);
	else
	  temp=0xFF;
	dst_r=temp;

	if (dst_g!=0)
	  temp=(U_INT_32_T)(0xFF*src_g/dst_g);
	else
	  temp=0xFF;
	dst_g=temp;

	if (dst_b!=0)
	  temp=(U_INT_32_T)(0xFF*src_b/dst_b);
	else
	  temp=0xFF;
	dst_b=temp;
	
	dat_offset = (*screen).w*y + x;
	*((U_INT_32_T*)(*screen).dat + dat_offset) =  dst_a<<24 | dst_r<<16 | dst_g<<8 | dst_b ;
	return;
      }
    case COPYMODE_DST_DIV_SRC:
      {
	U_INT_8_T temp;
	      
	if (src_r!=0)
	  temp=(U_INT_32_T)(0xFF*dst_r/src_r);
	else
	  temp=0xFF;
	dst_r=temp;

	if (src_g!=0)
	  temp=(U_INT_32_T)(0xFF*dst_g/src_g);
	else
	  temp=0xFF;
	dst_g=temp;

	if (src_b!=0)
	  temp=(U_INT_32_T)(0xFF*dst_b/src_b);
	else
	  temp=0xFF;
	dst_b=temp;

	dat_offset = (*screen).w*y + x;
	*((U_INT_32_T*)(*screen).dat + dat_offset) =  dst_a<<24 | dst_r<<16 | dst_g<<8 | dst_b ;
	return;
      }
    case COPYMODE_RESERVED0:
      return;
    case COPYMODE_RESERVED1:
      return;
    case COPYMODE_RESERVED2:
      return;
    }

  S_INT_32_T temp_r, temp_g, temp_b, temp_k, phi_src, phi_dst, n_xy_num, n_xy_den;
  FLOAT_T n_f;
  
  dat_offset=4*((*screen).w*y+x);
  dst_b = *((unsigned char*)(*screen).dat + dat_offset);
  dst_g = *((unsigned char*)(*screen).dat + dat_offset + 1); 
  dst_b = *((unsigned char*)(*screen).dat + dat_offset + 2);
  dst_a = *((unsigned char*)(*screen).dat + dat_offset + 3);

  switch (temp_copymode)
    {
    case COPYMODE_PORTERDUFF_CLEAR_DST:
      phi_src=0;       phi_dst=0;
      break;
    case COPYMODE_PORTERDUFF_COPY_SRC:
      phi_src=0xFF;   phi_dst=0;
      break;
    case COPYMODE_PORTERDUFF_LEAVES_DST:
      phi_src=0;       phi_dst=0xFF;
      break;
    case COPYMODE_PORTERDUFF_SRC_OVER_DST:
      phi_src=0xFF;   phi_dst=0xFF-src_a;
      break;
    case COPYMODE_PORTERDUFF_DST_OVER_SRC:
      phi_src=0xFF-dst_a;   phi_dst=0xFF;
      break;
    case COPYMODE_PORTERDUFF_SRC_IN_DST:
      phi_src=dst_a;   phi_dst=0;
      break;
    case COPYMODE_PORTERDUFF_DST_IN_SRC:
      phi_src=0;   phi_dst=src_a;
      break;
    case COPYMODE_PORTERDUFF_SRC_OUT_DST:
      phi_src=0xFF-dst_a;   phi_dst=0;
      break;
    case COPYMODE_PORTERDUFF_DST_OUT_SRC:
      phi_src=0;   phi_dst=0xFF-src_a;
      break;
    case COPYMODE_PORTERDUFF_SRC_ATOP_DST:
      phi_src=dst_a;   phi_dst=0xFF-src_a;
      break;
    case COPYMODE_PORTERDUFF_DST_ATOP_SRC:
      phi_src=0xFF-dst_a;   phi_dst=src_a;
      break;
    case COPYMODE_PORTERDUFF_XOR:
      phi_src=0xFF-dst_a;   phi_dst=0xFF-src_a;
      break;
    }
  n_xy_num= phi_src*src_a;
  n_xy_den = n_xy_num + phi_dst*dst_a;
  if (n_xy_den!=0)
    {
      n_f=(FLOAT_T)n_xy_num/(FLOAT_T)n_xy_den;
      //lb_ft_printf(ty_C, "n_xy= %f / %f =%f\r\n",(FLOAT_T)n_xy_num,(FLOAT_T)n_xy_den,(FLOAT_T)n_xy_num/n_xy_den);
      
      temp_r = round(n_f*src_r + (1.0-n_f)*dst_r);
      temp_g = round(n_f*src_g + (1.0-n_f)*dst_g);
      temp_b = round(n_f*src_b + (1.0-n_f)*dst_b);
      temp_k = round((phi_src*src_a +  phi_dst*dst_a)>>8);

      if((temp_r<0) || (temp_g<0) || (temp_b<0) || (temp_k<0)||
	 (temp_r>0xFF) || (temp_g>0xFF) || (temp_b>0xFF) || (temp_k>0xFF))
	{
	  printf("Error: lb_gr_fb_setpixel_ARGB_copymode()--> src=(%d,%d,%d,%d) dst=(%d,%d,%d,%d), tr=%ld, tg=%ld, tb=%ld, tk=%ld\r\n",
		 src_r, src_g, src_b, src_a,
		 dst_r, dst_g, dst_b, dst_a,
		 temp_r, temp_g, temp_b, temp_k);
	  exit(EXIT_FAILURE);
	}

      	dat_offset = (*screen).w*y + x;
	*((U_INT_32_T*)(*screen).dat + dat_offset) =  ((U_INT_8_T)temp_k<<24) | ((U_INT_8_T)temp_r<<16) | ((U_INT_8_T)temp_g<<8) | ((U_INT_8_T)temp_b) ;
    }
  return;
}

void lb_gr_fb_setpixel_ARGB_copymode(SCREEN_T *screen, S_INT_16_T x, S_INT_16_T y, U_INT_8_T src_r, U_INT_8_T src_g, U_INT_8_T src_b, U_INT_8_T src_a, COPYMODE_T copymode)
{
  if ( (x<0) || (x>=(*screen).w) || (y<0) || (y>=(*screen).h) )
    return;
  _lb_gr_fb_setpixel_ARGB_copymode(screen, x, y, src_r, src_g, src_b, src_a, copymode);
}

  
void _lb_gr_fb_setpixel_XRGB(SCREEN_T *screen, S_INT_16_T x, S_INT_16_T y, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b)
{
  U_INT_16_T dat_offset;

  /* Sets a pixel in the screen buffer with no blending and no boundary checks. */
  /* Can corrupt the memory if misused. */
  
  //#define METHOD_1
#ifdef METHOD_1
  /* Performance: 5.02 fps @ 1920x1080 on Raspberry Pi */
  dat_offset=4*((*screen).w*y+x);
  *((unsigned char*)(*screen).dat + dat_offset)     = b;
  *((unsigned char*)(*screen).dat + dat_offset + 1) = g; 
  *((unsigned char*)(*screen).dat + dat_offset + 2) = r;
#else
  /* Performance:  6.56 fps @ 1920x1080 on Raspberry Pi */
  dat_offset=(*screen).w*y + x;
  *((unsigned int*)(*screen).dat + dat_offset) =  r<<16 | g<<8 |  b ;
#endif
  return;
}

void lb_gr_fb_setpixel_XRGB(SCREEN_T *screen, S_INT_16_T x, S_INT_16_T y, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b)
{
  if ( (x<0) || (y<0) || (x>(*screen).w) || (y>(*screen).h) )
    return;
  _lb_gr_fb_setpixel_XRGB(screen, x, y, r, g, b);
}


void lb_gr_fb_line_h(SCREEN_T *screen, S_INT_16_T y0, S_INT_16_T x0, S_INT_16_T x1, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b)
{
  U_INT_32_T l_color, *pos, *pos_a, *pos_b;
  S_INT_16_T x_min, x_max;
  
  if ( (y0<0) || (y0>=(*screen).h) ) return;
  
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

  if (x_max>(*screen).w-1)
    x_max=(*screen).w-1;

  l_color=r<<16 | g<<8 |  b;

  pos_a = (U_INT_32_T*)(*screen).dat + (*screen).w*y0 + x_min;
  pos_b = (U_INT_32_T*)(*screen).dat + (*screen).w*y0 + x_max;;
  for (pos=pos_a; pos<=pos_b; pos++)
    *pos = l_color;
}


void lb_gr_fb_line_h_copymode(SCREEN_T *screen, S_INT_16_T y0, S_INT_16_T x0, S_INT_16_T x1, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b, U_INT_8_T a, COPYMODE_T copymode)
{
  S_INT_16_T x_min, x_max, x;
  
  if ( (y0<0) || (y0>=(*screen).h) ) return;
  
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

  if (x_max>(*screen).w-1)
    x_max=(*screen).w-1;


  for (x=x_min; x<=x_max; x++)
    _lb_gr_fb_setpixel_ARGB_copymode(screen, x, y0, r, g, b, a, copymode);
}


void lb_gr_fb_line_v(SCREEN_T *screen, S_INT_16_T x0, S_INT_16_T y0, S_INT_16_T y1, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b)
{
  U_INT_32_T l_color, *pos, *pos_a, *pos_b;
  S_INT_16_T y_min, y_max;
 
  if ( (x0<0) || (x0>=(*screen).w) ) return;
  
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

  if (y_max>(*screen).h-1)
    y_max=(*screen).h-1;

  l_color = r<<16 | g<<8 |  b;

  pos_a = (U_INT_32_T*)(*screen).dat + (*screen).w*y_min + x0;
  pos_b = (U_INT_32_T*)(*screen).dat + (*screen).w*y_max + x0;

  pos=pos_a;
  while (pos<=pos_b)
    {
      *pos=l_color;
      pos+=(*screen).w;
    }
}


void lb_gr_fb_line_v_copymode(SCREEN_T *screen, S_INT_16_T x0, S_INT_16_T y0, S_INT_16_T y1, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b, U_INT_8_T a, COPYMODE_T copymode)
{
  S_INT_16_T y_min, y_max, y;
 
  if ( (x0<0) || (x0>=(*screen).w) ) return;
  
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

  if (y_max>(*screen).h-1)
    y_max=(*screen).h-1;

  for (y=y_min;y<=y_max;y++)
    _lb_gr_fb_setpixel_ARGB_copymode(screen, x0, y, r, g, b, a, copymode);
}


void lb_gr_draw_pixel(PICTURE_T *Pic, S_INT_16_T x, S_INT_16_T y, PIXEL_T pixel, COPYMODE_T copymode)
{
  U_INT_32_T dat_offset;
  
  if (!copymode) /* This is the Fastest possible mode.  
		    Draws directly to the framebuffer */
    {
	
#if ( (N_BITS_R==8) && (N_BITS_G==8) && (N_BITS_B==8) && (N_BITS_A==8) )
      dat_offset = ty_screen.w*y + x;
      *((U_INT_32_T*)ty_screen.dat + dat_offset) =   pixel.r<<16 | pixel.g<<8 |  pixel.b ;
#else
      dat_offset=4*(ty_screen.w*y+x);
      *((U_INT_8_T*)ty_screen.dat + dat_offset    ) = FACTOR_N_TO_8_B*pixel.b;
      *((U_INT_8_T*)ty_screen.dat + dat_offset + 1) = FACTOR_N_TO_8_G*pixel.g; 
      *((U_INT_8_T*)ty_screen.dat + dat_offset + 2) = FACTOR_N_TO_8_R*pixel.r;
      *((U_INT_8_T*)ty_screen.dat + dat_offset + 3) = 0xFF;
#endif
      return;
    }

  COPYMODE_T temp_copymode;
  temp_copymode = copymode & COPYMODE_COPYMODE_MASK; /* The mode is defined in the 5-lower bits of the 16-bit parameter. */

  if (Pic!=NULL)
    {
      if ( (x<0) || (x>=(*Pic).w) || (y<0) || (y>=(*Pic).h) ) 
	return;
      
      if (temp_copymode==COPYMODE_COPY)
	{
	  (*Pic).pic[y][x]=pixel;
	  return;
	}
      else
	{
	  switch (temp_copymode)
	    {
	    case COPYMODE_AND: /* Performs logical AND between SRC and DST. Alpha remains unchanged. */
	      (*Pic).pic[y][x].r &= pixel.r;
	      (*Pic).pic[y][x].g &= pixel.g;
	      (*Pic).pic[y][x].b &= pixel.b;
	      return;
	    case COPYMODE_OR: /* Performs logical OR between SRC and DST. Alpha remains unchanged. */
	      (*Pic).pic[y][x].r |= pixel.r;
	      (*Pic).pic[y][x].g |= pixel.g;
	      (*Pic).pic[y][x].b |= pixel.b;
	      return;
	    case COPYMODE_XOR: /* Performs logical XOR between SRC and DST. Alpha remains unchanged. */
	      (*Pic).pic[y][x].r ^= pixel.r;
	      (*Pic).pic[y][x].g ^= pixel.g;
	      (*Pic).pic[y][x].b ^= pixel.b;
	      return;
	    case COPYMODE_NOT_SRC: /* Makes DST the logical NOT of SRC. Alpha remains unchanged. */
	      (*Pic).pic[y][x].r = ~pixel.r;
	      (*Pic).pic[y][x].g = ~pixel.g;
	      (*Pic).pic[y][x].b = ~pixel.b;
	      return;
	    case COPYMODE_NOT_DST: /* Performs a logical NOT on DST (regardless of SRC). Alpha remains unchanged. */
	      (*Pic).pic[y][x].r = ~(*Pic).pic[y][x].r;
	      (*Pic).pic[y][x].g = ~(*Pic).pic[y][x].g;
	      (*Pic).pic[y][x].b = ~(*Pic).pic[y][x].b;
	      return;
	    case COPYMODE_ADD: /* Sums each one of the color components but caps the value at the maximum if exceeded. */
	      {
		S_INT_16_T temp; /* Sets the alpha as the average of SRC and DST */
	      
		temp=(*Pic).pic[y][x].r + pixel.r;  
		if (temp>MAX_R)
		  temp=MAX_R;
		(*Pic).pic[y][x].r=temp;

		temp=(*Pic).pic[y][x].g + pixel.g;  
		if (temp>MAX_G)
		  temp=MAX_G;
		(*Pic).pic[y][x].g=temp;

		temp=(*Pic).pic[y][x].b + pixel.b;  
		if (temp>MAX_B)
		  temp=MAX_B;
		(*Pic).pic[y][x].b=temp;
		temp=((*Pic).pic[y][x].a+pixel.a)>>1;
		(*Pic).pic[y][x].a=temp;
		return;
	      }
	    case COPYMODE_SRC_MINUS_DST: /* Substracts each one of the color components of SRC minus DST.  Caps at zero. */
	      {
		S_INT_16_T temp;           /* Sets the alpha as the average of SRC and DST */
	      
		temp = pixel.r - (*Pic).pic[y][x].r;
		if (temp<0)
		  temp=0;
		(*Pic).pic[y][x].r =temp;

		temp = pixel.g - (*Pic).pic[y][x].g;
		if (temp<0)
		  temp=0;
		(*Pic).pic[y][x].g =temp;

		temp = pixel.b - (*Pic).pic[y][x].b;
		if (temp<0)
		  temp=0;
		(*Pic).pic[y][x].b =temp;

		temp = (pixel.a+(*Pic).pic[y][x].a)>>1;
		(*Pic).pic[y][x].a=temp;
		return;
	      }
	    case COPYMODE_DST_MINUS_SRC: /* Substracts each one of the color components of DST minus SRC.  Caps at zero. */
	      {
		S_INT_16_T temp;           /* Sets the alpha as the average of SRC and DST */
	      
		temp=(*Pic).pic[y][x].r - pixel.r;      
		if (temp<0)
		  temp=0;
		(*Pic).pic[y][x].r=temp;

		temp=(*Pic).pic[y][x].g - pixel.g;      
		if (temp<0)
		  temp=0;
		(*Pic).pic[y][x].g=temp;

		temp=(*Pic).pic[y][x].b - pixel.b;      
		if (temp<0)
		  temp=0;
		(*Pic).pic[y][x].b=temp;

		temp = (pixel.a+(*Pic).pic[y][x].a)>>1;
		(*Pic).pic[y][x].a=temp;
		return;
	      }
	    case COPYMODE_AVG: /* Averages each one of the components of SRC and DST as well as the alpha. */
	      (*Pic).pic[y][x].r = ((S_INT_16_T)(*Pic).pic[y][x].r + (S_INT_16_T)pixel.r) >> 1;
	      (*Pic).pic[y][x].g = ((S_INT_16_T)(*Pic).pic[y][x].g + (S_INT_16_T)pixel.g) >> 1;
	      (*Pic).pic[y][x].b = ((S_INT_16_T)(*Pic).pic[y][x].b + (S_INT_16_T)pixel.b) >> 1;
	      return;
	    case COPYMODE_BLEND:
	      (*Pic).pic[y][x].r = (U_INT_32_T)((MAX_A-pixel.a)*(*Pic).pic[y][x].r + pixel.a*pixel.r)/MAX_R;
	      (*Pic).pic[y][x].g = (U_INT_32_T)((MAX_A-pixel.a)*(*Pic).pic[y][x].g + pixel.a*pixel.g)/MAX_G;
	      (*Pic).pic[y][x].b = (U_INT_32_T)((MAX_A-pixel.a)*(*Pic).pic[y][x].b + pixel.a*pixel.b)/MAX_B;
	      //(*Pic).pic[y][x].a = (U_INT_32_T)((MAX_A-pixel.a)*(*Pic).pic[y][x].a + pixel.a*pixel.a)/MAX_A;
	      return;
	    case COPYMODE_MULTIPLY:
	      (*Pic).pic[y][x].r = (U_INT_32_T)(*Pic).pic[y][x].r*pixel.r/MAX_R;
	      (*Pic).pic[y][x].g = (U_INT_32_T)(*Pic).pic[y][x].g*pixel.r/MAX_G;
	      (*Pic).pic[y][x].b = (U_INT_32_T)(*Pic).pic[y][x].b*pixel.r/MAX_B;
	      return;
	    case COPYMODE_SRC_DIV_DST:
	      {
		U_INT_8_T temp;
	      
		if ((*Pic).pic[y][x].r!=0)
		  temp=(U_INT_32_T)(MAX_R*pixel.r/(*Pic).pic[y][x].r);
		else
		  temp=MAX_R;
		(*Pic).pic[y][x].r=temp;

		if ((*Pic).pic[y][x].g!=0)
		  temp=(U_INT_32_T)(MAX_G*pixel.g/(*Pic).pic[y][x].g);
		else
		  temp=MAX_G;
		(*Pic).pic[y][x].g=temp;

		if ((*Pic).pic[y][x].b!=0)
		  temp=(U_INT_32_T)(MAX_B*pixel.b/(*Pic).pic[y][x].b);
		else
		  temp=MAX_B;
		(*Pic).pic[y][x].b=temp;
		return;
	      }
	    case COPYMODE_DST_DIV_SRC:
	      {
		U_INT_8_T temp;
	      
		if (pixel.r!=0)
		  temp=(U_INT_32_T)(MAX_R*(*Pic).pic[y][x].r/pixel.r);
		else
		  temp=MAX_R;
		(*Pic).pic[y][x].r=temp;

		if (pixel.g!=0)
		  temp=(U_INT_32_T)(MAX_G*(*Pic).pic[y][x].g/pixel.g);
		else
		  temp=MAX_G;
		(*Pic).pic[y][x].g=temp;


		if (pixel.b!=0)
		  temp=(U_INT_32_T)(MAX_B*(*Pic).pic[y][x].b/pixel.b);
		else
		  temp=MAX_B;
		(*Pic).pic[y][x].b=temp;
		return;
	      }
	    case COPYMODE_RESERVED0:
	      return;
	    case COPYMODE_RESERVED1:
	      return;
	    case COPYMODE_RESERVED2:
	      return;
	    }

	  S_INT_32_T temp_r, temp_g, temp_b, temp_k, phi_src, phi_dst, n_xy_num, n_xy_den;
	  FLOAT_T n_f;
	  switch (temp_copymode)
	    {
	    case COPYMODE_PORTERDUFF_CLEAR_DST:
	      phi_src=0;       phi_dst=0;
	      break;
	    case COPYMODE_PORTERDUFF_COPY_SRC:
	      phi_src=MAX_A;   phi_dst=0;
	      break;
	    case COPYMODE_PORTERDUFF_LEAVES_DST:
	      phi_src=0;       phi_dst=MAX_A;
	      break;
	    case COPYMODE_PORTERDUFF_SRC_OVER_DST:
	      phi_src=MAX_A;   phi_dst=MAX_A-pixel.a;
	      break;
	    case COPYMODE_PORTERDUFF_DST_OVER_SRC:
	      phi_src=MAX_A-(*Pic).pic[y][x].a;   phi_dst=MAX_A;
	      break;
	    case COPYMODE_PORTERDUFF_SRC_IN_DST:
	      phi_src=(*Pic).pic[y][x].a;   phi_dst=0;
	      break;
	    case COPYMODE_PORTERDUFF_DST_IN_SRC:
	      phi_src=0;   phi_dst=pixel.a;
	      break;
	    case COPYMODE_PORTERDUFF_SRC_OUT_DST:
	      phi_src=MAX_A-(*Pic).pic[y][x].a;   phi_dst=0;
	      break;
	    case COPYMODE_PORTERDUFF_DST_OUT_SRC:
	      phi_src=0;   phi_dst=MAX_A-pixel.a;
	      break;
	    case COPYMODE_PORTERDUFF_SRC_ATOP_DST:
	      phi_src=(*Pic).pic[y][x].a;   phi_dst=MAX_A-pixel.a;
	      break;
	    case COPYMODE_PORTERDUFF_DST_ATOP_SRC:
	      phi_src=MAX_A-(*Pic).pic[y][x].a;   phi_dst=pixel.a;
	      break;
	    case COPYMODE_PORTERDUFF_XOR:
	      phi_src=MAX_A-(*Pic).pic[y][x].a;   phi_dst=MAX_A-pixel.a;
	      break;
	    }
	  n_xy_num= phi_src*pixel.a;
	  n_xy_den = n_xy_num + phi_dst*(*Pic).pic[y][x].a;
	  if (n_xy_den!=0)
	    {
	      n_f=(FLOAT_T)n_xy_num/(FLOAT_T)n_xy_den;
	      //lb_ft_printf(ty_C, "n_xy= %f / %f =%f\r\n",(FLOAT_T)n_xy_num,(FLOAT_T)n_xy_den,(FLOAT_T)n_xy_num/n_xy_den);
      
	      temp_r = round(n_f*pixel.r + (1.0-n_f)*(*Pic).pic[y][x].r);
	      temp_g = round(n_f*pixel.g + (1.0-n_f)*(*Pic).pic[y][x].g);
	      temp_b = round(n_f*pixel.b + (1.0-n_f)*(*Pic).pic[y][x].b);
	      temp_k = round((phi_src*pixel.a +  phi_dst*(*Pic).pic[y][x].a)/MAX_A);

	      if((temp_r<0) || (temp_g<0) || (temp_b<0) || (temp_k<0)||
		 (temp_r>MAX_R) || (temp_g>MAX_G) || (temp_b>MAX_B) || (temp_k>MAX_A))
		{
		  //lb_ft_printf(ty_C, "Error: lb_gr_draw_pixel()--> src=(%d,%d,%d,%d) dst=(%d,%d,%d,%d), tr=%d, tg=%d, tb=%d, tk=%d\r\n",
		  //		     src.r,src.g,src.b,src.a,    dst.r,dst.g,dst.b,dst.a, temp_r, temp_g, temp_b, temp_k);
		  //lb_fb_exit(1);
		}
	      (*Pic).pic[y][x].r=temp_r;
	      (*Pic).pic[y][x].g=temp_g;
	      (*Pic).pic[y][x].b=temp_b;
	      (*Pic).pic[y][x].a=temp_k;
	    }
	}
    }
  else /* Copying directly to the screen */
    {
      if (!(copymode & (COPYMODE_SCALEX_MASK | COPYMODE_SCALEY_MASK))) /* size == 1 */
#if ( (N_BITS_R==8) && (N_BITS_G==8) && (N_BITS_B==8) && (N_BITS_A==8) )
	lb_gr_fb_setpixel_ARGB_copymode(&ty_screen, x, y, pixel.r, pixel.g, pixel.b, pixel.a, copymode);
#else
	lb_gr_fb_setpixel_ARGB_copymode(&ty_screen, x, y, FACTOR_N_TO_8_R*pixel.r, FACTOR_N_TO_8_G*pixel.g, FACTOR_N_TO_8_B*pixel.b, FACTOR_N_TO_8_A*pixel.a, copymode);
#endif
      
      else /* if size != 1, e.g., a scale factor is being used */
	{
	  U_INT_8_T scale_x, scale_y;
	  scale_x = 1 + ((copymode & COPYMODE_SCALEX_MASK) >> COPYMODE_SCALEX_SHIFT );
	  scale_y = 1 + ((copymode & COPYMODE_SCALEY_MASK) >> COPYMODE_SCALEY_SHIFT);
	  //printf("copymode=0x%x, scale_x=%d, scale_y=%d\r\n",copymode,scale_x, scale_y);
	  //printf("%x\r\n",copymode);

	  switch (copymode & COPYMODE_PIXELMODE_MASK)
	    {
	    case COPYMODE_PIXELMODE_0: /* solid rectangle */
      
#if ( (N_BITS_R==8) && (N_BITS_G==8) && (N_BITS_B==8) && (N_BITS_A==8) )
	      lb_gr_fb_rectangle_copymode(&ty_screen, x*scale_x, y*scale_y, (x+1)*scale_x-1, (y+1)*scale_y-1,
					  pixel.r, pixel.g, pixel.b, pixel.a, copymode);
#else
	      lb_gr_fb_rectangle_copymode(&ty_screen, x*scale_x, y*scale_y, (x+1)*scale_x-1, (y+1)*scale_y-1,
					  FACTOR_N_TO_8_R*pixel.r, FACTOR_N_TO_8_G*pixel.g, FACTOR_N_TO_8_B*pixel.b, FACTOR_N_TO_8_A*pixel.a, copymode);
#endif
	      break;
	    case COPYMODE_PIXELMODE_1:
	      {
		U_INT_8_T border_r, border_g, border_b;
		border_r = ((copymode & COPYMODE_PIXELBG_R_MASK) >> COPYMODE_PIXELBG_R_SHIFT)*255/15;
		border_g = ((copymode & COPYMODE_PIXELBG_G_MASK) >> COPYMODE_PIXELBG_G_SHIFT)*255/15;
		border_b = ((copymode & COPYMODE_PIXELBG_B_MASK) >> COPYMODE_PIXELBG_B_SHIFT)*255/15;
		
		lb_gr_fb_line_h_copymode(&ty_screen,  y   *scale_y, x*scale_x,   (x+1)*scale_x,   border_r, border_g, border_b, 255, copymode);
		lb_gr_fb_line_h_copymode(&ty_screen, (y+1)*scale_y, x*scale_x,   (x+1)*scale_x,   border_r, border_g, border_b, 255, copymode);
		lb_gr_fb_line_v_copymode(&ty_screen,  x   *scale_x, y*scale_y+1, (y+1)*scale_y-1, border_r, border_g, border_b, 255, copymode);
		lb_gr_fb_line_v_copymode(&ty_screen, (x+1)*scale_x, y*scale_y+1, (y+1)*scale_y-1, border_r, border_g, border_b, 255, copymode );
		
#if ( (N_BITS_R==8) && (N_BITS_G==8) && (N_BITS_B==8) && (N_BITS_A==8) )
		lb_gr_fb_rectangle_copymode(&ty_screen, x*scale_x+1, y*scale_y+1, (x+1)*scale_x-1, (y+1)*scale_y-1, pixel.r, pixel.g, pixel.b, pixel.a, copymode);
#else
		lb_gr_fb_rectangle_copymode(&ty_screen, x*scale_x+1, y*scale_y+1, (x+1)*scale_x-1, (y+1)*scale_y-1,
					    FACTOR_N_TO_8_R*pixel.r, FACTOR_N_TO_8_G*pixel.g, FACTOR_N_TO_8_B*pixel.b, FACTOR_N_TO_8_A*pixel.a, copymode);
#endif
	      }
	      break;
	    case 2:
	      {
		U_INT_8_T border_r, border_g, border_b;
		border_r = ((copymode & COPYMODE_PIXELBG_R_MASK) >> COPYMODE_PIXELBG_R_SHIFT)*255/15;
		border_g = ((copymode & COPYMODE_PIXELBG_G_MASK) >> COPYMODE_PIXELBG_G_SHIFT)*255/15;
		border_b = ((copymode & COPYMODE_PIXELBG_B_MASK) >> COPYMODE_PIXELBG_B_SHIFT)*255/15;
		
		
	
		lb_gr_fb_line_h_copymode(&ty_screen,  y     *scale_y, x*scale_x,   (x+1)*scale_x-1,   border_r, border_g, border_b, 255, copymode);
		lb_gr_fb_line_h_copymode(&ty_screen, (y+1)*scale_y-1, x*scale_x,   (x+1)*scale_x-1,   border_r, border_g, border_b, 255, copymode);
		lb_gr_fb_line_v_copymode(&ty_screen,  x   *scale_x, y*scale_y+1, (y+1)*scale_y-2, border_r, border_g, border_b, 255, copymode);
		lb_gr_fb_line_v_copymode(&ty_screen, (x+1)*scale_x-1, y*scale_y+1, (y+1)*scale_y-2, border_r, border_g, border_b, 255, copymode );
		
		lb_gr_fb_rectangle_copymode(&ty_screen, x*scale_x+1, y*scale_y+1, (x+1)*scale_x-2, (y+1)*scale_y-2, pixel.r, pixel.g, pixel.b, pixel.a, copymode);
	      }
	      break;
	    case 3:
	      break;
	    case 4:
	      break;
	    case 5:
	      break;
	    case 6:
	      break;
	    case 7:
	      break;
	    }
	}
    }
}
 

void lb_gr_draw_polygon_i(PICTURE_T *Pic, LINE_2D_INT_T *L, FLOAT_T w, PIXEL_T color, COPYMODE_T copymode, LINEMODE_T linemode)
{
  S_INT_16_T k;

  switch(linemode)
    {
    case LINEMODE_DOTS_SOLID:
      for(k=0;k<(*L).items;k++)
	if ((*L).array[k].flag)
	  {
	    if(w<=1.0)
	      lb_gr_draw_pixel(Pic, (*L).array[k].x, (*L).array[k].y, color, copymode);
	    else
	      lb_gr_draw_circle_filled_fast(Pic, (*L).array[k].x, (*L).array[k].y, w, color, copymode);
	  }
      break;
    case LINEMODE_DOTS_FILTERED:
      for(k=0;k<(*L).items;k++)
	if ((*L).array[k].flag)
	  lb_gr_draw_circle_filled_antialiasing(Pic, (*L).array[k].x, (*L).array[k].y, w, color, copymode);
      break;
    default:
      for(k=0;k<(*L).items-1;k++)
	if ((*L).array[k].flag && (*L).array[k+1].flag)
	  lb_gr_draw_line(Pic, (*L).array[k].x, (*L).array[k].y, (*L).array[k+1].x,(*L).array[k+1].y, w, color, copymode, linemode);
      break;
    }
}

void lb_gr_draw_polygon_f(PICTURE_T *Pic, LINE_2D_FLOAT_T *L, FLOAT_T w, PIXEL_T color, COPYMODE_T copymode, LINEMODE_T linemode)
{
  S_INT_16_T k;

  switch(linemode)
    {
    case LINEMODE_DOTS_SOLID:
      for(k=0;k<(*L).items;k++)
	if ((*L).array[k].flag)
	  {
	    if(w<=1.0)
	      lb_gr_draw_pixel(Pic, round((*L).array[k].x), round((*L).array[k].y), color, copymode);
	    else
	      for(k=0;k<(*L).items;k++)
		lb_gr_draw_circle_filled_fast(Pic, (*L).array[k].x, (*L).array[k].y, w, color, copymode);
	  }
      break;
    case LINEMODE_DOTS_FILTERED:
      for(k=0;k<(*L).items;k++)
	if ((*L).array[k].flag)
	  lb_gr_draw_circle_filled_antialiasing(Pic, (*L).array[k].x, (*L).array[k].y, w, color, copymode); 
      break;
    default:
      for (k=0;k<(*L).items-1;k++)
	if ((*L).array[k].flag && (*L).array[k+1].flag)
	  lb_gr_draw_line(Pic, (*L).array[k].x, (*L).array[k].y, (*L).array[k+1].x,(*L).array[k+1].y, w,
			  color, copymode, linemode);
      break;
    }
}

void lb_gr_draw_triangle_fill_i(PICTURE_T *Pic, POINT_2D_INT_T P0, POINT_2D_INT_T P1, POINT_2D_INT_T P2,
				PIXEL_T color, COPYMODE_T copymode)
{
  // http://www-users.mat.uni.torun.pl/~wrona/3d_tutor/tri_fillers.html
  POINT_2D_INT_T A, B, C, S, E, P_temp;
  S_INT_16_T i, x_start, x_end;
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
  lb_gr_draw_pixel(Pic, A.x, A.y , color, copymode);
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
		    lb_gr_draw_pixel(Pic, E.x, E.y, color, copymode);
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
		  i=x_start+1;
		  while (i<x_end)
		    {
		      lb_gr_draw_pixel(Pic, i, E.y, color,  copymode);
		      i++;
		    }
		}
	    } while (E.y!=S.y);
	  /***************************/
	}
      lb_gr_draw_pixel(Pic, S.x, S.y, color, copymode);
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
		    lb_gr_draw_pixel(Pic, E.x, E.y, color, copymode);
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
		  i=x_start+1;
		  while (i<x_end)
		    {
		      lb_gr_draw_pixel(Pic, i, E.y, color, copymode);
		      i++;
		    }
		}
	    } while (E.y!=S.y);
	  /***************************/
	}
      lb_gr_draw_pixel(Pic, S.x, S.y, color, copymode);
      //lb_gr_delay(500);
    } while ((S.x!=C.x) || (S.y!=C.y));
  
  //lb_gr_draw_pixel(NULL, A.x, A.y, lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_YELLOW), copymode);
  //lb_gr_draw_pixel(NULL, B.x, B.y, lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_YELLOW), copymode);
  //lb_gr_draw_pixel(NULL, C.x, C.y, lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_YELLOW), copymode);
}


#ifdef NADA

void lb_gr_draw_triangle_fill_i(PICTURE_T *Pic, POINT_2D_INT_T P0, POINT_2D_INT_T P1, POINT_2D_INT_T P2, PIXEL_T color, COPYMODE_T copymode)
{
  // http://www-users.mat.uni.torun.pl/~wrona/3d_tutor/tri_fillers.html
  POINT_2D_INT_T A, B, C, P_temp;
  S_INT_16_T i, x_start, x_end, line_y, temp;
  
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

  lb_ft_printf(ty_C, "A = ( %d , %d )\r\n",A.x,A.y); 
  lb_ft_printf(ty_C, "B = ( %d , %d )\r\n",B.x,B.y); 
  lb_ft_printf(ty_C, "C = ( %d , %d )\r\n",C.x,C.y); 
  
  line_y=A.y;
  
  while (line_y<=B.y)
    {
      if (A.y!=B.y)
	x_start = B.x+(line_y-B.y)*(A.x-B.x)/(A.y-B.y);
      else
	{
	  x_start=A.x;
	  x_end=B.x;
	}
      if (A.y!=C.y)
	x_end   = C.x+(line_y-C.y)*(A.x-C.x)/(A.y-C.y);
      else
	{
	  if(A.x<=B.x)
	    x_start=A.x;
	  else
	    x_start=B.x;
	  if(C.x<x_start)
	    x_start=C.x;

	  if(A.x>=B.x)
	    x_end=A.x;
	  else
	    x_end=B.x;
	  if(C.x>x_end)
	    x_end=C.x;
	}
      lb_ft_printf(ty_C, "1) x_start= %d,  x_end=%d\r\n",x_start,x_end); 
      
      if (x_start>x_end)
	{
	  temp=x_end;
	  x_end=x_start;
	  x_start=temp;
	}
      	    
      for(i=x_start;i<=x_end;i++)
	lb_gr_draw_pixel(Pic, i, line_y, color, copymode);
      line_y++;
    }
  while (line_y <= C.y)
    {
      x_start = C.x+(line_y-C.y)*(B.x-C.x)/(B.y-C.y);
      x_end   = C.x+(line_y-C.y)*(A.x-C.x)/(A.y-C.y);
     
      lb_ft_printf(ty_C, "2) x_start= %d,  x_end=%d\r\n",x_start,x_end); 
   
      if (x_start>x_end)
	{
	  temp=x_end;
	  x_end=x_start;
	  x_start=temp;
	}
      for(i=x_start;i<=x_end;i++)
	lb_gr_draw_pixel(Pic, i, line_y, color, copymode);

      line_y++;
    }
}

void lb_gr_draw_triangle_fill_i(PICTURE_T *Pic, POINT_2D_FLOAT_T P0, POINT_2D_FLOAT_T P1, POINT_2D_FLOAT_T P2, PIXEL_T color, COPYMODE_T copymode)
{
  // http://www-users.mat.uni.torun.pl/~wrona/3d_tutor/tri_fillers.html
  POINT_2D_FLOAT_T A, B, C, P_temp;
  FLOAT_T i, x_start, x_end, line_y, temp;
  
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

  lb_ft_printf(ty_C, "A = ( %f , %f )\r\n",A.x,A.y); 
  lb_ft_printf(ty_C, "B = ( %f , %f )\r\n",B.x,B.y); 
  lb_ft_printf(ty_C, "C = ( %f , %f )\r\n",C.x,C.y); 
  
  line_y=A.y;
  
  while (line_y<=B.y)
    {
      x_start = B.x+0.5+(line_y-0.5-B.y)*(A.x-B.x)/(A.y-B.y);
      x_end   = C.x+0.5+(line_y-0.5-C.y)*(A.x-C.x)/(A.y-C.y);

      if
	lb_ft_printf(ty_C, "1) x_start= %f,  x_end=%f\r\n",x_start,x_end); 
      
      //lb_gr_draw_pixel(Pic,x_start, line_y,lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_RED),  COPYMODE_BLEND);
      // lb_gr_draw_pixel(Pic,x_end,   line_y,lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_BLUE), COPYMODE_BLEND);
      if (x_start>x_end)
	{
	  temp=x_end;
	  x_end=x_start;
	  x_start=temp;
	}
	    
      for(i=round(x_start);i<round(x_end);i++)
	lb_gr_draw_pixel(Pic,round(i), round(line_y),lb_gr_12RGB(COLOR_SOLID | COLOR_WHITE),  COPYMODE_BLEND);
      line_y++;
      // lb_gr_delay(10);
    }
  while (line_y <= C.y)
    {
      x_start = C.x+0.5+(line_y-0.5-C.y)*(B.x-C.x)/(B.y-C.y);
      x_end   = C.x+0.5+(line_y-0.5-C.y)*(A.x-C.x)/(A.y-C.y);
     
      lb_ft_printf(ty_C, "2) x_start= %f,  x_end=%f\r\n",x_start,x_end); 
   
      if (x_start>x_end)
	{
	  temp=x_end;
	  x_end=x_start;
	  x_start=temp;
	}
      for(i=round(x_start);i<round(x_end);i++)
	lb_gr_draw_pixel(Pic, round(i), round(line_y),lb_gr_12RGB(COLOR_SOLID | COLOR_GREEN),  COPYMODE_BLEND);

      //lb_gr_draw_pixel(Pic,x_start, line_y,lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_RED),  COPYMODE_BLEND);
      //lb_gr_draw_pixel(Pic,x_end,   line_y,lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_BLUE), COPYMODE_BLEND);
      line_y++;
      // lb_gr_delay(10);
    }
  //lb_gr_draw_line1_f(NULL, A.x, A.y, B.x, B.y, lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_BLUE), COPYMODE_BLEND);
  //lb_gr_draw_line1_f(NULL, B.x, B.y, C.x, C.y, lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_BLUE), COPYMODE_BLEND);
  //lb_gr_draw_line1_f(NULL, C.x, C.y, A.x, A.y, lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_BLUE), COPYMODE_BLEND);
  lb_gr_draw_pixel(NULL, A.x, A.y, lb_gr_12RGB(COLOR_YELLOW), copymode);
  lb_gr_draw_pixel(NULL, B.x, B.y, lb_gr_12RGB(COLOR_YELLOW), copymode);
  lb_gr_draw_pixel(NULL, C.x, C.y, lb_gr_12RGB(COLOR_YELLOW), copymode);


}
void lb_gr_draw_triangle_fill_i(PICTURE_T *Pic, POINT_2D_FLOAT_T P0, POINT_2D_FLOAT_T P1, POINT_2D_FLOAT_T P2, PIXEL_T color, COPYMODE_T copymode)
{
  // http://www-users.mat.uni.torun.pl/~wrona/3d_tutor/tri_fillers.html
  POINT_2D_FLOAT_T A, B, C, P_temp;
  FLOAT_T i, x_start, x_end, line_y, temp;
  
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

  lb_ft_printf(ty_C, "A = ( %f , %f )\r\n",A.x,A.y); 
  lb_ft_printf(ty_C, "B = ( %f , %f )\r\n",B.x,B.y); 
  lb_ft_printf(ty_C, "C = ( %f , %f )\r\n",C.x,C.y); 
  
  line_y=A.y;
  
  while (line_y<=B.y)
    {
      x_start = B.x+(line_y-B.y)*(A.x-B.x)/(A.y-B.y);
      x_end   = C.x+(line_y-C.y)*(A.x-C.x)/(A.y-C.y);

      
      lb_ft_printf(ty_C, "1) x_start= %f,  x_end=%f\r\n",x_start,x_end); 
      
      //lb_gr_draw_pixel(Pic,x_start, line_y,lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_RED),  COPYMODE_BLEND);
      // lb_gr_draw_pixel(Pic,x_end,   line_y,lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_BLUE), COPYMODE_BLEND);
      if (x_start>x_end)
	{
	  temp=x_end;
	  x_end=x_start;
	  x_start=temp;
	}
	    
      for(i=round(x_start);i<=round(x_end);i++)
	lb_gr_draw_pixel(Pic,i, round(line_y),lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_WHITE),  COPYMODE_BLEND);
      line_y++;
      //lb_gr_delay(10);
    }
  while (line_y <= C.y)
    {
      x_start = C.x+(line_y-C.y)*(B.x-C.x)/(B.y-C.y);
      x_end   = C.x+(line_y-C.y)*(A.x-C.x)/(A.y-C.y);
     
      lb_ft_printf(ty_C, "2) x_start= %f,  x_end=%f\r\n",x_start,x_end); 
   
      if (x_start>x_end)
	{
	  temp=x_end;
	  x_end=x_start;
	  x_start=temp;
	}
      for(i=round(x_start);i<=round(x_end);i++)
	lb_gr_draw_pixel(Pic,i, round(line_y),lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_GRAY),  COPYMODE_BLEND);

      //lb_gr_draw_pixel(Pic,x_start, line_y,lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_RED),  COPYMODE_BLEND);
      //lb_gr_draw_pixel(Pic,x_end,   line_y,lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_BLUE), COPYMODE_BLEND);
      line_y++;
      //lb_gr_delay(10);
    }  
 

  //lb_gr_draw_line1_f(NULL, A.x, A.y, B.x, B.y, lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_YELLOW), COPYMODE_BLEND);
  //lb_gr_draw_line1_f(NULL, B.x, B.y, C.x, C.y, lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_YELLOW), COPYMODE_BLEND);
  //lb_gr_draw_line1_f(NULL, C.x, C.y, A.x, A.y, lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_YELLOW), COPYMODE_BLEND);
  lb_gr_draw_pixel(NULL, A.x, A.y, lb_gr_12RGB(COLOR_YELLOW), copymode);
  lb_gr_draw_pixel(NULL, B.x, B.y, lb_gr_12RGB(COLOR_YELLOW), copymode);
  lb_gr_draw_pixel(NULL, C.x, C.y, lb_gr_12RGB(COLOR_YELLOW), copymode);
}
#endif



void lb_gr_draw_polygon_fill_i(PICTURE_T *Pic, LINE_2D_INT_T *L, PIXEL_T color, COPYMODE_T copymode)  
{
  S_INT_16_T i, j, image_top, image_bottom, pixel_x, pixel_y, nodes, swap, nodeX[20];
  
  /* make sure the polygon is a closed shape */
  if ( ((*L).array[0].x != (*L).array[(*L).items-1].x) || ((*L).array[0].y != (*L).array[(*L).items-1].y))
    {
      lb_ft_printf(ty_C, "invalid shape (not closed) --> lb_gr_draw_polygon_fill\r\n");
      return;
    }
  
  image_top=(*L).array[0].y;
  image_bottom=(*L).array[0].y;
  
  for (i=1;i<(*L).items;i++)
    {
      if ((*L).array[i].y<image_top)
	image_top=(*L).array[i].y;
      if ((*L).array[i].y>image_bottom)
	image_bottom=(*L).array[i].y;
    }

  for(pixel_y=image_top;pixel_y<image_bottom;pixel_y++)
    {
      nodes=0;
      j=(*L).items-1;
      
      for (i=0; i<(*L).items; i++) 
	{
	  if ( ((*L).array[i].y<(double)pixel_y && (*L).array[j].y>=(double)pixel_y) || 
	       ((*L).array[j].y<(double)pixel_y && (*L).array[i].y>=(double)pixel_y) )
	    {
	      nodeX[nodes]=llroundf((*L).array[i].x + (pixel_y-(*L).array[i].y)*((*L).array[j].x-(*L).array[i].x)/
				    ((*L).array[j].y-(*L).array[i].y));
	      nodes++;
	    }
	  j=i;
	}
      
      i=0;
      while (i<nodes-1) 
	{
	  if (nodeX[i]>nodeX[i+1]) 
	    {
	      swap=nodeX[i]; 
	      nodeX[i]=nodeX[i+1]; 
	      nodeX[i+1]=swap; 
	      if(i) 
		i--; 
	    }
	  else 
	    {
	      i++; 
	    }
	}
      for (i=0;i<nodes;i+=2) 
	for (pixel_x=nodeX[i]+1; pixel_x<=nodeX[i+1]; pixel_x++)
	  lb_gr_draw_pixel(Pic, pixel_x, pixel_y, color, copymode);  
    }
}

void lb_gr_draw_polygon_antialiasing(PICTURE_T *Pic, LINE_2D_FLOAT_T *L, FLOAT_T w, PIXEL_T color, COPYMODE_T copymode)
{
  S_INT_16_T i, j, k, width, height;
  FLOAT_T distance, temp, w_2, t0, tmp1, tmp2, tmp3;
  ERR_T error;
  PIXEL_T pixel;

  pixel=color;
  w_2=0.5*w;

  if (Pic==NULL)
    {
      width=ty_screen.w;
      height=ty_screen.h;
    }
  else
    {
      height=(*Pic).h;
      width =(*Pic).w;
    }
  
  for(i=0;i<height;i++)
    for(j=0;j<width;j++)
      {
	distance=1.0e37;
	error=e_none;
	for (k=0;k<(*L).items-1;k++)
	  {
	    
	    /* First, check the distance to each one of the extremes of the line segment */
	    temp=lb_re_sqrt(lb_re_sqr(j-(*L).array[k].x,&error)+lb_re_sqr(i-(*L).array[k].y,&error),&error);
	    if((error==e_none) && (temp<distance))
	      distance=temp;

	    temp=lb_re_sqrt(lb_re_sqr(j-(*L).array[k+1].x,&error)+lb_re_sqr(i-(*L).array[k+1].y,&error),&error);
	    if((error==e_none) && (temp<distance))
	      distance=temp;

	    /* Second, check the distance from an imaginary line passing both points */

	    tmp1=(*L).array[k+1].x-(*L).array[k].x;  /* (x1 - x0) */
	    tmp2=(*L).array[k+1].y-(*L).array[k].y;  /* (y1 - y0) */

	    tmp3=tmp1*tmp1+tmp2*tmp2;

	    /* (y1-y0)*xa - (x1-x0)*ya + x1*y0 - y1*x0 */
	    temp=tmp2*j-tmp1*i + (*L).array[k+1].x*(*L).array[k].y - (*L).array[k+1].y*(*L).array[k].x;
	    temp=fabs(temp)/lb_re_sqrt(tmp3,&error);
	    t0=(tmp1*(j-(*L).array[k].x)+tmp2*(i-(*L).array[k].y))/tmp3;
	    if((error==e_none) && (temp<distance) && (t0>=0.0) && (t0<=1.0))
	      distance=temp;
	  }
	pixel.a=0;
	if(distance<=w_2)
	  pixel.a=MAX_A;
	else
	  if(distance<(w_2+1.0))
	    pixel.a=MAX_A*(1.0-lb_re_frac(distance-w_2));
	lb_gr_draw_pixel(Pic, j, i, pixel, copymode);  
      }
}

void lb_gr_draw_rectangle(PICTURE_T *Pic, S_INT_16_T x0, S_INT_16_T y0, S_INT_16_T x1, S_INT_16_T y1,
			  PIXEL_T color, COPYMODE_T copymode)
{
  S_INT_16_T x_l, x_h, y_l, y_h, i, j;

  if (x0<=x1)
    {
      x_l=x0;
      x_h=x1;
    }
  else
    {
      x_l=x1;
      x_h=x0;
    }

  if (y0<=y1)
    {
      y_l=y0;
      y_h=y1;
    }
  else
    {
      y_l=y1;
      y_h=y0;
    }

  for (i=y_l;i<=y_h;i++)
    for (j=x_l;j<=x_h;j++)
      lb_gr_draw_pixel(Pic, j, i, color, copymode);
}

void lb_gr_draw_rectangle_bar(PICTURE_T *Pic, S_INT_16_T x0, S_INT_16_T y0, S_INT_16_T x1, S_INT_16_T y1, S_INT_16_T w,
			      PIXEL_T color_line, PIXEL_T color_background,  COPYMODE_T copymode)
{
  S_INT_16_T x_l, x_h, y_l, y_h, i, j;

  if (x0<=x1)
    {
      x_l=x0;
      x_h=x1;
    }
  else
    {
      x_l=x1;
      x_h=x0;
    }

  if (y0<=y1)
    {
      y_l=y0;
      y_h=y1;
    }
  else
    {
      y_l=y1;
      y_h=y0;
    }
  
  lb_gr_draw_rectangle_line(Pic, x0, y0, x1, y1, w, color_line, copymode);
  for (i=y_l+w;i<=(y_h-w);i++)
    for (j=x_l+w;j<=(x_h-w);j++)
      lb_gr_draw_pixel(Pic, j, i, color_background, copymode);
}


void lb_gr_draw_rectangle_line(PICTURE_T *Pic, S_INT_16_T x0, S_INT_16_T y0, S_INT_16_T x1, S_INT_16_T y1, S_INT_16_T w,
			       PIXEL_T color, COPYMODE_T copymode)
{
  S_INT_16_T x_l, x_h, y_l, y_h, i, j;

  if (x0<=x1)
    {
      x_l=x0;
      x_h=x1;
    }
  else
    {
      x_l=x1;
      x_h=x0;
    }

  if (y0<=y1)
    {
      y_l=y0;
      y_h=y1;
    }
  else
    {
      y_l=y1;
      y_h=y0;
    }

  for (i=0;i<=w;i++)
    for (j=x_l;j<=x_h;j++)
      {
	if ( (y_l+i)<=y_h )
	  lb_gr_draw_pixel(Pic, j, y_l + i, color, copymode);
	if ( (y_h-i)>=y_l)
	  lb_gr_draw_pixel(Pic, j, y_h - i, color, copymode);
      }

  for (j=0;j<=w;j++)
    for (i=y_l+w+1;i<=y_h-w-1;i++)
      {
	if ( (x_l+j)<=x_h )
	  lb_gr_draw_pixel(Pic, x_l + j, i, color, copymode);
	if ( (x_h-j)>=x_l)
	  lb_gr_draw_pixel(Pic, x_h - j, i, color, copymode);
      }
}


void lb_gr_draw_rectangle_solid(PICTURE_T *Pic, S_INT_16_T x0, S_INT_16_T y0, S_INT_16_T x1, S_INT_16_T y1,
				PIXEL_T color)
{
  S_INT_16_T x_l, x_h, y_l, y_h, i, j;

  if (x0<=x1)
    {
      x_l=x0;
      x_h=x1;
    }
  else
    {
      x_l=x1;
      x_h=x0;
    }

  if (y0<=y1)
    {
      y_l=y0;
      y_h=y1;
    }
  else
    {
      y_l=y1;
      y_h=y0;
    }
  
  if( Pic==NULL)
    {
      //      lb_fb_rectangle(ty_fb_main,x0, y0, ty_scale_x*(x1-x0),ty_scale_y*(y1-y0),
      //	      round(FACTOR_N_TO_8_R*color.r),
      //	      round(FACTOR_N_TO_8_G*color.g),
      //	      round(FACTOR_N_TO_8_B*color.b),
      //	      round(FACTOR_N_TO_8_K*color.a) );

      //      if (lb_fb_use_shadow())
      //	for (i=y_l;i<=y_h;i++)
      //  for (j=x_l;j<=x_h;j++)
      //    ty_pic_shadow.pic[i][j]=color;
      
      // else
      {
	for (i=y_l;i<=y_h;i++)
	  for (j=x_l;j<=x_h;j++)
	    (*Pic).pic[i][j]=color;
      }
    }
}

#ifdef NADA
void  lb_gr_draw_rectangle(PICTURE_T *Pic, S_INT_16_T x0, S_INT_16_T y0, S_INT_16_T x1, S_INT_16_T y1,
			   S_INT_16_T width, PIXEL_T color, COPYMODE_T copymode)
{
  S_INT_16_T x_l, x_h, y_l, y_h, i, j, max_width_x, max_width_y;

  if(width<=0)
    return;

  if ((*P)!=NULL)
    {
      if (x0<=x1)
	{
	  x_l=x0;
	  x_h=x1;
	}
      else
	{
	  x_l=x1;
	  x_h=x0;
	}

      if (y0<=y1)
	{
	  y_l=y0;
	  y_h=y1;
	}
      else
	{
	  y_l=y1;
	  y_h=y0;
	}

      max_width_x=width;
      if((x_h-x_l)/2<=width)
	max_width_x=(x_h-x_l)/2;

      max_width_y=width;
      if((y_h-y_l)/2<=width)
	max_width_y=(y_h-y_l)/2;
   
      if(max_width_x==0)
	max_width_x=1;

      if(max_width_y==0)
	max_width_y=1;

      //lb_ft_printf(ty_C, "max_width_x=%d\r\n",max_width_x);
      //lb_ft_printf(ty_C, "max_width_y=%d\r\n",max_width_y);


      for (i=0;i<max_width_y;i++)
	for (j=x_l;j<=x_h;j++)
	  {
	    lb_gr_draw_pixel(Pic, j, y_l + i, color, copymode);
	    lb_gr_draw_pixel(Pic, j, y_h - i, color, copymode);
	  }
   
      for (j=0;j<max_width_x;j++)
	for (i=y_l+max_width_y;i<=(y_h-max_width_y);i++)
	  {
	    lb_gr_draw_pixel(Pic, x_l + j, i, color, copymode);
	    lb_gr_draw_pixel(Pic, x_h - j, i, color, copymode);
	  }
	
    }
}

void lb_gr_draw_rectangle_fill(PICTURE_T *Pic, S_INT_16_T x0, S_INT_16_T y0, S_INT_16_T x1, S_INT_16_T y1,
			       S_INT_16_T width, PIXEL_T color_line, PIXEL_T color_inside, COPYMODE_T copymode)
{
  S_INT_16_T x_l, x_h, y_l, y_h, i, j;
   
  if (x0<=x1)
    {
      x_l=x0;
      x_h=x1;
    }
  else
    {
      x_l=x1;
      x_h=x0;
    }

  if (y0<=y1)
    {
      y_l=y0;
      y_h=y1;
    }
  else
    {
      y_l=y1;
      y_h=y0;
    }
  for (i=y_l;i<=y_h;i++)
    for (j=x_l;j<=x_h;j++)
      if ((j>=(x_l+width)) && (j<=(x_h-width)) && (i>=(y_l+width)) && (i<=(y_h-width)))
	lb_gr_draw_pixel(Pic, j, i, color_inside, copymode);
      else
	lb_gr_draw_pixel(Pic, j, i, color_line, copymode);
}
 
#endif
 

S_INT_16_T lb_gr_is_in_polygon_i(LINE_2D_INT_T *L, POINT_2D_INT_T P)
{
  S_INT_16_T k, wn; /* the  winding number counter */

  if (!lb_gr_assert_dimensions_line2d_i(L))
    {
      printf("Error: lb_gr_is_in_polygon_i() --> invalid dimension\r\n");
      exit(EXIT_FAILURE);
    }
  
  wn=0;
  for (k=0;k<(*L).items-1;k++)
    {   // edge from V[i] to  V[i+1]
      if ((*L).array[k].y <= P.y)
	{
	  // start y <= P.y
	  if ((*L).array[k+1].y > P.y)      // an upward crossing
	    if (lb_gr_check_left_i((*L).array[k],(*L).array[k+1],P) > 0) /* P left of  edge */
	      wn++;            // have  a valid up intersect
	}
      else
	{                        // start y > P.y (no test needed)
	  if ((*L).array[k+1].y  <= P.y)     // a downward crossing
	    if(lb_gr_check_left_i((*L).array[k],(*L).array[k+1],P) < 0) // P right of  edge
	      wn--;            // have  a valid down intersect
	}
    }
  return wn;
}

S_INT_16_T lb_gr_is_in_polygon_f(LINE_2D_FLOAT_T *L, POINT_2D_FLOAT_T P)
{
  S_INT_16_T k, wn; /* the  winding number counter */

  if (!lb_gr_assert_dimensions_line2d_f(L))
    {
      printf("Error: lb_gr_is_in_polygon_f() --> invalid dimension\r\n");
      exit(EXIT_FAILURE);
    }	
	  
  wn=0;
  for (k=0;k<(*L).items-1;k++)
    {   // edge from V[i] to  V[i+1]
      if ((*L).array[k].y <= P.y)
	{
	  // start y <= P.y
	  if ((*L).array[k+1].y > P.y)      // an upward crossing
	    if (lb_gr_check_left_f((*L).array[k],(*L).array[k+1],P) > 0) /* P left of  edge */
	      wn++;            // have  a valid up intersect
	}
      else
	{                        // start y > P.y (no test needed)
	  if ((*L).array[k+1].y  <= P.y)     // a downward crossing
	    if(lb_gr_check_left_f((*L).array[k],(*L).array[k+1],P) < 0) // P right of  edge
	      wn--;            // have  a valid down intersect
	}
    }
  return wn;
}

S_INT_8_T lb_gr_matrix_gs_to_pic(MATRIX_R_T *A, PICTURE_T *Pic)
{
  S_INT_16_T i, j, j_max, i_max;
  
  if (!lb_gr_assert_dimensions_picture(Pic))
    {
      printf("Error: lb_gr_matrix_gs_to_pic --> invalid dimension\r\n");
      exit(EXIT_FAILURE);
    }
  
  if (!lb_al_assert_dimensions_matrix_r(A))
    {
      printf("Error: lb_gr_matrix_gs_to_pic --> invalid dimension\r\n");
      exit(EXIT_FAILURE);
    }
  
  if ((*A).rows<=(*Pic).h)
    j_max=(*A).rows;
  else
    j_max=(*Pic).h;
 
  if ((*A).cols<=(*Pic).w)
    i_max=(*A).cols;
  else
    i_max=(*Pic).w;

  for (j=0;j<j_max;j++)
    for (i=0;i<i_max;i++)
      {
	if((*A).array[j][i]<0.0)
	  {
	    (*Pic).pic[i][j].r=0;
	    (*Pic).pic[i][j].g=0;
	    (*Pic).pic[i][j].b=0;
	  }
	else
	  if((*A).array[j][i]>1.0)
	    {
	      (*Pic).pic[i][j].r=MAX_R;
	      (*Pic).pic[i][j].g=MAX_G;
	      (*Pic).pic[i][j].b=MAX_B;
	    }
	  else
	    {
	      (*Pic).pic[i][j].r=round(MAX_R*(*A).array[j][i]);
	      (*Pic).pic[i][j].g=round(MAX_G*(*A).array[j][i]);
	      (*Pic).pic[i][j].b=round(MAX_B*(*A).array[j][i]);
	      (*Pic).pic[i][j].a=MAX_A;
	    }
      }	
  return 1;
}

void       lb_gr_plot2d(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, FLOAT_T xr, FLOAT_T yr, FLOAT_T w,
			PIXEL_T color, COPYMODE_T copymode, LINEMODE_T linemode)
{
  FLOAT_T xp, yp;

  lb_gr_project_2d(vp2d, xr, yr, &xp, &yp);
  switch(linemode)
    {
    case LINEMODE_DOTS_SOLID:
      if (w<=1.0)
	lb_gr_draw_pixel(Pic, round(xp), round(yp), color, copymode);
      else
	lb_gr_draw_circle_filled_fast(Pic, round(xp), round(yp), w, color, copymode);
      break;
    case LINEMODE_DOTS_FILTERED:
      lb_gr_draw_circle_filled_antialiasing(Pic, round(xp), round(yp), w, color, copymode);
      break;
    default:
      printf("Error: lb_gr_plot2d() --> invalid mode\r\n");
      exit(EXIT_FAILURE);
      break;
    }
}

void lb_gr_plot2d_line(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, LINE_2D_FLOAT_T *L, FLOAT_T w, PIXEL_T color, COPYMODE_T copymode, LINEMODE_T linemode)
{
  S_INT_16_T  k;
  FLOAT_T xp, yp;
  VECTOR_R_T Lpx, Lpy;
  
  switch(linemode)
    {
    case LINEMODE_DOTS_SOLID:
      for(k=0;k<(*L).items;k++)
	{
	  lb_gr_project_2d(vp2d, (*L).array[k].x, (*L).array[k].y, &xp, &yp);
	  if (w<=1.0)
	    lb_gr_draw_pixel(Pic, round(xp), round(yp), color, copymode);
	  else
	    lb_gr_draw_circle_filled_fast(Pic, round(xp), round(yp), w, color, copymode);
	}
      break;
    case LINEMODE_DOTS_FILTERED:
      for(k=0;k<(*L).items;k++)
	{
	  lb_gr_project_2d(vp2d, (*L).array[k].x, (*L).array[k].y, &xp, &yp);
	  lb_gr_draw_circle_filled_antialiasing(Pic, round(xp), round(yp), w, color, copymode);
	}
      break;
    case LINEMODE_SOLID:
    case LINEMODE_FILTERED:
      Lpx.items=(*L).items;
      Lpy.items=(*L).items;
      lb_al_create_vector_r(&Lpx);
      lb_al_create_vector_r(&Lpy);

      for(k=0;k<(*L).items;k++)
	lb_gr_project_2d(vp2d, (*L).array[k].x, (*L).array[k].y, &Lpx.array[k], &Lpy.array[k]);

      for (k=0;k<(*L).items-1;k++)
	lb_gr_draw_line(Pic, Lpx.array[k], Lpy.array[k], Lpx.array[k+1], Lpy.array[k+1],
			w, color, copymode, linemode);
  
      lb_al_release_vector_r(&Lpx);
      lb_al_release_vector_r(&Lpy);
      break;
    default:
      printf("Error: lb_gr_plot2d_line --> Invalid LINEMODE\r\n");
      exit(EXIT_FAILURE);
      break;
    }
}

void lb_gr_plot2d_line_reverse(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, VECTOR_R_T *Lx, VECTOR_R_T *Ly, FLOAT_T w, PIXEL_T color, COPYMODE_T copymode)
{
  /* This function draws an anti-aliased plot of a set of P(xi,yi) by efficiently checking the shortest distance from
     only the neighbor pixels to P(xi,yi) to each P(xi,y).  The function offers very good results at the expense of
     speed compared with other functions which just plot lines or anti-alised lines between points.
     This implementation has some academic merit as it also ilustrates the concept of using a reverse transformation
     from pixel coordinates to real coordinates.
     A simpler implementation, with no efficiency improvements is: lb_gr_plot2d_line_reverse_slow */ 

  FLOAT_T xr, yr, distance, temp, w2;
  S_INT_16_T i_min, i, j, i_max, j_min, j_max, k, xi, yi;
  ERR_T error;
  PIXEL_T pix_main;
  FLOAT_T scale;
  MATRIX_S_INT_8_T Mflags;

  pix_main=color;
  w2=0.5*w;

  /* The flag matrix tags the neighbor points around the line from which the shortest distance will be computed */ 
  /* j = x = cols */
  /* i = y = rows */
  /* matrix [i][j] */
  if (Pic==NULL)
    {
      Mflags.rows=ty_screen.h;
      Mflags.cols=ty_screen.w;
    }
  else
    {
      Mflags.rows=(*Pic).h;
      Mflags.cols=(*Pic).w;
    }

  lb_al_create_matrix_si8(&Mflags);

  for(i=0;i<Mflags.rows;i++)
    for(j=0;j<Mflags.cols;j++)
      Mflags.array[i][j]=FALSE;

  if ((*Lx).items != (*Ly).items)
    {
      printf("Error: lb_gr_plot2d_line_reverse --> vectors with different dimensions [%d] != [%d]\r\n",
	     (*Lx).items,(*Ly).items);
      exit(EXIT_FAILURE);
    }	
      
  for(k=0;k<(*Lx).items;k++)
    {
      lb_gr_project_2d(vp2d, (*Lx).array[k], (*Ly).array[k], &xr, &yr);
      /*** Check boundaries ***/
      j_min=round(xr-w2-1);
      if(j_min<0)
	j_min=0;
      if(j_min>Mflags.cols)
	j_min=Mflags.cols;

      j_max=round(xr+w2+1);
      if(j_max<0)
	j_max=0;
      if(j_max>Mflags.cols)
	j_max=Mflags.cols;

      i_min=round(yr-w2-1);
      if(i_min<0)
	i_min=0;
      if(i_min>Mflags.rows)
	i_min=Mflags.rows;
      
      i_max=round(yr+w2+1);
      if(i_max<0)
	i_max=0;
      if(i_max>Mflags.rows)
	i_max=Mflags.rows;
      
      for(i=i_min;i<i_max;i++)
	for(j=j_min;j<j_max;j++)
	  Mflags.array[i][j]=TRUE;
    }
    
  scale=lb_re_sqrt((lb_re_sqr(vp2d.xr_max-vp2d.xr_min,&error)+lb_re_sqr(vp2d.yr_max-vp2d.yr_min,&error))/
		   (lb_re_sqr(vp2d.xp_max-vp2d.xp_min,&error)+lb_re_sqr(vp2d.yp_max-vp2d.yp_min,&error)),&error);

  for(yi=0;yi<Mflags.rows;yi++)
    for(xi=0;xi<Mflags.cols;xi++)
      if(Mflags.array[yi][xi]) /* Work on only those points which were close to the line */
	{
	  pix_main.a=0;
	  lb_gr_project_2d_inv(vp2d, xi, yi, &xr, &yr);

	  /* For each pixel, we calculate the shortest distance to the curve */
	  distance=10000;
	  for(i=0;i<(*Lx).items;i++)
	    {
	      temp=lb_re_sqrt(lb_re_sqr(xr-(*Lx).array[i],&error)+lb_re_sqr(yr-(*Ly).array[i],&error),&error)/scale;
	      if (temp<distance)
		distance=temp;

	      if(distance<w2)
		pix_main.a=MAX_A;
	      else if(fabs(w2-distance)<=1.0)
		pix_main.a=round(MAX_A*(1.0-fabs(w2-distance))); 
	    }
	  if (pix_main.a!=0)
	    lb_gr_draw_pixel(Pic, xi, yi, pix_main, copymode); 
	}
  lb_al_release_matrix_si8(&Mflags);
}
  
void lb_gr_plot2d_line_reverse_slow(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, VECTOR_R_T *Lx, VECTOR_R_T *Ly, FLOAT_T w, PIXEL_T color, COPYMODE_T copymode)
{
  FLOAT_T xr, yr, distance, temp, w2;
  S_INT_16_T i, xi, yi, width, height;
  ERR_T error;
  PIXEL_T pix_main;
  FLOAT_T scale;
  FLOAT_T scale_x, scale_y;

	  
  if (Pic==NULL) 
    {
      width  = ty_screen.w;
      height = ty_screen.h;
      scale_x = 1.0 / (1 + ((copymode & COPYMODE_SCALEX_MASK) >> COPYMODE_SCALEX_SHIFT));
      scale_y = 1.0 / (1 + ((copymode & COPYMODE_SCALEY_MASK) >> COPYMODE_SCALEY_SHIFT));
    }
  else
    {
      width  = (*Pic).w;
      height = (*Pic).h;
      scale_x=1.0;
      scale_y=1.0;
    }
    
  pix_main=color;
  w2=0.5*w;

  if ((*Lx).items != (*Ly).items)
    {
      printf("Error: lb_gr_plot2d_line_reverse_slow --> vectors with different dimensions [%d] != [%d]\r\n",
	     (*Lx).items,(*Ly).items);
      exit(EXIT_FAILURE);
    }	
    
  scale=lb_re_sqrt((lb_re_sqr(vp2d.xr_max-vp2d.xr_min,&error)+lb_re_sqr(vp2d.yr_max-vp2d.yr_min,&error))/
		   (lb_re_sqr(vp2d.xp_max-vp2d.xp_min,&error)+lb_re_sqr(vp2d.yp_max-vp2d.yp_min,&error)),&error);

  for(yi=0;yi<height;yi++)
    for(xi=0;xi<width;xi++)
      {
	pix_main.a=0;
	lb_gr_project_2d_inv(vp2d, xi*scale_x, yi*scale_y, &xr, &yr);

	/* For each pixel, we calculate the shortest distance to the curve */
	distance=10000;
	for(i=0;i<(*Lx).items;i++)
	  {
	    temp=lb_re_sqrt(lb_re_sqr(xr-(*Lx).array[i],&error)+lb_re_sqr(yr-(*Ly).array[i],&error),&error)/scale;
	    if (temp<distance)
	      distance=temp;

	    if(distance<w2)
	      pix_main.a=MAX_A;
	    else if(fabs(w2-distance)<=1.0)
	      pix_main.a=MAX_A*(1.0-fabs(w2-distance)); 
	  }
	if (pix_main.a!=0)
	  lb_gr_draw_pixel(Pic, xi, yi, lb_gr_12RGB(COLOR_YELLOW | COLOR_SOLID),
			   copymode & COPYMODE_SCALEX_MASK & COPYMODE_SCALEY_MASK);
      }
}

void lb_gr_plot3d(PICTURE_T *Pic, VIEWPORT_3D_T vp3d,  FLOAT_T Rot[3][3], POINT_3D_FLOAT_T P, FLOAT_T w,
		  PIXEL_T color, COPYMODE_T copymode, LINEMODE_T linemode)
{
  FLOAT_T xp, yp, hz;
  
  lb_gr_project_3d(vp3d, Rot, P, &xp, &yp, &hz);
  
  switch(linemode)
    {
    case LINEMODE_DOTS_SOLID:
      if (w<=1.0)
	lb_gr_draw_pixel(Pic, round(xp), round(yp), color, copymode);
      else
	lb_gr_draw_circle_filled_fast(Pic, round(xp), round(yp), w, color, copymode);
      break;
    case LINEMODE_DOTS_FILTERED:
      lb_gr_draw_circle_filled_antialiasing(Pic, round(xp), round(yp), w, color, copymode);
      break;
    default:
      printf("Error: lb_gr_plot3d() --> invalid mode\r\n");
      exit(EXIT_FAILURE);
      break;
    }
}

void lb_gr_plot3d_surface(PICTURE_T *Pic, VIEWPORT_3D_T vp3d,  FLOAT_T Rot[3][3], MATRIX_POINT_3D_T *S,
			  FLOAT_T w, PIXEL_T color, COPYMODE_T copymode, LINEMODE_T linemode)
{
  S_INT_16_T i, j;
  FLOAT_T hz;
  MATRIX_POINT_2D_T M2d;

  M2d.rows=(*S).rows;
  M2d.cols=(*S).cols;
  lb_al_create_matrix_p2d(&M2d);
    
  for (i=0;i<(*S).rows;i++)
    for (j=0;j<(*S).cols;j++)
      lb_gr_project_3d(vp3d, Rot, (*S).array[i][j], &M2d.array[i][j].x, &M2d.array[i][j].y, &hz);

  for (i=0;i<(*S).rows-1;i++)
    for (j=0;j<(*S).cols-1;j++)
      {
	lb_gr_draw_line(Pic, M2d.array[i][j].x, M2d.array[i][j].y,  M2d.array[i+1][j].x, M2d.array[i+1][j].y,
			w, color, copymode, linemode);
	lb_gr_draw_line(Pic, M2d.array[i][j].x, M2d.array[i][j].y,  M2d.array[i][j+1].x, M2d.array[i][j+1].y,
			w, color, copymode, linemode);
      }

  for (i=0;i<(*S).rows-1;i++)
    lb_gr_draw_line(Pic,
		    M2d.array[i][(*S).cols-1].x,   M2d.array[i][(*S).cols-1].y,
		    M2d.array[i+1][(*S).cols-1].x, M2d.array[i+1][(*S).cols-1].y,
		    w, color, copymode, linemode);


  for (j=0;j<(*S).cols-1;j++)
    lb_gr_draw_line(Pic, M2d.array[(*S).rows-1][j].x, M2d.array[(*S).rows-1][j].y,
		    M2d.array[(*S).rows-1][j+1].x, M2d.array[(*S).rows-1][j+1].y,
		    w, color, copymode, linemode);
    
    
  lb_al_release_matrix_p2d(&M2d);
}

 
void lb_gr_plot3d_line(VIEWPORT_3D_T vp3d, FLOAT_T Rot[3][3], LINE_3D_FLOAT_T *L)
{ 
  FLOAT_T x1, y1, x2, y2, hz;
  S_INT_16_T i;
  POINT_3D_FLOAT_T P;

  P=(*L).array[0]; 
  lb_gr_project_3d(vp3d, Rot, P, &x1, &y1, &hz);
     
  for (i=0; i<(*L).items-1; i++)
    {
      P=(*L).array[i+1];
      lb_gr_project_3d(vp3d, Rot, P, &x2, &y2, &hz);
      // if (flag1 && flag2) line(x1,y1,x2,y2);
      x1=x2;
      y1=y2;
    }
}


void lb_gr_project_2d(VIEWPORT_2D_T vp2d, FLOAT_T xr, FLOAT_T yr, FLOAT_T *xp, FLOAT_T *yp)
{
  *xp = vp2d.xp_min + ( xr-vp2d.xr_min)*(vp2d.xp_max-vp2d.xp_min)/(vp2d.xr_max-vp2d.xr_min);
  *yp = vp2d.yp_min + ( yr-vp2d.yr_min)*(vp2d.yp_max-vp2d.yp_min)/(vp2d.yr_max-vp2d.yr_min);
}

void lb_gr_project_2d_inv(VIEWPORT_2D_T vp2d, FLOAT_T xp, FLOAT_T yp, FLOAT_T *xr, FLOAT_T *yr)
{
  *xr =  vp2d.xr_min+(xp-vp2d.xp_min)*(vp2d.xr_max-vp2d.xr_min)/(vp2d.xp_max-vp2d.xp_min);
  *yr =  vp2d.yr_min+(yp-vp2d.yp_min)*(vp2d.yr_max-vp2d.yr_min)/(vp2d.yp_max-vp2d.yp_min);
}

void lb_gr_project_2d_log(VIEWPORT_2D_T vp2d, FLOAT_T xr, FLOAT_T yr, FLOAT_T *xp, FLOAT_T *yp)
{
  if ( (xr<=0.0) || (vp2d.xr_min<=0) || (vp2d.xr_max<=0) )
    {
      printf("Error: lb_gr_project_2d_x_log() --> negative argument [xr=%f] [vp2d.xr_min=%f] [vp2d.xr_max=%f]\r\n",
	     xr, vp2d.xr_min, vp2d.xr_max);
      exit(EXIT_FAILURE);
    }	

  if (lb_re_equal(vp2d.xr_min, vp2d.xr_max))
    {
      printf("Error: lb_gr_project_2d_x_log() --> [vp2d.xr_min=%f] == [vp2d.xr_max=%f]\r\n",
	     vp2d.xr_min, vp2d.xr_max);
      exit(EXIT_FAILURE);
    }	
	
  if ( (yr<=0.0) || (vp2d.yr_min<=0) || (vp2d.yr_max<=0) )
    {
      printf("Error: lb_gr_project_2d_y_log() --> negative argument [yr=%f] [vp2d.yr_min=%f] [vp2d.yr_max=%f]\r\n",
	     yr, vp2d.yr_min, vp2d.yr_max);
      exit(EXIT_FAILURE);
    }	
    
  if (lb_re_equal(vp2d.yr_min, vp2d.yr_max))
    {
      printf("Error: lb_gr_project_2d_y_log() --> [vp2d.yr_min=%f] == [vp2d.yr_max=%f]\r\n",
	     vp2d.yr_min, vp2d.yr_max);
      exit(EXIT_FAILURE);
    }	

  *xp = vp2d.xp_min + (log10(xr)-log10(vp2d.xr_min))*(vp2d.xp_max-vp2d.xp_min)/(log10(vp2d.xr_max)-log10(vp2d.xr_min));
  *yp = vp2d.yp_min + (log10(yr)-log10(vp2d.yr_min))*(vp2d.yp_max-vp2d.yp_min)/(log10(vp2d.yr_max)-log10(vp2d.yr_min));
}

void lb_gr_project_2d_x(VIEWPORT_2D_T vp2d, FLOAT_T xr, FLOAT_T *xp)
{
  *xp = vp2d.xp_min + ( xr-vp2d.xr_min)*(vp2d.xp_max-vp2d.xp_min)/(vp2d.xr_max-vp2d.xr_min);
}

void lb_gr_project_2d_x_log(VIEWPORT_2D_T vp2d, FLOAT_T xr, FLOAT_T *xp)
{
  if ( (xr<=0.0) || (vp2d.xr_min<=0) || (vp2d.xr_max<=0) )
    {
      printf("Error: lb_gr_project_2d_x_log() --> negative argument [xr=%f] [vp2d.xr_min=%f] [vp2d.xr_max=%f]\r\n",
	     xr, vp2d.xr_min, vp2d.xr_max);
      exit(EXIT_FAILURE);
    }
  
  if (lb_re_equal(vp2d.xr_min, vp2d.xr_max))
    {
      printf("Error: lb_gr_project_2d_x_log() --> [vp2d.xr_min=%f] == [vp2d.xr_max=%f]\r\n",
	     vp2d.xr_min, vp2d.xr_max);
      exit(EXIT_FAILURE);
    }	

  *xp = vp2d.xp_min + (log10(xr)-log10(vp2d.xr_min))*(vp2d.xp_max-vp2d.xp_min)/(log10(vp2d.xr_max)-log10(vp2d.xr_min));
}

void lb_gr_project_2d_y(VIEWPORT_2D_T vp2d, FLOAT_T yr, FLOAT_T *yp)
{
  *yp = vp2d.yp_min + ( yr-vp2d.yr_min)*(vp2d.yp_max-vp2d.yp_min)/(vp2d.yr_max-vp2d.yr_min);
}

void lb_gr_project_2d_y_log(VIEWPORT_2D_T vp2d, FLOAT_T yr, FLOAT_T *yp)
{
  if ( yr<=0.0)
    {
      printf("Error: lb_gr_project_2d_y_log() --> negative argument [yr=%f]\r\n",yr);
      exit(EXIT_FAILURE);
    }	

  if ( vp2d.yr_min<=0.0)
    {
      printf("Error: lb_gr_project_2d_y_log() --> negative argument [vp2d.yr_min=%f]\r\n",vp2d.yr_min);
      exit(EXIT_FAILURE);
    }	

  if ( vp2d.yr_max<=0.0)
    {
      printf("Error: lb_gr_project_2d_y_log() --> negative argument [vp2d.yr_max=%f]\r\n",vp2d.yr_max);
      exit(EXIT_FAILURE);
    }	

  if (lb_re_equal(vp2d.yr_min, vp2d.yr_max))
    {
      printf("Error: lb_gr_project_2d_y_log() --> [vp2d.yr_min=%f] == [vp2d.yr_max=%f]\r\n",
	     vp2d.yr_min, vp2d.yr_max);
      exit(EXIT_FAILURE);
    }	
    
  *yp = vp2d.yp_min + (log10(yr)-log10(vp2d.yr_min))*(vp2d.yp_max-vp2d.yp_min)/(log10(vp2d.yr_max)-log10(vp2d.yr_min));
}

void lb_gr_project_2d_vector_c_to_line_int(VIEWPORT_2D_T vp2d, VECTOR_C_T *V, LINE_2D_INT_T *L)
{
  U_INT_16_T i;
  FLOAT_T xr, yr;
  
  if (!lb_al_assert_dimensions_vector_c(V))
    {
      printf("Error: lb_gr_project_2d_vector_c_to_line_int --> invalid dimension arg 1\r\n");
      exit(EXIT_FAILURE);
    }	

  if (!lb_gr_assert_dimensions_line2d_i(L))
    {
      printf("Error: lb_gr_project_2d_vector_c_to_line_int --> invalid dimension arg 2\r\n");
      exit(EXIT_FAILURE);
    }
  
  for (i=0;i<(*V).items;i++)
    {
      lb_gr_project_2d(vp2d, (*V).array[i].r, (*V).array[i].i, &xr, &yr);
      (*L).array[i].x=xr;
      (*L).array[i].y=yr;
    }
}

void lb_gr_project_2d_vector_r_to_line_int(VIEWPORT_2D_T vp2d, VECTOR_R_T *X, VECTOR_R_T *Y, LINE_2D_INT_T *L)
{
  U_INT_16_T i;
  FLOAT_T xr, yr;
  
  if (!lb_al_assert_dimensions_vector_r(X) || !lb_al_assert_dimensions_vector_r(Y))
    {
      printf("Error: lb_gr_project_2d_vector_r_to_line_int() --> invalid dimension\r\n");
      exit(EXIT_FAILURE);
    }	
    
  if ((*X).items != (*Y).items)
    {
      printf("Error: lb_gr_project_2d_vector_r_to_line_int() --> invalid dimension X[%d] != Y[%d]\r\n",(*X).items,(*Y).items);
      exit(EXIT_FAILURE);
    }	

  if ((*X).items != (*L).items)
    {
      printf("Error: lb_gr_project_2d_vector_r_to_line_int() --> invalid dimension X[%d] != L[%d]\r\n",(*X).items,(*L).items);
      exit(EXIT_FAILURE);
    }	

  for (i=0;i<(*X).items;i++)
    {
      
      lb_gr_project_2d(vp2d, (*X).array[i], (*Y).array[i], &xr, &yr);
      (*L).array[i].x=xr;
      (*L).array[i].y=yr;
    }
}

void lb_gr_release_line2d_i(LINE_2D_INT_T *L)
{
  free((*L).array);
}

void lb_gr_release_line2d_f(LINE_2D_FLOAT_T *L)
{
  free((*L).array);
}

void lb_gr_release_line3d_f(LINE_3D_FLOAT_T *L)
{
  free((*L).array);
}

void lb_gr_release_picture(volatile PICTURE_T *Pic)
{
  free((*Pic).pic);
}

S_INT_8_T lb_gr_return_octant(S_INT_16_T x0, S_INT_16_T y0, S_INT_16_T x1, S_INT_16_T y1)
{
  /* Octant determination */
  if (y1>=y0) /* Quadrants 1 or 2 */
    if (x1>=x0) /* Quadrant 1 */  
      if ((y1-y0)<=(x1-x0))
	return 0;
      else
	return 1;
    else /* Quadrant 2 */
      if ((y1-y0)<=(x1-x0))
	return 3;
      else
	return 2;
  else /* Quadrants 3 or 4 */
    if (x1<x0) /* Quadrant 3 */  
      if ((y1-y0)<=(x1-x0))
	return 4;
      else
	return 5;
    else /* Quadrant 4 */
      if ((y1-y0)<=(x1-x0))
	return 7;
      else
	return 6;
}

void       lg_gr_draw_axis_2d(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, FONT_T *font, FLOAT_T w_axis, PIXEL_T color_axis,
			      FLOAT_T w_grid, PIXEL_T color_grid_x, FLOAT_T delta_grid_x, PIXEL_T color_grid_y,
			      FLOAT_T delta_grid_y, U_INT_16_T options, COPYMODE_T copymode, LINEMODE_T linemode)
{
  FLOAT_T xr, yr, xr_min, xr_max, yr_min, yr_max, xp0, yp0, arrow_size;
  S_INT_8_T flag, sign_re_x, sign_re_y, sign_sc_x, sign_sc_y;
  S_INT_16_T k, delta, exp_inc;
  PIXEL_T color;
	       
  if (lb_re_equal(vp2d.xr_min, vp2d.xr_max))
    {
      printf("Error: lg_gr_draw_axis_2d() --> vp2d.xr_min [%f]= vp2d.xr_max [%f]\r\n",vp2d.xr_min,vp2d.xr_max);
      exit(EXIT_FAILURE);
    }
  
  if (lb_re_equal(vp2d.yr_min, vp2d.yr_max))
    {
      printf("Error: lg_gr_draw_axis_2d() --> vp2d.yr_min [%f]= vp2d.yr_max [%f]\r\n",vp2d.yr_min,vp2d.yr_max);
      exit(EXIT_FAILURE);
    }
  
  if (vp2d.xp_min == vp2d.xp_max)
    {
      printf("Error: lg_gr_draw_axis_2d() --> vp2d.xp_min [%d]= vp2d.xp_max [%d]\r\n",vp2d.xp_min,vp2d.xp_max);
      exit(EXIT_FAILURE);
    }	
	
  if (vp2d.yp_min == vp2d.yp_max)
    {
      printf("Error: lg_gr_draw_axis_2d() --> vp2d.yp_min [%d]= vp2d.yp_max [%d]\r\n",vp2d.yp_min,vp2d.yp_max);
      exit(EXIT_FAILURE);
    }
  
  if (vp2d.xr_min<vp2d.xr_max)
    {
      xr_min=vp2d.xr_min;
      xr_max=vp2d.xr_max;
      sign_re_x=1.0;
    }
  else
    {
      xr_min=vp2d.xr_max;
      xr_max=vp2d.xr_min;
      sign_re_x=-1.0;
    }

  if (vp2d.yr_min<vp2d.yr_max)
    {
      yr_min=vp2d.yr_min;
      yr_max=vp2d.yr_max;
      sign_re_y=1.0;
    }
  else
    {
      yr_min=vp2d.yr_max;
      yr_max=vp2d.yr_min;
      sign_re_y=-1.0;
    }

  if (vp2d.xp_min<vp2d.xp_max)
    sign_sc_x= 1.0;
  else
    sign_sc_x=-1.0;

  if (vp2d.yp_min<vp2d.yp_max)
    sign_sc_y= 1.0;
  else
    sign_sc_y=-1.0;


  if (options & AXIS_DRAW_X_GRID_LOG)
    {
      if (delta_grid_x<=0)
	{
	  printf("Error: lg_gr_draw_axis_2d() --> delta_grid_x <=0 [%f]\r\n",delta_grid_x);
	  exit(EXIT_FAILURE);
	}

      if (!lb_re_equal(lb_re_frac(delta_grid_x),0.0))
	{
	  printf("Error: lg_gr_draw_axis_2d() --> delta_grid_x must be integer [%f]\r\n",delta_grid_x);
	  exit(EXIT_FAILURE);
	}
	    
      delta=round(delta_grid_x);
      exp_inc=lb_re_normed_exponent(xr_min);
      k=1;
      while (xr<=xr_max)
	{
	  if (k==1)
	    xr=pow(10.0,exp_inc-1);
	  else
	    xr=(((FLOAT_T)k-1)/delta)*pow(10.0,exp_inc);
   
	  if ((xr>=xr_min) && (xr<=xr_max))
	    {
	      //lb_ft_printf(ty_C, "k=%d xr=%f exp_inc=%d\r\n",k,xr,exp_inc);
	      lb_gr_project_2d_x_log(vp2d, xr, &xp0);
	      color=color_grid_x;
	      if (options & AXIS_DRAW_COLORVALUES_X_1)
		{
		  color.r = color_grid_x.r*k/delta;
		  color.g = color_grid_x.g*k/delta;
		  color.b = color_grid_x.b*k/delta;
		  color.a = color_grid_x.a*k/delta;
		}
	      if (options & AXIS_DRAW_COLORVALUES_X_2)
		color=lb_gr_value_to_color(k);
	       
	      lb_gr_draw_line(Pic, xp0, vp2d.yp_min, xp0, vp2d.yp_max, w_grid, color, copymode, linemode);
	    }
	  k++;
	  if (k>delta)
	    {
	      k=1;
	      exp_inc++;
	    }
	}
    } /* End of AXIS_DRAW_X_GRID_LOG */


  if (options & AXIS_DRAW_Y_GRID_LOG)
    {
      if (delta_grid_y<=0)
	{
	  printf("Error: lg_gr_draw_axis_2d() --> delta_grid_y <=0 [%f]\r\n",delta_grid_x);
	  exit(EXIT_FAILURE);
	}
	  
      if (!lb_re_equal(lb_re_frac(delta_grid_y),0.0))
	{
	  printf("Error: lg_gr_draw_axis_2d() --> delta_grid_y must be integer [%f]\r\n",delta_grid_x);
	  exit(EXIT_FAILURE);
	}
	  
      delta=round(delta_grid_y);
      exp_inc=lb_re_normed_exponent(yr_min);
      k=1;
      while (yr<=yr_max)
	{
	  if (k==1)
	    yr=pow(10.0,exp_inc-1);
	  else
	    yr=(((FLOAT_T)k-1)/delta)*pow(10.0,exp_inc);
   
	  if ((yr>=yr_min) && (yr<=yr_max))
	    {
	      //		lb_ft_printf(ty_C, "k=%d yr=%f exp_inc=%d\r\n",k,yr,exp_inc);
	      lb_gr_project_2d_y_log(vp2d, yr, &yp0);
	      color=color_grid_y;
	      if (options & AXIS_DRAW_COLORVALUES_Y_1)
		{
		  color.r = color_grid_y.r*k/delta;
		  color.g = color_grid_y.g*k/delta;
		  color.b = color_grid_y.b*k/delta;
		  color.a = color_grid_y.a*k/delta;
		}
	      if (options & AXIS_DRAW_COLORVALUES_Y_2)
		color=lb_gr_value_to_color(k);
	       
	      lb_gr_draw_line(Pic, vp2d.xp_min, yp0, vp2d.xp_max, yp0, w_grid, color, copymode, linemode);
	    }
	  k++;
	  if (k>delta)
	    {
	      k=1;
	      exp_inc++;
	    }
	}
    } /* End of AXIS_DRAW_Y_GRID_LOG */


  if (options & AXIS_DRAW_X_GRID)
    {
      if (lb_re_equal(delta_grid_x,0.0))
	{
	  printf("Error: lg_gr_draw_axis_2d() --> delta_grid_x = 0\r\n");
	  exit(EXIT_FAILURE);
	}
	    
      xr=delta_grid_x*ceil(xr_min/delta_grid_x);
      while (xr<=xr_max)
	{
	  lb_gr_project_2d_x(vp2d, xr, &xp0);
	  lb_gr_draw_line(Pic, xp0, vp2d.yp_min, xp0, vp2d.yp_max, w_grid, color_grid_x, copymode, linemode);
	  xr+=delta_grid_x;
	}
    }

  if (options & AXIS_DRAW_Y_GRID)
    {
      if (lb_re_equal(delta_grid_y,0.0))
	{
	  printf("Error: lg_gr_draw_axis_2d() --> delta_grid_y = 0\r\n");
	  exit(EXIT_FAILURE);
	}
      
      yr=delta_grid_y*ceil(yr_min/delta_grid_y);
      while (yr<=yr_max)
	{
	  lb_gr_project_2d_y(vp2d, yr, &yp0);
	  lb_gr_draw_line(Pic, vp2d.xp_min, yp0, vp2d.xp_max, yp0, w_grid, color_grid_x, copymode, linemode);
	  yr+=delta_grid_y;
	}
    }
   
  /* Drawing the X axis */
  if (options & AXIS_DRAW_X)
    {
      flag=lb_nu_find_zero(vp2d.yp_min, vp2d.yr_min, vp2d.yp_max, vp2d.yr_max,  &yp0);
      if (flag==1)
	{
	  lb_gr_draw_line(Pic, vp2d.xp_min, yp0, vp2d.xp_max, yp0, w_axis, color_axis, copymode, linemode);
	  if (options & AXIS_DRAW_X_ARROWS)
	    {
	      /* The arrow indicates positive numbers */
	      arrow_size=0.025*fabs(vp2d.xp_max-vp2d.xp_min);
	      if(sign_re_x==1)
		{
		  lb_gr_draw_line(Pic, vp2d.xp_max, yp0, vp2d.xp_max-sign_sc_x*arrow_size, yp0-0.5*arrow_size, w_axis,
				  color_axis, copymode, linemode);
		  lb_gr_draw_line(Pic, vp2d.xp_max, yp0, vp2d.xp_max-sign_sc_x*arrow_size, yp0+0.5*arrow_size, w_axis,
				  color_axis, copymode, linemode);
		}
	      else
		{
		  lb_gr_draw_line(Pic, vp2d.xp_min, yp0, vp2d.xp_min+sign_sc_x*arrow_size, yp0-0.5*arrow_size, w_axis,
				  color_axis, copymode, linemode);
		  lb_gr_draw_line(Pic, vp2d.xp_min, yp0, vp2d.xp_min+sign_sc_x*arrow_size, yp0+0.5*arrow_size, w_axis,
				  color_axis, copymode, linemode);
		}
	    }
	}
    }

  /* Drawing the Y axis */
  if (options & AXIS_DRAW_Y)
    {
      flag=lb_nu_find_zero(vp2d.xp_min, vp2d.xr_min, vp2d.xp_max, vp2d.xr_max,  &xp0);
      if (flag==1)
	{
	  lb_gr_draw_line(Pic, xp0, vp2d.yp_min, xp0, vp2d.yp_max, w_axis, color_axis, copymode, linemode);
	  if (options & AXIS_DRAW_Y_ARROWS)
	    {
	      /* The arrow indicates positive numbers */
	      arrow_size=0.025*fabs(vp2d.yp_max-vp2d.yp_min);
	      if(sign_re_y==1)
		{
		  lb_gr_draw_line(Pic, xp0, vp2d.yp_max, xp0-0.5*arrow_size, vp2d.yp_max-sign_sc_y*arrow_size, w_axis,
				  color_axis, copymode, linemode);
		  lb_gr_draw_line(Pic, xp0, vp2d.yp_max, xp0+0.5*arrow_size, vp2d.yp_max-sign_sc_y*arrow_size, w_axis,
				  color_axis, copymode, linemode);
		}
	      else
		{
		  lb_gr_draw_line(Pic, xp0, vp2d.yp_min, xp0-0.5*arrow_size, vp2d.yp_min+sign_sc_y*arrow_size, w_axis,
				  color_axis, copymode, linemode);
		  lb_gr_draw_line(Pic, xp0, vp2d.yp_min, xp0+0.5*arrow_size, vp2d.yp_min+sign_sc_y*arrow_size, w_axis,
				  color_axis, copymode, linemode);
		}
	    }
	}
    }
}


// oxo
void       lg_gr_draw_axis_2d_polar(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, FONT_T *font,
				    FLOAT_T r0, FLOAT_T r1, FLOAT_T delta_r, PIXEL_T color_r, 
				    FLOAT_T t0, FLOAT_T t1, FLOAT_T delta_t, PIXEL_T color_t,
				    U_INT_16_T options, COPYMODE_T copymode)
{
  FLOAT_T r, t, t_temp, t_min, t_max, r_min, r_max, xr, yr, xp0, yp0, xp1, yp1, factor_delta;
  S_INT_16_T xp0_i, yp0_i, xp1_i, yp1_i;
  S_INT_8_T flag_adjacent, counter_adjacent, exponent;
  FLOAT_T scale_x, scale_y;
	  
  if (Pic==NULL) 
    {
      scale_x = 1.0 / (1 + ((copymode & COPYMODE_SCALEX_MASK) >> COPYMODE_SCALEX_SHIFT));
      scale_y = 1.0 / (1 + ((copymode & COPYMODE_SCALEY_MASK) >> COPYMODE_SCALEY_SHIFT));
    }
  else
    {
      scale_x=1.0;
      scale_y=1.0;
    }
  
  if (lb_re_equal(vp2d.xr_min, vp2d.xr_max))
    {
      printf("Error: lg_gr_draw_axis_2d_polar() --> vp2d.xr_min [%f]= vp2d.xr_max [%f]\r\n",vp2d.xr_min,vp2d.xr_max);
      exit(EXIT_FAILURE);
    }	
    
  if (lb_re_equal(vp2d.yr_min, vp2d.yr_max))
    {
      printf("Error: lg_gr_draw_axis_2d_polar() --> vp2d.yr_min [%f]= vp2d.yr_max [%f]\r\n",vp2d.yr_min,vp2d.yr_max);
      exit(EXIT_FAILURE);
    }	

  if (vp2d.xp_min == vp2d.xp_max)
    {
      printf("Error: lg_gr_draw_axis_2d_polar() --> vp2d.xp_min [%d]= vp2d.xp_max [%d]\r\n",vp2d.xp_min,vp2d.xp_max);
      exit(EXIT_FAILURE);
    }	

  if (vp2d.yp_min == vp2d.yp_max)
    {
      printf("Error: lg_gr_draw_axis_2d_polar() --> vp2d.yp_min [%d]= vp2d.yp_max [%d]\r\n",vp2d.yp_min,vp2d.yp_max);
      exit(EXIT_FAILURE);
    }	
	
  if ( (delta_r<=0) || (delta_t<=0) )
    {
      printf("Error: lg_gr_draw_axis_2d_polar() --> invalid parameter(s) delta_r=[%f] delta_t=[%f]\r\n",
	     delta_r,delta_t);
      exit(EXIT_FAILURE);
    }	
    
  if (t1>=t0)
    {
      t_min=t0;
      t_max=t1;
    }
  else
    {
      t_max=t0;
      t_min=t1;
    }

  if (r1>=r0)
    {
      r_min=r0;
      r_max=r1;
    }
  else
    {
      r_max=r0;
      r_min=r1;
    }

  /* Plotting the circles */
  r=r_min;
  while (r<=r_max)
    {
      t=t_min;
      xr=r*cos(t);
      yr=r*sin(t);
  
      lb_gr_project_2d(vp2d, xr, yr, &xp0, &yp0);
      xp0_i = round(xp0)*scale_x;
      yp0_i = round(yp0)*scale_y;
      lb_gr_draw_pixel(Pic, xp0_i, yp0_i, color_t, copymode);

      exponent=0;
      factor_delta=1.0;
      counter_adjacent=0;
      while (t<=t_max)
	{
	  do
	    {
	      t_temp=t;
	      t_temp+=delta_r*factor_delta;

	      xr=r*cos(t_temp);
	      yr=r*sin(t_temp);
  
	      lb_gr_project_2d(vp2d, xr, yr, &xp1, &yp1);
	      xp1_i = round(xp1)*scale_x;
	      yp1_i = round(yp1)*scale_y;
	      if ( (abs(xp1_i-xp0_i)<=1) && (abs(yp1_i-yp0_i)<=1) )
		{
		  flag_adjacent=TRUE;
		  counter_adjacent++;
		}
	      else
		{
		  flag_adjacent=FALSE;
		  counter_adjacent=0;
		}
	     
	      if (!flag_adjacent)
		{
		  exponent++;
		  factor_delta=pow(2.0,-exponent);
		  //lb_ft_printf(ty_C, "exponent=%d,factor =%f\r\n",exponent,factor_delta);
		}
	      else
		if (counter_adjacent>32)
		  {
		    /* After "several" consecutive adjacent points, the next larger step can be tried */ 
		    exponent--;
		    factor_delta=pow(2.0,-exponent);
		    //lb_ft_printf(ty_C, "exponent=%d,factor =%f\r\n",exponent,factor_delta);
		  }
	       
	    } while (!flag_adjacent);
	  t=t_temp;
	  lb_gr_draw_pixel(Pic, xp1_i, yp1_i, color_t, copymode);
	  xp0_i=xp1_i;
	  yp0_i=yp1_i;
	}
      r+=delta_r;
    }

  /* Plotting the radiuses */
  t=t_min;
  while (t<=t_max)
    {
      xr=r_min*cos(t);
      yr=r_min*sin(t);
      lb_gr_project_2d(vp2d, xr, yr, &xp0, &yp0);

      xr=r_max*cos(t);
      yr=r_max*sin(t);
      lb_gr_project_2d(vp2d, xr, yr, &xp1, &yp1);

      lb_gr_draw_line1_f(Pic, round(xp0*scale_x), round(yp0*scale_y),
			 round(xp1*scale_x), round(yp1*scale_y), color_r, copymode);
      t+=delta_t;
    }
}
  
void lg_gr_draw_axis_3d(PICTURE_T *Pic, VIEWPORT_3D_T vp3d, FLOAT_T Rot[3][3], FONT_T *font,
			FLOAT_T axis_size,
			FLOAT_T w_axis,
			PIXEL_T color_axis,
			FLOAT_T xr_min, FLOAT_T xr_max, FLOAT_T delta_grid_x,
			FLOAT_T yr_min, FLOAT_T yr_max, FLOAT_T delta_grid_y,
			FLOAT_T zr_min, FLOAT_T zr_max, FLOAT_T delta_grid_z,
			FLOAT_T w_grid,
			PIXEL_T color_grid,
			U_INT_16_T options,
			const char *label_o,
			const char *label_x,
			const char *label_y,
			const char *label_z,			      
			COPYMODE_T copymode,
			LINEMODE_T linemode)
{
  FLOAT_T p1_xp, p1_yp, p2_xp, p2_yp, x_xp, x_yp, y_xp, y_yp, z_xp, z_yp, xr, yr, zr, hz;

    
  if ( (xr_min>xr_max) || (yr_min>yr_max) || (zr_min>zr_max) )
    {
      printf("Error: lg_gr_draw_axis_3d() --> invalid argument xr[%f , %f] yr[%f , %f] zr[%f , %f]\r\n",
	     xr_min, xr_max, yr_min, yr_max, zr_min, zr_max);
      exit(EXIT_FAILURE);
    }	

  if ( lb_re_equal(delta_grid_x,0.0) || lb_re_equal(delta_grid_y,0.0) || lb_re_equal(delta_grid_z,0.0) )
    {
      printf("Error: lg_gr_draw_axis_3d() --> invalid argument delta_x=%f delta_y=%f delta_z=%f\r\n",
	     delta_grid_x, delta_grid_y, delta_grid_z);
      exit(EXIT_FAILURE);
    }	

  if (options & AXIS_DRAW_X_GRID)
    {
      for (xr=xr_min; xr<=xr_max; xr+=delta_grid_x)
	{
	  for (yr=yr_min; yr<=yr_max; yr+=delta_grid_y)
	    {
	      lb_gr_project_3d(vp3d, Rot, (POINT_3D_FLOAT_T){xr,yr,zr_min}, &p1_xp, &p1_yp, &hz);
	      lb_gr_project_3d(vp3d, Rot, (POINT_3D_FLOAT_T){xr,yr,zr_max}, &p2_xp, &p2_yp, &hz);
	      lb_gr_draw_line(Pic, p1_xp, p1_yp, p2_xp, p2_yp, w_grid, color_grid, copymode, linemode);
	    }
	  for (zr=zr_min; zr<=zr_max; zr+=delta_grid_z)
	    {
	      lb_gr_project_3d(vp3d, Rot, (POINT_3D_FLOAT_T){xr,yr_min,zr}, &p1_xp, &p1_yp, &hz);
	      lb_gr_project_3d(vp3d, Rot, (POINT_3D_FLOAT_T){xr,yr_max,zr}, &p2_xp, &p2_yp, &hz);
	      lb_gr_draw_line(Pic, p1_xp, p1_yp, p2_xp, p2_yp, w_grid, color_grid, copymode, linemode);
	    }
	}
    }

  if (options & AXIS_DRAW_Y_GRID)
    {
      for (yr=yr_min; yr<=yr_max; yr+=delta_grid_y)
	{
	  for (xr=xr_min; xr<=xr_max; xr+=delta_grid_x)
	    {
	      lb_gr_project_3d(vp3d, Rot, (POINT_3D_FLOAT_T){xr,yr,zr_min}, &p1_xp, &p1_yp, &hz);
	      lb_gr_project_3d(vp3d, Rot, (POINT_3D_FLOAT_T){xr,yr,zr_max}, &p2_xp, &p2_yp, &hz);
	      lb_gr_draw_line(Pic, p1_xp, p1_yp, p2_xp, p2_yp, w_grid, color_grid, copymode, linemode);
	    }
	  for (zr=zr_min; zr<=zr_max; zr+=delta_grid_z)
	    {
	      lb_gr_project_3d(vp3d, Rot, (POINT_3D_FLOAT_T){xr_min,yr,zr}, &p1_xp, &p1_yp, &hz);
	      lb_gr_project_3d(vp3d, Rot, (POINT_3D_FLOAT_T){xr_max,yr,zr}, &p2_xp, &p2_yp, &hz);
	      lb_gr_draw_line(Pic, p1_xp, p1_yp, p2_xp, p2_yp, w_grid, color_grid, copymode, linemode);
	    }
	}
    }

  if (options & AXIS_DRAW_Z_GRID)
    {
      for (zr=zr_min; zr<=zr_max; zr+=delta_grid_z)
	{
	  for (xr=xr_min; xr<=xr_max; xr+=delta_grid_x)
	    {
	      lb_gr_project_3d(vp3d, Rot, (POINT_3D_FLOAT_T){xr,yr_min,zr}, &p1_xp, &p1_yp, &hz);
	      lb_gr_project_3d(vp3d, Rot, (POINT_3D_FLOAT_T){xr,yr_max,zr}, &p2_xp, &p2_yp, &hz);
	      lb_gr_draw_line(Pic, p1_xp, p1_yp, p2_xp, p2_yp, w_grid, color_grid, copymode, linemode);
	    }
	  for (yr=yr_min; yr<=xr_max; yr+=delta_grid_y)
	    {
	      lb_gr_project_3d(vp3d, Rot, (POINT_3D_FLOAT_T){xr_min,yr,zr}, &p1_xp, &p1_yp, &hz);
	      lb_gr_project_3d(vp3d, Rot, (POINT_3D_FLOAT_T){xr_max,yr,zr}, &p2_xp, &p2_yp, &hz);
	      lb_gr_draw_line(Pic, p1_xp, p1_yp, p2_xp, p2_yp, w_grid, color_grid, copymode, linemode);
	    }
	}
    }
    
  lb_gr_project_3d(vp3d, Rot, (POINT_3D_FLOAT_T){0,         0,         0        }, &p1_xp, &p1_yp, &hz);
  lb_gr_project_3d(vp3d, Rot, (POINT_3D_FLOAT_T){axis_size, 0,         0        }, &x_xp,  &x_yp,  &hz);
  lb_gr_project_3d(vp3d, Rot, (POINT_3D_FLOAT_T){0,         axis_size, 0        }, &y_xp,  &y_yp,  &hz);
  lb_gr_project_3d(vp3d, Rot, (POINT_3D_FLOAT_T){0,         0,         axis_size}, &z_xp,  &z_yp,  &hz);


  lb_gr_draw_line(Pic, p1_xp, p1_yp, x_xp, x_yp, w_axis, color_axis, copymode, linemode);
  lb_gr_draw_line(Pic, p1_xp, p1_yp, y_xp, y_yp, w_axis, color_axis, copymode, linemode);
  lb_gr_draw_line(Pic, p1_xp, p1_yp, z_xp, z_yp, w_axis, color_axis, copymode, linemode);

  lb_ft_draw_text(Pic, font, p1_xp, p1_yp, label_o, copymode);
  lb_ft_draw_text(Pic, font, x_xp,  x_yp,  label_x, copymode);
  lb_ft_draw_text(Pic, font, y_xp,  y_yp,  label_y, copymode);
  lb_ft_draw_text(Pic, font, z_xp,  z_yp,  label_z, copymode);

  xr = 0.0;
  yr = 0.0;
  zr = 0.0;
    
  //if (options & AXIS_DRAW_X_GRID)
  //{
  //	if (lb_re_equal(delta_grid_x,0.0))
  //	  {
  //	    lb_ft_printf(ty_C, "Error: lg_gr_draw_axis_3d() --> delta_grid_x = 0\r\n");
  //	    exit(1);
  //	  }

   
}

void lb_gr_project_3d(VIEWPORT_3D_T vp3d, FLOAT_T Rot[3][3], POINT_3D_FLOAT_T P, FLOAT_T *xp, FLOAT_T *yp, FLOAT_T *hz)
{
  FLOAT_T V1[3], V2[3];
  FLOAT_T pp0, pp1;

  V1[0]=P.x;
  V1[1]=P.y;
  V1[2]=P.z;
 
  lb_al_multiply_matrix33_r_vector_r(Rot,V1,V2); 

  V2[0] = V2[0] - vp3d.cam.x - vp3d.cam_d;
  V2[1] = V2[1] - vp3d.cam.y;
  V2[2] = V2[2] - vp3d.cam.z;

  *hz = V2[2];
  if(vp3d.cam_h<=0.0)
    {
      pp0 =  V2[0]; 
      pp1 =  V2[1];
    }
  else
    {
      pp0 = vp3d.cam_h*V2[0] / (vp3d.cam_h-V2[2]); 
      pp1 = vp3d.cam_h*V2[1] / (vp3d.cam_h-V2[2]);
    }
  //lb_ft_printf(ty_C, "w.cam.x = %f\r\n",w.cam.x);
  //lb_ft_printf(ty_C, "w.cam.y = %f\r\n",w.cam.y);
  //lb_ft_printf(ty_C, "w.cam.z = %f\r\n",w.cam.z );
  //lb_ft_printf(ty_C, "(V2).array[0] = %f\r\n",(V2).array[0]);
  //lb_ft_printf(ty_C, "(V2).array[1]= % f\r\n",(V2).array[1]);
  //lb_ft_printf(ty_C, "(V2).array[2]= % f\r\n",(V2).array[2]);
  //lb_ft_printf(ty_C, "w.cam_d = %f\r\n",w.cam_d);
  //lb_ft_printf(ty_C, "w.cam_h = %f\r\n",w.cam_h);
  //lb_ft_printf(ty_C, "w.scale = %f\r\n",w.scale);
  //lb_ft_printf(ty_C, "pp0=%f\r\n",pp0);
  //lb_ft_printf(ty_C, "pp1=%f\r\n\r\n",pp1);

    
  *xp=0.5*(vp3d.xp_max-vp3d.xp_min) + vp3d.scale*pp0;
  *yp=0.5*(vp3d.yp_max-vp3d.yp_min) - vp3d.scale*pp1;
} 

void lb_gr_project_3d_analytical_two_vectors(VIEWPORT_3D_T vp3d, VECTOR_R_T *V1, VECTOR_R_T *V2, VECTOR_R_T *Pa, U_INT_16_T *xp, U_INT_16_T *yp, U_INT_8_T *flag, ERR_T *error)
{
  MATRIX_R_T M;
  M.rows=3;
  M.cols=3;
  lb_al_create_matrix_r(&M);

  VECTOR_R_T i_vec;
  i_vec.items=3;
  lb_al_create_vector_r(&i_vec);

  VECTOR_R_T j_vec;
  j_vec.items=3;
  lb_al_create_vector_r(&j_vec);

  VECTOR_R_T k_vec;
  k_vec.items=3;
  lb_al_create_vector_r(&k_vec);

  VECTOR_R_T temp_vec;
  temp_vec.items=3;
  lb_al_create_vector_r(&temp_vec);

  VECTOR_R_T X;
  X.items=3;
  lb_al_create_vector_r(&X);

  VECTOR_R_T Pp;
  Pp.items=3;
  lb_al_create_vector_r(&Pp);

  lb_al_cross_product_vector_r(V1,V2,&k_vec);
  lb_al_cross_product_vector_r(&k_vec,V1,&j_vec);

  lb_al_copy_vector_r(V1,&temp_vec);
  lb_al_normalize_vector_r(&temp_vec,&i_vec, error);

  lb_al_copy_vector_r(&j_vec,&temp_vec);
  lb_al_normalize_vector_r(&temp_vec,&j_vec,error);

  lb_al_copy_vector_r(&k_vec,&temp_vec);
  lb_al_normalize_vector_r(&temp_vec,&k_vec,error);

  //lb_al_vector_print(&i_vec,"i","%f");
  //lb_al_vector_print(&j_vec,"j","%08.4f");
  //lb_al_vector_print(&k_vec,"k","%08.4f");
 
  M.array[0][0]=j_vec.array[0];  
  M.array[0][1]=k_vec.array[0];  
  M.array[0][2]=(*Pa).array[0] - vp3d.cam.x + vp3d.cam_h*i_vec.array[0] - vp3d.cam_d*j_vec.array[0];

  M.array[1][0]=j_vec.array[1];
  M.array[1][1]=k_vec.array[1];  
  M.array[1][2]=(*Pa).array[1] - vp3d.cam.y + vp3d.cam_h*i_vec.array[1] - vp3d.cam_d*j_vec.array[1];
  
  M.array[2][0]=j_vec.array[2];  
  M.array[2][1]=k_vec.array[2];  
  M.array[2][2]=(*Pa).array[2] - vp3d.cam.z + vp3d.cam_h*i_vec.array[2] - vp3d.cam_d*j_vec.array[2];
  
  X.array[0] = (*Pa).array[0] - vp3d.cam.x;
  X.array[1] = (*Pa).array[1] - vp3d.cam.y;
  X.array[2] = (*Pa).array[2] - vp3d.cam.z;

  lb_al_solve_linear_system_r(&M, &X, &Pp);

  /* t becomes negative due to looking in the opposite direction */
  Pp.array[0] = -Pp.array[0] + vp3d.cam_d;
  
  *xp = 0.5*(vp3d.xp_max-vp3d.xp_min) + vp3d.scale*Pp.array[0];
  *yp = 0.5*(vp3d.yp_max-vp3d.yp_min) - vp3d.scale*Pp.array[1];
  *flag=1;

  if ((*xp<vp3d.xp_min) || (vp3d.xp_max<*xp) || (*yp<vp3d.yp_min) || (vp3d.yp_max<*yp) || (Pp.array[2]>1))
    {
      *xp=0;
      *yp=0;
      *flag=0;
    }

  lb_al_release_matrix_r(&M);
  lb_al_release_vector_r(&X);
  lb_al_release_vector_r(&i_vec);
  lb_al_release_vector_r(&j_vec);
  lb_al_release_vector_r(&k_vec);
  lb_al_release_vector_r(&temp_vec);
  lb_al_release_vector_r(&Pp);
} 


void lb_gr_render_picture(PICTURE_T *Pic, S_INT_16_T xc, S_INT_16_T yc, COPYMODE_T copymode)
{
  S_INT_16_T i, j;
  if (!lb_gr_assert_dimensions_picture(Pic))
    {
      printf("Error: lb_gr_render_picture() --> invalid dimension\r\n");
      exit(EXIT_FAILURE);
    }

  for (i=0;i<(*Pic).h;i++)
    for (j=0;j<(*Pic).w;j++)
      lb_gr_draw_pixel(NULL, j, i, (*Pic).pic[i][j], copymode);
}


#ifdef XXXXX
lb_gr_buffer_pixel(xc+j, yc+i, 255,255,255);
  
if ( (scale_x==1) && (scale_y==1) )
  {
    for (i=0;i<(*Pic).h;i++)
      for (j=0;j<(*Pic).w;j++)
	{
	  lb_fb_draw_pixel(ty_fb_main, xc + j, yc + i,
			   round(FACTOR_N_TO_8_R*(*Pic).pic[i][j].r),
			   round(FACTOR_N_TO_8_G*(*Pic).pic[i][j].g),
			   round(FACTOR_N_TO_8_B*(*Pic).pic[i][j].b),
			   round(FACTOR_N_TO_8_K*(*Pic).pic[i][j].a));
	}
  }
 else
   if(renderoptions & RENDEROPTIONS_LINE)
     {
       if ((scale_x<3) || (scale_y<3))
	 {
	   printf("Error: lb_gr_render_picture() --> invalid scale: scale_x=%d scale_y=%d\r\n",scale_x,scale_y);
	   exit(EXIT_FAILURE);
	 }
	
       for (i=0;i<(*Pic).h;i++)
	 for (j=0;j<(*Pic).w;j++)
	   {
	     U_INT_8_T temp_r, temp_g, temp_b, temp_k;
	     temp_r=round(FACTOR_N_TO_8_R*(*Pic).pic[i][j].r);
	     temp_g=round(FACTOR_N_TO_8_G*(*Pic).pic[i][j].g);
	     temp_b=round(FACTOR_N_TO_8_B*(*Pic).pic[i][j].b);
	     temp_k=round(FACTOR_N_TO_8_K*(*Pic).pic[i][j].a);
		  
	     lb_fb_line_h(ty_fb_main, yc+ scale_y*i, xc+ scale_x*j, xc+ scale_x*(j+1)-2,
			  temp_r, temp_g, temp_b, temp_k);

	     lb_fb_line_h(ty_fb_main, yc+ scale_y*(i+1)-2, xc+ scale_x*j, xc+ scale_x*(j+1)-2,
			  temp_r, temp_g, temp_b, temp_k);
	
	     lb_fb_line_v(ty_fb_main, xc+ scale_x*j, yc+ scale_y*i, yc+ scale_y*(i+1)-2,
			  temp_r, temp_g, temp_b, temp_k);

	     lb_fb_line_v(ty_fb_main, xc+ scale_x*(j+1)-2, yc+ scale_y*i, yc+ scale_y*(i+1)-2,
			  temp_r, temp_g, temp_b, temp_k);
	   }
     } 
   else
     for (i=0;i<(*Pic).h;i++)
       for (j=0;j<(*Pic).w;j++)
	 {
	   lb_fb_rectangle(ty_fb_main, xc+j*scale_x, yc+i*scale_y, xc+(j+1)*scale_x, yc+(i+1)*scale_y,
			   round(FACTOR_N_TO_8_R*(*Pic).pic[i][j].r),
			   round(FACTOR_N_TO_8_G*(*Pic).pic[i][j].g),
			   round(FACTOR_N_TO_8_B*(*Pic).pic[i][j].b),
			   round(FACTOR_N_TO_8_K*(*Pic).pic[i][j].a));	
	 }
}
#endif
    
void       lb_gr_plot_continuous_fn_2d(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, FUNCTION_X fx_t, FUNCTION_X fy_t,
				       FLOAT_T t0, FLOAT_T t1, FLOAT_T delta,
				       S_INT_16_T max_exp, PIXEL_T color, COPYMODE_T copymode)
{
  FLOAT_T t, t_temp, t_min, t_max, xr, yr, xp0, yp0, xp1, yp1, factor_delta;
  S_INT_16_T xp0_i, yp0_i, xp1_i, yp1_i;
  S_INT_8_T flag_adjacent, counter_adjacent, exponent;
  ERR_T error;
  if (t1>=t0)
    {
      t_min=t0;
      t_max=t1;
    }
  else
    {
      t_max=t0;
      t_min=t1;
    }
  t=t_min;
  xr=fx_t(t,&error);
  yr=fy_t(t,&error);
  
  lb_gr_project_2d(vp2d, xr, yr, &xp0, &yp0);
  xp0_i = round(xp0);
  yp0_i = round(yp0);
  lb_gr_draw_pixel(Pic, xp0_i, yp0_i, color, copymode);

  exponent=0;
  factor_delta=1.0;
  counter_adjacent=0;
  while (t<=t_max)
    {
      do
	{
	  t_temp=t;
	  t_temp+=delta*factor_delta;
	  xr=fx_t(t_temp,&error);
	  yr=fy_t(t_temp,&error);
  
	  lb_gr_project_2d(vp2d, xr, yr, &xp1, &yp1);
	  xp1_i = round(xp1);
	  yp1_i = round(yp1);
	  if ( (abs(xp1_i-xp0_i)<=1) && (abs(yp1_i-yp0_i)<=1) )
	    {
	      flag_adjacent=TRUE;
	      counter_adjacent++;
	    }
	  else
	    {
	      flag_adjacent=FALSE;
	      counter_adjacent=0;
	    }
	     
	  if (!flag_adjacent)
	    {
	      exponent++;
	      factor_delta=pow(2.0,-exponent);
	      //lb_ft_printf(ty_C, "exponent=%d,factor =%f\r\n",exponent,factor_delta);
	    }
	  else
	    if (counter_adjacent>32)
	      {
		/* After "several" consecutive adjacent points, the next larger step can be tried */ 
		exponent--;
		factor_delta=pow(2.0,-exponent);
		//lb_ft_printf(ty_C, "exponent=%d,factor =%f\r\n",exponent,factor_delta);
	      }
	       
	} while (!flag_adjacent);
      t=t_temp;
      lb_gr_draw_pixel(Pic, xp1_i, yp1_i, color, copymode);
      xp0_i=xp1_i;
      yp0_i=yp1_i;
    }
}

void lb_gr_plot_continuous_fn_2d_antialiasing(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, FUNCTION_X fx_t, FUNCTION_X fy_t,
					      FLOAT_T t0, FLOAT_T t1, FLOAT_T delta,
					      S_INT_16_T max_exp, FLOAT_T w, PIXEL_T color, COPYMODE_T copymode)
{
  MATRIX_S_INT_8_T Mflags;
  VECTOR_R_T Vx, Vy;
  FLOAT_T Vtemp_x[64], Vtemp_y[64];
  FLOAT_T t, t_temp, t_min, t_max, xr, yr, factor_delta, xp0_f, yp0_f, xp1_f, yp1_f, distance, temp, w2;
  S_INT_8_T flag_adjacent, counter_adjacent, exponent;
  S_INT_16_T n_temp, i, old_size_Vx, old_size_Vy, i_min, j, i_max, j_min, j_max, k;
  PIXEL_T pix_main;
  ERR_T error;
 
   
  if (t1>=t0)
    {
      t_min=t0;
      t_max=t1;
    }
  else
    {
      t_max=t0;
      t_min=t1;
    }
  t=t_min;
  xr=fx_t(t,&error);
  yr=fy_t(t,&error);

  Vx.items=0;
  Vy.items=0;
  lb_al_create_vector_r(&Vx);
  lb_al_create_vector_r(&Vy);

  lb_gr_project_2d(vp2d, xr, yr, &xp0_f, &yp0_f);
  Vtemp_x[0]=xp0_f;
  Vtemp_y[0]=yp0_f;

  exponent=0;
  factor_delta=1.0;
  counter_adjacent=0;
  n_temp=1;

  while (t<=t_max)
    {
      //lb_ft_printf(ty_C, "t=%f  [%f ; %f]\r\n",t,t_min,t_max);
      do
	{
	  t_temp=t;
	  t_temp+=delta*factor_delta;
	  xr=fx_t(t_temp,&error);
	  yr=fy_t(t_temp,&error);
	  lb_gr_project_2d(vp2d, xr, yr, &xp1_f, &yp1_f);
	   
	  /* Minimum distance between adjacent points should be at most one tenth of a pixel */
	  if ( (abs(xp1_f-xp0_f)<=0.5) && (abs(yp1_f-yp0_f)<=0.5) )
	    {
	      flag_adjacent=TRUE;
	      counter_adjacent++;
	      Vtemp_x[n_temp]=xp1_f;
	      Vtemp_y[n_temp]=yp1_f;
	      n_temp++;
	      if (n_temp==64) /* Enough data has been cached */
		{
		  old_size_Vx=Vx.items;
		  old_size_Vy=Vy.items;
		  lb_al_resize_vector_r(&Vx,old_size_Vx+64);
		  lb_al_resize_vector_r(&Vy,old_size_Vy+64);
		  for (i=0;i<64;i++)
		    {
		      Vx.array[old_size_Vx+i]=Vtemp_x[i];
		      Vy.array[old_size_Vy+i]=Vtemp_y[i];
		    }
		  n_temp=0;
		  //lb_al_print_vector_r(&Vy,"Vy",FLOAT_FORMAT);
		  //lb_ft_printf(ty_C, "\r\n\r\n");
		  // lb_al_print_vector_r(&Vy,"Vy",FLOAT_FORMAT);
		}
	    }
	  else
	    {
	      flag_adjacent=FALSE;
	      counter_adjacent=0;
	    }
	     
	  if (!flag_adjacent)
	    {
	      if (abs(exponent)<=max_exp)
		exponent--;
	      factor_delta=pow(1.2,exponent);
	      // lb_ft_printf(ty_C, "exponent=%d,factor =%f\r\n",exponent,factor_delta);
	    }
	  else
	    if (counter_adjacent>4)
	      {
		/* After "several" consecutive adjacent points, the next larger step can be tried */ 
		if (abs(exponent)<=max_exp)
		  exponent++;
		factor_delta=pow(1.2,exponent);
		//lb_ft_printf(ty_C, "exponent=%d,factor =%f\r\n",exponent,factor_delta);
	      }
	       
	} while (!flag_adjacent);
      t=t_temp;
      xp0_f=xp1_f;
      yp0_f=yp1_f;
    }
  if (n_temp!=0)
    {
      old_size_Vx=Vx.items;
      old_size_Vy=Vy.items;
      lb_al_resize_vector_r(&Vx,old_size_Vx+n_temp);
      lb_al_resize_vector_r(&Vy,old_size_Vy+n_temp);
      for (i=0;i<n_temp;i++)
	{
	  Vx.array[old_size_Vx+i]=Vtemp_x[i];
	  Vy.array[old_size_Vy+i]=Vtemp_y[i];
	}
      //lb_al_print_vector_r(&Vx,"Vx",FLOAT_FORMAT);
      //lb_al_print_vector_r(&Vy,"Vy",FLOAT_FORMAT);
    }
  //lb_al_print_vector_r(&Vx,"Vx",FLOAT_FORMAT);
  //lb_al_print_vector_r(&Vy,"Vy",FLOAT_FORMAT);
  
  
  pix_main=color;
  w2=0.5*w;

  /* The flag matrix tags the neighbor points around the line from which the shortest distance will be computed */ 
  /* j = x = cols */
  /* i = y = rows */
  /* matrix [i][j] */
  
  Mflags.rows=abs(vp2d.yp_max-vp2d.yp_min);
  Mflags.cols=abs(vp2d.xp_max-vp2d.xp_min);
  lb_al_create_matrix_si8(&Mflags);

  for(i=0;i<Mflags.rows;i++)
    for(j=0;j<Mflags.cols;j++)
      Mflags.array[i][j]=FALSE;
      
  for(k=0;k<Vx.items;k++)
    {
      /*** Check boundaries ***/
      j_min=floor(Vx.array[k]-w2-vp2d.xp_min);
      if(j_min<0)
	j_min=0;
      if(j_min>Mflags.cols)
	j_min=Mflags.cols;

      j_max=ceil(Vx.array[k]+w2-vp2d.xp_min);
      if(j_max<0)
	j_max=0;
      if(j_max>Mflags.cols)
	j_max=Mflags.cols;

      i_min=floor(Vy.array[k]-w2-vp2d.yp_min);
      if(i_min<0)
	i_min=0;
      if(i_min>Mflags.rows)
	i_min=Mflags.rows;
      
      i_max=ceil(Vy.array[k]+w2-vp2d.yp_min);
      if(i_max<0)
	i_max=0;
      if(i_max>Mflags.rows)
	i_max=Mflags.rows;
      
      for(i=i_min;i<i_max;i++)
	for(j=j_min;j<j_max;j++)
	  {
	    Mflags.array[i][j]=TRUE;
	    //lb_gr_draw_pixel(Pic, j+vp.xp_min+1, i+vp.yp_min+1, lb_gr_12RGB(COLOR_YELLOW | COLOR_SOLID), copymode);
	  }
    }
    
  for(i=0;i<Mflags.rows;i++)
    for(j=0;j<Mflags.cols;j++)
      if(Mflags.array[i][j] ) /* Work on only those points which were close to the line */
	{
	  //lb_gr_draw_pixel(Pic, xi+vp.xp_min, yi+vp.yp_min, pix_main, copymode); 
	  pix_main.a=0;

	  /* For each pixel, we calculate the shortest distance to the curve */
	  distance=10000;
	  
	  for(k=0;k<Vx.items;k++)
	    {
	      temp=lb_re_sqrt(lb_re_sqr(j+vp2d.xp_min-Vx.array[k],&error)+lb_re_sqr(i+vp2d.yp_min-Vy.array[k],&error),&error);
	      if (temp<distance)
		distance=temp;
	      //lb_ft_printf(ty_C, "distance=%f\r\n",distance);
	      if(distance<w2)
		pix_main.a=MAX_A;
	      else if(fabs(w2-distance)<=1.0)
		pix_main.a=round(MAX_A*(1.0-fabs(w2-distance))*M_SQRT1_2); 
	    }
	  if (pix_main.a!=0)
	    lb_gr_draw_pixel(Pic, j+vp2d.xp_min, i+vp2d.yp_min, pix_main, copymode); 
	}
  lb_al_release_matrix_si8(&Mflags);
  lb_al_release_vector_r(&Vx);
  lb_al_release_vector_r(&Vy);
}		  

PIXEL_T lb_gr_value_to_color(S_INT_8_T value)
{
  PIXEL_T color;
  switch (value % 10)
    {
    case 0:
      color=lb_gr_12RGB(COLOR_BLACK);
      break;
    case 1:
      color=lb_gr_12RGB(COLOR_BROWN);
      break;
    case 2:
      color=lb_gr_12RGB(COLOR_RED);
      break;
    case 3:
      color=lb_gr_12RGB(COLOR_ORANGE);
      break;
    case 4:
      color=lb_gr_12RGB(COLOR_YELLOW);
      break;
    case 5:
      color=lb_gr_12RGB(COLOR_GREEN);
      break;
    case 6:
      color=lb_gr_12RGB(COLOR_BLUE);
      break;
    case 7:
      color=lb_gr_12RGB(COLOR_VIOLET);
      break;
    case 8:
      color=lb_gr_12RGB(COLOR_GRAY);
      break;
    case 9:
      color=lb_gr_12RGB(COLOR_WHITE);
      break;
    }
  return color;
}
