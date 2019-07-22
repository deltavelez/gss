#include <termios.h>
#include <unistd.h> 
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <wchar.h>
#include <locale.h>
#include <pthread.h>
#include <stdarg.h>
#include <sys/sysinfo.h>
#include <SDL2/SDL.h>

#include "lb_audio.h"
#include "lb_geometry.h"
#include "lb_types.h"
#include "lb_fourier.h"
#include "lb_algebra.h"
#include "lb_integer.h"
#include "lb_real.h"
#include "lb_statistics.h"
#include "lb_complex.h"
#include "lb_parser.h" 
#include "lb_real.h"
#include "lb_fonts.h"
#include "lb_graphics.h"
#include "lb_console.h"
#include "lb_numer.h"
#include "lb_serial.h"

#define DEBUGGING
#define N_HALF_SAMPLES 1024
#define N_EXPERIMENT_MAX 100000
#define NOISE_STEPS_MAX 100
#define FREQ_1 4291.0
#define FREQ_2 5464.0


extern PICTURE_T ty_pic_shadow;
extern CONSOLE_T *ty_C;


/* for DEMO_PARALELL */

void *floatsum(void *x_max_par)
{
  double  x, x_max, sum;
  clock_t begin, end;

  x_max= *((double *)x_max_par);

  begin=clock();

  sum=0.0;
  x=0;
  begin=clock();
  while (x<x_max)
    {
      sum+=x;
      x+=1e-8;
    }
  end = clock();
  printf("sum=%0.4e  ",sum);
  printf("time elapsed = %.4e\n",(double)(end - begin) / CLOCKS_PER_SEC);
  /* Check CPU usage: 
     mpstat -P ALL 1
  */
  return 0;
}


FLOAT_T diego_sin(FLOAT_T x, FLOAT_T y, ERR_T *error)
{
  return -0.1*x;
}

FLOAT_T diego_x(FLOAT_T t, ERR_T *error)
{
  return 0.75*sin(5*t)*cos(t);
}

FLOAT_T diego_y(FLOAT_T t, ERR_T *error)
{
  return 0.75*sin(5*t)*sin(t);
  
}

FLOAT_T f(FLOAT_T t, FLOAT_T w1, FLOAT_T w2) 
{
  if (t<0) 
    return sin(w2*(t+ 2*M_PI/w2));
  if ((0<=t) && (t<=2*M_PI/w1))
    return sin(w1*t);
  else
    return sin(w2*(t-2*M_PI/w1));
  return sin(w1*t);
}

S_INT_8_T parse_vector(VECTOR_C_T *buffer)
{
  S_INT_8_T sign;
  U_INT_16_T time_stamp[5];
  U_INT_16_T k, i, time_thereshold;
  time_thereshold=round((*buffer).items*0.25*(1.0/FREQ_1+1.0/FREQ_2)/(1.0/FREQ_2+1.5/FREQ_1));
  //printf("Thereshold = %i\r\n",time_thereshold);
  k=0;
  i=0;
  sign=lb_re_sign((*buffer).array[0].r);
  while ((i<(*buffer).items) && (k<5))
    {
      if (lb_re_sign((*buffer).array[i].r) != sign)
	{ 
	  sign=lb_re_sign((*buffer).array[i].r);
	  time_stamp[k]=i;
	  k++;
	}
      i++;
    }
  /*  for (k=0;k<5;k++)
      {
      printf("K [%i] =  %i\r\n",k,time_stamp[k]);
      }
  */

  if ( ((time_stamp[1]-time_stamp[0])<= time_thereshold) &&
       ((time_stamp[2]-time_stamp[1])<= time_thereshold) &&
       ((time_stamp[3]-time_stamp[2])>  time_thereshold) &&
       ((time_stamp[4]-time_stamp[3])>  time_thereshold) )
    return 1; 
  return 0;
}

FLOAT_T RMS_noise_time_complex(VECTOR_C_T *V, U_INT_16_T a, U_INT_16_T b)
{
  FLOAT_T temp;
  U_INT_16_T i;

  temp=0.0;
  for(i=a;i<b;i++)
    temp+=(*V).array[i].r*(*V).array[i].r + (*V).array[i].i*(*V).array[i].i;
  return sqrt(temp/((FLOAT_T)b-(FLOAT_T)a));
} 


S_INT_16_T lb_gis_height(FLOAT_T lon, FLOAT_T lat)
{
  FILE *fp;
  char filename[16];
  char str[4];
  long pos_lat, pos_lon;
  int height;
    
  strcpy(filename,"\0");
  if (lat>=0.0)
    {
      strcat(filename,"N");
      sprintf(str, "%02d", (S_INT_16_T)lat);
      pos_lat=round(1201.0*lb_re_frac(lat));
    }
  else
    {
      strcat(filename,"S");
      sprintf(str, "%02d", (S_INT_16_T)ceil(-lat));
      pos_lat=round(1201.0*(1.0+lb_re_frac(lat)));
    }
  strcat(filename,str);
  if (lon>=0.0)
    {
      strcat(filename,"E");
      sprintf(str, "%03d", (S_INT_16_T)lon);
      pos_lon=round(1201*lb_re_frac(lon));
    }
  else
    {
      strcat(filename,"W");
      sprintf(str, "%03d", (S_INT_16_T)ceil(-lon));
      pos_lon=round(1201.0*(1.0+lb_re_frac(lon)));
    }
  strcat(filename,str);
  strcat(filename,".hgt");
  // printf("Filename: %s, lon=%f pos_lon=%d, lat=%f pos_lat=%d  ",filename,lat,pos_lon,pos_lat);
  
  if (!(fp = fopen(filename, "r")))
    {
      perror("invalid filename\r\n");
      return 0;
    }
  
  fseek(fp,2*1201*pos_lat+2*pos_lon,SEEK_SET);
  fread(&height,sizeof(height),1,fp);
  fclose(fp);
  return lb_in_littleS16_to_bigS16(height);
}


 
//  lb_co_color_fg(lb_gr_12RGB(COLOR_WHITE));
//lb_co_color_bg(lb_gr_12RGB(COLOR_BLACK));

void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
  int bpp = surface->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to set */
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

  switch(bpp) {
  case 1:
    *p = pixel;
    break;

  case 2:
    *(Uint16 *)p = pixel;
    break;

  case 3:
    if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
      p[0] = (pixel >> 16) & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = pixel & 0xff;
    } else {
      p[0] = pixel & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = (pixel >> 16) & 0xff;
    }
    break;

  case 4:
    *(Uint32 *)p = pixel;
    break;
  }
}

  
int test_f1(int x, int y, int z)
{
  return x+y+z;
}

int test_f2(int x, int y, int z)
{
  return test_f1(x, y, z);
}

int (*ptr2)(int, int);
int (*ptr3)(int, int, int);

int sum2(int a, int b)
{
  return a+b;
}

int sum3(int a, int b, int c)
{
  return a+b+c;
}


#define pel(surf, x, y, rgb) ((unsigned char *)(surf->pixels))[y*(surf->pitch)+x*3+rgb]

//#define WIDTH 1920
//#define HEIGHT 1080

//#define WIDTH 1200
//#define HEIGHT 800

//#define WIDTH 512
//#define HEIGHT 512

//#define WIDTH 256
//#define HEIGHT 256

#define WIDTH 1024
#define HEIGHT 768

int main()
{

  //#define NEW_DEMO_SDL
#ifdef NEW_DEMO_SDL
  SDL_Event event;
  
  int x,y,z;
  char color;
  clock_t begin, end;

  lb_gr_SDL_init(SDL_INIT_VIDEO, 320, 200);
  

  while (1)
    {
      if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
	break;
    }
  begin=clock();
  end=clock();
  printf("frames per second = %f\n",(double)1000.0*CLOCKS_PER_SEC/(double)(end - begin) );
  SDL_Quit();
  return EXIT_SUCCESS;
#endif

  //#define DEMO_SDL
#ifdef DEMO_SDL
  SDL_Event event;
  SDL_Renderer *renderer;
  SDL_Window *window;
  SDL_Texture *texture;
  
  int x,y,z;

  unsigned char *pixels;
  int pitch;

  SDL_Init(SDL_INIT_VIDEO);
  //SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);
  window = SDL_CreateWindow("Hello World", 0, 0, WIDTH, HEIGHT, 0);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
    
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);
  char color;
  clock_t begin, end;


  begin=clock();
 
  for (z=0; z<1000; z++)
    {
      SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);
      for (y=0; y<HEIGHT; y++)
	for (x=0; x<WIDTH; x++)
	  {
	    unsigned int offset;
	    offset= WIDTH*4*y+x*4;
	    *((unsigned char*)pixels+offset)=  x % 255;
	    *((unsigned char*)pixels+offset+1)= y % 255;
	    *((unsigned char*)pixels+offset+2)=  z % 255;
	    *((unsigned char*)pixels+offset+3)=  255;
	  }

      if (0) for (y=0; y<HEIGHT; y++)
	       for (x=0; x<WIDTH; x++)
		 {
		   unsigned int offset;
		   offset= WIDTH*4*y+x*4;
		   printf("value = %u, %u, %u, %u\r\n",
			  (unsigned int)*((unsigned char*)pixels+offset),
			  (unsigned int)*((unsigned char*)pixels+offset+1),
			  (unsigned int)*((unsigned char*)pixels+offset+2),
			  (unsigned int)*((unsigned char*)pixels+offset+3));
		 }
   
      //SDL_RenderClear(renderer);
      SDL_UnlockTexture(texture);
      SDL_UpdateTexture(texture, NULL, &pixels[0], pitch); //Copy entire array only once
      SDL_RenderCopy(renderer, texture, NULL, NULL);
      SDL_RenderPresent(renderer);
      //lb_gr_delay(200);
    }
  end=clock();
  printf("frames per second = %f\n",(double)1000.0*CLOCKS_PER_SEC/(double)(end - begin) );

  SDL_Quit();
  while (1) {
    if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
      break;
  }
  return EXIT_SUCCESS;
  
  exit(1);
#endif

  
#ifdef DEMO_SDL_WORKS
  SDL_Event event;
  SDL_Renderer *renderer;
  SDL_Window *window;
  SDL_Texture *texture;
  
  int x,y,z;

  char *pixels;
  int pitch;

  pitch=WIDTH*4;

  pixels=malloc(WIDTH*HEIGHT*4);

  SDL_Init(SDL_INIT_VIDEO);
  //SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);
  window = SDL_CreateWindow("Hello World", 0, 0, WIDTH, HEIGHT, 0);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, WIDTH, HEIGHT);
    
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);
  char color;
  clock_t begin, end;


  begin=clock();
  for (z=0; z<1000; z++)
    {
      //SDL_LockTexture(texture, NULL, &pixels, &pitch); //Copy entire array only once
      //printf("z=%d\r\n",z);
      for (y=0; y<HEIGHT; y++)
	for (x=0; x<WIDTH; x++)
	  {
	    unsigned int offset;
	    offset= WIDTH*4*y+x*4;
	    *((char*)pixels+offset)=  x % 255;
	    *((char*)pixels+offset+1)= y % 255;
	    *((char*)pixels+offset+2)=  z % 255;
	    *((char*)pixels+offset+3)=  255;
	    printf("x=%d, y=%d, z=%d\r\n",x,y,z);
	  }
      //SDL_RenderClear(renderer);
      // SDL_UnlockTexture(texture);
      SDL_UpdateTexture(texture, NULL, &pixels[0], pitch); //Copy entire array only once
      SDL_RenderCopy(renderer, texture, NULL, NULL);
      SDL_RenderPresent(renderer);
      //lb_gr_delay(200);
    }
  end=clock();
  printf("frames per second = %f\n",(double)1000.0*CLOCKS_PER_SEC/(double)(end - begin) );

  SDL_Quit();
  while (1) {
    if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
      break;
  }
  return EXIT_SUCCESS;
  
  exit(1);
#endif
  
  //#define DEMO_TIMIMNG_FB
#ifdef DEMO_TIMIMNG_FB
  S_INT_16_T x, y, z, width, height;
  clock_t begin, end;

  lb_fb_get_res("/dev/fb0", &width, &height);
  printf("width = %d, height=%d\r\n",width,height);
 	
  lb_fb_open("/dev/fb0", "/dev/tty1", 8, 8,  0*RENDEROPTIONS_LINE | RENDEROPTIONS_GRAPHICS_ONLY );

  begin=clock();
 
  for (z=0; z<1000; z++)
    for (y=0; y<height; y++)
      for (x=0; x<width; x++)
	lb_fb_draw_pixel(ty_fb_main, x, y, x%255, y%255, z%255,255);
      
  end=clock();
  lb_fb_setmode_text();
  printf("frames per second = %f\n",(double)1000.0*CLOCKS_PER_SEC/(double)(end - begin) );
  
  lb_fb_exit(1);

  
#endif


  //#define MATRIX_NDIM
#ifdef MATRIX_NDIM
  
    /*******************************************************************************************************************/
    /* General Matrix Handling test                                                                                        */
    /*******************************************************************************************************************/

  	  
    ARRAY_R_T M;
    M.n=4;
    M.dim[0]=4;
    M.dim[1]=3;
    M.dim[2]=3;
    M.dim[3]=2;
    M.dim[4]=2;
    U_INT_16_T i,j,k,l;
    long cummulative;
    S_INT_8_T flag;
    struct sysinfo info_1, info_2, info_3;

    lb_al_create_array_r(&M);

    switch (M.n)
      {
      case 1:
	for (j=0;j<M.dim[0];j++)
	  M.a1[j]=((FLOAT_T)rand()/RAND_MAX-0.5)*pow(10,10*(FLOAT_T)rand()/RAND_MAX);
	break;
      case 2:
	for (i=0;i<M.dim[1];i++)
	  for (j=0;j<M.dim[0];j++)
	    M.a2[i][j]=((FLOAT_T)rand()/RAND_MAX-0.5)*pow(10,10*(FLOAT_T)rand()/RAND_MAX);
	break;
      case 3:
	for (k=0;k<M.dim[2];k++)
	  for (i=0;i<M.dim[1];i++)
	    for (j=0;j<M.dim[0];j++)
	      M.a3[k][i][j]=((FLOAT_T)rand()/RAND_MAX-0.5)*pow(10,10*(FLOAT_T)rand()/RAND_MAX);
	break;
      case 4:
	for (l=0;l<M.dim[3];l++)
	  for (k=0;k<M.dim[2];k++)
	    for (i=0;i<M.dim[1];i++)
	      for (j=0;j<M.dim[0];j++)
		M.a4[l][k][i][j]=((FLOAT_T)rand()/RAND_MAX-0.5)*pow(10,10*(FLOAT_T)rand()/RAND_MAX);
	break;
      }
    lb_al_print_array_r(&M, "",12,2 );
    lb_al_release_array_r(&M);
  

    exit(1);
  
    cummulative=0;
    /* Checking for memory leaks */

    if (0) for (l=0;l<1000000;l++)
	     {
	       //printf("size=%d\r\n",sizeof(FLOAT_T));

	       //flag=!(l % 1000);
	       flag=TRUE;
	       if (flag)
		 sysinfo(&info_1);
	       lb_al_create_array_r(&M);
	       //lb_gr_delay(2000);
 
	       if (flag)
		 sysinfo(&info_2);
	       lb_al_release_array_r(&M);
	       //lb_gr_delay(2000);
 
	       if (flag)
		 sysinfo(&info_3);
  
	       if (flag)
		 {
		   cummulative+=info_2.freeram-info_1.freeram+info_3.freeram-info_2.freeram;
		   printf("[%d] 1) %f  2) %f 3) %f cum KBytes=%ld\r\n", 
			  l,
			  100.0*info_1.freeram/info_1.totalram,
			  100.0*info_2.freeram/info_1.totalram,
			  100.0*info_3.freeram/info_1.totalram,
			  cummulative/1024);
		 }
	     }
  
    exit(1);
#endif
  
  
    /*******************************************************************************************************************/
    /* General Computer Science                                                                                        */
    /*******************************************************************************************************************/
  
    /* This demo is the base for a two-dimensional circular buffer such as the one used in text consoles */

    //#define DEMO_2D_CIRCULAR_BUFFER_TEXT
#ifdef DEMO_2D_CIRCULAR_BUFFER_TEXT
#define MAX_COLS 4
#define MAX_ROWS 12
#define MAX_ROWS_V 4
#define MAX_N 10

    S_INT_16_T n_row, w_row, w_col, s_col, flag_insert, n_back, n_back_adj, j, n_vis_rows;
    char c;
    char V[MAX_ROWS][MAX_COLS];
  
    n_row=0;
    w_row=0;
    w_col=0;
    n_back=0;
    s_col=0;
    flag_insert=TRUE;
    if (1)
      while (c!=ASCII_ESC)
	{
	  while (!kbhit()) ;
	  c=pc_getch();
	  switch (c)
	    {
	    case PCKEY_UP:
	      if (n_row>n_back)
		{
		  n_back++;
		  if (n_back>MAX_ROWS-1)
		    n_back=MAX_ROWS-1;
		}
	      if (n_back>MAX_ROWS_V-1)
		n_back_adj=n_back-MAX_ROWS_V+1;
	      break;
	    case PCKEY_DOWN:
	      n_back--;
	      if (n_back<0)
		n_back=0;

	      printf("\n** n_row-n_back =%d\r\n",n_row-n_back);
	      if (n_row-n_back>MAX_ROWS_V-1)
		n_back_adj=n_back;
	      break;
	    case PCKEY_LEFT:
	      s_col--;
	      if (s_col<0)
		{
		  if (n_row>n_back)
		    {
		      s_col=MAX_COLS-1;
		      n_back++;
		      if (n_back>MAX_ROWS-1)
			n_back=MAX_ROWS-1;
		      if (n_back>MAX_ROWS_V-1)
			n_back_adj=n_back-MAX_ROWS_V+1;
		    }
		  else
		    s_col=0;
		}
	      break;
	    case PCKEY_RIGHT:
	      if (n_back==0)
		{
		  if (s_col<w_col)
		    s_col++;
		}
	      else
		{
		  s_col++;
		  if (s_col>=MAX_COLS)
		    {
		      s_col=0;
		      n_back--;
		      if (n_back<0)
			n_back=0;
		      if (n_row-n_back>MAX_ROWS_V-1)
			n_back_adj=n_back;
		    }
		}
	      break;
	    case PCKEY_INSERT:
	      flag_insert=!flag_insert;
	      printf("Insert =%d\r\n",flag_insert);;
	      break;
	    case PCKEY_BS:
	      w_col--;
	      V[w_row][w_col]='\0';
	      if (w_col<0)
		{
		  if (n_row>0)
		    {
		      w_col=MAX_COLS-1;
		      n_row--;
		      w_row--;
		      if (w_row<0)
			w_row=MAX_ROWS-1;
		    }
		  else
		    w_col=0;
		}
	      n_back=0;
	      n_back_adj=n_back;
	      s_col=w_col;
	      break;
	    case PCKEY_ENTER:
	      w_col=0;
	      w_row++;
	      if (w_row>=MAX_ROWS)
		w_row=0;
	
	      for (j=0;j<MAX_COLS;j++)
		V[w_row][j]='\0';
	      n_row++;
	      if (n_row>=MAX_ROWS)
		n_row=MAX_ROWS-1;
	      n_back=0;
	      s_col=w_col;
	      break;
	    case '\r':
	      w_col=0;
	      s_col=w_col;
	      break;
	    default:
	      V[w_row][w_col]=c;
	      w_col++;
	      if (w_col>=MAX_COLS)
		{
		  w_col=0;
		  w_row++;
		  if (w_row>=MAX_ROWS)
		    w_row=0;
		  printf("\r\n");
		  for (j=0;j<MAX_COLS;j++)
		    V[w_row][j]='\0';
		  n_row++;
		  if (n_row>=MAX_ROWS)
		    n_row=MAX_ROWS-1;
		}
	      n_back=0;
	      n_back_adj=n_back;
	      s_col=w_col;
	      break;
	    }
	
	  V[w_row][w_col]='_';
	
	  for (j=0;j<MAX_COLS;j++)
	    printf("-");
	  printf("\n");

	  if (1) for (i=0;i<MAX_ROWS;i++)
		   {
		     for (j=0;j<MAX_COLS;j++)
		       printf("[%c]",V[i][j]);
		     printf("\n");
		   }

	  printf("\n");
	  for (j=0;j<MAX_COLS;j++)
	    printf("-");
	  printf("\n");

	  n_vis_rows=lb_in_min(n_row,MAX_ROWS_V-1);
	  printf("n_row=%d, n_back=%d\r\n",n_row,n_back);
	  for (i=0;i<=n_vis_rows;i++)
	    {
	      S_INT_16_T temp_cursor, temp_row;
	      for (j=0;j<MAX_COLS;j++)
		{
		  temp_cursor=(w_row-n_back+MAX_ROWS)%MAX_ROWS;
		  temp_row=(i+w_row-n_back_adj-n_vis_rows+MAX_ROWS)%MAX_ROWS;
		  if ((temp_cursor==temp_row) && (j==s_col))
		    lb_co_color_fg(lb_gr_12RGB(COLOR_BLUE));
		  printf("[%c]",V[temp_row][j]);
		  if ((temp_cursor==temp_row) && (j==s_col))
		    lb_co_color_fg(lb_gr_12RGB(COLOR_WHITE));
		}
	      printf("\n");
	    }
	}
    exit(1);
#endif

    /*******************************************************************************************************************/
    /* Vectors and Matrices */
    /******************************************************************************************************************/

    //#define DEMO_SWAP_VECTOR
#ifdef DEMO_SWAP_VECTOR
    VECTOR_R_T A, B;
    U_INT_16_T i;
    A.items=10; 
    B.items=20;

    lb_al_create_vector_r(&A); 
    lb_al_create_vector_r(&B); 

    for(i=0;i<A.items;i++)
      A.array[i]=100+0.1*i;
    for(i=0;i<B.items;i++)
      B.array[i]=999+0.1*i;

    lb_al_print_vector_r(&A, "A", "%0.2f\r\n");
    lb_al_print_vector_r(&B, "B", "%0.2f\r\n");

    lb_al_swap_vector_r(&A, &B);
	
    lb_al_print_vector_r(&A, "A", "%0.2f\r\n");
    lb_al_print_vector_r(&B, "B", "%0.2f\r\n");

    lb_al_release_vector_r(&A);
    lb_al_release_vector_r(&B);
    exit(1);
#endif 

    //#define DEMO_INSERT_VECTOR
#ifdef DEMO_INSERT_VECTOR
    VECTOR_R_T V;
    U_INT_16_T k;
    V.items=0;
 
    for (k=11;k<=22;k++)
      {
	lb_al_insert_item_vector_r(&V, k,0);
	lb_al_print_vector_r(&V, "V", "%0.4f\r\n");
      }
    //lb_al_release_vector_r(&V);
    exit(1);
#endif

    //  #define DEMO_DELETE_ITEM_VECTOR
#ifdef DEMO_DELETE_ITEM_VECTOR
    VECTOR_R_T V;
    U_INT_16_T i, k;
     
    for(k=0;k<1;k++)
      {
	system("clear");
	V.items=10;
	lb_al_create_vector_r(&V);
	for (i=0;i<V.items;i++)
	  V.array[i]=k+0.1*i;
	lb_al_print_vector_r(&V, "V", "%0.4f\r\n");
	printf("N_before=%d\r\n",V.items);
	lb_al_delete_item_vector_r(&V, 0);
	lb_al_delete_item_vector_r(&V, 0);
	lb_al_delete_item_vector_r(&V, 0);
	lb_al_delete_item_vector_r(&V, 0);
	lb_al_delete_item_vector_r(&V, 0);
	lb_al_delete_item_vector_r(&V, 0);
	lb_al_delete_item_vector_r(&V, 0);
	lb_al_delete_item_vector_r(&V, 0);
	lb_al_delete_item_vector_r(&V, 0);
	lb_al_print_vector_r(&V, "W", "%0.4f\r\n");
	lb_al_delete_item_vector_r(&V, 0);
	lb_al_print_vector_r(&V, "Z", "%0.4f\r\n");
     
	lb_al_release_vector_r(&V);
      }
    exit(1);
#endif 

  
    //#define DEMO_INTERPOLATE
#ifdef DEMO_INTERPOLATE
    MATRIX_R_T A;
    S_INT_16_T i,j;
    FLOAT_T Q;
    ERR_T error=e_none;
      
    A.rows=4; /* vertical */
    A.cols=6; /* horizontal */
    lb_al_create_matrix_r(&A);

    for (i=0;i<A.rows;i++)
      for (j=0;j<A.cols;j++)
	A.array[i][j]=10*i+1000.0*j;
    lb_al_print_matrix_r(&A,"A",FLOAT_FORMAT_MATRIX);

    lb_al_inter_matrix(&A, 2.001, 4.999, &Q);
    //lb_al_inter_matrix(&A, 3.0, 5.0, &Q);
    if(error!=e_none)
      printf("An error happened\r\n");
    printf("Q= %f\r\n",Q);

    lb_al_release_matrix_r(&A);
    lb_gr_delay(10000);
    exit(1);
#endif

    //#define DEMO_DETERMINANT
#ifdef DEMO_DETERMINANT
    MATRIX_R_T A;
    FLOAT_T det;
    A.rows=5;
    A.cols=5;
    lb_al_create_matrix_r(&A);

    A.array[0][0]=4;   A.array[0][1]=-2;  A.array[0][2]=4;   A.array[0][3]=2;   A.array[0][4]=7;
    A.array[1][0]=8;   A.array[1][1]=-4;   A.array[1][2]=8;  A.array[1][3]=4;   A.array[1][4]=14;
    A.array[2][0]=-2;  A.array[2][1]=3;   A.array[2][2]=0.5; A.array[2][3]=1;   A.array[2][4]=0;
    A.array[3][0]=0.1; A.array[3][1]=2.2; A.array[3][2]=5;   A.array[3][3]=10;  A.array[3][4]=-5;
    A.array[4][0]=-2;  A.array[4][1]=3;   A.array[4][2]=0.5; A.array[4][3]=1;   A.array[4][4]=8;

    printf("**************************\r\n");
    lb_al_print_matrix_r(&A,"A",FLOAT_FORMAT_MATRIX);
    printf("**************************\r\n");
    det=lb_al_determinant_matrix_r(&A);
    printf("Determinant (recursive)= %f\r\n",det);
    det=lb_al_determinant_matrix_r_gauss(&A);
    printf("Determinant (gauss)= %f\r\n",det);
    lb_al_release_matrix_r(&A);
    exit(1);
#endif

    //#define DEMO_MATRIX_33
#ifdef  DEMO_MATRIX_33
    FLOAT_T M[3][3],  G[3][3], x;
  
    M[0][0]=1;  M[0][1]= -2;  M[0][2]=3;
    M[1][0]=10; M[1][1]=0.5;  M[1][2]=7;
    M[2][0]=40; M[2][1]=  5;  M[2][2]=-3;

    lb_al_multiply_matrix33_r(M, M, G);

    printf("G= [ %f, %f, %f]\r\n",G[0][0],G[0][1],G[0][2]);
    printf("G= [ %f, %f, %f]\r\n",G[1][0],G[1][1],G[1][2]);
    printf("G= [ %f, %f, %f]\r\n",G[2][0],G[2][1],G[2][2]);

    x=lb_al_determinant_matrix33_r(M);
    printf("Det3x3 = %f\r\n",x);

    exit(1);
#endif

    //#define DEMO_MATRIX_22
#ifdef  DEMO_MATRIX_22
    FLOAT_T M1[2][2], x;
  
    M1[0][0] = 3.14;  M1[0][1] = 4;
    M1[1][0] = -6;    M1[1][1] = 0.2;
    x=lb_al_determinant_matrix22_r(M1);
    printf("Det2x2 = %f\r\n",x);
  
    exit(1);
#endif

    //#define DEMO_VIRTUAL_CONSOLE
#ifdef DEMO_VIRTUAL_CONSOLE
    PICTURE_T Pic_console;
    FONT_T font_console;
    CONSOLE_T Con;  char c;

    lb_fb_open("/dev/fb0", "/dev/tty1", 1, 1, RENDEROPTIONS_LINE | 0*RENDEROPTIONS_GRAPHICS_ONLY);

    Pic_console.w=ty_width;
    Pic_console.h=ty_height;
    lb_gr_create_picture(&Pic_console,lb_gr_12RGB(COLOR_BLUE | COLOR_SOLID));

    lb_ft_load_GLCDfont("fonts/Font_hp48G_large.lcd", &font_console);
    font_console.scale_x=4;
    font_console.scale_y=4;
    font_console.gap_x=2;
    font_console.gap_y=2;
    font_console.max_x=10;
    font_console.angle=0;
    font_console.flag_fg=TRUE;
    font_console.flag_bg=TRUE;
    font_console.color_fg=lb_gr_12RGB(COLOR_WHITE | COLOR_SOLID);
    font_console.color_bg=lb_gr_12RGB(COLOR_BLUE | COLOR_SOLID);
    lb_ft_resize_console(&Pic_console, &font_console, &Con);

    Con.color_fg=lb_gr_12RGB(COLOR_WHITE | COLOR_SOLID);
    Con.color_bg=lb_gr_12RGB(COLOR_BLUE | COLOR_SOLID);
    lb_ft_create_console(&Con);
    lb_ft_set_active_console(&Con);
    
    while (c!=ASCII_ESC)
      {
	while (!kbhit()) ;
	c=pc_getch();
	//lb_ft_printf(&C,"%02x",c);
	lb_ft_printc(&Con,c);
	lb_ft_draw_console(&Pic_console, &font_console, &Con, COPYMODE_COPY);
	lb_gr_render_picture(&Pic_console, 0, 0, 1, 1,  1*RENDEROPTIONS_LINE);
      }
    lb_ft_release_console(&Con);
    lb_gr_release_picture(&Pic_console);
  
    lb_fb_exit(1);
#endif  	


    //#define DEMO_FRAMEBUFFER_PRIMITIVES
#ifdef DEMO_FRAMEBUFFER_PRIMITIVES
    int x, y;

    lb_fb_open("/dev/fb0", "/dev/tty1", 4, 4, RENDEROPTIONS_LINE );
    lb_fb_parse_finfo();
    lb_fb_parse_vinfo();
    printf("ty_width=%d, ty_height=%d\r\n",ty_width, ty_height);
    for (y=0;y<ty_height;y++)
      lb_fb_line_h(ty_fb_main, y, 100, 200, 0, 0, y % 255, 255);

    lb_gr_delay(2000);

    for (x=0;x<ty_width;x++)
      lb_fb_line_v(ty_fb_main, x, 100, 200, 0, x % 255, 0, 255);

    lb_gr_delay(2000);
  
    lb_fb_rectangle(ty_fb_main, 150, 150, 450, 450, 255,0,0, 255);
    lb_gr_delay(2000);

    lb_fb_exit(1);
#endif


    /*******************************************************************************************************************/
    /* Graphic Primitives */
    /******************************************************************************************************************/

    //#define DEMO_PIXEL
#ifdef DEMO_PIXEL
    SDL_Event event;
    S_INT_16_T x, y, z, width, height;
    clock_t begin, end;
    FLOAT_T time_total=0;
    int frames_count=0;
    PIXEL_T color;
  
    //lb_gr_SDL_init(SDL_INIT_VIDEO, 1920, 1080, 0, 22, 33, 55);
    lb_gr_SDL_init(SDL_INIT_VIDEO, 800, 600, 0, 22, 33, 55);
    //ty_videomode= VIDEOMODE_FAST;

    printf("Speed comparison"); 

    long n;
    int neg_offset=-RAND_MAX/2;
    int x1, y1, x2, y2;
    time_total=0;

    int flag_test=0;
  
  

    //lb_gr_buffer_line_v(200, 10, ty_height-10, 2550, 1, 0);
    for(z=0;z<255*8;z++)
      {
	lb_gr_buffer_clear(0, 5, z% 255);
	lb_gr_refresh();
      }
    lb_gr_delay(4000);
 
  
    if (1)
      {
	for(z=0;z<2055;z++)
	  {
	    lb_gr_buffer_rectangle(rand() % ty_width, rand() % ty_height,
				   rand() % ty_width, rand() % ty_height,
				   rand() % 255, rand() % 255, rand() % 255);
	    //lb_gr_buffer_rectangle(200,200, 410, 720, 0,0, 255);
	    lb_gr_refresh();
	  }
	lb_gr_delay(4000);
	exit(1);
      }
  
    if (0) for(z=0;z<255;z++) 
	     {
	       begin=clock();
	       for(y=0;y<ty_height;y++)
		 for(x=0;x<ty_width;x++)
		   {
		     color.r= x % MAX_R;
		     color.g= y % MAX_G;
		     color.b= z % MAX_B;
		     //printf("color.r= %d, color.g =%d, color.b = %d\r\n",color.r,color.g, color.b); 
		     lb_gr_draw_pixel(NULL, x, y, color, COPYMODE_COPY);
		     //lb_gr_draw_pixel_fast(x, y,  255, 155, 55);
		   }
	       lb_gr_refresh();
	       end=clock();
	       time_total+=(FLOAT_T)end-(FLOAT_T)begin;
	       frames_count++;
	       printf("FPS = %f\n",(double)CLOCKS_PER_SEC*frames_count/time_total);
	       int i=0;
	     }
    SDL_Quit();
    return EXIT_SUCCESS;
  
    while (1)
      {
	if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
	  break;
      }
#endif


    //#define DEMO_LINE1
#ifdef DEMO_LINE1
    SDL_Event event;
    S_INT_16_T x, y, z, width, height;
    clock_t begin, end;
    PIXEL_T color;
  
    //lb_gr_SDL_init(SDL_INIT_VIDEO, 1920, 1080, 1, 1, 0);
    lb_gr_SDL_init(SDL_INIT_VIDEO, 800, 600, 1, 1, 0);
    ty_videomode= VIDEOMODE_FAST;
  
  
    printf("ty_scale_x=%d, ty_scale_y=%d\r\n",ty_scale_x,ty_scale_y);
 
  
    for(z=0;z<1000;z++) 
      {
	begin=clock();
	{
	  lb_gr_draw_line1(NULL, ty_width/2, ty_height/2, rand() % ty_width, rand() % ty_height, lb_gr_12RGB(COLOR_BLUE | COLOR_SOLID),COPYMODE_COPY);
	}
	lb_gr_refresh();
	end=clock();
	printf("FPS = %f\n",(double)CLOCKS_PER_SEC/(double)(end - begin)  );
      }
    SDL_Quit();
    return EXIT_SUCCESS;
  
    while (1)
      {
	if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
	  break;
      }
#endif

 
  

    //#define DEMO_LINE2
#ifdef DEMO_LINE2
    int i;
    lb_fb_open("/dev/fb0", "/dev/tty1", 8, 8,  0*RENDEROPTIONS_LINE);
    lb_fb_clear(ty_fb_main, 50,50,50,255);
    //printf("ty_width=%d, ty_height=%d\r\n",ty_width, ty_height);
    for (i=0;i<30000; i++)
      lb_gr_draw_line2(NULL, rand() % ty_width, rand() % ty_height, rand() % ty_width, rand() % ty_height,
		       lb_gr_12RGB(rand() % 0x0FFF), COPYMODE_COPY);
    lb_gr_delay(10000);
    lb_fb_exit(1);
#endif

    //#define DEMO_LINE3
#ifdef DEMO_LINE3
    int i;
    lb_fb_open("/dev/fb0", "/dev/tty1", 2, 2, 0*RENDEROPTIONS_LINE | RENDEROPTIONS_GRAPHICS_ONLY);
    lb_fb_clear(ty_fb_main, 0,0, 55,0);

    for (i=0;i<1000; i++)
      lb_gr_draw_line3(NULL, rand() % ty_width, rand() % ty_height, rand() % ty_width, rand() % ty_height,
		       lb_gr_12RGB(rand() % 0x0FFF), COPYMODE_COPY);
    lb_gr_delay(2000);
    lb_fb_exit(1);
#endif

    //#define DEMO_LINE1_FLOAT
#ifdef DEMO_LINE1_FLOAT
    S_INT_16_T i, j,k;

    lb_fb_open("/dev/fb0", "/dev/tty1", 2, 2,  0*RENDEROPTIONS_LINE | RENDEROPTIONS_GRAPHICS_ONLY);
    lb_gr_clear_picture(NULL, lb_gr_12RGB(0x333 | COLOR_SOLID));
    for(i=0;i<1000;i++)
      lb_gr_draw_line1(NULL, ty_width/2, ty_height/2, rand() % ty_width , rand() % ty_height,
		       lb_gr_12RGB(rand() % 0xFFF |0xF000),COPYMODE_COPY);
    lb_gr_delay(10000);
    lb_fb_exit(1);
#endif





    //#define DEMO_LINE_ANTIALIASING
#ifdef DEMO_LINE_ANTIALIASING
    SDL_Event event;
    long k;
    clock_t begin, end;
    PIXEL_T a, b;
    int x, y, z, SIZE_X=60, SIZE_Y=60;

    //  begin=clock();
    //for (k=0; k<100000000; k++)
    // test_f1(0,0,0);
    //end=clock();
    //printf("Ellapsed time = %f\r\n",(FLOAT_T)(end-begin)/CLOCKS_PER_SEC);

    //begin=clock();
    //for (k=0; k<100000000; k++)
    // test_f2(0,0,0);
    //end=clock();
    //printf("Ellapsed time = %f\r\n",(FLOAT_T)(end-begin)/CLOCKS_PER_SEC);

    lb_gr_SDL_init("Bienvenidos", SDL_INIT_VIDEO, 1200, 800, 100, 0, 5);

    for (y=0; y<ty_screen.h; y++)
      for (x=0; x<ty_screen.w; x++)
	lb_gr_fb_setpixel_ARGB_copymode(&ty_screen, x, y, 255*x/ty_screen.w, 255*x/ty_screen.w, 255*x/ty_screen.w, 255, COPYMODE_COPY);
  
    //lb_gr_fb_setpixel_ARGB_copymode(&ty_screen, x, y, 100, 100, 100, 0, COPYMODE_COPY);

    //lb_gr_fb_rectangle_copymode(&ty_screen, 100, 100, 200, 200, 255, 255, 255, 255, COPYMODE_BLEND);

    lb_gr_refresh();

    if (1) for (y=0; y<ty_screen.h/SIZE_Y; y++)
	     {
	       for (x=0; x<ty_screen.w/SIZE_X; x++)
		 lb_gr_draw_pixel(NULL, x,y, lb_gr_12RGB(0x500f),
				  COPYMODE_BGCOLOR(0x0FFFF) | COPYMODE_PIXELMODE_1 | COPYMODE_COPY | COPYMODE_SCALE_X(SIZE_X) |  COPYMODE_SCALE_Y(SIZE_Y));
	       lb_gr_refresh();
	       lb_gr_delay(1000);
	       while (SDL_PollEvent(&event))
		 {
		   if (event.type == SDL_QUIT)
		     {
		       SDL_Quit();
		       return EXIT_SUCCESS;
		     }
		 }

	     }
    lb_gr_delay(5000);
#endif


    //#define DEMO_LINE_ANTIALIASING2
#ifdef DEMO_LINE_ANTIALIASING2
    SDL_Event event;
    FLOAT_T angle;
    PICTURE_T Pic;
    U_INT_8_T pix_x=32, pix_y=32;
  
    lb_gr_SDL_init("Hola", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0,0,0);
    Pic.w= ty_screen.w/pix_x;
    Pic.h= ty_screen.h/pix_y;

    lb_gr_create_picture(&Pic,lb_gr_12RGB(0x0000) );

    angle=0.0;
    while (angle<2*M_PI)
      {
	printf("angle=%f\r\n",angle*180.0/M_PI);
	lb_gr_draw_rectangle(&Pic,0,0,Pic.w,Pic.h,lb_gr_12RGB(0xF060),COPYMODE_COPY);
	lb_gr_draw_line_antialiasing(&Pic, 0.5*Pic.w, 0.5*Pic.h,
				     0.5*Pic.w  + 0.3*Pic.h*cos(angle),
				     0.5*Pic.h  - 0.3*Pic.h*sin(angle), 
				     5, lb_gr_12RGB(0xFfFFF), COPYMODE_BLEND );
	lb_gr_render_picture(&Pic, 0, 0, COPYMODE_COPY |  COPYMODE_BGCOLOR(0x0FFFF) | COPYMODE_PIXELMODE_1 | COPYMODE_SCALE_X(pix_x) |  COPYMODE_SCALE_Y(pix_y));
	lb_gr_refresh();

	angle+=M_PI/180;
	lb_gr_delay(100);

	while (SDL_PollEvent(&event))
	  {
	    if (event.type == SDL_QUIT)
	      {
		SDL_Quit();
		return EXIT_SUCCESS;
	      }
	  }
      }
#endif

    //#define DEMO_LINE_ANTIALIASING3
#ifdef DEMO_LINE_ANTIALIASING3
    SDL_Event event;
    S_INT_32_T i;
    S_INT_16_T x1, y1, x2, y2;
    PIXEL_T color;

    lb_gr_SDL_init("Hola", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0,0,0);
    lb_gr_clear_picture(NULL, lb_gr_12RGB(0x333 | COLOR_SOLID));
  
    for (i=0;i<1000000;i++)
      {
	x1= rand() % ty_screen.w;
	y1= rand() % ty_screen.h;
	x2= rand() % ty_screen.w;
	y2= rand() % ty_screen.h;

	color.r= rand() % MAX_R;
	color.g= rand() % MAX_G;
	color.b= rand() % MAX_B;
	lb_gr_draw_line_antialiasing2(NULL, x1, y1, x2, y2, color, COPYMODE_BLEND);
	//lb_gr_draw_line_antialiasing3(NULL, x1, y1, x2, y2, color, COPYMODE_BLEND);
	if (! (i % 100) )
	  lb_gr_refresh();
	//lb_gr_delay(1000);

	while (SDL_PollEvent(&event))
	  {
	    if (event.type == SDL_QUIT)
	      {
		SDL_Quit();
		return EXIT_SUCCESS;
	      }
	  }
      }
#endif

    //#define DEMO_LINE_GENERAL
#ifdef DEMO_LINE_GENERAL
    SDL_Event event;
    lb_gr_SDL_init("Hola", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0,0,0);
      
    lb_gr_draw_line(NULL, 0, 0, ty_screen.w, ty_screen.h, 50,
		    lb_gr_12RGB(COLOR_BLUE | COLOR_SOLID), COPYMODE_COPY, LINEMODE_FILTERED);

    lb_gr_draw_line(NULL, ty_screen.w, 0,  0, ty_screen.h, 30,
		    lb_gr_12RGB(COLOR_RED | COLOR_SOLID), COPYMODE_COPY, LINEMODE_FILTERED);
	
    lb_gr_draw_line(NULL, ty_screen.w/2,0, ty_screen.h/2, ty_screen.h, 20,
		    lb_gr_12RGB(COLOR_GREEN | COLOR_SOLID), COPYMODE_XOR, LINEMODE_FILTERED);

    lb_gr_refresh();

    while (1)
      while (SDL_PollEvent(&event))
	{
	  if (event.type == SDL_QUIT)
	    {
	      SDL_Quit();
	      return EXIT_SUCCESS;
	    }
	}
#endif

    //#define DEMO_CIRCLE
#ifdef DEMO_CIRCLE
    SDL_Event event;
    lb_gr_SDL_init("Hola", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0,0,0);
    lb_gr_clear_picture(NULL, lb_gr_12RGB(0x333 | COLOR_SOLID));
    lb_gr_draw_circle(NULL, ty_screen.w/2, ty_screen.h/2, ty_screen.h/2-1, lb_gr_12RGB(0x100F), COPYMODE_COPY);
    lb_gr_refresh();

    while (1)
      while (SDL_PollEvent(&event))
	{
	  if (event.type == SDL_QUIT)
	    {
	      SDL_Quit();
	      return EXIT_SUCCESS;
	    }
	}
#endif

    //#define DEMO_CIRCLE_ANTIALIASING_SIMPLE
#ifdef DEMO_CIRCLE_ANTIALIASING_SIMPLE
    SDL_Event event;
    lb_gr_SDL_init("Hola", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0,0,0);

    lb_gr_clear_picture(NULL, lb_gr_12RGB(0x333 | COLOR_SOLID));
    lb_gr_draw_circle_antialiasing_simple(NULL, ty_screen.w/2, ty_screen.h/2, 0.35*ty_screen.h,20,
					  lb_gr_12RGB(0x1F00), COPYMODE_BLEND);
    lb_gr_refresh();

    while (1)
      while (SDL_PollEvent(&event))
	{
	  if (event.type == SDL_QUIT)
	    {
	      SDL_Quit();
	      return EXIT_SUCCESS;
	    }
	}
#endif

    //#define DEMO_CIRCLE_ANTIALIASING23
#ifdef DEMO_CIRCLE_ANTIALIASING23
    SDL_Event event;
    int pix_x=20, pix_y=20;
    lb_gr_SDL_init("Hola", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0, 0, 0);
    //      lb_gr_fb_rectangle_copymode(&ty_screen, 0, 0, ty_screen.w, ty_screen.h, 0xff, 0xff, 0xff, 0xff, 0);
    lb_gr_fb_rectangle_copymode(&ty_screen, 0, 0, ty_screen.w, ty_screen.h, 0, 0, 0, 0xff, 0);

      
    //lb_gr_draw_circle_antialiasing2(NULL, ty_screen.w/(2*pix_x), ty_screen.h/(2*pix_y), ty_screen.h/(3*pix_y), lb_gr_12RGB(0xF00f),
    //				      COPYMODE_BLEND | COPYMODE_BGCOLOR(0xFFFF) | COPYMODE_PIXELMODE_1 | COPYMODE_SCALE_X(pix_x) |  COPYMODE_SCALE_Y(pix_y));
    lb_gr_draw_circle_antialiasing3(NULL, ty_screen.w/(2*pix_x), ty_screen.h/(2*pix_y), ty_screen.h/(3*pix_y), lb_gr_12RGB(0xFFa0),
				    COPYMODE_BLEND | COPYMODE_BGCOLOR(0xFFFF) | COPYMODE_PIXELMODE_1 | COPYMODE_SCALE_X(pix_x) |  COPYMODE_SCALE_Y(pix_y));
    lb_gr_refresh();
    while (1)
      while (SDL_PollEvent(&event))
	{
	  if (event.type == SDL_QUIT)
	    {
	      SDL_Quit();
	      return EXIT_SUCCESS;
	    }
	}
#endif

    //#define DEMO_CIRCLE_ANTIALIASING 
#ifdef DEMO_CIRCLE_ANTIALIASING
      SDL_Event event;
      int pix_x=10, pix_y=10;
      lb_gr_SDL_init("Hola", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0, 0, 0);
      //      lb_gr_fb_rectangle_copymode(&ty_screen, 0, 0, ty_screen.w, ty_screen.h, 0xff, 0xff, 0xff, 0xff, 0);
      lb_gr_fb_rectangle_copymode(&ty_screen, 0, 0, ty_screen.w, ty_screen.h, 0, 0, 0, 0xff, 0);

      lb_gr_draw_circle_antialiasing(NULL, ty_screen.w/(2*pix_x),   ty_screen.h/(2*pix_y), 0.25*ty_screen.w/pix_x, 10, lb_gr_12RGB(0xfF00),
      				     COPYMODE_BLEND |  COPYMODE_BGCOLOR(0xFFFF) | COPYMODE_PIXELMODE_1 | COPYMODE_SCALE_X(pix_x) |  COPYMODE_SCALE_Y(pix_y));
      lb_gr_draw_circle_antialiasing(NULL, ty_screen.w/(3*pix_x),   ty_screen.h/(3*pix_y), 0.15*ty_screen.h/pix_y, 10, lb_gr_12RGB(0xf0f0), 
   				     COPYMODE_BLEND |  COPYMODE_BGCOLOR(0xFFFF) | COPYMODE_PIXELMODE_1 | COPYMODE_SCALE_X(pix_x) |  COPYMODE_SCALE_Y(pix_y));

      lb_gr_draw_circle_antialiasing(NULL, ty_screen.w*2/(3*pix_x), ty_screen.h/(3*pix_y), 0.15*ty_screen.h/pix_y, 10, lb_gr_12RGB(0xf00F),
				     COPYMODE_BLEND |  COPYMODE_BGCOLOR(0xFFFF) | COPYMODE_PIXELMODE_1 | COPYMODE_SCALE_X(pix_x) |  COPYMODE_SCALE_Y(pix_y));
   

      lb_gr_refresh();
      while (1)
	while (SDL_PollEvent(&event))
	  {
	    if (event.type == SDL_QUIT)
	      {
		SDL_Quit();
		return EXIT_SUCCESS;
	      }
	  }
#endif

      //      #define DEMO_CIRCLE_FILLED_SLOW
#ifdef DEMO_CIRCLE_FILLED_SLOW
      SDL_Event event;
      int pix_x=30, pix_y=30;
      lb_gr_SDL_init("Hola", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0, 0, 0);
      lb_gr_clear_picture(NULL, lb_gr_12RGB(COLOR_SOLID | COLOR_WHITE));

      lb_gr_draw_circle_filled_slow(NULL, ty_screen.w/(2*pix_x), ty_screen.h/(2*pix_y), ty_screen.h/(3*pix_y), lb_gr_12RGB(0xF00F),
				    COPYMODE_COPY |  COPYMODE_BGCOLOR(0xFFFF) | COPYMODE_PIXELMODE_1 | COPYMODE_SCALE_X(pix_x) |  COPYMODE_SCALE_Y(pix_y));
      lb_gr_refresh();
      while (1)
	while (SDL_PollEvent(&event))
	  {
	    if (event.type == SDL_QUIT)
	      {
		SDL_Quit();
		return EXIT_SUCCESS;
	      }
	  }
#endif

      //#define DEMO_CIRCLE_FILLED_FAST
#ifdef DEMO_CIRCLE_FILLED_FAST
	SDL_Event event;
	int pix_x=10, pix_y=10;
	lb_gr_SDL_init("Hola", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0, 0, 0);
	lb_gr_clear_picture(NULL, lb_gr_12RGB(COLOR_SOLID | COLOR_WHITE));
      
	lb_gr_draw_circle_filled_antialiasing_f(NULL, ty_screen.w/(3*pix_x), ty_screen.h/(3*pix_y), ty_screen.h/(4*pix_y),
						lb_gr_12RGB(COLOR_RED | COLOR_SOLID), COPYMODE_BLEND | COPYMODE_SCALE_X(pix_x) |  COPYMODE_SCALE_Y(pix_y));
	lb_gr_draw_circle_filled_antialiasing_f(NULL, 2*ty_screen.w/(3*pix_x), ty_screen.h/(3*pix_y), ty_screen.h/(4*pix_y),
						lb_gr_12RGB(COLOR_LIME  | COLOR_SOLID), COPYMODE_BLEND | COPYMODE_SCALE_X(pix_x) |  COPYMODE_SCALE_Y(pix_y));
	lb_gr_draw_circle_filled_antialiasing_f(NULL, ty_screen.w/(2*pix_x), 2*ty_screen.h/(3*pix_y), ty_screen.h/(4*pix_y),
						lb_gr_12RGB(COLOR_BLUE  | COLOR_SOLID), COPYMODE_BLEND | COPYMODE_SCALE_X(pix_x) |  COPYMODE_SCALE_Y(pix_y));
	lb_gr_refresh();
	while (1)
	  while (SDL_PollEvent(&event))
	    {
	      if (event.type == SDL_QUIT)
		{
		  SDL_Quit();
		  return EXIT_SUCCESS;
		}
	    }
#endif

	//#define DEMO_CIRCLE_FILLED_ANTIALIASING
#ifdef DEMO_CIRCLE_FILLED_ANTIALIASING
	  SDL_Event event;
	  int pix_x=10, pix_y=10;
	  lb_gr_SDL_init("Hola", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0, 0, 0);
	  lb_gr_clear_picture(NULL, lb_gr_12RGB(COLOR_SOLID | COLOR_WHITE));


	  lb_gr_draw_circle_filled_antialiasing(NULL, ty_screen.w/(pix_x*3), ty_screen.h/(pix_y*3), ty_screen.h/(pix_x*4),
						lb_gr_12RGB(0xF00F),COPYMODE_BLEND | COPYMODE_SCALE_X(pix_x) |  COPYMODE_SCALE_Y(pix_y));
	  lb_gr_draw_circle_filled_antialiasing(NULL, 2*ty_screen.w/(pix_x*3), ty_screen.h/(pix_y*3), ty_screen.h/(pix_y*4),
						lb_gr_12RGB(0xf0F0), COPYMODE_BLEND | COPYMODE_SCALE_X(pix_x) |  COPYMODE_SCALE_Y(pix_y));
	  lb_gr_draw_circle_filled_antialiasing(NULL, ty_screen.w/(pix_x*2), 2*ty_screen.h/(pix_y*3), ty_screen.h/(pix_y*4),
						lb_gr_12RGB(0xfF00), COPYMODE_BLEND | COPYMODE_SCALE_X(pix_x) |  COPYMODE_SCALE_Y(pix_y));
	  lb_gr_draw_circle_filled_antialiasing(NULL, ty_screen.w/(pix_x*2), 2*ty_screen.h/(pix_y*3), ty_screen.h/(pix_y*4),
						lb_gr_12RGB(0xfF00), COPYMODE_BLEND | COPYMODE_SCALE_X(pix_x) |  COPYMODE_SCALE_Y(pix_y));
	  lb_gr_refresh();

	  while (1)
	    while (SDL_PollEvent(&event))
	      {
		if (event.type == SDL_QUIT)
		  {
		      
		    SDL_Quit();
		    return EXIT_SUCCESS;
		  }
	      }
#endif

	  //#define DEMO_CIRCLE_ARC
#ifdef DEMO_CIRCLE_ARC
	    SDL_Event event;
	    int pix_x=10, pix_y=10;
	    lb_gr_SDL_init("Hola", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0, 0, 0);
	    lb_gr_clear_picture(NULL, lb_gr_12RGB(COLOR_SOLID | COLOR_BLACK));

	    lb_gr_draw_circle_arc(NULL, ty_screen.w/(pix_x*2), ty_screen.h/(pix_y*2), ty_screen.h/(pix_y*2)-1,
				  0, M_PI/3,lb_gr_12RGB(0xFfff), COPYMODE_COPY | COPYMODE_SCALE_X(pix_x) |  COPYMODE_SCALE_Y(pix_y));
	    lb_gr_refresh();

	    while (1)
	      while (SDL_PollEvent(&event))
		{
		  if (event.type == SDL_QUIT)
		    {
		      
		      SDL_Quit();
		      return EXIT_SUCCESS;
		    }
		}
#endif

	    //#define DEMO_ELLIPSE
#ifdef DEMO_ELLIPSE
	    FLOAT_T angle;

	    SDL_Event event;
	    int pix_x=10, pix_y=10;
	    lb_gr_SDL_init("Hola", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0, 0, 0);
	    lb_gr_clear_picture(NULL, lb_gr_12RGB(COLOR_SOLID | COLOR_BLACK));

	    /* Example 1: */
	    lb_gr_draw_ellipse(NULL, ty_screen.w/(2*pix_x), ty_screen.h/(2*pix_y), ty_screen.w*0.25/pix_x, ty_screen.h*0.22/pix_y,lb_gr_12RGB(COLOR_WHITE),
	    COPYMODE_COPY | COPYMODE_SCALE_X(pix_x) |  COPYMODE_SCALE_Y(pix_y));
	
	    /* Example 2: */
	    lb_gr_draw_ellipse_antialiasing2(NULL, ty_screen.w/(2*pix_x), ty_screen.h/(2*pix_y), ty_screen.w*0.25/pix_x,ty_screen.h*0.22/pix_y, lb_gr_12RGB(COLOR_WHITE),
	    COPYMODE_BLEND | COPYMODE_SCALE_X(pix_x) |  COPYMODE_SCALE_Y(pix_y));

	    /* Example 3: */
	    lb_gr_draw_ellipse_antialiasing3(NULL, ty_screen.w/(2*pix_x), ty_screen.h/(2*pix_y), ty_screen.w*0.25/pix_x,ty_screen.w*0.22/pix_y,lb_gr_12RGB(COLOR_WHITE),
	    				   COPYMODE_BLEND | COPYMODE_SCALE_X(pix_x) |  COPYMODE_SCALE_Y(pix_y)); 
	
	    angle=0.0;
	    lb_gr_refresh();
	    if (0) for (angle=0;angle<=M_PI/2;angle+=1*M_PI/180)
		     {
		       lb_gr_clear_picture(NULL,lb_gr_12RGB(0x0000));
		       // lb_gr_draw_rectangle(NULL,0,0,ty_width,ty_height,lb_gr_12RGB(0xF000),COPYMODE_COPY);
		       lb_gr_draw_ellipse_rotated_antialiasing(NULL, ty_screen.w/(2*pix_x), ty_screen.h/(2*pix_y), ty_screen.w*0.45/pix_x,ty_screen.w*0.10/pix_x, angle, 8, 100,
							       lb_gr_12RGB(COLOR_BLUE), COPYMODE_BLEND | COPYMODE_SCALE_X(pix_x) |  COPYMODE_SCALE_Y(pix_y), LINEMODE_FILTERED);
		       lb_gr_delay(100);
		       lb_gr_refresh();
		     }

	    while (1)
	      while (SDL_PollEvent(&event))
		{
		  if (event.type == SDL_QUIT)
		    {
		      
		      SDL_Quit();
		      return EXIT_SUCCESS;
		    }
		}
#endif

  
	    //#define DEMO_TRIANGLE
#ifdef DEMO_TRIANGLE
    	    SDL_Event event;
	    int pix_x=1, pix_y=1;
	    S_INT_16_T i;
	    POINT_2D_INT_T A, B, C, D, X;
	    clock_t begin, end;
	    lb_gr_SDL_init("Hola", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0, 0, 0);
	    lb_gr_clear_picture(NULL, lb_gr_12RGB(COLOR_SOLID | COLOR_BLACK));
	    
	    begin=clock();
	    for (i=0;i<10000;i++)
	      {
		A.x=rand() % ty_screen.w/pix_x;
		A.y=rand() % ty_screen.h/pix_y;
		B.x=rand() % ty_screen.w/pix_x;
		B.y=rand() % ty_screen.h/pix_y;
		C.x=rand() % ty_screen.w/pix_x;
		C.y=rand() % ty_screen.h/pix_y;
		//lb_gr_draw_triangle_fill_i(NULL, A, B, C, lb_gr_12RGB(COLOR_SOLID | rand()%0x0FFF), COPYMODE_BLEND |  COPYMODE_SCALE_X(pix_x) |  COPYMODE_SCALE_Y(pix_y));
		lb_gr_draw_triangle_fill_i(NULL, A, B, C, lb_gr_12RGB(COLOR_SOLID | rand()%0x0FFF), 0);
		if (!(i % 100))
		  lb_gr_refresh();
	      }
	    end=clock();
	    printf("time elapsed = %f\n",(double)(end - begin) / CLOCKS_PER_SEC);
  
	    while (1)
	      while (SDL_PollEvent(&event))
		{
		  if (event.type == SDL_QUIT)
		    {
		      
		      SDL_Quit();
		      return EXIT_SUCCESS;
		    }
		}
#endif

  
	    //#define DEMO_POLYGON
#ifdef DEMO_POLYGON
    	    SDL_Event event;
	    int pix_x=1, pix_y=1;
	    LINE_2D_INT_T myPol;
	    int i;
	    float phase;

	    lb_gr_SDL_init("Hola", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0, 0, 0);
	    lb_gr_clear_picture(NULL, lb_gr_12RGB(COLOR_SOLID | COLOR_WHITE));
	
	    /* Polygon creation and testing */
	    myPol.items=10;
	    lb_gr_create_line2d_i(&myPol);
	    for (phase=0; phase<6.2832; phase+=0.01) 
	      {
		lb_gr_clear_picture(NULL,lb_gr_12RGB(0xfEEE));

		for (i=0;i<myPol.items;i++)
		  {
		    myPol.array[i].x=round(0.5*ty_screen.w/pix_x + 0.4*ty_screen.h*cos(phase+6.2832*i/(myPol.items-1))/pix_y);
		    myPol.array[i].y=round(0.5*ty_screen.h/pix_x - 0.4*ty_screen.h*sin(phase+6.2832*i/(myPol.items-1))/pix_y);
		   }
		//lb_gr_draw_polygon_i(NULL,&myPol,7,lb_gr_12RGB(COLOR_RED|COLOR_SOLID), COPYMODE_BLEND,LINEMODE_DOTS_FILTERED);
		lb_gr_draw_polygon_i(NULL,&myPol,3,lb_gr_12RGB(COLOR_RED|COLOR_SOLID), COPYMODE_BLEND | COPYMODE_SCALE_X(pix_x) | COPYMODE_SCALE_Y(pix_y), LINEMODE_FILTERED);
		lb_gr_refresh();
		 
		lb_gr_delay(500);
	      }
	    lb_gr_release_line2d_i(&myPol);

	    while (1)
	      while (SDL_PollEvent(&event))
		{
		  if (event.type == SDL_QUIT)
		    {
		      
		      SDL_Quit();
		      return EXIT_SUCCESS;
		    }
		}
#endif

	    //#define DEMO_POLYGON_FLOAT
#ifdef DEMO_POLYGON_FLOAT
	    SDL_Event event;
	    int pix_x=1, pix_y=1;
	    LINE_2D_FLOAT_T myPol;
	    int i;
	    float phase;

	    lb_gr_SDL_init("Hola", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0, 0, 0);
	    lb_gr_clear_picture(NULL, lb_gr_12RGB(COLOR_SOLID | COLOR_WHITE));
	    lb_gr_refresh();
 
	    /* Polygon creation and testing */ 
	    myPol.items=10;
	    lb_gr_create_line2d_f(&myPol);
	    printf("\a\r\n");
	    for (phase=0; phase<6.2832; phase+=0.01) 
	      {
		//lb_gr_clear_picture(NULL,lb_gr_12RGB(0xffff));
		for (i=0;i<myPol.items;i++)
		  {
		    myPol.array[i].x=0.5*ty_screen.w/pix_x + 0.45*ty_screen.h*cos(phase+6.2832*i/(myPol.items-1)/pix_y);
		    myPol.array[i].y=0.5*ty_screen.h/pix_y - 0.45*ty_screen.h*sin(phase+6.2832*i/(myPol.items-1)/pix_y);
		  }
		lb_gr_draw_polygon_antialiasing(NULL,&myPol,6.1,lb_gr_12RGB(COLOR_BLUE), COPYMODE_BLEND | COPYMODE_SCALE_X(pix_x) | COPYMODE_SCALE_Y(pix_y));
		printf("hi\r\n");
		lb_gr_refresh();
		//lb_gr_delay(50);
	      }
	    lb_gr_release_line2d_f(&myPol);
	    while (1)
	      while (SDL_PollEvent(&event))
		{
		  if (event.type == SDL_QUIT)
		    {
		      SDL_Quit();
		      return EXIT_SUCCESS;
		    }
		}
#endif

	    //oxo
#define DEMO_INSIDE_POLYGON_INT
#ifdef DEMO_INSIDE_POLYGON_INT
	    SDL_Event event;
	    int pix_x=12, pix_y=12;
	    S_INT_16_T i, j;
	    LINE_2D_INT_T Poly_int;
	    POINT_2D_INT_T P;

	    lb_gr_SDL_init("Hola", SDL_INIT_VIDEO, 0,0, 0, 0, 0);
	    lb_gr_clear_picture(NULL, lb_gr_12RGB(COLOR_SOLID | COLOR_WHITE));
	    
	    /* Polygon creation and testing */
	    Poly_int.items=6;
	    lb_gr_create_line2d_i(&Poly_int);
	    for (i=0;i<Poly_int.items-1;i++)
	      {
		Poly_int.array[i].x=ty_screen.w/(2*pix_x) + 0.35*(ty_screen.h/pix_y)*cos(2*M_PI*i/(Poly_int.items-1));
		Poly_int.array[i].y=ty_screen.h/(2*pix_y) - 0.35*(ty_screen.h/pix_y)*sin(2*M_PI*i/(Poly_int.items-1));
	      }
	    Poly_int.array[Poly_int.items-1] = Poly_int.array[0]; /* This ensures the polygon is "closed" */
      
	    lb_gr_draw_polygon_i(NULL,&Poly_int,4,lb_gr_12RGB(COLOR_BLUE), COPYMODE_BLEND | COPYMODE_SCALE_X(pix_x) | COPYMODE_SCALE_Y(pix_y), LINEMODE_FILTERED);
	    lb_gr_refresh();
	    lb_gr_delay(5000);

	    for(i=0;i<ty_screen.h/pix_y;i++)
	      for(j=0;j<ty_screen.w/pix_x;j++)
		{
		  P.x=j;
		  P.y=i;
		  if (lb_gr_is_in_polygon_i(&Poly_int,P))
		    lb_gr_draw_pixel(NULL, j, i, lb_gr_12RGB(COLOR_BEIGE), COPYMODE_COPY | COPYMODE_SCALE_X(pix_x) | COPYMODE_SCALE_Y(pix_y));
		  else
		    lb_gr_draw_pixel(NULL, j, i, lb_gr_12RGB(COLOR_PINK), COPYMODE_COPY | COPYMODE_SCALE_X(pix_x) | COPYMODE_SCALE_Y(pix_y));
		}
	    lb_gr_refresh();

	    while (1)
	      while (SDL_PollEvent(&event))
		{
		  if (event.type == SDL_QUIT)
		    {
		      
		      SDL_Quit();
		      return EXIT_SUCCESS;
		    }
		}
#endif

	    //#define DEMO_INSIDE_POLYGON_FLOAT
#ifdef DEMO_INSIDE_POLYGON_FLOAT
	    S_INT_16_T i, j;
	    LINE_2D_FLOAT_T Poly_f;
	    POINT_2D_FLOAT_T P;
	    VIEWPORT_2D_T win;

	    lb_fb_open("/dev/fb0", "/dev/tty1", 1, 1, 0*RENDEROPTIONS_LINE | 1*RENDEROPTIONS_GRAPHICS_ONLY);

	    win.xp_min=0;
	    win.yp_min=0;
	    win.xp_max=ty_width;
	    win.yp_max=ty_height;
	    win.xr_min=-2*1.024;
	    win.xr_max=2*1.024;
	    win.yr_min=-2*0.768;
	    win.yr_max= 2*0.768;

	    /* Polygon creation and testing */
	    Poly_f.items=6;
	    lb_gr_create_line2d_f(&Poly_f);
	    for (i=0;i<Poly_f.items-1;i++)
	      {
		Poly_f.array[i].x=cos(2*M_PI*i/(Poly_f.items-1));
		Poly_f.array[i].y=sin(2*M_PI*i/(Poly_f.items-1));
	      }
	    Poly_f.array[Poly_f.items-1] = Poly_f.array[0]; /* This ensures the polygon is "closed" */
      
	    for(i=0;i<ty_height;i++)
	      for(j=0;j<ty_width;j++)
		{
		  lb_gr_project_2d_inv(win, j, i, &P.x, &P.y);

		  if (lb_gr_is_in_polygon_f(&Poly_f,P))
		    lb_gr_draw_pixel(NULL, j, i, lb_gr_12RGB(COLOR_GREEN | 0xF000), COPYMODE_BLEND);
		  else
		    lb_gr_draw_pixel(NULL, j, i, lb_gr_12RGB(COLOR_BLUE | COLOR_SOLID), COPYMODE_BLEND);
		} 
	    lb_gr_delay(10000);
	    lb_gr_release_line2d_f(&Poly_f);
	    lb_fb_exit(1);
#endif

	    //#define DEMO_POLYGON_FILL
#ifdef DEMO_POLYGON_FILL
	    LINE_2D_INT_T myPol;
	    int k=40;

	    lb_fb_open("/dev/fb0", "/dev/tty1", 4, 4, 0*RENDEROPTIONS_LINE | 1*RENDEROPTIONS_GRAPHICS_ONLY);

	    /* Polygon creation and testing */
	    myPol.items=5;
      
	    lb_gr_create_line2d_i(&myPol);
	    myPol.array[0].x=10+k; myPol.array[0].y=10+k;
	    myPol.array[1].x=96+k; myPol.array[1].y=10+k;
	    myPol.array[2].x=10+k; myPol.array[2].y=80+k;
	    myPol.array[3].x=96+k; myPol.array[3].y=80+k;
	    myPol.array[4].x=10+k; myPol.array[4].y=10+k;
      
	    lb_gr_draw_polygon_fill_i(NULL,&myPol,lb_gr_12RGB(0xFF00),COPYMODE_BLEND);
	    lb_gr_draw_polygon_fill_i(NULL,&myPol,lb_gr_12RGB(0xF0F0),COPYMODE_BLEND);
	    lb_gr_draw_polygon_i(NULL,&myPol, 4,lb_gr_12RGB(0xF00F),COPYMODE_BLEND,LINEMODE_FILTERED);
	    lb_gr_delay(10000);
	    lb_gr_release_line2d_i(&myPol);
	    lb_fb_exit(1);
#endif

	    //#define DUFF_PORTER
#ifdef DUFF_PORTER
	    PICTURE_T pic1;
	    S_INT_16_T i,j, alpha;
	    LINE_2D_INT_T myPol;
	    int k=40;

	    lb_fb_open("/dev/fb0", "/dev/tty1", 4, 4, 0*RENDEROPTIONS_LINE | 1*RENDEROPTIONS_GRAPHICS_ONLY);

	    lb_gr_BMPfile_getsize("sdl24bit.bmp",&i,&j);

	    pic1.w=i;
	    pic1.h=j;

	    lb_gr_create_picture(&pic1,lb_gr_12RGB(0xF000));
      
	    /* Polygon creation and testing */
	    myPol.items=5;
      
	    lb_gr_create_line2d_i(&myPol);
	    myPol.array[0].x=10+k; myPol.array[0].y=10+k;
	    myPol.array[1].x=96+k; myPol.array[1].y=10+k;
	    myPol.array[2].x=10+k; myPol.array[2].y=80+k;
	    myPol.array[3].x=96+k; myPol.array[3].y=80+k;
	    myPol.array[4].x=10+k; myPol.array[4].y=10+k;

	    /* 
	       COPYMODE_DST_DIV_SRC
	       COPYMODE_PORTERDUFF_CLEAR_DST,   
	       COPYMODE_PORTERDUFF_COPY_SRC,    
	       COPYMODE_PORTERDUFF_LEAVES_DST,   
	       COPYMODE_PORTERDUFF_SRC_OVER_DST,
	       COPYMODE_PORTERDUFF_DST_OVER_SRC,
	       COPYMODE_PORTERDUFF_SRC_IN_DST,
	       COPYMODE_PORTERDUFF_DST_IN_SRC,
	       COPYMODE_PORTERDUFF_SRC_OUT_DST,
	       COPYMODE_PORTERDUFF_DST_OUT_SRC,
	       COPYMODE_PORTERDUFF_SRC_ATOP_DST,
	       COPYMODE_PORTERDUFF_DST_ATOP_SRC,
	       COPYMODE_PORTERDUFF_XOR */

	    //      for(alpha=0;alpha<=MAX_K;alpha++)
	    //lb_gr_clear_picture(&pic1, lb_gr_12RGB(0x0000));
	    alpha=MAX_K;
	    lb_gr_BMPfile_load_to_pic("sdl24bit.bmp",&pic1,alpha);
	    printf("1. alpha=%d\r\n",alpha);
	    lb_gr_draw_polygon_fill_i(&pic1,&myPol,lb_gr_12RGB(0xFFFF), COPYMODE_XOR);
	    lb_gr_render_picture(&pic1, 0, 0, 4, 4, RENDEROPTIONS_DEFAULT);
	    lb_gr_delay(5000);
	    lb_gr_draw_polygon_fill_i(&pic1,&myPol,lb_gr_12RGB(0xFFFF), COPYMODE_XOR);
	    lb_gr_render_picture(&pic1, 0, 0, 4, 4, RENDEROPTIONS_DEFAULT);
	    lb_gr_delay(5000);
     
	    lb_gr_release_line2d_i(&myPol);
	    lb_gr_release_picture(&pic1);
	    lb_fb_exit(1);
#endif
    
  
	    /*******************************************************************************************************************/
	    /* Advanced 2D Graphics */
	    /******************************************************************************************************************/

	    //#define ROTATE_ALGORITHM
#ifdef ROTATE_ALGORITHM
	    /* This helps ilustrating  an older algorithm used to rotate bitmaps */
	    PICTURE_T Pic;
	    S_INT_16_T k;
	    LINE_2D_FLOAT_T P1, P2, P3;
	    VIEWPORT_2D_T win;
	    FLOAT_T angle;

	    lb_fb_open("/dev/fb0", "/dev/tty1", 2, 2,  0*RENDEROPTIONS_LINE | 1*RENDEROPTIONS_GRAPHICS_ONLY);
 
	    angle=45.0*M_PI/180;
     
	    Pic.w=ty_width;
	    Pic.h=ty_height;
            
	    win.xp_min=0;
	    win.yp_min=0.;
	    win.xp_max=Pic.w;
	    win.yp_max=Pic.h;
	    win.xr_min=-2.0;
	    win.xr_max=2.0;
	    win.yr_min=-2.0;
	    win.yr_max=2.0;

	    lb_gr_create_picture(&Pic,lb_gr_12RGB(0x7000));

	    /* Polygon creation and testing */
	    P1.items=5;
	    lb_gr_create_line2d_f(&P1);
	    P2.items=5;
	    lb_gr_create_line2d_f(&P2);
	    P3.items=5;
	    lb_gr_create_line2d_f(&P3);
	    P1.array[0].x=0.0;  P1.array[0].y=0.0;
	    P1.array[1].x=1.0;  P1.array[1].y=0.0;
	    P1.array[2].x=1.0;  P1.array[2].y=1.0;
	    P1.array[3].x=0.0;  P1.array[3].y=1.0;
	    P1.array[4] = P1.array[0]; /* This ensures the polygon is "closed" */

	    for(angle=0*45*M_PI/180;angle<2*45*M_PI/180;angle+=M_PI/180)
	      {
		lb_gr_clear_picture(&Pic,lb_gr_12RGB(COLOR_BLACK));
		for(k=0;k<5;k++)
		  {
		    P2.array[k].x=P1.array[k].x;
		    P2.array[k].y=P1.array[k].x*sin(angle)+P1.array[k].y*cos(angle);
		  }
      
		for(k=0;k<5;k++)
		  {
		    P3.array[k].x=P2.array[k].x*cos(angle)-tan(angle)*(P2.array[k].y-P2.array[k].x*sin(angle));
		    P3.array[k].y=P2.array[k].y;
		  }

		lb_gr_plot2d_line(&Pic, win, &P1, 3, lb_gr_12RGB(COLOR_BLUE | COLOR_SOLID), COPYMODE_BLEND, LINEMODE_FILTERED);
		lb_gr_plot2d_line(&Pic, win, &P2, 3, lb_gr_12RGB(COLOR_LIME | COLOR_SOLID), COPYMODE_BLEND, LINEMODE_FILTERED);
		lb_gr_plot2d_line(&Pic, win, &P3, 3, lb_gr_12RGB(COLOR_WHITE | COLOR_SOLID), COPYMODE_BLEND, LINEMODE_FILTERED);

		lb_gr_render_picture(&Pic, 0, 0, 2, 2, RENDEROPTIONS_DEFAULT);
		lb_gr_delay(1000);
	      }
	    lb_gr_release_picture(&Pic);
	    lb_gr_release_line2d_f(&P1);
	    lb_gr_release_line2d_f(&P2);
	    lb_gr_release_line2d_f(&P3);
	    lb_fb_exit(1);
#endif

  
	    //#define DEMO_ROTATE_BITMAP
#ifdef  DEMO_ROTATE_BITMAP
	    PICTURE_T pic_src, pic_dst;
	    S_INT_16_T width, height;
	    FLOAT_T angle;
	    //pending: strange line on top
  
	    lb_fb_open("/dev/fb0", "/dev/tty1", 2, 2, 0*RENDEROPTIONS_LINE | 1*RENDEROPTIONS_GRAPHICS_ONLY);

	    /* Load initial image and present it */
	    lb_gr_BMPfile_getsize("test24bit.bmp",&width,&height);
	    pic_src.w=384;
	    pic_src.h=256;
	    printf("%d %d\r\n",width, height);
	    //lb_fb_exit(1);
	    lb_gr_create_picture(&pic_src,lb_gr_12RGB(COLOR_RED));
       
	    lb_gr_BMPfile_load_to_pic("sdl24bit.bmp",&pic_src, 0);
	    lb_gr_render_picture(&pic_src, 0, 0, 2, 2, 0*RENDEROPTIONS_LINE);
  
	    for(angle=0;angle<2*M_PI;angle+=M_PI/180)
	      {
		lb_gr_bitmap_rotate(&pic_src, &pic_dst, angle, lb_gr_12RGB(COLOR_BLACK | COLOR_SOLID));
		lb_gr_render_picture(&pic_dst, 0, 0, 2, 2, RENDEROPTIONS_DEFAULT);
		lb_gr_delay(500);
	      }
	    lb_gr_delay(20000);
	    lb_gr_release_picture(&pic_src);
	    lb_fb_exit(1);
#endif

	    //#define DEMO_ROTATE_BITMAP_SAMPLING
#ifdef  DEMO_ROTATE_BITMAP_SAMPLING
	    PICTURE_T pic_src, pic_dst;
	    S_INT_16_T width, height;
	    FLOAT_T angle;

	    lb_fb_open("/dev/fb0", "/dev/tty1", 2, 2, 0*RENDEROPTIONS_LINE | 1*RENDEROPTIONS_GRAPHICS_ONLY);

	    /* Load initial image and present it */
	    lb_gr_BMPfile_getsize("sdl24bit.bmp",&width,&height);
	    pic_src.w=width;
	    pic_src.h=height;
	    lb_gr_create_picture(&pic_src,lb_gr_12RGB(COLOR_RED));
     
	    lb_gr_BMPfile_load_to_pic("sdl24bit.bmp",&pic_src, 0);

	    for(angle=0.0;angle<2*M_PI;angle+=0.5*M_PI/180)
	      {
		lb_gr_bitmap_rotate_sampling(&pic_src, &pic_dst, angle, 3, lb_gr_12RGB(COLOR_LIME));
		lb_gr_render_picture(&pic_dst, 0, 0, 2, 2, RENDEROPTIONS_DEFAULT);
	      }
	    lb_gr_release_picture(&pic_src);
	    lb_gr_release_picture(&pic_dst);
	    lb_fb_exit(1);
#endif

	    //#define DEMO_BMP_TO_MATRIX
#ifdef DEMO_BMP_TO_MATRIX
	    MATRIX_R_T R, G, B;
	    S_INT_16_T i,j;
	    PIXEL_T color;

	    lb_fb_open("/dev/fb0", "/dev/tty1", 1, 1, 0*RENDEROPTIONS_LINE | 1*RENDEROPTIONS_GRAPHICS_ONLY);
	    lb_gr_clear_picture(NULL,lb_gr_12RGB(COLOR_BLACK));

	    lb_gr_BMPfile_getsize("sdl24bit.bmp",&i,&j);

	    R.rows=j;
	    R.cols=i;
	    lb_al_create_matrix_r(&R);

	    G.rows=j;
	    G.cols=i;
	    lb_al_create_matrix_r(&G);
  
	    B.rows=j;
	    B.cols=i;
	    lb_al_create_matrix_r(&B);
      
	    lb_gr_BMPfile_load_to_matrix("sdl24bit.bmp",&R,&G,&B);

	    for (i=0;i<R.rows;i++)
	      for (j=0;j<R.cols;j++)
		{
		  color.r=R.array[i][j]*MAX_R;
		  color.g=0*R.array[i][j]*MAX_G;
		  color.b=0*R.array[i][j]*MAX_B;
		  lb_gr_draw_pixel(NULL,j,R.rows-i,color,COPYMODE_COPY);

		  color.r=0*G.array[i][j]*MAX_R;
		  color.g=G.array[i][j]*MAX_G;
		  color.b=0*G.array[i][j]*MAX_B;
		  lb_gr_draw_pixel(NULL,j,2*G.rows-i,color,COPYMODE_COPY);

		  color.r=0*B.array[i][j]*MAX_R;
		  color.g=0*B.array[i][j]*MAX_G;
		  color.b=B.array[i][j]*MAX_B;
		  lb_gr_draw_pixel(NULL,j,3*B.rows-i,color,COPYMODE_COPY);

		  color.r=R.array[i][j]*MAX_R;
		  color.g=G.array[i][j]*MAX_G;
		  color.b=B.array[i][j]*MAX_B;
		  lb_gr_draw_pixel(NULL,j+G.cols,2*G.rows-i,color,COPYMODE_COPY);
		}
	    lb_gr_delay(10000);
          
	    lb_al_release_matrix_r(&R);
	    lb_al_release_matrix_r(&G);
	    lb_al_release_matrix_r(&B);
	    lb_fb_exit(1);
#endif

	    //#define DEMO_PROJECT_2D
#ifdef DEMO_PROJECT_2D
	    FLOAT_T xr, yr, xp, yp;
	    VIEWPORT_2D_T win;

	    lb_fb_open("/dev/fb0", "/dev/tty1", 1, 1, 0*RENDEROPTIONS_LINE | 1*RENDEROPTIONS_GRAPHICS_ONLY);

	    win.xp_min=0;
	    win.yp_min=0;
	    win.xp_max=ty_width;
	    win.yp_max=ty_height;
	    win.xr_min=-2.5;
	    win.xr_max=2.5;
	    win.yr_min=5.5; 
	    win.yr_max=-0.5;

	    for(xr=-2.0;xr<2.0;xr+=0.01)
	      {
		yr=0.5*xr*xr;
		lb_gr_project_2d(win, xr, yr, &xp, &yp);
		lb_gr_draw_pixel(NULL, xp, yp, lb_gr_12RGB(COLOR_BLUE), COPYMODE_COPY);
	      }
	    lb_gr_delay(5000);
	    lb_fb_exit(1);
#endif

	    //#define DEMO_GAMMA
#ifdef DEMO_GAMMA
	    FLOAT_T xr, yr, xr_min, yr_min, xr_max, yr_max;
	    VIEWPORT_2D_T win;

	    SDL_Event event;
	    PIXEL_T color;

	    FILE *file;
	    char c;

	    xr_min=1e10;
	    yr_min=1e10;
	    xr_max=-1e10;
	    yr_max=-1e10;
  
	    lb_gr_SDL_init(SDL_INIT_VIDEO, 1600, 900, 1, 1, 0, 255,255,255);
	    ty_videomode= VIDEOMODE_FAST;
	    lb_gr_refresh();

	    win.xp_min=0;
	    win.xp_max=ty_width;
	    win.yp_min=0;
	    win.yp_max=ty_height;
	    win.xr_min=-40;
	    win.xr_max=160;
	    win.yr_min=2330; 
	    win.yr_max=2420;

	    char words[15][200];
	    int col, char_counter,i;
  
	    /* Parsing Accelerometer Data */

	    file = fopen("GAMMA_Complete_cycle.txt","r");
	    if (file==NULL)
	      {
		printf("Error: could not open file\r\n");
		exit(EXIT_FAILURE);
	      }

	    col=0;
	    char_counter=0;
	    while ( !feof(file) )
	      {
		c= fgetc(file);

		switch (c)
		  {
		  case '\n':
		    words[col][char_counter]='\0';
		    for (i=0;i<11;i++)
		      {
			//printf("%s",words[i]);
			//printf("\t");
			/* Here we do the conversion */
			if (atoll(words[0])==0)
			  {
			    xr = atof(words[3]);
			    yr = atof(words[5]);
			    if (xr<xr_min)
			      xr_min=xr;
			    if (yr<yr_min)
			      yr_min=yr;
			    if (xr>xr_max)
			      xr_max=xr;
			    if (yr>yr_max)
			      yr_max=yr;
		  
			    printf("xr=%4.1f  yr=%4.1f  xr: [%4.1f, %4.1f], xr: [%4.1f, %4.1f]\r\n",xr,yr,xr_min,xr_max,yr_min,yr_max);
			    lb_gr_plot2d(NULL, win, xr, yr, 1, lb_gr_12RGB(COLOR_BLUE), COPYMODE_COPY, LINEMODE_DOTS_SOLID);
			    //		  lb_gr_refresh();
			  }
		      }
		    printf("\r\n");
	      
		    col=0;
		    char_counter=0;
	   
		    break;
		  case ' ':
		  case '\t':
		  case ',':
		    // if (char_counter)
		    {
		      words[col][char_counter]='\0';
		      col++;
		      char_counter=0;
		    }  
		    break;
		  case '\r':
		    /* Ignore */
		    break;
		  default:
		    words[col][char_counter]=c;
		    char_counter++;
		    break;
		  }
	      }
   	 
	    fclose(file);
	    lb_gr_refresh();
    
	    while (1)
	      {
		if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
		  {
		    SDL_Quit();
		    return EXIT_SUCCESS;
		  }
		if (SDL_PollEvent(&event) && event.type == SDL_WINDOWEVENT)
		  lb_gr_refresh();
	      }

#endif

  
    
	    //#define DEMO_PLOT2D_COMPLEX
#ifdef DEMO_PLOT2D_COMPLEX
	    int i;
	    VIEWPORT_2D_T win;

	    lb_fb_open("/dev/fb0", "/dev/tty1", 1, 1, 0*RENDEROPTIONS_LINE | 1*RENDEROPTIONS_GRAPHICS_ONLY);

	    win.xp_min=0;
	    win.yp_min=0;
	    win.xp_max=ty_width;
	    win.yp_max=ty_height;
	    win.xr_min=-3.1416;
	    win.xr_max=3.1416;
	    win.yr_min=-1.2;
	    win.yr_max=1.2;

	    VECTOR_C_T vec;
	    vec.items=50;
	    lb_al_create_vector_c(&vec);
      
	    LINE_2D_INT_T vec_i;
	    vec_i.items=50;
	    lb_gr_create_line2d_i(&vec_i);

	    for (i=0;i<vec.items;i++)
	      {
		vec.array[i].r=win.xr_min +i*(win.xr_max - win.xr_min)/vec.items;
		vec.array[i].i=sin(vec.array[i].r);
		//vec.array[i].i=1.0*i/vec.items;
	      }

	    lb_gr_project_2d_vector_c_to_line_int(win, &vec, &vec_i);
	    lb_gr_draw_polygon_i(NULL,&vec_i,3,lb_gr_12RGB(COLOR_BLUE),COPYMODE_BLEND,LINEMODE_FILTERED);
      
	    lb_gr_delay(20000);
     
	    lb_al_release_vector_c(&vec);
	    lb_gr_release_line2d_i(&vec_i);
	    lb_fb_exit(1);
#endif
    
	    //#define DEMO_PLOT_IMPLICIT 
#ifdef DEMO_PLOT_IMPLICIT
	    MATRIX_R_T A;
	    VIEWPORT_2D_T vp;
	    S_INT_16_T i,j;
	    FLOAT_T xr, yr, zr;

	    lb_fb_open("/dev/fb0", "/dev/tty1", 1, 1, 0*RENDEROPTIONS_LINE | 0*RENDEROPTIONS_GRAPHICS_ONLY);

	    A.rows=20; /* vertical */
	    A.cols=20; /* horizontal */
	    lb_al_create_matrix_r(&A);

	    /* Viewport definition */
	    vp.xp_min = 0;
	    vp.xp_max = ty_width;
	    vp.yp_min = 0;
	    vp.yp_max = ty_height;

	    vp.xr_min = -5;
	    vp.xr_max =  5;
	    vp.yr_min = -5;
	    vp.yr_max =  5;
         
	    for (i=0;i<A.rows;i++)
	      for (j=0;j<A.cols;j++)
		{
		  xr = vp.xr_min + j*(vp.xr_max-vp.xr_min)/(A.cols-1);
		  yr = vp.yr_min + i*(vp.yr_max-vp.yr_min)/(A.rows-1);
		  zr=4-xr*xr-yr*yr;
		  A.array[i][j]=zr;
		}
	    //lb_al_print_matrix_r(&A,"A",FLOAT_FORMAT_MATRIX);
      
	    lb_gr_clear_picture(NULL,lb_gr_12RGB(COLOR_WHITE));
	    lb_gr_implicit_2d(NULL, vp, &A, 1, lb_gr_12RGB(COLOR_RED),COPYMODE_COPY, LINEMODE_SOLID);
	    lb_gr_delay(10000);
	    lb_al_release_matrix_r(&A);
	    lb_fb_exit(1);
#endif

	    //#define DEMO_PLOT2D_REVERSE
#ifdef DEMO_PLOT2D_REVERSE
	    FLOAT_T t, t0, t1;
	    S_INT_16_T i;
	    VIEWPORT_2D_T vp;
	    VECTOR_R_T Lx, Ly;

	    lb_fb_open("/dev/fb0", "/dev/tty1", 4, 4, 0*RENDEROPTIONS_LINE | 0*RENDEROPTIONS_GRAPHICS_ONLY);

	    /* Viewport definition */
	    vp.xp_min = 0;
	    vp.xp_max = ty_width;
	    vp.yp_min = 0;
	    vp.yp_max = ty_height;

	    vp.xr_min = -2*1.024;
	    vp.xr_max = 2*1.024;
	    vp.yr_min = -2*0.768;
	    vp.yr_max = 2*0.768;

	    /* Create and load vectors */
	    Lx.items=400;
	    Ly.items=400;
	    lb_al_create_vector_r(&Lx);
	    lb_al_create_vector_r(&Ly);

	    t=0;
	    t0=0.0; t1=6.2832;
	    for(i=0;i<Lx.items;i++)
	      {
		printf("hello %d\r\n",i);
		Lx.array[i]=cos(3*t)*cos(t);  /* an example: petal flower */
		Ly.array[i]=cos(3*t)*sin(t);
		t+=(t1-t0)/(FLOAT_T)Lx.items;
	      }
	    lb_gr_plot2d_line_reverse(NULL, vp, &Lx, &Ly, 8, lb_gr_12RGB(0xffaa), COPYMODE_BLEND);
   
	    lb_gr_BMPfile_save("flower.bmp", NULL);
	    lb_gr_delay(15000);
	    lb_al_release_vector_r(&Lx);
	    lb_al_release_vector_r(&Ly);
	    lb_fb_exit(1);
#endif

	    //#define DEMO_PLOT2D_REVERSE_SLOW
#ifdef DEMO_PLOT2D_REVERSE_SLOW
	    /* oxo: missing lines due to emulation error  */
	    FLOAT_T t, t0, t1;
	    S_INT_16_T i;
	    VIEWPORT_2D_T vp;
	    VECTOR_R_T Lx, Ly;

	    lb_fb_open("/dev/fb0", "/dev/tty1", 1, 1, 0*RENDEROPTIONS_LINE | 1*RENDEROPTIONS_GRAPHICS_ONLY);

	    /* Viewport definition */
	    vp.xp_min = 0;
	    vp.xp_max = ty_width;
	    vp.yp_min = 0;
	    vp.yp_max = ty_height;

	    vp.xr_min = -2*1.024;
	    vp.xr_max = 2*1.024;
	    vp.yr_min = -2*0.768;
	    vp.yr_max = 2*0.768;

	    lb_gr_draw_circle_antialiasing3(NULL, ty_width/3,   ty_height/3,   ty_height/5, lb_gr_12RGB(0xf00f), COPYMODE_BLEND);
	    lb_gr_draw_circle_antialiasing3(NULL, ty_width*2/3, ty_height/3,   ty_height/5, lb_gr_12RGB(0xf0f0), COPYMODE_BLEND);
	    lb_gr_draw_circle_antialiasing3(NULL, ty_width/2,   ty_height*2/3, ty_height/5, lb_gr_12RGB(0x3F00), COPYMODE_BLEND);

	    /* Create and load vectors */ 
	    Lx.items=500;
	    Ly.items=500;
	    lb_al_create_vector_r(&Lx);
	    lb_al_create_vector_r(&Ly);

	    t=0;
	    t0=0.0; t1=6.2832;
	    for(i=0;i<Lx.items;i++)
	      {
		Lx.array[i]=cos(3*t)*cos(t); 
		Ly.array[i]=cos(3*t)*sin(t);
		t+=(t1-t0)/(FLOAT_T)Lx.items;
	      }

	    lb_gr_plot2d_line_reverse_slow(NULL, vp, &Lx, &Ly, 20, lb_gr_12RGB(0xfF00), COPYMODE_BLEND);
	    lb_gr_delay(15000);
      
	    lb_al_release_vector_r(&Lx);
	    lb_al_release_vector_r(&Ly);
	    lb_fb_exit(1);
#endif
    
	    //#define DEMO_JPG
#ifdef DEMO_JPG
	    PICTURE_T pic1, pic2;
	    S_INT_16_T i,j;

	    lb_fb_open("/dev/fb0", "/dev/tty1", 2, 2, 0*RENDEROPTIONS_LINE | 1*RENDEROPTIONS_GRAPHICS_ONLY);

	    lb_gr_BMPfile_getsize("sdl24bit.bmp",&i,&j);

	    pic1.w=i;
	    pic1.h=j;
	    lb_gr_create_picture(&pic1,lb_gr_12RGB(0x1000));
      
	    //lb_gr_BMPfile_load("sdl24bit.bmp",&pic1);
	    lb_gr_BMPfile_load_to_pic("sdl24bit.bmp",&pic1,0xFF);
	    lb_gr_render_picture(&pic1, 0, 0, 2, 2, 0);
      
	    pic2.w=pic1.h;
	    pic2.h=pic1.w;
	    lb_gr_create_picture(&pic2,lb_gr_12RGB(0x1000));
     
	    lb_gr_delay(5000);

	    for (j=0;j<pic1.w;j++)
	      for (i=0;i<pic1.h;i++)
		pic2.pic[j][i]=pic1.pic[i][j];

	    lb_gr_render_picture(&pic2, 0, 0, 2, 2, RENDEROPTIONS_DEFAULT);
	    lb_gr_delay(5000);
     
	    //      lb_gr_BMPfile_save("test.png",&pic2,3);
	    lb_gr_JPGfile_save("dieguini.jpg", &pic1, 15);

	    lb_gr_release_picture(&pic1);
	    lb_gr_release_picture(&pic2);
	    lb_fb_exit(1);
#endif

	    //#define DEMO_POLAR_AXIS
#ifdef DEMO_POLAR_AXIS
	    VIEWPORT_2D_T win;
	    FONT_T my_font;

	    lb_fb_open("/dev/fb0", "/dev/tty1", 1, 1, 0*RENDEROPTIONS_LINE | 1*RENDEROPTIONS_GRAPHICS_ONLY);

	    lb_ft_load_GLCDfont("fonts/Font_hp48G_large.lcd", &my_font);
	    my_font.scale_x=1;
	    my_font.scale_y=1;
	    my_font.gap_x=2;
	    my_font.gap_y=1;
	    my_font.max_x=40;
	    my_font.angle=0;
	    my_font.flag_fg=TRUE;
	    my_font.flag_bg=FALSE;
	    my_font.color_fg=lb_gr_12RGB(COLOR_YELLOW | COLOR_SOLID);
	    my_font.color_bg=lb_gr_12RGB(COLOR_BLACK);

	    win.xp_min=0;
	    win.xp_max=ty_width;
	    win.yp_min=0;
	    win.yp_max=ty_height;

	    win.xr_min= -1.024*3;
	    win.xr_max=  1.024*3;
	    win.yr_min=  0.768*3;
	    win.yr_max= -0.768*3; 

	    lb_gr_draw_rectangle(NULL, win.xp_min, win.yp_min, win.xp_max, win.yp_max, lb_gr_12RGB(COLOR_BLACK | COLOR_SOLID), COPYMODE_COPY);
	    lg_gr_draw_axis_2d_polar(NULL, win, &my_font, 0, 4, 1, lb_gr_12RGB(COLOR_BLUE), 0, 2*M_PI, 30*M_PI/180,
				     lb_gr_12RGB(COLOR_RED | COLOR_SOLID), 0, COPYMODE_BLEND);
	    lb_gr_delay(10000);
	    lb_fb_exit(1);
#endif

	    //#define DEMO_ADXIS_2D
#ifdef DEMO_AXIS_2D
	    VIEWPORT_2D_T win;
	    FONT_T my_font;

	    lb_fb_open("/dev/fb0", "/dev/tty1", 1, 1, 0*RENDEROPTIONS_LINE | 1*RENDEROPTIONS_GRAPHICS_ONLY);

	    lb_ft_load_GLCDfont("fonts/Font_hp48G_large.lcd", &my_font);
	    my_font.scale_x=1;
	    my_font.scale_y=1;
	    my_font.gap_x=2;
	    my_font.gap_y=1;
	    my_font.max_x=40;
	    my_font.angle=0;
	    my_font.flag_fg=TRUE;
	    my_font.flag_bg=FALSE;
	    my_font.color_fg=lb_gr_12RGB(COLOR_YELLOW | COLOR_SOLID);
	    my_font.color_bg=lb_gr_12RGB(COLOR_BLACK);

	    win.xp_min=20;
	    win.xp_max=ty_width-20;
	    win.yp_min=20;
	    win.yp_max=ty_height-20;
    
	    win.xr_min= 1e-2;
	    win.xr_max=  1e2;
	    win.yr_min=  1e5;
	    win.yr_max=  1E1; 

	    lb_gr_draw_rectangle(NULL, win.xp_min, win.yp_min, win.xp_max, win.yp_max,
				 lb_gr_12RGB(COLOR_WHITE), COPYMODE_COPY);
	    lg_gr_draw_axis_2d(NULL, win, &my_font, 3, lb_gr_12RGB(COLOR_WHITE), 1,
			       lb_gr_12RGB(COLOR_GREEN),10, lb_gr_12RGB(COLOR_YELLOW), 10, 
			       AXIS_DRAW_X_GRID_LOG | AXIS_DRAW_Y_GRID_LOG,
			       COPYMODE_COPY, LINEMODE_SOLID);
	    lb_gr_delay(30000);
	    lb_fb_exit(1); 
#endif

	    //#define DEMO_PLOT_CONTINUOUS
#ifdef DEMO_PLOT_CONTINUOUS
	    VIEWPORT_2D_T win;
	    FONT_T my_font;

	    lb_fb_open("/dev/fb0", "/dev/tty1", 2, 2, 0*RENDEROPTIONS_LINE | 1*RENDEROPTIONS_GRAPHICS_ONLY);

	    lb_ft_load_GLCDfont("fonts/Font_hp48G_large.lcd", &my_font);
	    my_font.scale_x=1;
	    my_font.scale_y=1;
	    my_font.gap_x=2;
	    my_font.gap_y=1;
	    my_font.max_x=40;
	    my_font.angle=0;
	    my_font.flag_fg=TRUE;
	    my_font.flag_bg=FALSE;
	    my_font.color_fg=lb_gr_12RGB(COLOR_YELLOW | COLOR_SOLID);
	    my_font.color_bg=lb_gr_12RGB(COLOR_BLACK);

	    win.xp_min=5;
	    win.xp_max=ty_width-5;
	    win.yp_min=5;
	    win.yp_max=ty_height-5;

	    win.xr_min= -1.024;
	    win.xr_max=  1.024;
	    win.yr_min= -0.768;
	    win.yr_max=  0.768; 

	    lb_gr_draw_rectangle(NULL, win.xp_min, win.yp_min, win.xp_max, win.yp_max, lb_gr_12RGB(COLOR_DIMGRAY), COPYMODE_COPY);
      
	    lg_gr_draw_axis_2d(NULL, win, &my_font, 3, lb_gr_12RGB(COLOR_WHITE), 2.5,
			       lb_gr_12RGB(COLOR_GREEN),1, lb_gr_12RGB(COLOR_BLUE),1,
			       AXIS_DRAW_X | AXIS_DRAW_X_ARROWS | AXIS_DRAW_X_GRID |
			       AXIS_DRAW_Y | AXIS_DRAW_Y_ARROWS | AXIS_DRAW_Y_GRID,
			       COPYMODE_BLEND, LINEMODE_FILTERED); 

	    lb_gr_plot_continuous_fn_2d(NULL, win, diego_x, diego_y,
					0, 2*10*M_PI, 1,
					20, lb_gr_12RGB(COLOR_BLUE | COLOR_SOLID), COPYMODE_COPY);

	    lb_gr_delay(10000);
    
	    lb_fb_exit(1); 
#endif

	    //#define DEMO_PLOT_CONTINUOUS_ANTIALIASING
#ifdef DEMO_PLOT_CONTINUOUS_ANTIALIASING
	    VIEWPORT_2D_T win;
	    FONT_T my_font;

	    lb_fb_open("/dev/fb0", "/dev/tty1", 1, 1, 0*RENDEROPTIONS_LINE | 1*RENDEROPTIONS_GRAPHICS_ONLY);

	    lb_ft_load_GLCDfont("fonts/Font_hp48G_large.lcd", &my_font);
	    my_font.scale_x=1;
	    my_font.scale_y=1;
	    my_font.gap_x=2;
	    my_font.gap_y=1;
	    my_font.max_x=40;
	    my_font.angle=0;
	    my_font.flag_fg=TRUE;
	    my_font.flag_bg=FALSE;
	    my_font.color_fg=lb_gr_12RGB(COLOR_YELLOW | COLOR_SOLID);
	    my_font.color_bg=lb_gr_12RGB(COLOR_BLACK);

	    win.xp_min=5;
	    win.xp_max=ty_width-5;
	    win.yp_min=5;
	    win.yp_max=ty_height-5;
     
	    win.xr_min= -1.024;
	    win.xr_max=  1.024;
	    win.yr_min=  0.768;
	    win.yr_max= -0.768; 

	    lb_gr_draw_rectangle(NULL, win.xp_min, win.yp_min, win.xp_max, win.yp_max,
				 lb_gr_12RGB(COLOR_WHITE | COLOR_SOLID), COPYMODE_COPY);
      
	    lg_gr_draw_axis_2d(NULL, win, &my_font, 3, lb_gr_12RGB(COLOR_BLACK), 3,
			       lb_gr_12RGB(COLOR_GREEN),1, lb_gr_12RGB(COLOR_BLUE),1,
			       AXIS_DRAW_X | AXIS_DRAW_X_ARROWS | AXIS_DRAW_X_GRID |
			       AXIS_DRAW_Y | AXIS_DRAW_Y_ARROWS | AXIS_DRAW_Y_GRID,
			       COPYMODE_COPY, LINEMODE_SOLID); 

	    lb_gr_plot_continuous_fn_2d_antialiasing(NULL, win, diego_x, diego_y,
						     0, 2*M_PI, 0.1, 32,
						     16, lb_gr_12RGB(COLOR_BLUE|COLOR_SOLID), COPYMODE_BLEND);
	    lb_gr_delay(10000);
	    lb_fb_exit(1);
#endif


	    //#define DEMO_MALDENBROT
#ifdef DEMO_MALDENBROT
	    int xp, yp, iterations;
	    FLOAT_T xr, yr;
	    COMPLEX_T z, p;
	    VIEWPORT_2D_T win;

	    lb_fb_open("/dev/fb0", "/dev/tty1", 1, 1, 0*RENDEROPTIONS_LINE | 0*RENDEROPTIONS_GRAPHICS_ONLY);

	    win.xp_min=0;
	    win.yp_min=0;
	    win.xp_max=ty_width;
	    win.yp_max=ty_height;
	    win.xr_min=-1.024*2;
	    win.xr_max=1.024*2;
	    win.yr_min=-0.768*2; 
	    win.yr_max=0.768*2;

	    for(xp=0;xp<win.xp_max;xp++)
	      for(yp=0;yp<win.yp_max;yp++)
		{
		  lb_gr_project_2d_inv(win, xp, yp, &xr, &yr);
		  iterations=0;
		  z.r=xr;
		  z.i=yr;
		  while ((lb_cp_abs(z)<2.0) && (iterations<15)) 
		    {
		      p.r=xr;
		      p.i=yr;
		      z=lb_cp_add(lb_cp_multiply(z,z),p);
		      iterations++;
		    }
		  lb_gr_draw_pixel(NULL, xp, yp, lb_gr_12RGB(iterations), COPYMODE_COPY);
		}
	    lb_gr_BMPfile_save("maldenbrot.bmp",NULL);
	    lb_gr_delay(10000);
	    lb_fb_exit(1);
#endif

	    //#define DEMO_VIDEO
#ifdef DEMO_VIDEO
	    int xp, yp, iterations, k;
	    FLOAT_T xr, yr, z_zoom;
	    char filename[12];
	    COMPLEX_T z, p;
	    VIEWPORT_2D_T win;

	    lb_fb_open("/dev/fb0", "/dev/tty1", 4, 4, 0*RENDEROPTIONS_LINE | 0*RENDEROPTIONS_GRAPHICS_ONLY);

	    z_zoom=1.0;
	    win.xp_min=0;
	    win.yp_min=0;
	    win.xp_max=ty_width;
	    win.yp_max=ty_height;

	    for(k=0;k<1440;k++)
	      {
		win.xr_min=0.32-1.0/z_zoom;
		win.xr_max=0.32+1.0/z_zoom;
		win.yr_min=-0.32/z_zoom; 
		win.yr_max=0.32/z_zoom;

		for(xp=0;xp<win.xp_max;xp++)
		  for(yp=0;yp<win.yp_max;yp++)
		    {
		      lb_gr_project_2d_inv(win, xp, yp, &xr, &yr);
		      iterations=0;
		      z.r=xr;
		      z.i=yr;
		      while ((lb_cp_abs(z)<2.0) && (iterations<15)) 
			{
			  p.r=xr;
			  p.i=yr;
			  z=lb_cp_add(lb_cp_multiply(z,z),p);
			  iterations++;
			}
		      lb_gr_draw_pixel(NULL, xp, yp, lb_gr_12RGB(iterations), COPYMODE_COPY);
		    }
		sprintf(filename,"malde%04d.jpg",k);
		lb_gr_JPGfile_save(filename, NULL, 75);
		z_zoom*=1.001;
	      }
	    lb_fb_exit(1);
#endif

	    //#define DEMO_PLOT3D
#ifdef DEMO_PLOT3D
	    PICTURE_T Pic, Pic_console;
	    CONSOLE_T Con;
  
	    VIEWPORT_3D_T vp3d;
	    //FLOAT_T u_a, u_b, v_a, v_b;
	    FLOAT_T Rot[3][3], Rx_p[3][3], Rx_n[3][3], Ry_p[3][3], Ry_n[3][3], Rz_p[3][3], Rz_n[3][3];
	    S_INT_8_T flag_exit;
    
	    //MATRIX_POINT_3D_T S;
	    MATRIX_R_T Z;
	    char c;
	    FONT_T font_console;

	    lb_fb_open("/dev/fb0", "/dev/tty1", 1, 1, 0*RENDEROPTIONS_LINE | 1*RENDEROPTIONS_GRAPHICS_ONLY);
    
  
	    Pic.w=ty_width;
	    Pic.h=ty_height/2;
	    lb_gr_create_picture(&Pic,lb_gr_12RGB(COLOR_RED | COLOR_SOLID));

	    Pic_console.w=ty_width;
	    Pic_console.h=ty_height/2;
	    lb_gr_create_picture(&Pic_console,lb_gr_12RGB(COLOR_BLUE | COLOR_SOLID));


	    lb_ft_load_GLCDfont("fonts/Font_hp48G_large.lcd", &font_console);
	    font_console.scale_x=1;
	    font_console.scale_y=1;
	    font_console.gap_x=2;
	    font_console.gap_y=2;
	    font_console.max_x=10;
	    font_console.angle=0;
	    font_console.flag_fg=TRUE;
	    font_console.flag_bg=FALSE;
	    font_console.color_fg=lb_gr_12RGB(COLOR_WHITE | COLOR_SOLID);
	    font_console.color_bg=lb_gr_12RGB(COLOR_BLUE | COLOR_SOLID);
 
	    lb_ft_resize_console(&Pic_console, &font_console, &Con);
	    Con.color_fg=lb_gr_12RGB(COLOR_WHITE | COLOR_SOLID);
	    Con.color_bg=lb_gr_12RGB(COLOR_BLUE | COLOR_SOLID);
	    lb_ft_create_console(&Con);
	    lb_ft_set_active_console(&Con);
    
	    flag_exit=FALSE;
      
	    vp3d.xp_min=0;
	    vp3d.yp_min=0;
	    vp3d.xp_max=Pic.w;
	    vp3d.yp_max=Pic.h;
	    vp3d.scale =200.0;    /* Zoom */
	    vp3d.cam_d= 0.0;   /* Stereoscopic */
	    vp3d.cam_h=10.0;    /* Depth */
	    vp3d.cam.x=0.0;   /* Camera's location */
	    vp3d.cam.y=0.0;   /* Camera's location */
	    vp3d.cam.z=40.0;   /* Camera's location */
		    
      
	    lb_al_fill_rotation_matrix33_Z(Rz_p, M_PI/90);
	    lb_al_fill_rotation_matrix33_Z(Rz_n,-M_PI/90);
	    lb_al_fill_rotation_matrix33_Y(Ry_p, M_PI/90);
	    lb_al_fill_rotation_matrix33_Y(Ry_n,-M_PI/90);
	    lb_al_fill_rotation_matrix33_X(Rx_p, M_PI/90);
	    lb_al_fill_rotation_matrix33_X(Rx_n,-M_PI/90);

    
	    lb_al_print_matrix33_r(Rz_p,"Rz_p",FLOAT_FORMAT_MATRIX);
	    lb_al_print_matrix33_r(Rz_n,"Rz_n",FLOAT_FORMAT_MATRIX);
	    lb_al_print_matrix33_r(Ry_p,"Ry_p",FLOAT_FORMAT_MATRIX);
	    lb_al_print_matrix33_r(Ry_n,"Ry_n",FLOAT_FORMAT_MATRIX);
	    lb_al_print_matrix33_r(Rx_p,"Rx_p",FLOAT_FORMAT_MATRIX);
	    lb_al_print_matrix33_r(Rx_n,"Rx_n",FLOAT_FORMAT_MATRIX);


	    lb_al_fill_rotation_matrix33_tait_bryan_ZYX(Rot,0,0,-M_PI/4);

	    /* Surface */
	    //S.rows=20;
	    //S.cols=30;
	    //lb_al_create_matrix_p3d(&S);
	    //u_a=0.0;
	    //u_b= 2*M_PI;
	    //v_a=0;
	    //v_b=5.0;


	    /* Z-buffer */
	    lb_gr_create_zbuffer(NULL, &Z);


    
	    //for (i=0;i<S.rows;i++)
	    //  for (j=0;j<S.cols;j++)
	    //	{
	    //	  S.array[i][j].x=(j*(v_b-v_a)/(S.cols-1))*cos(u_a + i*(u_b-u_a)/(S.rows-1));
	    //	  S.array[i][j].y=(j*(v_b-v_a)/(S.cols-1))*sin(u_a + i*(u_b-u_a)/(S.rows-1));;
	    //	  S.array[i][j].z=sin(S.array[i][j].x*S.array[i][j].x+S.array[i][j].y*S.array[i][j].y)/2.5;
	    //	  S.array[i][j].z=(S.array[i][j].x*S.array[i][j].x+S.array[i][j].y*S.array[i][j].y)/10.0;
	    //printf("[%f\t%f\t%f]\r\n",S.array[i][j].x,S.array[i][j].y,S.array[i][j].z); 
	    //	}
    

	    while (!flag_exit)
	      {
		lb_gr_clear_picture(&Pic,lb_gr_12RGB(COLOR_BLACK));

		lb_gr_reset_zbuffer(&Z);
	
		//lb_gr_plot3d_surface(NULL, vp3d, Rot, &S,
		//		     1.0, lb_gr_12RGB(COLOR_BLUE), COPYMODE_COPY, LINEMODE_SOLID);

		lb_gr_plot_zbuffer_line_1(&Pic, vp3d, Rot, &Z, (POINT_3D_FLOAT_T){0,0,0}, (POINT_3D_FLOAT_T){2,2,2},
					  lb_gr_12RGB(COLOR_WHITE), COPYMODE_COPY);

		lb_gr_plot_zbuffer_line_1(&Pic, vp3d, Rot, &Z, (POINT_3D_FLOAT_T){1,0,0}, (POINT_3D_FLOAT_T){0,1,0},
					  lb_gr_12RGB(COLOR_BLUE), COPYMODE_COPY);

		lb_gr_plot_zbuffer_triangle(&Pic, vp3d, Rot, &Z, (POINT_3D_FLOAT_T){0,0,1}, (POINT_3D_FLOAT_T){0,3,1},
					    (POINT_3D_FLOAT_T){3,0,1}, lb_gr_12RGB(COLOR_SOLID | COLOR_FORESTGREEN), COPYMODE_BLEND);

		lb_gr_plot_zbuffer_triangle(&Pic, vp3d, Rot, &Z, (POINT_3D_FLOAT_T){1,0,0}, (POINT_3D_FLOAT_T){1,2,0},
					    (POINT_3D_FLOAT_T){1,0,2}, lb_gr_12RGB(COLOR_SOLID | COLOR_YELLOW), COPYMODE_BLEND);

		lb_gr_plot_zbuffer_triangle(&Pic, vp3d, Rot, &Z, (POINT_3D_FLOAT_T){0,0,0}, (POINT_3D_FLOAT_T){0,5,0},
					    (POINT_3D_FLOAT_T){5,0,0}, lb_gr_12RGB(COLOR_SOLID | 0xa00), COPYMODE_BLEND);

		lb_gr_plot_zbuffer_triangle(&Pic, vp3d, Rot, &Z, (POINT_3D_FLOAT_T){5,5,0}, (POINT_3D_FLOAT_T){0,5,0},
					    (POINT_3D_FLOAT_T){5,0,0}, lb_gr_12RGB(COLOR_SOLID | 0xa00), COPYMODE_BLEND);

		lb_gr_plot_zbuffer_triangle(&Pic, vp3d, Rot, &Z, (POINT_3D_FLOAT_T){0,0,0}, (POINT_3D_FLOAT_T){1,1,1},
					    (POINT_3D_FLOAT_T){2,2,2}, lb_gr_12RGB(COLOR_SOLID | 0xF00), COPYMODE_BLEND);

	
		vp3d.cam_d=0;


		lg_gr_draw_axis_3d(&Pic, vp3d, Rot, &font_console,
				   5.0, 4.0, lb_gr_12RGB(COLOR_WHITE),
				   0, 5.0, 1.0,
				   0, 5.0, 1.0,
				   0, 1.0, 1.0,
				   2.0, lb_gr_12RGB(0xF911),
				   0*AXIS_DRAW_X_GRID | 0*AXIS_DRAW_Y_GRID | AXIS_DRAW_Z_GRID,
				   "O","X","Y","Z",
				   COPYMODE_BLEND, LINEMODE_FILTERED);

		lb_gr_render_picture(&Pic, 0, 0, 1, 1, 1*RENDEROPTIONS_LINE);

	
		// Wait for the user to press a character.

		lb_gr_clear_picture(&Pic_console, lb_gr_12RGB(COLOR_GRAY));
		lb_ft_draw_console(&Pic_console, &font_console, &Con, COPYMODE_COPY);
		lb_gr_render_picture(&Pic_console, 0, Pic.h, 1, 1, 0*RENDEROPTIONS_LINE);

		S_INT_8_T flag_process_keys=TRUE;
		while (!lb_co_kbhit()) ;
		while (flag_process_keys)
		  {
		    c=lb_co_getch_pc();
	  
		    switch(c)
		      {
		      case 'r':
			vp3d.xp_min=0;
			vp3d.yp_min=0;
			vp3d.xp_max=ty_width;
			vp3d.yp_max=ty_height;
			vp3d.scale =100.0;    /* Zoom */
			vp3d.cam_d= 0.0;   /* Stereoscopic */
			vp3d.cam_h=10.0;    /* Depth */
			vp3d.cam.x=0.0;   /* Camera's location */
			vp3d.cam.y=0.0;   /* Camera's location */
			vp3d.cam.z=20.0;   /* Camera's location */
			break;
		      case 'h':
			vp3d.cam_h*=1.1;
			break;
		      case 'H':
			vp3d.cam_h/=1.1;
			break;
		      case 'x':
			lb_al_multiply_matrix33_r_copy(Rot,Rx_p,Rot);
			break;
		      case 'X':
			lb_al_multiply_matrix33_r_copy(Rot,Rx_n,Rot);
			break;
		      case 'y':
			lb_al_multiply_matrix33_r_copy(Rot,Ry_p,Rot);
			break;
		      case 'Y':
			lb_al_multiply_matrix33_r_copy(Rot,Ry_n,Rot);
			break;
		      case 'z':
			lb_al_multiply_matrix33_r_copy(Rot,Rz_p,Rot);
			break;
		      case 'Z':
			lb_al_multiply_matrix33_r_copy(Rot,Rz_n,Rot);
			break;
		      case PCKEY_PAGE_UP:
			vp3d.scale*=1.1;
			break;
		      case PCKEY_PAGE_DOWN:
			vp3d.scale/=1.1;
			break;
		      case PCKEY_UP:
			vp3d.cam.y+=1.0;
			break;
		      case PCKEY_DOWN:
			vp3d.cam.y-=1.0;
			break;
		      case PCKEY_LEFT:
			vp3d.cam.x+=1.0;
			break;
		      case PCKEY_RIGHT:
			vp3d.cam.x-=1.0;
			break;
		      case PCKEY_ESC:
			flag_exit=TRUE;
			break;
		      }
		    if(lb_co_kbhit())
		      flag_process_keys=TRUE;
		    else
		      flag_process_keys=FALSE;
		  }
	      }
	    lb_ft_release_GLCDfont(&font_console);
	    lb_gr_release_picture(&Pic);
	    lb_gr_release_picture(&Pic_console);
	    lb_ft_release_console(&Con);
	    lb_fb_exit_msg("Closing program\r\n");    
	    //lb_al_release_matrix_p3d(&S);
  
#endif


	    /*******************************************************************************************************************/
	    /* Fonts */
	    /******************************************************************************************************************/

	    //#define DEMO_NEW_FONT
#ifdef DEMO_NEW_FONT
	    FONT_T my_font;
	    char text[40];
	    FLOAT_T x;

	    lb_fb_open("/dev/fb0", "/dev/tty1", 4, 4, 0*RENDEROPTIONS_LINE | 0*RENDEROPTIONS_GRAPHICS_ONLY);

	    //printf("-10/10=%d\r\n",lb_in_round_div_up(1000,1001));
	    //lb_fb_exit(1);
    
	    lb_ft_load_GLCDfont("fonts/Font_hp48G_large.lcd", &my_font);
	    //lb_ft_load_GLCDfont("fonts/Font_hp48G_small.lcd", &my_font);
	    my_font.scale_x=4;
	    my_font.scale_y=3;
	    my_font.gap_x=2;
	    my_font.gap_y=1;
	    my_font.max_x=40;
	    my_font.angle=0;
	    my_font.flag_fg=TRUE;
	    my_font.flag_bg=TRUE;
	    my_font.color_fg=lb_gr_12RGB(COLOR_TEAL);
	    my_font.color_bg=lb_gr_12RGB(COLOR_BLACK);

	    for(x=0;x<50.0;x+=0.1)
	      {
		sprintf(text,"%02.1f",x);
		lb_ft_draw_text(NULL, &my_font, ty_width/2, ty_height/2, text, COPYMODE_COPY);
		lb_gr_delay(200);
	      } 

	    strcpy(text,"hola \r\namigo");
	    lb_ft_draw_text(NULL, &my_font, ty_width/2, ty_height/2, text, COPYMODE_COPY);
	    lb_ft_draw_text_centered(NULL, &my_font, 20, 70, 200, 50,  text, COPYMODE_COPY);
	    lb_gr_draw_rectangle(NULL, 20, 70, 200, 50, lb_gr_12RGB(COLOR_WHITE), COPYMODE_COPY);
	    lb_gr_delay(10000);
	    lb_ft_release_GLCDfont(&my_font);
	    lb_fb_exit(1);
#endif

    
	    /*******************************************************************************************************************/
	    /* Financial */
	    /******************************************************************************************************************/

	    //#define DEMO_FINANCIAL
#ifdef DEMO_FINANCIAL
	    ERR_T e_code;
	    FLOAT_T apr, monthly;
	    apr=6.75;
	    monthly=lb_re_APR_to_monthly(apr, &e_code);
	    printf("APR = %.4f %% half yearly, not in advanced corresponds to %.4f %% monthly\r\n",
		   apr,monthly);
	    printf("monthly = %.4f %% corresponds to %.4f %% effective yearly\r\n",
		   monthly,lb_re_monthly_to_effective(monthly,&e_code));
	    printf("monthly = %.4f %% corresponds to %.4f %% APR\r\n",
		   monthly,lb_re_monthly_to_APR(monthly,&e_code));
	    exit(1);
#endif

	    /*******************************************************************************************************************/
	    /* Parsing */
	    /******************************************************************************************************************/

	    //#define DEMO_PARSER_REAL
#ifdef DEMO_PARSER_REAL
	    ERR_T e_code;
	    FLOAT_T x,y,z,result;
	    char variables[]="x,y,i"; 
	    x=1;
	    y=M_PI*60/180;
	    z=3;  
    
      
	    /* Example: pi as an infinite addition - Machin */
	    //char expression[]="4*SIGMA(1,20000,(-1)^(i+1)/(2*i-1),i,1)";   
     
	    /* Example: pi as an infinite product */
	    //char expression[]="2*PROD(1,100000,4*i^2/(4*i^2-1),i,1)";   
        
	    /* Example: sin(x) as an infinite addition */
	    //char expression[]="sigma(0,10,(-1)^i*y^(2*i+1)/(2*i+1)!,i,1)"; 

	    /* Example: cos(x) as an infinite addition */
	    //char expression[]="sigma(0,2,(-1)^i*y^(2*i)/(2*i)!,i,1)";    
      
	    /* Example: first order derivate */
	    //char expression[]="diff(sin(x)/cos(x+pi/8),x,30*pi/180,1e-2)";   
	    //char expression[]="sin(y)/cos(y+pi/8)";   

	    /* Example: first order derivate */
	    //char expression[]="diff(sin(x),x,30*pi/180,1e-4)";   

	    /* Example: second order derivate */
	    //char expression[]="diff2(sin(x),x,30*pi/180,1e-4)";   

	    /* Example: integral */
	    //char expression[]="idef(0,pi/6,sin(x),x,2)"; 
      
	    /* Example: double integral */
	    char expression[]="idef2(1,5,0,2,x^2*y^3,x,y,5)"; 

	    /* Example: first order differential */
	    //char expression[]="diff(-x^2,x,10,1e-5)";   
      
	    /* Example: second order differential */
	    //char expression[]="diff2(sin(x),x,30*pi/180,1e-2)";   

	    /* Example: Gauss' childhood sum */
	    // char expression[]="sigma(0,100,i,i,1)";   

	    /* Example: product */
	    //char expression[]="prod(2,7,i,i,1)";   

	    /* Example: comparison */
	    //char expression[]="100*((10=10)*(10=4))";   

	    // char expression[]="diff(idef(0,y,sin(x),x,1000),y,pi/6,0.0001)";   
	    //char expression[]="rand(2)";   
      
	    //char expression[]="(0+10+idef(0,pi/2,sin(x),x,10))/11";   
      
	    //char expression[]="avg(1,2,3,4,5,6,7,8,sin(3+x*pi/4),10)";
	    //char expression[]="diff(1+2+3+sin(x),x,0.4,0.01)";   
	    //char expression[]="diff(idef(0,y,x^2,x,10),x,0.444,0.01)";   
	    //char expression[]="45+idef(4.25,z,1,x,10)";   
	    //char expression[]="diff(sin(x+y),x,0.4,0.0001)";   
	    //char expression[]="diff(idef(0,y,sin(x),x,10),y,0.4,0.01)";   
	    //char expression[]="diff(cos(x),x,pi/2,0.001)";   
	    //char expression[]="exp(3.25)";   
	    //char expression[]="idef(pi/2,pi,sin(x),x,65000)";   
	    //char expression[]="sin(1+y*cos(3+pi/4))";   
	    //char expression[]="1+2-avg(x,2,3,4,5,6,7,8)+10*sin(pi/4)+1000+2000+1+2+3";   
	    //char expression[]="100+250+10*sin(pi/4)+1000+2000+sin(pi/4)";   
	    //char expression[]="sin(sin(1,2,3,4,5)+2+3,sin(1,2,3,4,5,6),sin(1))"; 
	    //char expression[]="avg(10,20)";
     
      
	    e_code=e_none;
	    system("clear");
	    printf("Formula: %s\r\n",expression);
	    printf(" Variables: %s\r\n", variables);
	    printf(" Values: %0.4f y=%0.4f z=%0.4f\r\n", x,y,z);
	    result=lb_pa_formula(expression,variables,x,y,z,&e_code);
	    printf("Error: %s\r\n", errors[e_code].msg);
	    printf("Result=%0.6f\r\n",result);
	    exit(1);
#endif

  
	    //#define DEMO_PARSER_COMPLEX
#ifdef DEMO_PARSER_COMPLEX 
	    ERR_T e_code;
	    COMPLEX_T var1,var2,imag,result;
	    var1.r=0;
	    var1.i=0;
	    var2.r=0;
	    var2.i=0;
	    imag.r=0;
	    imag.i=1;

	    /* Example: pi as a product */
	    char variables[]="x,k,i";
	    //char expression[]="2*PROD(1,10000,4*k^2/(4*k^2-1),k+1,1)";   
    
	    /* Example: Gauss'childhood sum, using complex numbers  */
	    char expression[]="sigma(100,1,0.5*k+k*i,K,-1)";   
    
  
	    /* Example: first order derivate */
	    //char expression[]="diff(e^x,x,30*pi/180+0.5*i,1e-4)";   

	    /* Error: Example: complex integral */
	    //char expression[]="idef(0,abs(pi*30/180,sin(x)+cos(x)*i),x,100)";   

	    /* Error: Example: complex integral */
	    //char expression[]="idef(0,pi*30/180,abs(sin(x)+cos(x)*i),x,100)";   
     
	    /* Example: double integral */
	    //char expression[]="idef2(0,pi/4,0,pi/2,sin(x+y*i),x,y,1.1)"; 

	    /* Example: integral */
	    //char expression[]="idef(i,pi/4+i/3,cos(x)+sin(x)*i,x,10)"; 
 
	    /* Example: first order differential  */
	    //char expression[]="diff(sin(x)+i*cos(x),x,30*pi/180,0.001)"; 
 
	    /* Example: second order differential  */
	    //char expression[]="diff2(sin(x)+i*cos(x),x,30*pi/180,0.01)"; 

	    /* Example: larger of two complex numbers  */
	    //char expression[]="max2(5+4*i,-3*i)"; 
 
	    /* Example: Gauss'childhood sum, using complex numbers  */
	    //char expression[]="sigma(100,1,0.5*x+x*i,x,-1)";   
    
	    /* Example: product  */
	    //char expression[]="prod(1,3,x+i,x,1)";   
    
	    /*
	      x.r=-1.0;
	      x.i=0.0;
	      y.r=1.0;
	      y.i=0.0;
	      lb_cp_print_c(x,"X",FLOAT_FORMAT);
	      printf("\r\n");
	      lb_cp_print_c(y,"Y",FLOAT_FORMAT);
	      printf("\r\n");
	      lb_cp_print_c(lb_cp_power(x,y,&e_code),"(-1)^1",FLOAT_FORMAT);
	      printf("\r\n"); */
      
	    //char expression[]="sigma(0,10,(-1)^k,k,1)";   
	    //char expression[]="sigma(0,10,x^(2*k+1),k,1)";   
	    //char expression[]="sigma(0,4+0*0.01*i,(-1)^k*x^(2*k+1)/(2*k+1)!,k,1+0.1*0.01*i)";   
      
	    /* Problem */
	    // char expression[]="(20)^2";   
	    //char expression[]="(-1)^1*x^(2.0*1.0+1.0)/(2.0*1.0+1.0)!";   
	    //char expression[]="(-1)^k*x^(2*k+1)/(2*k+2)";   

       
	    e_code=e_none;
	    system("clear");
	    printf("Expression: %s\r\n",expression);
	    printf(" Variables: %s\r\n", variables);
	    printf(" Values: x=[%0.4f , %0.4f]  y=[%0.4f , %0.4f]\r\n", var1.r,var1.i,var2.r,var2.i);
	    result=lb_pa_formula_complex(expression,variables,var1,var2,imag,&e_code);
	    printf("Error: %s\r\n", errors[e_code].msg);
	    printf("Result= [%0.8f , %0.8f] \r\n",result.r, result.i);
	    exit(1);
#endif

	    /*******************************************************************************************************************/
	    /* GIS demos */
	    /******************************************************************************************************************/

	    //#define DEMO_GIS
#ifdef DEMO_GIS
	    FLOAT_T lon, lat;
	    FILE *fp;
	    int i_lat, i_lon, parsing_lon;
	    S_INT_16_T i, j;
	    char c, str_lat[128], str_lon[128];

	    lb_fb_open("/dev/fb0", "/dev/tty1", 1, 1, 0*RENDEROPTIONS_LINE | 1*RENDEROPTIONS_GRAPHICS_ONLY);
	    lb_fb_clear(ty_fb_main, 0,0,0,255);
  
	    if (!(fp = fopen("world_map_large.csv", "r")))
	      {
		perror("invalid world data file");
		lb_fb_exit(1);
	      }

	    i_lat=0; i_lon=0;
	    parsing_lon=TRUE;
	    while ((c = fgetc(fp)) != EOF)
	      {
		if (parsing_lon)
		  {
		    if(isdigit(c) || (c=='.') || (c=='-'))
		      {
			str_lon[i_lon]=c;
			i_lon++;
		      }
		    else
		      {
			if ((i_lon>0) && (c==' '))
			  {
			    str_lon[i_lon]='\0';
			    lon=atof(str_lon);
			    //printf("%f",lat);
			    parsing_lon=FALSE;
			    i_lon=0;		      
			  }
		      }
		  }
		else
		  {
		    if(isdigit(c) || (c=='.') || (c=='-'))
		      {
			str_lat[i_lat]=c;
			i_lat++;
		      }
		    else
		      {
			if ((i_lat>0) && ((c==',') || (c==')')))
			  {
			    str_lat[i_lat]='\0';
			    lat=atof(str_lat);
			    //printf("***%f\r\n",lon);
			    fflush(stdout);
			    parsing_lon=TRUE;
			    i_lat=0;

			    j=0.5*ty_width*(1.0+lon/180.0);
			    i=0.3*ty_height*(1.0-lat/90.0);

			    if ( (-90.0<lat) && (lat<90.0) && (-180.0<lon) && (lon<180.0))
			      lb_gr_draw_pixel(NULL,j,i,lb_gr_12RGB(COLOR_BLUE),COPYMODE_COPY);
			  }
		      }
		  }
		//printf("%c",c);
	      }
	    fclose(fp);
	    lb_gr_delay(30000);
	    lb_fb_exit(1);
#endif

	    //#define DEMO_GIS_HEIGHT
#ifdef DEMO_GIS_HEIGHT
	    S_INT_16_T i, j, height, height_max;

	    lb_fb_open("/dev/fb0", "/dev/tty1", 1, 1, 0*RENDEROPTIONS_LINE | 1*RENDEROPTIONS_GRAPHICS_ONLY);
	    lb_fb_clear(ty_fb_main, 0,0,0,255);

	    height_max=0;
      
	    for (i=0;i<ty_height;i++)
	      for (j=0;j<ty_width;j++)
		{
		  height=lb_gis_height(-73+(FLOAT_T)j/ty_width, 8.0+(FLOAT_T)i/ty_height);

		  if (height>height_max)
		    height_max=height;
	    
		  //printf("height=%d\r\n",height);

		  PIXEL_T pix;
		  pix.r=0;
		  pix.g=round(MAX_R*(FLOAT_T)height/3619.0);
		  pix.b=0;
		  pix.k=MAX_K;
	  
		  lb_gr_draw_pixel(NULL,j,i,pix,COPYMODE_COPY);

		  //lb_gr_delay(50);
		}
	    printf("height-max=%d\r\n",height_max);      

	    char c;
	    while ((c=lb_co_getch())!='x') ;
	    lb_fb_exit(1);
#endif

	    //#define DEMO_PARALELL
#ifdef DEMO_PARALELL
	    pthread_t threads[4];
	    int rc;
	    double l0, l1, l2, l3;
	    l3=200;
	    l2=200;
	    l1=200;
	    l0=200;
	    rc=pthread_create(&threads[0],NULL, floatsum, (void *) &l0);
	    rc=pthread_create(&threads[1],NULL, floatsum, (void *) &l1);
	    rc=pthread_create(&threads[2],NULL, floatsum, (void *) &l2);
	    rc=pthread_create(&threads[3],NULL, floatsum, (void *) &l3);

	    rc=pthread_join(threads[0],NULL);;
	    rc=pthread_join(threads[1],NULL);;
	    rc=pthread_join(threads[2],NULL);;
	    rc=pthread_join(threads[3],NULL);;
  
#endif
      
	    /*******************************************************************************************************************/
	    /* Statistics */
	    /******************************************************************************************************************/

	    //#define DEMO_HISTOGRAM
#ifdef DEMO_HISTOGRAM
	    VIEWPORT_2D_T win;
	    VECTOR_R_T Data, Bins;
	    FONT_T my_font;
	    char text[40];
	    FLOAT_T x;
	    S_INT_16_T i;
	    FLOAT_T mu, sigma2;

	    lb_fb_open("/dev/fb0", "/dev/tty1", 1, 1, 0*RENDEROPTIONS_LINE | 1*RENDEROPTIONS_GRAPHICS_ONLY);

	    win.xp_min=0;
	    win.xp_max=ty_width;
	    win.yp_min=0;
	    win.yp_max=ty_height;
	    win.xr_min=-40;
	    win.xr_max=400;
	    win.yr_min=3000.0; 
	    win.yr_max=-30;

	    /* Data generation */
	    Data.items=30000;
	    lb_al_create_vector_r(&Data);
	    for(i=0;i<Data.items;i++)
	      Data.array[i]=lb_st_marsaglia_polar(30);
      
	    Bins.items=40;
	    lb_al_create_vector_r(&Bins);
	    lb_st_histogram(&Data,&Bins,-100,100,&mu, &sigma2);
	    lb_al_print_vector_r(&Bins, "Bin", "%04.2f\r\n");

	    printf("mu=%f,  sigma2=%f\r\n",mu,sigma2);
	    lb_gr_draw_histogram(NULL, win, &Bins, 
				 lb_gr_12RGB(COLOR_WHITE), lb_gr_12RGB(COLOR_DIMGRAY),
				 lb_gr_12RGB(COLOR_BLACK), lb_gr_12RGB(COLOR_DARKBLUE));
           
	    lb_ft_load_GLCDfont("fonts/Font_hp48G_large.lcd", &my_font);
	    //lb_ft_load_GLCDfont("fonts/Font_hp48G_small.lcd", &my_font);
	    my_font.scale_x=2;
	    my_font.scale_y=2;
	    my_font.gap_x=1;
	    my_font.gap_y=1;
	    my_font.max_x=40;
	    my_font.angle=0;
	    my_font.flag_fg=TRUE;
	    my_font.flag_bg=FALSE;
	    my_font.color_fg=lb_gr_12RGB(COLOR_BLUE);
	    my_font.color_bg=lb_gr_12RGB(COLOR_RED);

	    lb_gr_delay(10000);
	    lb_ft_release_GLCDfont(&my_font);
	    lb_al_release_vector_r(&Data);
	    lb_fb_exit(1);
#endif

	    /*******************************************************************************************************************/
	    /* General Mathematics */
	    /******************************************************************************************************************/

	    //#define DEMO_PRIMES
#ifdef DEMO_PRIMES
	    VIEWPORT_2D_T win;
	    FONT_T my_font;
	    VECTOR_S_INT_16_T P;
	    S_INT_32_T k,j;
	    FLOAT_T t, x, y, xp, yp;

	    lb_fb_open("/dev/fb0", "/dev/tty1", 1, 1, 0*RENDEROPTIONS_LINE | 1*RENDEROPTIONS_GRAPHICS_ONLY);

	    lb_ft_load_GLCDfont("fonts/Font_hp48G_large.lcd", &my_font);
	    my_font.scale_x=1;
	    my_font.scale_y=1;
	    my_font.gap_x=2;
	    my_font.gap_y=1;
	    my_font.max_x=40;
	    my_font.angle=0;
	    my_font.flag_fg=TRUE;
	    my_font.flag_bg=FALSE;
	    my_font.color_fg=lb_gr_12RGB(COLOR_YELLOW | COLOR_SOLID);
	    my_font.color_bg=lb_gr_12RGB(COLOR_BLACK);

          
	    win.xp_min=5;
	    win.xp_max=ty_width-5;
	    win.yp_min=5;
	    win.yp_max=ty_height-5;

   
	    win.xr_min= 0.0;
	    win.xr_max=  10.0;
	    win.yr_min=  10.0;
	    win.yr_max= 0.0; 


	    P.items=100;
	    lb_al_create_vector_si16(&P);



	    lb_sieve_erathostenes_2(&P, 32500);
	    lb_al_print_vector_si16(&P,"Primes");

	    j=0;
	    for(k=1;k<10000000;k++)
	      {
		if(lb_in_is_perfectsquare(k)==TRUE)
		  {
		    printf("%d\r\n",k);
		    j++;
		  }
	      }

	    k=10000000000;
	    printf("isqrt(%d)=%d\r\n",k,lb_in_isqrt(k));

	    for (k=1;k<P.items;k++)
	      {
		x=log(k);
		y=log(P.array[k]);
		lb_gr_project_2d(win, x, y, &xp, &yp);
		lb_gr_draw_pixel(NULL,round(xp),round(yp),lb_gr_12RGB(COLOR_BLUE),COPYMODE_COPY);
	      }
	    lb_gr_delay(10000);
	    lb_al_release_vector_si16(&P);
	    lb_fb_exit(1);
#endif

    
	    /*******************************************************************************************************************/
	    /* Numerical Methods */
	    /******************************************************************************************************************/

	    //#define DEMO_RK4
#ifdef DEMO_RK4
	    FLOAT_T t_n, y_rk4, y_euler, xp, yp;
	    VIEWPORT_2D_T win;
	    ERR_T error;

	    lb_fb_open("/dev/fb0", "/dev/tty1", 1, 1, 0*RENDEROPTIONS_LINE | 1*RENDEROPTIONS_GRAPHICS_ONLY);
	    lb_fb_clear(ty_fb_main, 0,0,0,255);
  
	    win.xp_min=0;
	    win.yp_min=0;
	    win.xp_max=ty_width;
	    win.yp_max=ty_height;
	    win.xr_min=0.0;
	    win.xr_max=6.2832*8.0;
	    win.yr_min=-2.5; 
	    win.yr_max=2.5;

	    t_n=0;
	    y_rk4=0;
	    y_euler=0;
	    error=e_none;
      
	    while((t_n<=win.xr_max) && (error==e_none))
	      {
		printf("t_n= %f, y_euler=%f, y_rk4=%f\r\n",t_n,y_euler, y_rk4);
		lb_gr_project_2d(win, t_n, y_rk4, &xp, &yp);
		lb_gr_draw_pixel(NULL, xp, yp, lb_gr_12RGB(COLOR_BLUE), COPYMODE_COPY);
		lb_gr_project_2d(win, t_n, y_euler, &xp, &yp);
		lb_gr_draw_pixel(NULL, xp, yp, lb_gr_12RGB(COLOR_RED), COPYMODE_COPY);
		rk4(&t_n, &y_rk4, 0.1, &error, diego_sin);
		rk4(&t_n, &y_euler, 0.1, &error, diego_sin);
	      }
	    lb_gr_delay(20000);
	    lb_fb_exit(1); 
#endif

	    //#define DEMO_GENERAL_RUNGE_KUTTA
#ifdef DEMO_GENERAL_RUNGE_KUTTA
	    VECTOR_R_T C, B, K;
	    MATRIX_R_T A;
	    S_INT_16_T i, j;
	    FLOAT_T t_n, y_n, h, temp_t, temp_y, y_next, xp, yp;
	    VIEWPORT_2D_T win;

	    lb_fb_open("/dev/fb0", "/dev/tty1", 1, 1, 0*RENDEROPTIONS_LINE | 1*RENDEROPTIONS_GRAPHICS_ONLY);
	    lb_fb_clear(ty_fb_main, 0,0,0,255);

  
	    win.xp_min=0;
	    win.yp_min=0;
	    win.xp_max=ty_width;
	    win.yp_max=ty_height;
	    win.xr_min=-5.0;
	    win.xr_max=5.0;
	    win.yr_min=-3.5; 
	    win.yr_max=3.5;

	    /* This defines the coeficients for RK4 */
	    A.cols=4;
	    A.rows=4;
	    lb_al_create_matrix_r(&A);
	    A.array[0][0]=0.0;   A.array[0][1]=0.0;   A.array[0][2]=0.0; A.array[0][3]=0.0;
	    A.array[1][0]=1.0/2; A.array[1][1]=0.0;   A.array[1][2]=0.0; A.array[1][3]=0.0;
	    A.array[2][0]=0.0;   A.array[2][1]=1.0/2; A.array[2][2]=0.0; A.array[2][3]=0.0;
	    A.array[3][0]=0.0;   A.array[3][1]=0.0;   A.array[3][2]=1.0; A.array[3][3]=0.0;
      
	    B.items=4;
	    lb_al_create_vector_r(&B);
	    B.array[0]=1.0/6;  B.array[1]=1.0/3;  B.array[2]=1.0/3;  B.array[3]=1.0/6;
 
	    C.items=4;
	    lb_al_create_vector_r(&C);
	    C.array[0]=0.0;  C.array[1]=1.0/2;  C.array[2]=1.0/2;  C.array[3]=1.0;

	    K.items=4;
	    lb_al_create_vector_r(&K);
     
    
	    t_n=-3.1416;
	    y_n=1.0;
	    h=0.01;

	    while(t_n<=3.1416)
	      {
		printf("t_n= %f, y_n= %f\r\n",t_n,y_n);
		lb_gr_project_2d(win, t_n, y_n, &xp, &yp);
		lb_gr_draw_pixel(NULL, xp, yp, lb_gr_12RGB(COLOR_BLUE), COPYMODE_COPY);
    	
		for(i=0;i<K.items;i++)
		  {
		    temp_t = t_n + h*C.array[i];
		    temp_y = 0.0;
		    for(j=0;j<=(i-1);j++)
		      {
			//printf("A[%d][%d]*K[%d] = %f * %f = %f\r\n",i,j,j,A.array[i][j],K.array[j],A.array[i][j]*K.array[j]);
			temp_y+=A.array[i][j]*K.array[j];
		      }
		    temp_y*=h;
		    temp_y+=y_n;
		    /* test function f(temp_t, temp_y) */
		    K.array[i]=-sin(temp_t);
		  }

		y_next=0;
		for(i=0;i<A.cols;i++)
		  {
		    //printf("B[%d] * K[%d] = %f, * %f\r\n",i,i,B.array[i],K.array[i]);
		    y_next+=B.array[i]*K.array[i];
		  }
		y_next*=h;
		y_next+=y_n;
		printf("y_next=%f\r\n",y_next);

		y_n=y_next;
		t_n+=h;
	      }
	    lb_gr_delay(4000);
      
	    lb_al_release_matrix_r(&A);
	    lb_al_release_vector_r(&B);
	    lb_al_release_vector_r(&C);
	    lb_fb_exit(1); 
#endif

	    /*******************************************************************************************************************/
	    /* System Functions */
	    /******************************************************************************************************************/
      
	    //#define DEMO_TIME_TEST
#ifdef DEMO_TIME_TEST
	    unsigned long i, max;
	    S_INT_16_T j;
	    clock_t begin, end;

	    max=1000000000;

	    begin=clock();
	    for (i=0;i<max;i++)
	      j=round(12.232);
	    end = clock();
	    printf("time elapsed = %f\n",(double)(end - begin) / CLOCKS_PER_SEC);

	    begin=clock();
	    for (i=0;i<max;i++)
	      j=lround(12.232);
	    end = clock();
	    printf("time elapsed = %f\n",(double)(end - begin) / CLOCKS_PER_SEC);

	    begin=clock();
	    for (i=0;i<max;i++)
	      j=0.5+12.232;
	    end = clock();
	    printf("time elapsed = %f\n",(double)(end - begin) / CLOCKS_PER_SEC);

	    exit(1);
#endif

	    /*******************************************************************************************************************/
	    /* Simulation of Telecoil modulation */
	    /******************************************************************************************************************/

	    //#define DEMO_ZERO_CROSSING
#ifdef DEMO_ZERO_CROSSING
	    VECTOR_C_T signal, frequencies, W;
	    FLOAT_T variance=0.1, RMS_noise, t_min=-0.25/FREQ_1, t_max=1/FREQ_2+1.25/FREQ_1, t, xp, yp;
	    S_INT_32_T n_experiment, errors_count;
	    S_INT_16_T i, k, noise_step;
	    VIEWPORT_2D_T win, win2;
	    FONT_T my_font;

	    lb_fb_open("/dev/fb0", "/dev/tty1", 1, 1, 0*RENDEROPTIONS_LINE | 1*RENDEROPTIONS_GRAPHICS_ONLY);
	    lb_fb_clear(ty_fb_main, 0,0,0,255);

    
	    //FLOAT_T w1=2*M_PI*FREQ_2, w2=2*M_PI*FREQ_1;
  
	    win.xp_min=50;
	    win.xp_max=ty_width-50;
	    win.yp_min=50;
	    win.yp_max=ty_height/2-50;
      
	    win.xr_min= t_min;
	    win.xr_max=  t_max;
	    win.yr_min=  2*sqrt(2);
	    win.yr_max=  -2*sqrt(2);


	    win2.xp_min=50;
	    win2.xp_max=ty_width-50;
	    win2.yp_min=ty_height/2+50;
	    win2.yp_max=ty_height-50;
      
	    win2.xr_min=   4;
	    win2.xr_max=  12;
	    win2.yr_min=  1e-1;
	    win2.yr_max=  1e-5;

	    lg_gr_draw_axis_2d(NULL, win2, &my_font, 3, lb_gr_12RGB(COLOR_WHITE), 1,
			       lb_gr_12RGB(COLOR_GRAY), 1.0,
			       lb_gr_12RGB(COLOR_BLUE), 10,
			       AXIS_DRAW_X | AXIS_DRAW_X_GRID |   
			       AXIS_DRAW_Y_GRID_LOG | AXIS_DRAW_COLORVALUES_Y_1,
			       COPYMODE_COPY, LINEMODE_SOLID); 

	    printf("t_mon = %f, t_max=%f\r\n", t_min, t_max);
      
	    // for(t=t_min;t<t_max;t+=(t_max-t_min)/1000)
	    //{
	    //  lb_gr_project_2d(win, t, f(t,w1,w2), &xp, &yp);
	    //  lb_gr_draw_pixel(&Pic, xp, yp, lb_gr_12RGB(COLOR_BLUE), COPYMODE_COPY);
	    //}


	    //Chart.w=win.xp_max-win.xp_min;
	    //Chart.h=win.yp_max-win.yp_min;
	    //lb_gr_create_picture(&Chart,lb_gr_12RGB(COLOR_BLUE));


	    // printf("x=%f x 10 ^%f\r\n",lb_re_normed_significand(10.01),lb_re_normed_exponent(10.01));
	    // lb_gr_render_picture(&Pic, my_renderer, 0, 0, PIXEL_SIZE_X, PIXEL_SIZE_Y, RENDERMODE_BOX);

            
	    system("clear");
	    printf("Simulation running... please wait... \r\n");
	    srand(time(NULL));

	    signal.items=2*N_HALF_SAMPLES;
	    frequencies.items=2*N_HALF_SAMPLES;
	    W.items=N_HALF_SAMPLES;


	    lb_al_create_vector_c(&signal);
	    lb_al_create_vector_c(&frequencies);
	    lb_al_create_vector_c(&W);
	    lb_fo_calculate_W(&W, 1);

	    for (noise_step=10;noise_step<=NOISE_STEPS_MAX;noise_step++)
	      {
		errors_count=0;
		for (n_experiment=0;n_experiment<N_EXPERIMENT_MAX;n_experiment++)
		  {
		    for(k=0;k<2*N_HALF_SAMPLES;k++)
		      {
			signal.array[k].r=lb_st_marsaglia_polar(variance);
			signal.array[k].i=0.0;
		      }
	    
		    /* Noise using the time's Domain */
		    // printf("RMS Noise (time domain method) = %f\r\n",RMS_noise_time_complex(signal_complex,0,N_SAMPLES));

		    /* Noise using the frequency's Domain */
		    lb_fo_FFT_C(&signal, &frequencies, &W,-1); 
	     
		    // printf("RMS Noise (frequency domain method) = %f\r\n", RMS_noise_frequency(frequencies,N_POINTS));
  
		    /* A perfect Bandpass filter: we're leaving the a fourth of the lower frequencies  */
		    /* Why 12 ?
		       The circuit being simulated is a 1-st order active Low Pass with R=27.4K and C=220p, hence f=26.403KHz
		       The DFT' base period is tmax = 1/5464 + 1/4291 = 4.1606*10^-4 seg   ==> d_freq=2403.5 Hz

		       To fiter using a DTF we need to "keep"the base frequencies:
		       f0  = DC
		       f1  = 24
		       03.5
		       f2  = 4807
		       f3  = 7210
		       f4  = 9614
		       f5  = 12017.5
		       f6  = 24421
		       f7  = 16824.5
		       f8  = 19228
		       f9  = 21631.5
		       f10 = 24035
		       f11 = 26438.5
		       f12 = 0, etc. */
		    for(i=13;i<2*N_HALF_SAMPLES;i++)
		
		      {
			frequencies.array[i].r=0;
			frequencies.array[i].i=0;
		      } 

		    /* Digital filter */
		    lb_fo_FFT_C(&frequencies, &signal, &W, 1); 
	   
		    //RMS_noise=RMS_noise_time_complex(signal_complex,round(0.25*N_SAMPLES/FREQ_1),N_SAMPLES-round(0.25*N_SAMPLES/FREQ_1));
		    //printf("RMS Noise (time domain method) after filtering = %f\r\n",RMS_noise);
		    // printf("RMS Noise (frequency domain method) after filtering = %f\r\n",RMS_noise_frequency(frequencies,N_POINTS));
		    //	  escale_complex_vector(signal_complex,(1.0*(FLOAT_TYPE)noise_step/(FLOAT_TYPE)NOISE_STEPS_MAX)/RMS_noise,N_SAMPLES

		    for(i=0;i<2*N_HALF_SAMPLES;i++)
		      {
			signal.array[i].i=0;
		      }
		    RMS_noise=RMS_noise_time_complex(&signal,round(0.25*2*N_HALF_SAMPLES/FREQ_1),
						     2*N_HALF_SAMPLES-round(0.25*2*N_HALF_SAMPLES/FREQ_1));

 
		    lb_al_multiply_vector_c_real(&signal,(0.5*(FLOAT_T)noise_step/NOISE_STEPS_MAX)/RMS_noise);
	      
		    // printf("RMS Noise (time domain method) *** = %f\r\n",
		    // RMS_noise_time_complex(signal_complex,round(0.25*N_SAMPLES/FREQ_1),N_SAMPLES-round(0.25*N_SAMPLES/FREQ_1)));
		    // printf("RMS Noise (frequency domain method) after filtering = %f\r\n",RMS_noise_frequency(signal_complex,N_SAMPLES));

		    if ((n_experiment % 64) == 0)
		      {
			lb_gr_draw_rectangle_solid(NULL, 0, 0, ty_width, ty_height/2, lb_gr_12RGB(COLOR_DARKSLATEGRAY));

			//lb_gr_draw_rectangle_bar(NULL, 50, 50, 500, 500, 8, lb_gr_12RGB(COLOR_BLUE | COLOR_SEMI_SOLID),
			//			     lb_gr_12RGB(COLOR_RED | COLOR_SEMI_SOLID), COPYMODE_ADD);
	   
			//lb_gr_draw_rectangle_bar(NULL, 70, 70, 512, 512, 8, lb_gr_12RGB(COLOR_PINK | COLOR_SEMI_SOLID),
			//			     lb_gr_12RGB(COLOR_GREEN | COLOR_SEMI_SOLID), COPYMODE_ADD);
	   
			lg_gr_draw_axis_2d(NULL, win, &my_font, 3, lb_gr_12RGB(COLOR_WHITE), 2.5,
					   lb_gr_12RGB(COLOR_GREEN), 1e-3, lb_gr_12RGB(COLOR_ORANGE), 0.5,
					   AXIS_DRAW_X | AXIS_DRAW_X_ARROWS | AXIS_DRAW_X_GRID |
					   AXIS_DRAW_Y | AXIS_DRAW_Y_ARROWS | AXIS_DRAW_Y_GRID,
					   COPYMODE_COPY, LINEMODE_SOLID);
		      }
	
		    for(i=0;i<2*N_HALF_SAMPLES;i++)
		      {
			t=t_min+i*(t_max-t_min)/(2*N_HALF_SAMPLES);
			signal.array[i].r += sqrt(2)*f(t,2*M_PI*FREQ_2,2*M_PI*FREQ_1);
			/* We don have to plot all experiments, just some would be representative */
			if ((n_experiment % 64) == 0)
			  {
			    lb_gr_project_2d(win, t, signal.array[i].r, &xp, &yp);
			    lb_gr_draw_pixel(NULL, xp, yp, lb_gr_12RGB(COLOR_YELLOW), COPYMODE_COPY);
			  }
		      }

		    if (!parse_vector(&signal))
		      errors_count++;
		    printf("Progress: %2.3f %% done E = %li  /  %i \r",
			   100.0*n_experiment/N_EXPERIMENT_MAX, errors_count,N_EXPERIMENT_MAX);
		  }

		if ( errors_count!=0) 
		  {
		    lb_gr_project_2d_x(win2, -10*log10(0.5*(FLOAT_T)noise_step/NOISE_STEPS_MAX), &xp);
		    lb_gr_project_2d_y_log(win2, (FLOAT_T)errors_count/N_EXPERIMENT_MAX, &yp);
		    //lb_gr_draw_pixel(NULL, xp, yp, lb_gr_12RGB(COLOR_WHITE), COPYMODE_COPY);
		    lb_gr_draw_circle_filled_fast(NULL, xp, yp, 4, lb_gr_12RGB(0xFFFF), COPYMODE_COPY);
		  }
	  
		printf("Noise step = %i / %i, SNR = %f, BER = %li / %i\r\n",
		       noise_step,
		       NOISE_STEPS_MAX,
		       -10*log10(0.5*(FLOAT_T)noise_step/NOISE_STEPS_MAX),
		       errors_count, N_EXPERIMENT_MAX);
	      }

	    lb_al_release_vector_c(&signal);
	    lb_al_release_vector_c(&frequencies);
	    lb_al_release_vector_c(&W);
#endif

	    //#define DEMO_KEYBOARD
#ifdef DEMO_KEYBOARD
	    unsigned char c;
	    S_INT_8_T flag;

	    flag=FALSE;
	    lb_co_set_echo(0); 

	    while (!flag)
	      {
		while (!(lb_co_kbhit())) ;
		c=lb_co_getch_pc();
		printf("value= dec=%d hex=%x\r\n",c,c);
		switch(c)
		  {
		  case PCKEY_UP:          printf("PCKEY_UP\r\n");          break;
		  case PCKEY_DOWN:        printf("PCKEY_DOWN\r\n");        break;
		  case PCKEY_RIGHT:       printf("PCKEY_RIGHT\r\n");       break;
		  case PCKEY_LEFT:        printf("PCKEY_LEFT\r\n");        break;
		  case PCKEY_PAGE_UP:     printf("PCKEY_PAGE_UP\r\n");     break;
		  case PCKEY_PAGE_DOWN:   printf("PCKEY_PAGE_DOWN\r\n");   break;
		  case PCKEY_INSERT:      printf("PCKEY_INSERT\r\n");      break;
		  case PCKEY_BS:          printf("PCKEY_BS\r\n");          break;
		  case PCKEY_TAB:         printf("PCKEY_TAB\r\n");         break;
		  case PCKEY_ENTER:       printf("PCKEY_ENTER\r\n");       break;
		  case PCKEY_F1:          printf("PCKEY_F1\r\n");          break;
		  case PCKEY_F2:          printf("PCKEY_F2\r\n");          break;
		  case PCKEY_F3:          printf("PCKEY_F3\r\n");          break;
		  case PCKEY_F4:          printf("PCKEY_F4\r\n");          break;
		  case PCKEY_F5:          printf("PCKEY_F5\r\n");          break;
		  case PCKEY_F6:          printf("PCKEY_F6\r\n");          break;
		  case PCKEY_F7:          printf("PCKEY_F7\r\n");          break;
		  case PCKEY_F8:          printf("PCKEY_F8\r\n");          break;
		  case PCKEY_F9:          printf("PCKEY_F9\r\n");          break;
		  case PCKEY_F10:         printf("PCKEY_F10\r\n");         break;
		  case PCKEY_F11:         printf("PCKEY_F11\r\n");         break;
		  case PCKEY_F12:         printf("PCKEY_F12\r\n");         break;
		  case PCKEY_HOME:        printf("PCKEY_HOME\r\n");        break;
		  case PCKEY_PAUSE:       printf("PCKEY_PAUSE\r\n");       break;
		  case PCKEY_ALT:         printf("PCKEY_ALT\r\n");         break;
		  case PCKEY_SCROLL_LOCK: printf("PCKEY_SCROLL_LOCK\r\n"); break;
		  case PCKEY_ESC:         printf("PCKEY_ESC\r\n");         break;
		  case PCKEY_CAPS_LOCK:   printf("PCKEY_CAPS_LOCK\r\n");   break;
		  case PCKEY_SHIFT:       printf("PCKEY_SHIFT\r\n");       break;
		  case PCKEY_CONTROL:     printf("PCKEY_CONTROL\r\n");     break;
		  case PCKEY_NUM_LOCK:    printf("PCKEY_NUM_LOCK\r\n");    break;
		  case PCKEY_DEL:         printf("PCKEY_DEL\r\n");         break;
		  case PCKEY_END:         printf("PCKEY_END\r\n");         break;
		  case ASCII_NUL:         printf("NULL\r\n");              break;
		  default: printf("%c\r\n",c); break;
		  }
	  
		if (c=='x')
		  flag=TRUE;
	      }
	    exit(1);
#endif

   
	    //#define DEMO_CONSOLE
#ifdef DEMO_CONSOLE
	    lb_co_cls();
	    printf("hello world\r\n"); 
	    lb_co_gotoxy(1,1);
	    printf("1"); 
	    lb_co_gotoxy(2,2);
	    printf("2"); 
	    lb_co_gotoxy(3,3);
	    printf("3"); 

	    lb_co_gotox(2);
	    printf("2");
	    lb_co_gotoxy(10,10);
	    printf("DIEGO VELEZ\r\n"); 
	    lb_co_gotoxy(10+6,10);
	    lb_co_cls_from_cursor();
	    lb_co_gotoxy(20,20);
	    printf("*");
	    lb_co_gotoxy(20,20);
	    lb_co_cursor_shift(-1,0);
	    printf("-\r\n");
	    //lb_co_printf_block("DIEGO ALBERTO VELEZ HENAO", 1, 3);
	    char text[40];

	    strcpy(text,"DON DIEGO VELEZ");
	    lb_co_printf_lastn(text, 5);
	    lb_co_gotoxy(2,2);
	    lb_co_color(TEXT_COLOR_BLUE);
	    lb_co_color(TEXT_COLOR_WHITE + TEXT_COLOR_BACKGROUND);
	    lb_co_color(TEXT_MODE_BOLD);
	    printf("TEXT_COLOR_BLUE\r\n");
	    lb_co_color(TEXT_MODE_NOBOLD);
	    lb_co_gotoxy(3,3);

	    //lb_co_color(TEXT_COLOR_DEFAULT);
	    //lb_co_color(TEXT_COLOR_DEFAULT + TEXT_COLOR_BACKGROUND);
	    lb_co_cls();
	    lb_co_reset();
	    char r, g, b;
	    for (b=0;b<16;b++)
	      for (g=0;g<16;g++) 
		{
		  lb_co_gotox(1);
		  lb_co_color(TEXT_COLOR_WHITE);
		  printf("0x%x",(b<<4)+g);
		  lb_co_gotox(10);
		  for (r=0;r<16;r++)
		    {
		      lb_co_color_bg_RGB(lb_gr_12RGB((b<<8)+(g<<4)+r));
		      lb_co_color_fg_RGB(lb_gr_12RGB((b<<8)+(g<<4)+r));
		      printf("*");
		    }
		  lb_co_color_bg_RGB(lb_gr_12RGB(0x0000));
		  printf("\r\n");
		  lb_gr_delay(1000);
		}
	    printf("TEXT_COLOR_0x00f\r\n");
	    printf("\a");


	    //lb_co_cls();
	    lb_fb_exit(1);
#endif

	    /*******************************************************************************************************************/
	    /* Audio DEMOS */
	    /******************************************************************************************************************/

	    //#define DEMO_DTMF
#ifdef DEMO_DTMF
	    lb_au_SDL_audio_init_DTMF();
	    lb_au_SDL_audio_start();

	    lb_au_freq_DTMF('4', &_lb_au_f0, &_lb_au_f1);
	    SDL_Delay(200);
	    _lb_au_f0=0.0;
	    _lb_au_f1=0.0;
	    SDL_Delay(200);

	    lb_au_freq_DTMF('0', &_lb_au_f0, &_lb_au_f1);
	    SDL_Delay(200);
	    _lb_au_f0=0.0;
	    _lb_au_f1=0.0;
	    SDL_Delay(200);

	    lb_au_freq_DTMF('3', &_lb_au_f0, &_lb_au_f1);
	    SDL_Delay(200);
	    _lb_au_f0=0.0;
	    _lb_au_f1=0.0;
	    SDL_Delay(200);

	    lb_au_freq_DTMF('5', &_lb_au_f0, &_lb_au_f1);
	    SDL_Delay(200);
	    _lb_au_f0=0.0;
	    _lb_au_f1=0.0;
	    SDL_Delay(200);
  
	    lb_au_freq_DTMF('3', &_lb_au_f0, &_lb_au_f1);
	    SDL_Delay(200);
	    _lb_au_f0=0.0;
	    _lb_au_f1=0.0;
	    SDL_Delay(200);

	    lb_au_freq_DTMF('1', &_lb_au_f0, &_lb_au_f1);
	    SDL_Delay(200);
	    _lb_au_f0=0.0;
	    _lb_au_f1=0.0;
	    SDL_Delay(200);

	    lb_au_freq_DTMF('2', &_lb_au_f0, &_lb_au_f1);
	    SDL_Delay(200);
	    _lb_au_f0=0.0;
	    _lb_au_f1=0.0;
	    SDL_Delay(200);

	    lb_au_freq_DTMF('6', &_lb_au_f0, &_lb_au_f1);
	    SDL_Delay(200);
	    _lb_au_f0=0.0;
	    _lb_au_f1=0.0;
	    SDL_Delay(200);

	    lb_au_freq_DTMF('1', &_lb_au_f0, &_lb_au_f1);
	    SDL_Delay(200);
	    _lb_au_f0=0.0;
	    _lb_au_f1=0.0;
	    SDL_Delay(200);

	    lb_au_freq_DTMF('6', &_lb_au_f0, &_lb_au_f1);
	    SDL_Delay(200);
	    _lb_au_f0=0.0;
	    _lb_au_f1=0.0;
	    SDL_Delay(200);


	    _lb_au_f0=0.0;
	    _lb_au_f1=0.0;

	    lb_au_SDL_audio_close_DTMF();

	    lb_fb_exit(1);
#endif

  
#define SAMPLES 22050
	    //#define DEMO_WAV_WRITE
#ifdef DEMO_WAV_WRITE
	    VECTOR_R_T V;
	    U_INT_16_T i;
	    FLOAT_T f1,f2;
  
	    V.items=SAMPLES;
	    lb_al_create_vector_r(&V);

	    lb_au_freq_DTMF('3', &f1, &f2);
	    for (i=0;i<SAMPLES;i++)
	      V.array[i]=0.5*sin(2*M_PI*f1*i/SAMPLES) + 0.5*sin(2*M_PI*f2*i/SAMPLES);
      
	    lb_au_wave_write_or_append_from_vector_r("test2.wav", &V, SAMPLES, 8);

	    lb_al_release_vector_r(&V);
  
	    lb_fb_exit(1);
#endif

	    //#define PLAY_WAVE
#ifdef PLAY_WAVE
	    // Initialize SDL.
	    if (SDL_Init(SDL_INIT_AUDIO) < 0)
	      return 1;

	    // local variables
	    extern Uint32 lb_au_audio_len; /* Extern variable declared in lb_audio.h */
	    extern Uint8 *lb_au_audio_pos; /* Extern variable declared in lb_audio.h */

	    static Uint32 wav_length; // length of our sample
	    static Uint8 *wav_buffer; // buffer containing our audio file
	    static SDL_AudioSpec wav_spec; // the specs of our piece of music
  
	
	    /* Load the WAV */
	    // the specs, length and buffer of our wav are filled
	    if( SDL_LoadWAV("toto.wav", &wav_spec, &wav_buffer, &wav_length) == NULL )
	      {
		printf("Error loading wav file\r\n");
		lb_fb_exit(1);
	      }
	    // set the callback function
	    wav_spec.callback = lb_au_callback_copy;
	    wav_spec.userdata = NULL;
	    // set our global static variables
	    lb_au_audio_pos = wav_buffer; // copy sound buffer
	    lb_au_audio_len = wav_length; // copy file length
	
	    /* Open the audio device */
	    printf("hello\r\n");
	    if ( SDL_OpenAudio(&wav_spec, NULL) < 0 )
	      {
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		lb_fb_exit(-1);
	      }
	
	    /* Start playing */
	    SDL_PauseAudio(0);

	    // wait until we're don't playing
	    while ( lb_au_audio_len > 0 )
	      {
		SDL_Delay(100); 
	      }
	
	    // shut everything down
	    SDL_CloseAudio();
	    SDL_FreeWAV(wav_buffer);
#endif

  
	    //#define DEMO_WAV
#ifdef DEMO_WAV
	    VECTOR_R_T V;
	    U_INT_16_T i;
	    V.items=VECTOR_MAX_ITEMS;
	    lb_al_create_vector_r(&V);

	    for (i=0;i<V.items;i++)
	      V.array[i]=sin(2*M_PI*500.0*i/11025);
	    lb_au_wave_write_from_vector_r("wav2.wav",&V, 11025,8);

	    lb_al_release_vector_r(&V);
	    lb_fb_exit(1);
#endif



	    //#define DEMO_UNICODE
#ifdef DEMO_UNICODE
	    //https://stackoverflow.com/questions/34937375/printing-a-unicode-box-in-c
	    setlocale(LC_ALL, "");
	    //setlocale(LC_ALL, "en_US.UTF-8");
	    unsigned int i;


	    //printf("\n");
	    for (i=0;i<0xffff;i++)
	      {

		// wprintf(L"%lc", (wchar_t)i);  works
		if ((i % 16) == 0)
		  {
		    printf("\n0x%x\t",i);
		    lb_gr_delay(250);
		  }
		printf("%lc", i);
 	
		//sprintf(text,"\\x%x\\x%x\\x%x\r\n" ,(x>>16) & 0xff, (x>>8) & 0xff, x & 0xff);
		//printf(text);
	      }

#endif

	    //#define DEMO_SERIAL
#ifdef DEMO_SERIAL

	    COMM_PORT_T port1;
	    port1.device=dev_S0;
	    port1.baud_rate=B75;
	    port1.data_bits = CS8;
	    port1.stop_bits =SB_1;
	    port1.parity=PA_none;
	    port1.flow_control=FC_none;
	    lb_se_init(&port1);

	    COMM_PORT_T port2;
	    port2.device=dev_S1;
	    port2.baud_rate=B75;
	    port2.data_bits = CS8;
	    port2.stop_bits =SB_1;
	    port2.parity=PA_none;
	    port2.flow_control=FC_none;
	    lb_se_init(&port2);

	    char a=0;
	    while(1)
	      {
		lb_se_tx_byte(&port1, a);
		lb_se_tx_byte(&port2, a);
		lb_se_process_rx(&port1);
		lb_se_process_rx(&port2);
		lb_co_textcolor(TEXT_COLOR_WHITE);
		fflush(stdout);

		lb_se_print_buffer(&port1);
		lb_co_textcolor(TEXT_COLOR_YELLOW);
		fflush(stdout);
		lb_se_print_buffer(&port2);
		a++;
		if (a==128)
		  a=0;
	      }
	    lb_se_close(&port1);
	    lb_se_close(&port2);
#endif

	    //#define DEMO_BATTERY
#ifdef DEMO_BATTERY

	    /******************************************************************************/
	    /*          B A T T E R Y    C H A R G I N G    P R O F I L E S               */
	    /******************************************************************************/
  

	    /* End of Battery Profile Definition ********************************************************************************/
	    char message[90];
	    char Key;

	    fd_set set;
	    struct timeval timeout;
	    int select_result;
	    int FLAG_STOP=0;

	    fprintf(stderr,"\r\nGot here");
  
	    if (argc != 7)
	      {
		printf("Error: invalid number of parameters\r\n");
		printf("device [/dev/ttyS0] baud_rate [1200 2400 9600] data_bits [db8, db7] stop_bits [sb1, sb2] parity [none, even, odd] flow_control [no hw sw] \r\n");
		lb_fb_exit(0);
	      }
  
	    if (!(strcmp(argv[1],"/dev/ttyS0"))) Data_Parameters.device=dev_S0;
	    else if (!(strcmp(argv[1],"/dev/ttyS1"))) Data_Parameters.device=dev_S1;
	    else
	      {
		printf("Error: invalid device");
		lb_fb_exit(0);
	      }
      
	    if (!(strcmp(argv[2],"50")))
	      Data_Parameters.baud_rate=B50;
	    else if (!(strcmp(argv[2],"75")))
	      Data_Parameters.baud_rate=B75;
	    else if (!(strcmp(argv[2],"110")))
	      Data_Parameters.baud_rate=B110;
	    else if (!(strcmp(argv[2],"134")))
	      Data_Parameters.baud_rate=B134;
	    else if (!(strcmp(argv[2],"150")))
	      Data_Parameters.baud_rate=B150;
	    else if (!(strcmp(argv[2],"200")))
	      Data_Parameters.baud_rate=B200;
	    else if (!(strcmp(argv[2],"300")))
	      Data_Parameters.baud_rate=B300;
	    else if (!(strcmp(argv[2],"600")))
	      Data_Parameters.baud_rate=B600;
	    else if (!(strcmp(argv[2],"1200")))
	      Data_Parameters.baud_rate=B1200;
	    else if (!(strcmp(argv[2],"1800")))
	      Data_Parameters.baud_rate=B1800;
	    else if (!(strcmp(argv[2],"2400")))
	      Data_Parameters.baud_rate=B2400;
	    else if (!(strcmp(argv[2],"4800")))
	      Data_Parameters.baud_rate=B4800;
	    else if (!(strcmp(argv[2],"9600")))
	      Data_Parameters.baud_rate=B9600;
	    else if (!(strcmp(argv[2],"19200")))
	      Data_Parameters.baud_rate=B19200;
	    else if (!(strcmp(argv[2],"38400")))
	      Data_Parameters.baud_rate=B38400;
	    else if (!(strcmp(argv[2],"57600")))
	      Data_Parameters.baud_rate=B57600;
	    else if (!(strcmp(argv[2],"115200")))
	      Data_Parameters.baud_rate=B115200;
	    else 
	      {
		printf("Error: invalid baud rate");
		lb_fb_exit(0);
	      }

	    if (!strcmp(argv[3],"db5")) Data_Parameters.data_bits      = CS5;
	    else if (!strcmp(argv[3],"db6")) Data_Parameters.data_bits = CS6;
	    else if (!strcmp(argv[3],"db7")) Data_Parameters.data_bits = CS7;
	    else if (!strcmp(argv[3],"db8")) Data_Parameters.data_bits = CS8;
	    else 
	      {
		printf("Error: invalid data bits");
		lb_fb_exit(0);
	      }

	    if (!strcmp(argv[4],"sb1")) Data_Parameters.stop_bits=SB_1;
	    else if (!strcmp(argv[4],"sb2")) Data_Parameters.stop_bits=SB_2;
	    else 
	      {
		printf("Error: invalid stop bits");
		lb_fb_exit(0);
	      }

	    if (!strcmp(argv[5],"none")) Data_Parameters.parity=PA_none;
	    else if (!strcmp(argv[5],"even")) Data_Parameters.parity=PA_even;
	    else if (!strcmp(argv[5],"odd")) Data_Parameters.parity=PA_odd;
	    else 
	      {
		printf("Error: invalid parity");
		lb_fb_exit(0);
	      }

	    if (!strcmp(argv[6],"no")) Data_Parameters.flow_control=FC_none;
	    else if (!strcmp(argv[6],"hw")) Data_Parameters.flow_control=FC_hw;
	    else if (!strcmp(argv[6],"sw")) Data_Parameters.flow_control=FC_sw;
	    else 
	      {
		printf("Error: invalid flow control");
		lb_fb_exit(0);
	      }

	    text_file=fopen("./charge_data.txt", "w");
	    if (text_file== NULL)
	      { 
		fprintf(stderr,"Error in opening text file..\r\n");
		return 1;
	      }
	    CONSOLE_Initialize();
	    SERCOM_Initialize();
  
	    fprintf(output,"\E[H\E[2J");
	    fflush(output);
  
	    sprintf(message,"BATTERY TEST PROGRAM\r\n");
	    fputs(message,output);   
	    fflush(output);
	    fprintf(stderr,"--------------------------------------------------------------------------------\r\n");

	    gettimeofday(&t_initial, NULL);
	    /* printf("\r\nt_initial = %f",(float)t_initial.tv_sec); */
	    t_previous=0;
	    t_delta=0;
	    SetupPSU(0);

	    while (!FLAG_STOP)
	      {
		/************************************************************************/
		/*  Re-initialize the file descriptor set.                              */
		/************************************************************************/
		FD_ZERO (&set);
		FD_SET (fd, &set);
		FD_SET (tty, &set);
		/* Initialize the timeout data structure. */
		timeout.tv_sec = 0.25;
		timeout.tv_usec = 0;
		select_result=select(sizeof(set)*8,&set,NULL,NULL,&timeout);
		/************************************************************************/

		if (current_stage==N_STAGES)
		  {
		    fprintf(output,"Sequence completed...\r\n");
		    FLAG_STOP=1;
		  }  

		if (CONSOLE_RxByte(&Key))  /* if a key was hit */
		  {
		    CONSOLE_TxByte(Key);
		    SERCOM_TxByte(Key);
		    if (Key==0x1b) 
		      FLAG_STOP=1;
		  }

		if (FLAG_STOP)
		  {
		    SERCOM_TxStr("OUTP:STAT 0\r\n");
		    SERCOM_Flush();
		    SERCOM_Close();
		    CONSOLE_Close();
		    fclose(text_file);
		    lb_fb_exit(0);
		  }
		RunProcess();
	      }
#endif

  
}

