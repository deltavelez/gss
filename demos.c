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
#include "lb_gpio.h"
#include "lb_time.h" 

  
int main(int argc, char *argv[])
{
  /* Time tests */

  /******************************************************************************/
  /* Demo: Multi_dimensional Matrices.                                          */
  /* This shows the creation, printing, and release of multi-dimentional        */
  /* matrices.  A memory-leak test can also be performed, which can also show   */
  /* such a test could be applied to other cases.                               */
  /******************************************************************************/
  
  //#define MATRIX_NDIM
#ifdef MATRIX_NDIM
  	  
  ARRAY_R_T M;
  M.n=4;
  M.dim[0]=4;
  M.dim[1]=3;
  M.dim[2]=3;
  M.dim[3]=2;
  M.dim[4]=2;
  UINT16_T i,j,k,l;
  long cummulative;
  SINT8_T flag;
  struct sysinfo info_1, info_2, info_3;

  lb_al_create_array_r(&M);

  switch (M.n)
    {
    case 1:
      for (j=0;j<M.dim[0];j++)
	M.a1[j]=((REAL_T)rand()/RAND_MAX-0.5)*pow(10,10*(REAL_T)rand()/RAND_MAX);
      break;
    case 2:
      for (i=0;i<M.dim[1];i++)
	for (j=0;j<M.dim[0];j++)
	  M.a2[i][j]=((REAL_T)rand()/RAND_MAX-0.5)*pow(10,10*(REAL_T)rand()/RAND_MAX);
      break;
    case 3:
      for (k=0;k<M.dim[2];k++)
	for (i=0;i<M.dim[1];i++)
	  for (j=0;j<M.dim[0];j++)
	    M.a3[k][i][j]=((REAL_T)rand()/RAND_MAX-0.5)*pow(10,10*(REAL_T)rand()/RAND_MAX);
      break;
    case 4:
      for (l=0;l<M.dim[3];l++)
	for (k=0;k<M.dim[2];k++)
	  for (i=0;i<M.dim[1];i++)
	    for (j=0;j<M.dim[0];j++)
	      M.a4[l][k][i][j]=((REAL_T)rand()/RAND_MAX-0.5)*pow(10,10*(REAL_T)rand()/RAND_MAX);
      break;
    }
  lb_al_print_array_r(&M, "",12,2 );
  lb_al_release_array_r(&M);
  

  cummulative=0;
  /* Checking for memory leaks */

  if (1) for (l=0;l<10000;l++)
	   {
	     //printf("size=%d\r\n",sizeof(REAL_T));

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
  
  /******************************************************************************/
  /* General Computer Science Demos                                             */
  /******************************************************************************/

  /******************************************************************************/
  /* Demo: A two-dimentional circular Buffer.                                   */
  /* Shows the fundamentals for building a text console.                        */
  /* to_do: to implement an 'insert' feature.                                   */
  /******************************************************************************/

  //#define DEMO_2D_CIRCULAR_BUFFER_TEXT
#ifdef DEMO_2D_CIRCULAR_BUFFER_TEXT
#define MAX_COLS 4
#define MAX_ROWS 12
#define MAX_ROWS_V 4
#define MAX_N 10

  SINT16_T n_row, w_row, w_col, s_col, flag_insert, n_back, n_back_adj, j, n_vis_rows;
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
	while (!lb_co_kbhit()) ;
	c=lb_co_getch_pc();
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
	    SINT16_T temp_cursor, temp_row;
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
  
  /******************************************************************************/
  /* Demo: Exploring paralelism                                                 */
  /* Shows the basics of multi-threading processing.                             */
  /******************************************************************************/

  //#define DEMO_THREADS
#ifdef DEMO_THREADS
  
  pthread_t threads[4];
  int rc;
  double l0, l1, l2, l3;

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
	x+=1e-6;
      }
    end = clock();
    printf("sum=%0.4e  ",sum);
    printf("time elapsed = %.4e\n",(double)(end - begin) / CLOCKS_PER_SEC);
    /* Check CPU usage: 
       mpstat -P ALL 1
    */
    return 0;
  }

  printf("Starting...\r\n");
  l3=100;
  l2=200;
  l1=400;
  l0=800;
  rc=pthread_create(&threads[0],NULL, floatsum, (void *) &l0);
  rc=pthread_create(&threads[1],NULL, floatsum, (void *) &l1);
  rc=pthread_create(&threads[2],NULL, floatsum, (void *) &l2);
  rc=pthread_create(&threads[3],NULL, floatsum, (void *) &l3);

  rc=pthread_join(threads[0],NULL);;
  rc=pthread_join(threads[1],NULL);;
  rc=pthread_join(threads[2],NULL);;
  rc=pthread_join(threads[3],NULL);;
  
#endif


  /******************************************************************************/
  /* Demo: Exploring paralelism - part 2                                        */
  /* We now perform an actual useful task, such as adding up n-numbers          */
  /* dividing the work between N_THREADS, topping the processor                 */
  /******************************************************************************/

  //#define DEMO_THREADS2
#ifdef DEMO_THREADS2
  #define N_THREADS 8
#define VECTOR_SIZE 1024

  SINT16_T Vec[VECTOR_SIZE];
  int i;

  typedef struct 
  {
    SINT16_T a, b;
  } ARGS_T;

  pthread_t threads[N_THREADS];
  ARGS_T arguments[N_THREADS];
  
  for (i=0;i<VECTOR_SIZE;i++)
    Vec[i]=i;

  void *int_sum(void *args)
  {
    /* It is simpler to first de-reference the arguments from the structrure, then use them */

    SINT16_T _a, _b, k;
    REAL_T total=0;
    _a=(*(ARGS_T *)args).a;
    _b=(*(ARGS_T *)args).b;
    
    //printf("a = %d\r\n",_a);
    //printf("b = %d\r\n",_b);
    for (k=_a;k<=_b;k++)
      {
    	total+=Vec[k];
	//printf("*");
	//fflush(stdout);
    	lb_ti_delay_ms(1000);
    }
    printf("Total = %f, theoretical = %f\r\n",total,0.5*_b*(_b+1)-0.5*(_a-1)*_a);
    fflush(stdout);
    return 0;
  }
    
  for (i=0;i<N_THREADS;i++)
  {
     arguments[i].a=i*VECTOR_SIZE/N_THREADS;
     arguments[i].b=(i+1)*VECTOR_SIZE/N_THREADS-1;
     printf("a= %d, b=%d\r\n",arguments[i].a,arguments[i].b);
     pthread_create(&threads[i],NULL, int_sum, (void *) &arguments[i]);
  }
 
  for (i=0;i<N_THREADS;i++)
    pthread_join(threads[i],NULL);

  printf("Ended\r\n");
#endif

  
  /******************************************************************************/
  /* Vectors and Matrices.                                                      */
  /******************************************************************************/

  /******************************************************************************/
  /* Demo: Creation, swap, and printing of Vectors.                             */
  /******************************************************************************/

  //#define DEMO_SWAP_VECTOR
#ifdef DEMO_SWAP_VECTOR
  VECTOR_R_T A, B;
  UINT16_T i;
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

  /******************************************************************************/
  /* Demo: Insertion of elements in a Vector.                                   */
  /******************************************************************************/

  //#define DEMO_INSERT_VECTOR
#ifdef DEMO_INSERT_VECTOR
  VECTOR_R_T V;
  UINT16_T k;
  V.items=0;
 
  for (k=11;k<=22;k++)
    {
      lb_al_insert_item_vector_r(&V, k,0);
      lb_al_print_vector_r(&V, "V", "%0.4f\r\n");
    }
  lb_al_release_vector_r(&V);
  exit(1);
#endif

  /******************************************************************************/
  /* Demo: Deletion of elements in a Vector.                                    */
  /******************************************************************************/

  //#define DEMO_DELETE_ITEM_VECTOR
#ifdef DEMO_DELETE_ITEM_VECTOR
  VECTOR_R_T V;
  UINT16_T i, k;
     
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

  
  /******************************************************************************/
  /* Demo: Bi-inear Interpolation of data contained within a Matrix.            */
  /* to_do: A similar function could be created using non-linear techniques.    */
  /******************************************************************************/

  //  #define DEMO_INTERPOLATE
#ifdef DEMO_INTERPOLATE
  MATRIX_R_T A;
  SINT16_T i,j;
  REAL_T Q;
      
  A.rows=4; /* vertical */
  A.cols=6; /* horizontal */
  lb_al_create_matrix_r(&A);

  for (i=0;i<A.rows;i++)
    for (j=0;j<A.cols;j++)
      A.array[i][j]=10*i+1000.0*j;
  lb_al_print_matrix_r(&A,"A",FLOAT_FORMAT_MATRIX);

  lb_al_inter_matrix(&A, 2.001, 4.999, &Q);
  //lb_al_inter_matrix(&A, 3.0, 5.0, &Q);

  printf("Q= %f\r\n",Q);

  lb_al_release_matrix_r(&A);
  lb_ti_delay_ms(1000);
  exit(1);
#endif

  /******************************************************************************/
  /* Demo: Determinants.                                                        */
  /******************************************************************************/

  //#define DEMO_DETERMINANT
#ifdef DEMO_DETERMINANT
  MATRIX_R_T A;
  REAL_T det;
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

  /******************************************************************************/
  /* Demo: Matrix 3x3 multiplication and determinants.                          */
  /* The "classic" c-style for defining matrices is difficult to be beat-up     */
  /* when it comes to performance.  This example is just a brush-up.            */
  /******************************************************************************/

  //#define DEMO_MATRIX_33
#ifdef  DEMO_MATRIX_33
  REAL_T M[3][3],  G[3][3], x;
  
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

  /******************************************************************************/
  /* Demo: Matrix 2x2 determinants.                                             */
  /******************************************************************************/

  //#define DEMO_MATRIX_22
#ifdef  DEMO_MATRIX_22
  REAL_T M1[2][2], x;
  
  M1[0][0] = 3.14;  M1[0][1] = 4;
  M1[1][0] = -6;    M1[1][1] = 0.2;
  x=lb_al_determinant_matrix22_r(M1);
  printf("Det2x2 = %f\r\n",x);
  
  exit(1);
#endif


  /******************************************************************************/
  /* Demo: Virtual Consoles                                                     */
  /* This is the foundation on how to build your own 'console'                  */
  /* to_do: it would be nice to implement 'insertion', fast jumps, etc.         */
  /******************************************************************************/
   
  //#define DEMO_VIRTUAL_CONSOLE
#ifdef DEMO_VIRTUAL_CONSOLE
  char SDL_Keysym_to_char(SDL_Keysym ks)
  {
    char c;
    switch(ks.sym)
      {
      case SDLK_UP:
	c=PCKEY_UP;
	break;
      case SDLK_DOWN:
	c=PCKEY_DOWN;
	break;
      case SDLK_RIGHT:
	c=PCKEY_RIGHT;
	break;
      case SDLK_LEFT:
	c=PCKEY_LEFT;
	break;
      case SDLK_PAGEUP:
	c=PCKEY_PAGE_UP;
	break;
      case SDLK_PAGEDOWN:
	c=PCKEY_PAGE_DOWN;
	break;
      case SDLK_INSERT:
	c=PCKEY_INSERT;
	break;
      case SDLK_BACKSPACE:
	c=PCKEY_BS;
	break;
      case SDLK_TAB:
	c=PCKEY_TAB;
	break;
      case SDLK_RETURN:
	c=PCKEY_ENTER;
	break;
      case SDLK_F1:
	c=PCKEY_F1;
	break;
      case SDLK_F2:
	c=PCKEY_F2;
	break;
      case SDLK_F3:
	c=PCKEY_F3;
	break;
      case SDLK_F4:
	c=PCKEY_F4;
	break;
      case SDLK_F5:
	c=PCKEY_F5;
	break;
      case SDLK_F6:
	c=PCKEY_F6;
	break;
      case SDLK_F7:
	c=PCKEY_F7;
	break;
      case SDLK_F8:
	c=PCKEY_F8;
	break;
      case SDLK_F9:
	c=PCKEY_F9;
	break;
      case SDLK_F10:
	c=PCKEY_F10;
	break;
      case SDLK_F11:
	c=PCKEY_F11;
	break;
      case SDLK_F12:
	c=PCKEY_F12;
	break;
      case SDLK_HOME:
	c=PCKEY_HOME;
	break;
      case SDLK_PAUSE:
	c=PCKEY_PAUSE;
	break;
      case SDLK_LALT:
      case SDLK_RALT:
	c=PCKEY_ALT;
	break;
      case SDLK_SCROLLLOCK:
	c=PCKEY_SCROLL_LOCK;
	break;
      case SDLK_ESCAPE:
	c=PCKEY_ESC;
	break;
      case SDLK_CAPSLOCK:
	c=PCKEY_CAPS_LOCK;
	break;
      case SDLK_LSHIFT:
      case SDLK_RSHIFT:
	c=PCKEY_SHIFT;
	break;
      case SDLK_LCTRL:
      case SDLK_RCTRL:
	c=PCKEY_CONTROL;
	break;
      case SDLK_NUMLOCKCLEAR:
	c=PCKEY_NUM_LOCK;
	break;
      case SDLK_DELETE:
	c=PCKEY_DEL;
	break;
      case SDLK_END:
	c=PCKEY_END;
	break;
      default:
	c = 0;
	break;
      }
    return c;
  }
  
  SDL_Event event;
  PICTURE_T Pic_console;
  FONT_T font_console;
  CONSOLE_T Con;
  char c;

  lb_gr_SDL_init("DEMO_VIRTUAL_CONSOLE", SDL_INIT_VIDEO, 800, 600, 0, 0, 0);
  
  Pic_console.w=ty_screen.w;
  Pic_console.h=ty_screen.h;
  lb_gr_create_picture(&Pic_console,lb_gr_12RGB(COLOR_BLUE | COLOR_SOLID));

  lb_ft_load_GLCDfont("./fonts/Font_hp48G_large.lcd", &font_console);
  font_console.scale_x=2;
  font_console.scale_y=2;
  font_console.gap_x=2;
  font_console.gap_y=2;
  font_console.max_x=10;
  font_console.angle=0;
  font_console.flag_fg=TRUE;
  font_console.flag_bg=TRUE;
  font_console.color_fg=lb_gr_12RGB(COLOR_WHITE | COLOR_SOLID);
  font_console.color_bg=lb_gr_12RGB(COLOR_BLUE | COLOR_SOLID);
  lb_ft_resize_console(&Pic_console, &font_console, &Con);
  printf("Hello\r\n");
  
  Con.color_fg=lb_gr_12RGB(COLOR_WHITE | COLOR_SOLID);
  Con.color_bg=lb_gr_12RGB(COLOR_BLUE | COLOR_SOLID);
  lb_ft_create_console(&Con);

  SDL_StartTextInput();
  UINT8_T flag_console_changed=FALSE;
  while (1)
    while (SDL_PollEvent(&event))
      {
	switch (event.type)
	  {
	  case SDL_TEXTINPUT:
	    printf("\r\nSDL_TEXTINPUT: %s",event.text.text);
	    c=event.text.text[0];
	    lb_ft_printc(&Con,c);
	    flag_console_changed=TRUE;
	    break; 
	  case SDL_QUIT:
	    lb_ft_release_console(&Con);
	    lb_gr_release_picture(&Pic_console);
	    SDL_Quit();
	    return EXIT_SUCCESS;
	  case SDL_KEYUP:
	    c=SDL_Keysym_to_char(event.key.keysym);
	    if ( (c==PCKEY_UP) || (c== PCKEY_DOWN) || (c==PCKEY_LEFT) || (c==PCKEY_RIGHT) || (c==PCKEY_INSERT) ||
		 (c==PCKEY_BS) || (c==PCKEY_ENTER) )
	      {
		printf("\r\nSDL_KEYUP: %x",c);
		lb_ft_printc(&Con,c);
		flag_console_changed=TRUE;
	      }
	    break;
	  }
	fflush(stdout);
	if (flag_console_changed)
	  {
	    flag_console_changed=FALSE;
	    
	    lb_ft_draw_console(&Pic_console, &font_console, &Con, COPYMODE_COPY);
	    lb_gr_render_picture(&Pic_console, &ty_screen, 0, 0, COPYMODE_COPY,
				 RENDERMODE_BGCOLOR(0x0Fabc) | RENDERMODE_PIXELMODE_1 | RENDERMODE_SCALE_X(1) |  RENDERMODE_SCALE_Y(1));
	    lb_gr_refresh(&ty_screen);
	  }
      }
  lb_gr_SDL_close();
#endif  	


  /******************************************************************************/
  /* Demo: Graphic Primitves                                                    */
  /******************************************************************************/


  /******************************************************************************/
  /* Demo: Pixels                                                               */
  /* Shows: graphics initialization, pixel manupulation, horizontal lines,      */
  /* rectangles, screen refreshing, and benchmarking.                           */
  /******************************************************************************/

  //#define DEMO_PIXEL
#ifdef DEMO_PIXEL
  SDL_Event event;
  SINT16_T x, y, z;
  clock_t begin, end;
  REAL_T time_total=0;
  int frames_count=0;
  PIXEL_T color;
  
  lb_gr_SDL_init("DEMO_PIXEL", SDL_INIT_VIDEO, 800, 600, 22, 33, 55);
 
  printf("Speed comparison"); 

  time_total=0;

  for(y=0;y<ty_screen.h;y++)
    {
      lb_gr_fb_line_h(&ty_screen, y, 0, ty_screen.h, y % 255, 0, 0);
      lb_gr_refresh(&ty_screen);
    }
 
  lb_ti_delay_ms(4000);

  for(z=0;z<2055;z++)
    {
      lb_gr_draw_rectangle_solid(NULL, rand() % ty_screen.w, rand() % ty_screen.h,
				 rand() % ty_screen.h, rand() % ty_screen.h,
				 lb_gr_12RGB(rand() % 0xFFFF));
      lb_gr_refresh(&ty_screen);
    }
  lb_ti_delay_ms(4000);
  
  for(z=0;z<255;z++) 
    {
      begin=clock();
      for(y=0;y<ty_screen.h;y++)
	for(x=0;x<ty_screen.w;x++)
	  {
	    color.r= x % MAX_R;
	    color.g= y % MAX_G;
	    color.b= z % MAX_B;
	    //printf("color.r= %d, color.g =%d, color.b = %d\r\n",color.r,color.g, color.b); 
	    lb_gr_draw_pixel(NULL, x, y, color, COPYMODE_COPY);
	    //lb_gr_draw_pixel_fast(x, y,  255, 155, 55);
	  }
      lb_gr_refresh(&ty_screen);
      end=clock();
      time_total+=(REAL_T)end-(REAL_T)begin;
      frames_count++;
      //printf("FPS = %f\n",(double)CLOCKS_PER_SEC*frames_count/time_total);
    }
  SDL_Quit();
  return EXIT_SUCCESS;
  
  while (1)
    {
      if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
	break;
    }
  lb_gr_SDL_close();
#endif

  /******************************************************************************/
  /* Demo: Line1                                                                */
  /* Shows: graphics initialization, pixel manupulation, horizontal lines,      */
  /* rectangles, screen refreshing, and benchmarking.                           */
  /******************************************************************************/

  //#define DEMO_LINE1
#ifdef DEMO_LINE1
  SDL_Event event;
  SINT16_T z;
  clock_t begin, end;
 
  lb_gr_SDL_init("DEMO_LINE1", SDL_INIT_VIDEO, 800, 600, 22, 33, 55);
  
  for(z=0;z<1000;z++) 
    {
      begin=clock();
      {
	lb_gr_draw_line1(NULL, ty_screen.w/2, ty_screen.h/2,
			 rand() % ty_screen.w, rand() % ty_screen.h,lb_gr_12RGB(COLOR_BLUE | COLOR_SOLID),COPYMODE_COPY);
      }
      lb_gr_refresh(&ty_screen);
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
  lb_gr_SDL_close();
#endif

  /******************************************************************************/
  /* Demo: Line2                                                                */
  /* Shows: Simple test using 2-pixel width lines                               */
  /******************************************************************************/
  
  //#define DEMO_LINE2
#ifdef DEMO_LINE2
  SDL_Event event;
  int i;

  lb_gr_SDL_init("DEMO_LINE2", SDL_INIT_VIDEO, 800, 600, 220, 120, 155);

  for (i=0;i<3000; i++)
    {
      lb_gr_draw_line2(NULL, rand() % ty_screen.w, rand() % ty_screen.h,
		       rand() % ty_screen.w, rand() % ty_screen.h,
		       lb_gr_12RGB(rand() % 0x0FFF), COPYMODE_COPY);
      lb_gr_refresh(&ty_screen);
    }
  while (1)
    {
      if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
	break;
    }
  lb_gr_SDL_close();
#endif

  /******************************************************************************/
  /* Demo: Line3                                                                */
  /* Shows: Simple test using 3-pixel width lines                               */
  /******************************************************************************/

  //#define DEMO_LINE3
#ifdef DEMO_LINE3
  SDL_Event event;
  int i;

  lb_gr_SDL_init("DEMO_LINE3", SDL_INIT_VIDEO, 800, 600, 220, 200, 255);

  for (i=0;i<1000; i++)
    {
      lb_gr_draw_line3(NULL, rand() % ty_screen.w, rand() % ty_screen.h,
		       rand() % ty_screen.w, rand() % ty_screen.h,
		       lb_gr_12RGB(rand() % 0x0FFF), COPYMODE_COPY);
      lb_gr_refresh(&ty_screen);
    }
  while (1)
    {
      if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
	break;
    }
  lb_gr_SDL_close();
#endif

  /******************************************************************************/
  /* Demo: Line1_float                                                          */
  /* Shows: Simple test plotting a line using a floating point method           */
  /******************************************************************************/

  //#define DEMO_LINE1_FLOAT
#ifdef DEMO_LINE1_FLOAT
  SDL_Event event;
  SINT16_T i;

  lb_gr_SDL_init("DEMO_LINE1_FLOAT", SDL_INIT_VIDEO, 800, 600, 20, 255, 255);
  lb_gr_clear_picture(NULL , lb_gr_12RGB(0x333 | COLOR_SOLID));
  for(i=0;i<1000;i++)
    {
      lb_gr_draw_line1(NULL, ty_screen.w/2, ty_screen.h/2, rand() % ty_screen.w , rand() % ty_screen.h,
		       lb_gr_12RGB(rand() % 0xFFF |0xF000),COPYMODE_COPY);
      lb_gr_refresh(&ty_screen);
    }

  while (1)
    {
      if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
	break;
    }
  lb_gr_SDL_close();
#endif


  /******************************************************************************/
  /* Demo: Line_antialiasing                                                    */
  /* Shows the use and advantages of the Antialiasing primitives.        */
  /******************************************************************************/

  //#define DEMO_LINE_ANTIALIASING
#ifdef DEMO_LINE_ANTIALIASING
  SDL_Event event;
  REAL_T angle;
  PICTURE_T Pic;
  UINT8_T pix_x=32, pix_y=32, flag_running=TRUE;
  
  lb_gr_SDL_init("DEMO_LINE_ANTIALIASING", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0,0,0);
  Pic.w= ty_screen.w/pix_x;
  Pic.h= ty_screen.h/pix_y;

  lb_gr_create_picture(&Pic,lb_gr_12RGB(0x0000) );

  angle=0.0;
  while (flag_running)
    {
      printf("angle=%f\r\n",angle*180.0/M_PI);
      lb_gr_draw_rectangle(&Pic,0,0,Pic.w,Pic.h,lb_gr_12RGB(0xFFFF),COPYMODE_COPY);
      lb_gr_draw_line_antialiasing(&Pic, 0.5*Pic.w, 0.5*Pic.h,
				   0.5*Pic.w  + 0.5*Pic.h*cos(angle),
				   0.5*Pic.h  - 0.5*Pic.h*sin(angle), 
				   1, lb_gr_12RGB(0xF090));
      lb_gr_render_picture(&Pic, &ty_screen, 0, 0, COPYMODE_COPY, RENDERMODE_PIXELMODE_1 | RENDERMODE_SCALE_X(pix_x) |  RENDERMODE_SCALE_Y(pix_y));
      lb_gr_refresh(&ty_screen);

      angle+=M_PI/180;
      lb_ti_delay_ms(100);

      while (SDL_PollEvent(&event))
	if (event.type == SDL_QUIT)
	  flag_running=FALSE;
    }
  lb_gr_BMPfile_save("line.bmp", &Pic);
  lb_gr_release_picture(&Pic);
  lb_gr_SDL_close();
#endif


  /******************************************************************************/
  /* Demo: Line_antialiasing                                                    */
  /* Shows a variation of the Antialiasing primitives to draw lines with a      */
  /* two or three pixel width.                                                  */
  /******************************************************************************/

  //#define DEMO_LINE_ANTIALIASING2
#ifdef DEMO_LINE_ANTIALIASING2
  SDL_Event event;
  REAL_T angle;
  PICTURE_T Pic;
  UINT8_T pix_x=32, pix_y=32, flag_running=TRUE;
  
  lb_gr_SDL_init("DEMO_LINE_ANTIALIASING2", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0,0,0);
  Pic.w= ty_screen.w/pix_x;
  Pic.h= ty_screen.h/pix_y;

  lb_gr_create_picture(&Pic,lb_gr_12RGB(0x0000) );

  angle=0.0;
  while (flag_running)
    {
      printf("angle=%f\r\n",angle*180.0/M_PI);
      lb_gr_draw_rectangle(&Pic,0,0,Pic.w,Pic.h,lb_gr_12RGB(0xFFFF),COPYMODE_COPY);
      lb_gr_draw_line_antialiasing3(&Pic, 0.5*Pic.w, 0.5*Pic.h,
				    0.5*Pic.w  + 0.5*Pic.h*cos(angle),
				    0.5*Pic.h  - 0.5*Pic.h*sin(angle), 
				    lb_gr_12RGB(0xF900));
      //lb_gr_draw_line_antialiasing2(&Pic, 0.5*Pic.w, 0.5*Pic.h,
      //				    0.5*Pic.w  + 0.5*Pic.h*cos(angle),
      //			    0.5*Pic.h  - 0.5*Pic.h*sin(angle), 
      //			    lb_gr_12RGB(0xF900));
      lb_gr_render_picture(&Pic, &ty_screen, 0, 0, COPYMODE_COPY, RENDERMODE_PIXELMODE_1 | RENDERMODE_SCALE_X(pix_x) |  RENDERMODE_SCALE_Y(pix_y));
      lb_gr_refresh(&ty_screen);

      angle+=M_PI/180;
      lb_ti_delay_ms(500);


      while (SDL_PollEvent(&event))
	{
	  if (event.type == SDL_QUIT)
	    flag_running=FALSE;
	}
    }
  lb_gr_BMPfile_save("line.bmp", &Pic);
  lb_gr_release_picture(&Pic);
  lb_gr_SDL_close();
#endif


  /******************************************************************************/
  /* Demo: Line_general                                                         */
  /* lb_gr_draw_line() is a wrapper which call a number of line functions,      */
  /* according to the parameters being passed                                   */
  /* to_do:  the blending (add/or/xor, etc.) seems not to work anymore for      */
  /* line widths larger than 3                                                  */
  /******************************************************************************/
  
  //#define DEMO_LINE_GENERAL
#ifdef DEMO_LINE_GENERAL
  SDL_Event event;
  
  lb_gr_SDL_init("DEMO_LINE_GENERAL", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0,0,0);
      
  lb_gr_draw_line(NULL, 0, 0, ty_screen.w, ty_screen.h, 30,
		  lb_gr_12RGB(COLOR_BLUE | 0*COLOR_SOLID), COPYMODE_COPY, LINEMODE_FILTERED);

  lb_gr_draw_line(NULL, ty_screen.w, 0,  0, ty_screen.h, 50,
		  lb_gr_12RGB(COLOR_RED | 0*COLOR_SOLID), COPYMODE_ADD, LINEMODE_FILTERED);
	
  lb_gr_draw_line(NULL, ty_screen.w/2,0, ty_screen.h/2, ty_screen.h, 40,
		  lb_gr_12RGB(COLOR_GREEN | 0*COLOR_SOLID), COPYMODE_XOR, LINEMODE_FILTERED);

  lb_gr_refresh(&ty_screen);

  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
	    return EXIT_SUCCESS;
	  }
      }
#endif


  /******************************************************************************/
  /* Demo: Circle                                                               */
  /* Just a simple, 1-pixel width circle.                                       */
  /******************************************************************************/

  //#define DEMO_CIRCLE
#ifdef DEMO_CIRCLE
  SDL_Event event;
  lb_gr_SDL_init("DEMO_CIRCLE", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0,0,0);
  lb_gr_clear_picture(NULL, lb_gr_12RGB(0x333 | COLOR_SOLID));
  lb_gr_draw_circle(NULL, ty_screen.w/2, ty_screen.h/2, ty_screen.h/2-1, lb_gr_12RGB(0x100F), COPYMODE_COPY);
  lb_gr_refresh(&ty_screen);
  
  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
	    return EXIT_SUCCESS;
	  }
      }
#endif

  
  /******************************************************************************/
  /* Demo: Circle                                                               */
  /* Just a simple, 1-pixel width circle with antialiasing.                     */
  /******************************************************************************/

  //#define DEMO_CIRCLE_ANTIALIASING_SIMPLE
#ifdef DEMO_CIRCLE_ANTIALIASING_SIMPLE
  SDL_Event event;
  lb_gr_SDL_init("DEMO_CIRCLE_ANTIALIASING_SIMPLE", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0,0,0);

  lb_gr_clear_picture(NULL, lb_gr_12RGB(0xFFFF | COLOR_SOLID));
  lb_gr_draw_circle_antialiasing_simple(NULL, ty_screen.w/2, ty_screen.h/2, 0.35*ty_screen.h,20,
					lb_gr_12RGB(0x1F00));

  lb_gr_BMPfile_save("circle.bmp", NULL);

  lb_gr_refresh(&ty_screen);

  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
	    return EXIT_SUCCESS;
	  }
      }
#endif

  /******************************************************************************/
  /* Demo: Circle 2, 3                                                          */
  /* Just a simple, 1-pixel width circle with antialiasing.                     */
  /******************************************************************************/

  //#define DEMO_CIRCLE_ANTIALIASING23
#ifdef DEMO_CIRCLE_ANTIALIASING23
  SDL_Event event;
  PICTURE_T Pic;
  UINT8_T pix_x=20, pix_y=20;
  
  lb_gr_SDL_init("DEMO_CIRCLE_ANTIALIASING23", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0,0,0);
  Pic.w= ty_screen.w/pix_x;
  Pic.h= ty_screen.h/pix_y;

  lb_gr_create_picture(&Pic,lb_gr_12RGB(0xFFFF));
  lb_gr_draw_circle_antialiasing3(&Pic, ty_screen.w/(2*pix_x), ty_screen.h/(2*pix_y), ty_screen.h/(3*pix_y), lb_gr_12RGB(0xFF00));
  lb_gr_render_picture(&Pic, &ty_screen, 0, 0, COPYMODE_COPY, RENDERMODE_SCALE_X(pix_x) |  RENDERMODE_SCALE_Y(pix_y) | RENDERMODE_PIXELMODE_1);
  
  lb_gr_refresh(&ty_screen);
  lb_ti_delay_ms(1000);
  lb_gr_BMPfile_save("circle2.bmp", NULL);

  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
	    return EXIT_SUCCESS;
	  }
      }
#endif

  /******************************************************************************/
  /* Demo: Circle with antialiasing.                                            */
  /* A circle with an arbitrary with and antialiasing.                          */
  /* to_do:  when trying to render a picture with a NULL parameter, an error    */
  /* should be reported.    */
  /******************************************************************************/

  //#define DEMO_CIRCLE_ANTIALIASING 
#ifdef DEMO_CIRCLE_ANTIALIASING
  SDL_Event event;
  int pix_x=10, pix_y=10;
  lb_gr_SDL_init("DEMO_CIRCLE_ANTIALIASING", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0, 0, 0);
  //      lb_gr_fb_rectangle_copymode(&ty_screen, 0, 0, ty_screen.w, ty_screen.h, 0xff, 0xff, 0xff, 0xff, 0);
  lb_gr_fb_rectangle_copymode(&ty_screen, 0, 0, ty_screen.w, ty_screen.h, 0, 0, 0, 0xff, 0);

  lb_gr_draw_circle_antialiasing(NULL, ty_screen.w/(2*pix_x),   ty_screen.h/(2*pix_y), 0.25*ty_screen.w/pix_x, 10, lb_gr_12RGB(0xfF00));
  lb_gr_draw_circle_antialiasing(NULL, ty_screen.w/(3*pix_x),   ty_screen.h/(3*pix_y), 0.15*ty_screen.h/pix_y, 10, lb_gr_12RGB(0xf0f0)); 
  lb_gr_draw_circle_antialiasing(NULL, ty_screen.w*2/(3*pix_x), ty_screen.h/(3*pix_y), 0.15*ty_screen.h/pix_y, 10, lb_gr_12RGB(0xf00F));
  
  // --> This should be improved:
  //     lb_gr_render_picture(NULL, &ty_screen, 0, 0, COPYMODE_COPY, RENDERMODE_SCALE_X(pix_x) |  RENDERMODE_SCALE_Y(pix_y) | RENDERMODE_PIXELMODE_1);

  lb_gr_refresh(&ty_screen);
  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif


  /******************************************************************************/
  /* Demo: Slow filled-circle drawing routine                                   */
  /******************************************************************************/

  //#define DEMO_CIRCLE_FILLED_SLOW
#ifdef DEMO_CIRCLE_FILLED_SLOW
  SDL_Event event;
  int pix_x=30, pix_y=30;
  lb_gr_SDL_init("DEMO_CIRCLE_FILLED_SLOW", SDL_INIT_VIDEO, 0,0, 0, 0, 0);
  lb_gr_clear_picture(NULL, lb_gr_12RGB(COLOR_SOLID | COLOR_BLUE));

  //  lb_gr_draw_circle_filled_slow(NULL, ty_screen.w/(2*pix_x), ty_screen.h/(2*pix_y), ty_screen.h/(2*pix_y), lb_gr_12RGB(0xF00F),
  //COPYMODE_COPY |  COPYMODE_BGCOLOR(0xFFFF) | COPYMODE_PIXELMODE_1 | COPYMODE_SCALE_X(pix_x) |  COPYMODE_SCALE_Y(pix_y));

  lb_gr_draw_circle_filled_slow(NULL, ty_screen.w/(2*pix_x), ty_screen.h/(2*pix_y), ty_screen.h/(2*pix_y), lb_gr_12RGB(0xF00F), COPYMODE_COPY);

  lb_gr_refresh(&ty_screen);
  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif

  /******************************************************************************/
  /* Demo: Fast filled-circle drawing routine                                   */
  /******************************************************************************/

  //#define DEMO_CIRCLE_FILLED_FAST
#ifdef DEMO_CIRCLE_FILLED_FAST
  SDL_Event event;
  int pix_x=10, pix_y=10;
  lb_gr_SDL_init("DEMO_CIRCLE_FILLED_FAST", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0, 0, 0);
  lb_gr_clear_picture(NULL, lb_gr_12RGB(COLOR_SOLID | COLOR_WHITE));
      
  lb_gr_draw_circle_filled_antialiasing_r(NULL, ty_screen.w/(3*pix_x), ty_screen.h/(3*pix_y), ty_screen.h/(4*pix_y),
					  lb_gr_12RGB(COLOR_RED | COLOR_SOLID));
  lb_gr_draw_circle_filled_antialiasing_r(NULL, 2*ty_screen.w/(3*pix_x), ty_screen.h/(3*pix_y), ty_screen.h/(4*pix_y),
					  lb_gr_12RGB(COLOR_LIME  | COLOR_SOLID));
  lb_gr_draw_circle_filled_antialiasing_r(NULL, ty_screen.w/(2*pix_x), 2*ty_screen.h/(3*pix_y), ty_screen.h/(4*pix_y),
					  lb_gr_12RGB(COLOR_BLUE  | COLOR_SOLID));
  lb_gr_refresh(&ty_screen);
  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif

  /******************************************************************************/
  /* Demo: Fast filled-circle drawing routine with antialiasing                 */
  /******************************************************************************/

  //#define DEMO_CIRCLE_FILLED_ANTIALIASING
#ifdef DEMO_CIRCLE_FILLED_ANTIALIASING
  SDL_Event event;
  lb_gr_SDL_init("DEMO_CIRCLE_FILLED_ANTIALIASING", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0, 0, 0);
  lb_gr_clear_picture(NULL, lb_gr_12RGB(COLOR_SOLID | COLOR_WHITE));

  lb_gr_draw_circle_filled_antialiasing(NULL, ty_screen.w/3,   ty_screen.h/3, ty_screen.h/4, lb_gr_12RGB(0xF00F));
  lb_gr_draw_circle_filled_antialiasing(NULL, 2*ty_screen.w/3, ty_screen.h/3, ty_screen.h/4, lb_gr_12RGB(0xf0F0));
  lb_gr_draw_circle_filled_antialiasing(NULL, ty_screen.w/2, 2*ty_screen.h/3, ty_screen.h/4, lb_gr_12RGB(0xfF00));
  lb_gr_draw_circle_filled_antialiasing(NULL, ty_screen.w/2, 2*ty_screen.h/3, ty_screen.h/4, lb_gr_12RGB(0xfF00));
  lb_gr_refresh(&ty_screen);
  lb_gr_BMPfile_save("filled_circles.bmp", NULL);

  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif

  /******************************************************************************/
  /* Demo: A circular arc                                                       */
  /******************************************************************************/

  //#define DEMO_CIRCLE_ARC
#ifdef DEMO_CIRCLE_ARC
  SDL_Event event;
  int pix_x=10, pix_y=10;
  lb_gr_SDL_init("DEMO_CIRCLE_ARC", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0, 0, 0);
  lb_gr_clear_picture(NULL, lb_gr_12RGB(COLOR_SOLID | COLOR_BLACK));

  lb_gr_draw_circle_arc(NULL, ty_screen.w/(pix_x*2), ty_screen.h/(pix_y*2), ty_screen.h/(pix_y*2)-1,
			0, M_PI/3,lb_gr_12RGB(0xFfff), COPYMODE_COPY);
  lb_gr_refresh(&ty_screen);


  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif

  /******************************************************************************/
  /* Demo: Ellipse primitive                                                    */
  /******************************************************************************/

  //#define DEMO_ELLIPSE
#ifdef DEMO_ELLIPSE
  REAL_T angle;
  SDL_Event event;
  PICTURE_T Pic;
  SINT16_T pix_x=20, pix_y=20;
  
  lb_gr_SDL_init("DEMO_ELLIPSE", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0, 0, 0);

  Pic.w= ty_screen.w/pix_x;
  Pic.h= ty_screen.h/pix_y;

  lb_gr_create_picture(&Pic,lb_gr_12RGB(0x0000));
  
  angle=0.0;
  lb_gr_refresh(&ty_screen);
  for (angle=0;angle<=M_PI/2;angle+=1.0*M_PI/180.0)
    {
      lb_gr_clear_picture(&Pic, lb_gr_12RGB(COLOR_WHITE));
      
      lb_gr_draw_ellipse_rotated(&Pic, ty_screen.w/(pix_x*2), ty_screen.h/(pix_y*2), ty_screen.w*0.45/pix_x,ty_screen.w*0.10/pix_y, angle, 
				 lb_gr_12RGB(COLOR_BLACK), COPYMODE_COPY);
	     
      lb_gr_render_picture(&Pic, &ty_screen, 0, 0, COPYMODE_COPY, RENDERMODE_PIXELMODE_1 | RENDERMODE_SCALE_X(pix_x) |  RENDERMODE_SCALE_Y(pix_y));
      lb_gr_BMPfile_save("rotated_ellipse.bmp", NULL);
	     
      lb_gr_refresh(&ty_screen);
      lb_ti_delay_ms(500);
      printf("Angle=%f\r\n",angle);
    }

  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif


  /******************************************************************************/
  /* Demo: Filled Triangles                                                     */
  /* Triangles are a special shape, as they are tha basis of any polyedra       */
  /* It is important, therefore, to draw them FAST.                             */
  /* In fact, if there is ANY primitive i CG you must get really fast, this is  */
  /* the one.                                                                   */
  /******************************************************************************/

  //#define DEMO_TRIANGLE
#ifdef DEMO_TRIANGLE
  SDL_Event event;
  int pix_x=1, pix_y=1;
  SINT16_T i;
  POINT_2D_SINT16_T A, B, C;
  clock_t begin, end;
  lb_gr_SDL_init("DEMO_TRIANGLE", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0, 0, 0);
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
	lb_gr_refresh(&ty_screen);
    }
  end=clock();
  printf("time elapsed = %f\n",(double)(end - begin) / CLOCKS_PER_SEC);
  
  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif
  
  /******************************************************************************/
  /* Demo: Filled Polygons                                                      */
  /* the one.                                                                   */
  /******************************************************************************/

  //#define DEMO_POLYGON
#ifdef DEMO_POLYGON
  SDL_Event event;
  int pix_x=1, pix_y=1;
  LINE_2D_SINT16_T myPol;
  int i;
  float phase;

  lb_gr_SDL_init("DEMO_POLYGON", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0, 0, 0);
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
      lb_gr_draw_polygon_i(NULL,&myPol,3,lb_gr_12RGB(COLOR_RED|COLOR_SOLID), COPYMODE_BLEND, LINEMODE_DOTS_FILTERED);
      lb_gr_draw_polygon_i(NULL,&myPol,3,lb_gr_12RGB(COLOR_RED|COLOR_SOLID), COPYMODE_BLEND, LINEMODE_FILTERED);
      lb_gr_refresh(&ty_screen);
		 
      lb_ti_delay_ms(500);
    }
  lb_gr_release_line2d_i(&myPol);

  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif

  /******************************************************************************/
  /* Demo: Filled Polygons (floating point version)                             */
  /* the one.                                                                   */
  /******************************************************************************/

  //#define DEMO_POLYGON_FLOAT
#ifdef DEMO_POLYGON_FLOAT
  SDL_Event event;
  int pix_x=1, pix_y=1;
  LINE_2D_REAL_T myPol;
  int i;
  float phase;

  lb_gr_SDL_init("DEMO_POLYGON_FLOAT", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0, 0, 0);
 
  /* Polygon creation and testing */ 
  myPol.items=10;
  lb_gr_create_line2d_r(&myPol);
  printf("\a\r\n");
  for (phase=0; phase<6.2832; phase+=M_PI/180.0) 
    {
      lb_gr_clear_picture(NULL,lb_gr_12RGB(0xfaaa));
      for (i=0;i<myPol.items;i++)
	{
	  myPol.array[i].x=0.5*ty_screen.w/pix_x + 0.45*ty_screen.h*cos(phase+6.2832*i/(myPol.items-1)/pix_y);
	  myPol.array[i].y=0.5*ty_screen.h/pix_y - 0.45*ty_screen.h*sin(phase+6.2832*i/(myPol.items-1)/pix_y);
	}
      lb_gr_draw_polygon_antialiasing(NULL,&myPol,6.1,lb_gr_12RGB(COLOR_BLUE));
      printf("hi\r\n");
      //lb_ti_delay_ms(50);
      lb_gr_refresh(&ty_screen);
    }
  lb_gr_release_line2d_f(&myPol);
  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif

  /******************************************************************************/
  /* Demo: Detecting whether an integer point is located within a Polygon       */
  /******************************************************************************/

  //#define DEMO_INSIDE_POLYGON_INT
#ifdef DEMO_INSIDE_POLYGON_INT
  SDL_Event event;
  int pix_x=12, pix_y=12;
  SINT16_T i, j;
  LINE_2D_SINT16_T Poly_int;
  POINT_2D_SINT16_T P;

  lb_gr_SDL_init("DEMO_INSIDE_POLYGON_INT", SDL_INIT_VIDEO, 0*400,0*400, 0, 0, 0);
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
      
  lb_gr_draw_polygon_i(NULL,&Poly_int,4,lb_gr_12RGB(COLOR_BLUE), COPYMODE_BLEND, LINEMODE_FILTERED);
  lb_gr_refresh(&ty_screen);
  lb_ti_delay_ms(500);

  for(i=0;i<ty_screen.h/pix_y;i++)
    for(j=0;j<ty_screen.w/pix_x;j++)
      {
	P.x=j;
	P.y=i;
	if (lb_gr_is_in_polygon_i(&Poly_int,P))
	  lb_gr_draw_pixel(NULL, j, i, lb_gr_12RGB(COLOR_BEIGE), COPYMODE_COPY);
	else
	  lb_gr_draw_pixel(NULL, j, i, lb_gr_12RGB(COLOR_PINK), COPYMODE_COPY);
      }
  lb_gr_refresh(&ty_screen);

  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif

  /******************************************************************************/
  /* Demo: Detecting whether an integer point is located within a Polygon       */
  /* (floating point version)                                                   */
  /******************************************************************************/

  //#define DEMO_INSIDE_POLYGON_FLOAT
#ifdef DEMO_INSIDE_POLYGON_FLOAT
  SDL_Event event;
  int pix_x=1, pix_y=1;
  SINT16_T i, j;
  LINE_2D_REAL_T Poly_f;
  POINT_2D_REAL_T P;
  VIEWPORT_2D_T win;

  lb_gr_SDL_init("DEMO_INSIDE_POLYGON_FLOAT", SDL_INIT_VIDEO, 0*400,0*400, 0, 0, 0);
  lb_gr_clear_picture(NULL, lb_gr_12RGB(COLOR_SOLID | COLOR_WHITE));

  win.xp_min=0;
  win.yp_min=0;
  win.xp_max=ty_screen.w/pix_x;
  win.yp_max=ty_screen.h/pix_y;
  win.xr_min=-2*1.024;
  win.xr_max=2*1.024;
  win.yr_min=-2*0.768;
  win.yr_max= 2*0.768;

  /* Polygon creation and testing */
  Poly_f.items=6;
  lb_gr_create_line2d_r(&Poly_f);
  for (i=0;i<Poly_f.items-1;i++)
    {
      Poly_f.array[i].x=cos(2*M_PI*i/(Poly_f.items-1));
      Poly_f.array[i].y=sin(2*M_PI*i/(Poly_f.items-1));
    }
  Poly_f.array[Poly_f.items-1] = Poly_f.array[0]; /* This ensures the polygon is "closed" */
      
  for(i=0;i<ty_screen.h/pix_y;i++)
    for(j=0;j<ty_screen.w/pix_x;j++)
      {
	lb_gr_project_2d_inv(win, j, i, &P.x, &P.y);

	if (lb_gr_is_in_polygon_f(&Poly_f,P))
	  lb_gr_draw_pixel(NULL, j, i, lb_gr_12RGB(COLOR_GREEN | 0xF000), COPYMODE_BLEND);
	else
	  lb_gr_draw_pixel(NULL, j, i, lb_gr_12RGB(COLOR_BLUE | COLOR_SOLID), COPYMODE_BLEND);
      }
  lb_gr_refresh(&ty_screen);

  lb_gr_release_line2d_f(&Poly_f);
  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif

  /******************************************************************************/
  /* Demo: Polygon Filling                                                      */
  /******************************************************************************/

  //#define DEMO_POLYGON_FILL
#ifdef DEMO_POLYGON_FILL
  SDL_Event event;
  LINE_2D_SINT16_T myPol;
  int k=40;

  lb_gr_SDL_init("DEMO_POLYGON_FILL", SDL_INIT_VIDEO, 0*400,0*400, 0, 0, 0);
  lb_gr_clear_picture(NULL, lb_gr_12RGB(COLOR_SOLID | COLOR_BLACK));

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
  lb_gr_refresh(&ty_screen);

  lb_gr_release_line2d_i(&myPol);
  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif
  
  /******************************************************************************/
  /* Demo: The Duff-Porter operators                                            */
  /* These operator can be seen as generalizations in the analog world of       */
  /* digital operators such as NO, XOR, AND, OR, etc.                           */
  /******************************************************************************/

  //#define DEMO_DUFF_PORTER
#ifdef DEMO_DUFF_PORTER
  SDL_Event event;
  PICTURE_T pic1;
  SINT16_T i,j, alpha;
  LINE_2D_SINT16_T myPol;
  int k=40;
  int pix_x=3, pix_y=3;
  const char *filename="./media/images/sdl24bit.bmp";
  
  lb_gr_BMPfile_getsize(filename,&i,&j);
  lb_gr_SDL_init("DEMO_DUFF_PORTER", SDL_INIT_VIDEO, pix_x*i,pix_y*j, 0, 0, 0);
  
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
  alpha=MAX_A;
  lb_gr_BMPfile_load_to_pic(filename,&pic1,alpha);
  printf("1. alpha=%d\r\n",alpha);
  lb_gr_draw_polygon_fill_i(&pic1,&myPol,lb_gr_12RGB(0xFFFF), COPYMODE_XOR);
  lb_gr_render_picture(&pic1, &ty_screen, 0, 0, COPYMODE_COPY, RENDERMODE_PIXELMODE_0 | RENDERMODE_SCALE_X(pix_x) |  RENDERMODE_SCALE_Y(pix_y));
  
  lb_gr_refresh(&ty_screen);
  lb_ti_delay_ms(3000);
  lb_gr_draw_polygon_fill_i(&pic1,&myPol,lb_gr_12RGB(0xFFFF), COPYMODE_XOR);
  lb_gr_render_picture(&pic1, &ty_screen, 0, 0, COPYMODE_COPY, RENDERMODE_PIXELMODE_0 | RENDERMODE_SCALE_X(pix_x) |  RENDERMODE_SCALE_Y(pix_y));
  lb_gr_refresh(&ty_screen);
  lb_ti_delay_ms(3000);
     
  lb_gr_release_line2d_i(&myPol);
  lb_gr_release_picture(&pic1);

  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
   	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif
    
  /******************************************************************************/
  /* Demo: Advanced 2D Graphics                                                 */
  /******************************************************************************/

  /******************************************************************************/
  /* Demo: Plot 2D: transforms the xr, yr real coordinates of a point to screen */
  /* coordinates, and plots a dot                                               */
  /******************************************************************************/

  //#define DEMO_PLOT2D
#ifdef  DEMO_PLOT2D
  SDL_Event event;
  VIEWPORT_2D_T vp2d;
  PICTURE_T Pic;
  UINT8_T pix_x=8, pix_y=8, flag_running=TRUE;
  
  lb_gr_SDL_init("DEMO_PLOT2D", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0xFF, 0xFF, 0xFF);
  Pic.w= ty_screen.w/pix_x;
  Pic.h= ty_screen.h/pix_y;

  lb_gr_create_picture(&Pic,lb_gr_12RGB(0xFFFF));

  vp2d.xp_min=0;
  vp2d.yp_min=0.;
  vp2d.xp_max=Pic.w;
  vp2d.yp_max=Pic.h;
  vp2d.xr_min=-2.0;
  vp2d.xr_max=2.0;
  vp2d.yr_min=-2.0;
  vp2d.yr_max=2.0;

  lb_gr_plot2d(&Pic, vp2d, 0, 0, 4, lb_gr_12RGB(COLOR_RED), COPYMODE_COPY, LINEMODE_DOTS_SOLID);
  lb_gr_plot2d(&Pic, vp2d, 1.5, 0, 4, lb_gr_12RGB(COLOR_BLUE), COPYMODE_COPY, LINEMODE_DOTS_FILTERED);
  lb_gr_render_picture(&Pic, &ty_screen, 0,0, COPYMODE_COPY, RENDERMODE_PIXELMODE_1 | RENDERMODE_SCALE_X(pix_x) |  RENDERMODE_SCALE_Y(pix_y));

  lb_gr_refresh(&ty_screen);
  while (flag_running)
    while (SDL_PollEvent(&event))
      if (event.type == SDL_QUIT)
	flag_running=FALSE;
    
  //lb_gr_BMPfile_save("line.bmp", &Pic);
  lb_gr_release_picture(&Pic);
  lb_gr_SDL_close();
  SDL_Quit();
#endif
    
  /******************************************************************************/
  /* Demo: Rotation of bitmaps                                                  */
  /* This shows an older technique called "Shearing"for rotating bitmaps.       */
  /* https://www.ocf.berkeley.edu/~fricke/projects/israel/paeth/rotation_by_shearing.html    */
  /******************************************************************************/

  //#define DEMO_ROTATE_ALGORITHM
#ifdef DEMO_ROTATE_ALGORITHM
  SDL_Event event;
  PICTURE_T Pic;
  SINT16_T k;
  LINE_2D_REAL_T P1, P2, P3;
  VIEWPORT_2D_T win;
  REAL_T angle;

  lb_gr_SDL_init("DEMO_ROTATE_ALGORITHM", SDL_INIT_VIDEO, 1920*0.9, 1080*0.9, 0xFF, 0xFF, 0xFF);
 
  angle=45.0*M_PI/180;
     
  Pic.w=ty_screen.w;
  Pic.h=ty_screen.h;
            
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
  lb_gr_create_line2d_r(&P1);
  P2.items=5;
  lb_gr_create_line2d_r(&P2);
  P3.items=5;
  lb_gr_create_line2d_r(&P3);
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

      lb_gr_render_picture(&Pic, &ty_screen, 0, 0, COPYMODE_COPY, RENDERMODE_PIXELMODE_0);
      lb_gr_refresh(&ty_screen);
      lb_ti_delay_ms(1000);
    }

  lb_gr_release_picture(&Pic);
  lb_gr_release_line2d_f(&P1);
  lb_gr_release_line2d_f(&P2);
  lb_gr_release_line2d_f(&P3);

  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
   	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif

  /******************************************************************************/
  /* Demo: Rotation of bitmaps                                                  */
  /******************************************************************************/

  //#define DEMO_ROTATE_BITMAP
#ifdef  DEMO_ROTATE_BITMAP
  SDL_Event event;
  PICTURE_T pic_src, pic_dst;
  SINT16_T width, height;
  UINT8_T pix_x=2, pix_y=2;
  REAL_T angle;
  const char *filename="./media/images/test24bit.bmp";
  
  //pending: strange line on top

  /* Load initial image and present it */
  lb_gr_BMPfile_getsize(filename,&width,&height);
  pic_src.w=384;
  pic_src.h=256;
  printf("%d %d\r\n",width, height);
  lb_gr_SDL_init("DEMO_ROTATE_BITMAP", SDL_INIT_VIDEO, width*pix_x*2, height*pix_y*2, 0xFF, 0xFF, 0xFF);
  lb_gr_create_picture(&pic_src,lb_gr_12RGB(COLOR_RED));
       
  lb_gr_BMPfile_load_to_pic(filename,&pic_src, 0);
  
  for(angle=0;angle<2*M_PI;angle+=M_PI/180)
    {
      lb_gr_bitmap_rotate(&pic_src, &pic_dst, angle, lb_gr_12RGB(COLOR_BLACK | COLOR_SOLID));
      lb_gr_render_picture(&pic_dst, &ty_screen, 0, 0, COPYMODE_COPY, RENDERMODE_PIXELMODE_0 | RENDERMODE_SCALE_X(pix_x) |  RENDERMODE_SCALE_Y(pix_y));
      lb_gr_refresh(&ty_screen);
      printf("\r\nangle= %f",angle);
      lb_ti_delay_ms(500);
    }
  lb_gr_release_picture(&pic_src);
  
  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
   	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif

  /******************************************************************************/
  /* Demo: Rotation of bitmaps                                                  */
  /* The "sampling" technique is shown here.                                    */
  /******************************************************************************/

  //#define DEMO_ROTATE_BITMAP_SAMPLING
#ifdef  DEMO_ROTATE_BITMAP_SAMPLING
  SDL_Event event;
  PICTURE_T pic_src, pic_dst;
  SINT16_T width, height;
  REAL_T angle;
  UINT8_T pix_x=2, pix_y=2;
  const char *filename="./media/images/test24bit.bmp";

  /* Load initial image and present it */
  lb_gr_BMPfile_getsize(filename,&width,&height);
  lb_gr_SDL_init("DEMO_ROTATE_BITMAP_SAMPLING", SDL_INIT_VIDEO, width*pix_x*2, height*pix_y*2, 0xFF, 0xFF, 0xFF);

  pic_src.w=width;
  pic_src.h=height;
  lb_gr_create_picture(&pic_src,lb_gr_12RGB(COLOR_RED));
     
  lb_gr_BMPfile_load_to_pic(filename,&pic_src, 0);

  for(angle=0.0;angle<2*M_PI;angle+=0.5*M_PI/180)
    {
      lb_gr_bitmap_rotate_sampling(&pic_src, &pic_dst, angle, 10, lb_gr_12RGB(COLOR_LIME));
      lb_gr_render_picture(&pic_dst, &ty_screen, 0, 0, COPYMODE_COPY,
			   RENDERMODE_PIXELMODE_0 | RENDERMODE_SCALE_X(pix_x) |  RENDERMODE_SCALE_Y(pix_y));
      lb_gr_refresh(&ty_screen);
    }
  lb_gr_release_picture(&pic_src);
  lb_gr_release_picture(&pic_dst);

  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
   	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif

  /******************************************************************************/
  /* Demo: Matrix to BMP                                                        */
  /* Shows how picture data can be loaded to a Matrix to do image processing.   */
  /* In this example, the data is decomposed in their Red, Green, and Blue      */
  /* components.  Each component is plotted separately.  Then, they're again    */
  /* merged.                                                                    */
  /* to_do: the demo works, but it seems I ahve i and j backwards               */
  /******************************************************************************/
  
  //#define DEMO_BMP_TO_MATRIX
#ifdef DEMO_BMP_TO_MATRIX
  SDL_Event event;
  MATRIX_R_T R, G, B;
  SINT16_T i,j;
  PIXEL_T color;

  const char *filename="./media/images/test24bit.bmp";

  lb_gr_clear_picture(NULL,lb_gr_12RGB(COLOR_BLACK));

  lb_gr_BMPfile_getsize(filename,&i,&j);
  lb_gr_SDL_init("DEMO_BMP_TO_MATRIX", SDL_INIT_VIDEO, i*2, j*3, 0xaF, 0xaF, 0x8F);

  R.rows=j;
  R.cols=i;
  lb_al_create_matrix_r(&R);

  G.rows=j;
  G.cols=i;
  lb_al_create_matrix_r(&G);
  
  B.rows=j;
  B.cols=i;
  lb_al_create_matrix_r(&B);
      
  lb_gr_BMPfile_load_to_matrix(filename,&R,&G,&B);

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
  lb_gr_refresh(&ty_screen);
  lb_ti_delay_ms(10000);
          
  lb_al_release_matrix_r(&R);
  lb_al_release_matrix_r(&G);
  lb_al_release_matrix_r(&B);
  
  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
   	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif

  /******************************************************************************/
  /* Demo: Two-dimensional projection.                                          */
  /* Shows how to convert 2d real coordinates to screen coordinates.            */
  /******************************************************************************/

  //#define DEMO_PROJECT_2D
#ifdef DEMO_PROJECT_2D
  REAL_T xr, yr, xp, yp;
  VIEWPORT_2D_T win;
  SDL_Event event;

  lb_gr_SDL_init("DEMO_PROJECT_2D", SDL_INIT_VIDEO, 800, 600, 0xaF, 0xaF, 0x8F);

  win.xp_min=0;
  win.yp_min=0;
  win.xp_max=ty_screen.w;
  win.yp_max=ty_screen.h;
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
  lb_gr_refresh(&ty_screen);
  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
   	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif

  /******************************************************************************/
  /* Demo: Plotting complex numbers                                             */
  /* Data is loaded to a vector of Complex Numbers, then converted to a Vector  */
  /* of Integer 2D_points (a Polygon), and finally plotted.                     */
  /******************************************************************************/

  //#define DEMO_PLOT2D_COMPLEX
#ifdef DEMO_PLOT2D_COMPLEX
  SDL_Event event;
  int i;
  VIEWPORT_2D_T win;

  lb_gr_SDL_init("DEMO_PLOT2D_COMPLEX", SDL_INIT_VIDEO, 800, 600, 0xaF, 0xaF, 0x8F);

  win.xp_min=0;
  win.yp_min=0;
  win.xp_max=ty_screen.w;
  win.yp_max=ty_screen.h;
  win.xr_min=-3.1416;
  win.xr_max=3.1416;
  win.yr_min=-1.2;
  win.yr_max=1.2;

  VECTOR_C_T vec;
  vec.items=50;
  lb_al_create_vector_c(&vec);
      
  LINE_2D_SINT16_T vec_i;
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
      
  lb_al_release_vector_c(&vec);
  lb_gr_release_line2d_i(&vec_i);
  lb_gr_refresh(&ty_screen);
  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
   	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif

  /******************************************************************************/
  /* Demo: Plotting implicit-form functions                                     */
  /* to_do: in lb_gr_BMPfile_save(), an invalid path causes a segmentation      */
  /* fault.                                                                     */
  /* to_do: it looks like there is a minor glitch at the left boundary.         */
  /******************************************************************************/

  //#define DEMO_PLOT_IMPLICIT 
#ifdef DEMO_PLOT_IMPLICIT
  SDL_Event event;
  MATRIX_R_T A;
  VIEWPORT_2D_T vp;
  SINT16_T i,j;
  REAL_T xr, yr, zr;

  lb_gr_SDL_init("DEMO_PLOT_IMPLICIT", SDL_INIT_VIDEO, 1024, 1024, 0, 0, 0);

  A.rows=100; /* vertical */
  A.cols=100; /* horizontal */
  lb_al_create_matrix_r(&A);

  /* Viewport definition */
  vp.xp_min = 0;
  vp.xp_max = ty_screen.w;
  vp.yp_min = 0;
  vp.yp_max = ty_screen.h;

  vp.xr_min = -5;
  vp.xr_max =  5;
  vp.yr_min = -5;
  vp.yr_max =  5;
         
  for (i=0;i<A.rows;i++)
    for (j=0;j<A.cols;j++)
      {
	xr = vp.xr_min + j*(vp.xr_max-vp.xr_min)/(A.cols-1);
	yr = vp.yr_min + i*(vp.yr_max-vp.yr_min)/(A.rows-1);
	zr=sin(xr*xr+yr*yr);
	A.array[i][j]=zr;
      }
  //lb_al_print_matrix_r(&A,"A",FLOAT_FORMAT_MATRIX);
      
  lb_gr_clear_picture(NULL,lb_gr_12RGB(COLOR_WHITE));
  lb_gr_implicit_2d(NULL, vp, &A, 3, lb_gr_12RGB(COLOR_RED),COPYMODE_BLEND, LINEMODE_FILTERED);
  lb_gr_refresh(&ty_screen);
  lb_gr_BMPfile_save("./media/images/implicit.bmp", NULL);
  lb_al_release_matrix_r(&A);
  lb_gr_SDL_close();
  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
   	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif

  /******************************************************************************/
  /* Demo: Plot2d_Reverse                                                       */
  /* It is tricky to generate proper (high quality) anti-aliased polygonal      */
  /* lines.                                                                     */
  /* This method examines all the points in the neighborhood of a Polygon in    */
  /* order to determine whether each one is solid, should be blended,           */
  /* or -otherwise- should not be plotted at all. This is a slow method.        */
  /* It is usually enough (as well as much faster) to trace the polygon by a    */
  /* sucession or filtered lines.                                               */
  /******************************************************************************/

  //#define DEMO_PLOT2D_REVERSE
#ifdef DEMO_PLOT2D_REVERSE
  SDL_Event event;
  REAL_T t, t0, t1;
  SINT16_T i;
  VIEWPORT_2D_T vp;
  VECTOR_R_T Lx, Ly;

  lb_gr_SDL_init("DEMO_PLOT2D_REVERSE", SDL_INIT_VIDEO, 1200, 1024, 0xFF, 0xFF, 0xFF);

  /* Viewport definition */
  vp.xp_min = 0;
  vp.xp_max = ty_screen.w;
  vp.yp_min = 0;
  vp.yp_max = ty_screen.h;

  vp.xr_min = -1.1;
  vp.xr_max = 1.1;
  vp.yr_min = -1.1;
  vp.yr_max = 1.1;

  /* Create and load vectors */
  Lx.items=8000;
  Ly.items=8000;
  lb_al_create_vector_r(&Lx);
  lb_al_create_vector_r(&Ly);

  t=0;
  t0=0.0; t1=6.2832;
  for(i=0;i<Lx.items;i++)
    {
      printf("hello %d\r\n",i);
      Lx.array[i]=cos(3*t)*cos(t);  /* an example: petal flower */
      Ly.array[i]=cos(3*t)*sin(t);
      t+=(t1-t0)/(REAL_T)Lx.items;
    }
  lb_gr_plot2d_line_antialiasing_neighbor(NULL, vp, &Lx, &Ly, 12, lb_gr_12RGB(0xf00f));
  lb_gr_refresh(&ty_screen);
  
  lb_gr_BMPfile_save("flower.bmp", NULL);

  lb_al_release_vector_r(&Lx);
  lb_al_release_vector_r(&Ly);
  
  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif

  /******************************************************************************/
  /* Demo: Plot2d_Reverse_slow                                                  */
  /* It is tricky to generate proper (high quality) anti-aliased polygonal      */
  /* lines.                                                                     */
  /* This method examines all the points in the window to determine whether     */
  /* each one is solid, should be blended, or -otherwise- should not be         */
  /* plotted at all. This is a VERY slow method but it is easier to understand  */
  /* and could possibly be adapted for doing other things.                      */
  /******************************************************************************/

  //#define DEMO_PLOT2D_REVERSE_SLOW
#ifdef DEMO_PLOT2D_REVERSE_SLOW
  SDL_Event event;
  REAL_T t, t0, t1;
  SINT16_T i;
  VIEWPORT_2D_T vp;
  VECTOR_R_T Lx, Ly;

  lb_gr_SDL_init("DEMO_PLOT2D_REVERSE_SLOW", SDL_INIT_VIDEO, 0*400,0*400, 0, 0, 0);
  lb_gr_clear_picture(NULL, lb_gr_12RGB(COLOR_SOLID | COLOR_BLACK));

  /* Viewport definition */
  vp.xp_min = 0;
  vp.xp_max = ty_screen.w;
  vp.yp_min = 0;
  vp.yp_max = ty_screen.h;

  vp.xr_min = -2*1.024;
  vp.xr_max = 2*1.024;
  vp.yr_min = -2*0.768;
  vp.yr_max = 2*0.768;

  lb_gr_draw_circle_antialiasing3(NULL, ty_screen.w/3,   ty_screen.h/3,   ty_screen.h/5, lb_gr_12RGB(0xf00f));
  lb_gr_draw_circle_antialiasing3(NULL, ty_screen.w*2/3, ty_screen.h/3,   ty_screen.h/5, lb_gr_12RGB(0xf0f0));
  lb_gr_draw_circle_antialiasing3(NULL, ty_screen.w/2,   ty_screen.h*2/3, ty_screen.h/5, lb_gr_12RGB(0x3F00));

  /* Create and load vectors */ 
  Lx.items=450;
  Ly.items=450;
  lb_al_create_vector_r(&Lx);
  lb_al_create_vector_r(&Ly);

  t=0;
  t0=0.0; t1=6.2832;
  for(i=0;i<Lx.items;i++)
    {
      Lx.array[i]=cos(3*t)*cos(t); 
      Ly.array[i]=cos(3*t)*sin(t);
      t+=(t1-t0)/(REAL_T)Lx.items;
    }

  lb_gr_plot2d_line_antialiasing_neighbor_slow(NULL, vp, &Lx, &Ly, 1, lb_gr_12RGB(0xfF00));
  lb_gr_refresh(&ty_screen);
      
  lb_al_release_vector_r(&Lx);
  lb_al_release_vector_r(&Ly);

  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
 	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif

  /******************************************************************************/
  /* Demo: Handling JPEG files.                                                 */
  /* to_do:  It would be nice to have also a function for loading JPG's to a    */
  /* picture.                                                                   */
  /******************************************************************************/

  //#define DEMO_JPG
#ifdef DEMO_JPG
  SDL_Event event;
  int pix_x=2, pix_y=2;
  PICTURE_T pic1, pic2;
  SINT16_T i,j;

  lb_gr_SDL_init("DEMO_JPG", SDL_INIT_VIDEO, 0*400,0*400, 0, 0, 0);
  lb_gr_clear_picture(NULL, lb_gr_12RGB(COLOR_SOLID | COLOR_WHITE));

  lb_gr_BMPfile_getsize("./media/images/sdl24bit.bmp",&i,&j);
  pic1.w=i;
  pic1.h=j;
  lb_gr_create_picture(&pic1,lb_gr_12RGB(0x1000));
	    
  lb_gr_BMPfile_load_to_pic("./media/images/sdl24bit.bmp", &pic1, 0xFF);
  lb_gr_render_picture(&pic1, &ty_screen, 0, 0, COPYMODE_COPY, RENDERMODE_PIXELMODE_0 | RENDERMODE_SCALE_X(pix_x) |  RENDERMODE_SCALE_Y(pix_y));
  lb_gr_refresh(&ty_screen);

  lb_ti_delay_ms(1000);
  lb_gr_BMPfile_load_to_pic("./media/images/test.bmp",&pic1,0xFF);
  lb_gr_render_picture(&pic1, &ty_screen, 300, 300, COPYMODE_COPY, RENDERMODE_PIXELMODE_0 | RENDERMODE_SCALE_X(pix_x) |  RENDERMODE_SCALE_Y(pix_y));
  lb_gr_refresh(&ty_screen);

  pic2.w=pic1.h;
  pic2.h=pic1.w;
  lb_gr_create_picture(&pic2,lb_gr_12RGB(0x1000));
     	  
  for (j=0;j<pic1.w;j++)
    for (i=0;i<pic1.h;i++)
      pic2.pic[j][i]=pic1.pic[i][j];

  lb_gr_render_picture(&pic2, &ty_screen, 0, 0, COPYMODE_COPY, RENDERMODE_PIXELMODE_0 | RENDERMODE_SCALE_X(pix_x) |  RENDERMODE_SCALE_Y(pix_y));
  lb_gr_refresh(&ty_screen);
	    
  lb_gr_JPGfile_save("./media/images/delete_me.jpg", &pic1, 40);

  lb_gr_release_picture(&pic1);
  lb_gr_release_picture(&pic2);

  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
 	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif

  /******************************************************************************/
  /* Demo: Adding one axis or two axes, and a grid, to build a chart.           */
  /******************************************************************************/

  //#define DEMO_AXIS_2D
#ifdef DEMO_AXIS_2D
  VIEWPORT_2D_T win;
  SDL_Event event;
  FONT_T my_font;

  lb_gr_SDL_init("DEMO_AXIS_2D", SDL_INIT_VIDEO, 1200,800, 0xff, 0xff, 0xff);

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
  win.xp_max=ty_screen.w-20;
  win.yp_min=ty_screen.h-20;
  win.yp_max=20;


  win.xr_min= -5;
  win.xr_max=  10;
  win.yr_min=  -1;
  win.yr_max=  4; 

  //  win.xr_min= 1e-2;
  //win.xr_max=  1e2;
  //win.yr_min=  1e5;
  //win.yr_max=  1E1; 

  /* #define AXIS_DRAW_X               0b0000000000000001
     #define AXIS_DRAW_X_ARROWS        0b0000000000000010
     #define AXIS_DRAW_X_GRID          0b0000000000000100
     #define AXIS_DRAW_X_GRID_LOG      0b0000000000001000
     #define AXIS_DRAW_X_LABEL         0b0000000000010000
     \     #define AXIS_DRAW_Y               0b0000000000100000
     #define AXIS_DRAW_Y_ARROWS        0b0000000001000000
     #define AXIS_DRAW_Y_GRID          0b0000000010000000
     #define AXIS_DRAW_Y_GRID_LOG      0b0000000100000000
     #define AXIS_DRAW_Z_GRID          0b0000001000000000

     #define AXIS_DRAW_COLORVALUES_X_1 0b0000010000000000 Degradé
     #define AXIS_DRAW_COLORVALUES_X_2 0b0000100000000000 Color code 
     #define AXIS_DRAW_COLORVALUES_Y_1 0b0001000000000000 Degradé 
     #define AXIS_DRAW_COLORVALUES_Y_2 0b0010000000000000 Color code */

  lb_gr_draw_rectangle(NULL, win.xp_min, win.yp_min, win.xp_max, win.yp_max,
		       lb_gr_12RGB(COLOR_WHITE), COPYMODE_COPY);

  lg_gr_draw_axis_2d(NULL, win, &my_font,
		     lb_gr_12RGB(COLOR_BLUE), 5, 15,
		     lb_gr_12RGB(COLOR_GREEN), 1,
		     lb_gr_12RGB(COLOR_YELLOW), 1, 2, 
		     AXIS_DRAW_X | AXIS_DRAW_X_ARROWS | AXIS_DRAW_X_GRID |
		     AXIS_DRAW_Y | AXIS_DRAW_Y_ARROWS | AXIS_DRAW_Y_GRID |
		     0*AXIS_DRAW_X_GRID_LOG | 0*AXIS_DRAW_Y_GRID_LOG,
		     COPYMODE_BLEND, LINEMODE_FILTERED);
  
  lb_gr_refresh(&ty_screen);
  lb_gr_BMPfile_save("axis_2d.bmp", NULL);

  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
 	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif

  /******************************************************************************/
  /* Demo: Adding one log-axis or two log axes to build a semilog or log-log    */
  /* chart.                                                                     */
  /******************************************************************************/

  //#define DEMO_AXIS_2D_LOG
#ifdef DEMO_AXIS_2D_LOG
  VIEWPORT_2D_T win;
  SDL_Event event;
  FONT_T my_font;

  lb_gr_SDL_init("DEMO_AXIS_2D_LOG", SDL_INIT_VIDEO, 1200,800, 0xff, 0xff, 0xff);

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
  win.xp_max=ty_screen.w-20;
  win.yp_min=ty_screen.h-20;
  win.yp_max=20;

  win.xr_min= 1;
  win.xr_max=  1e4;
  win.yr_min=  -0.1e4;
  win.yr_max=  1e4; 

  lb_gr_draw_rectangle(NULL, win.xp_min, win.yp_min, win.xp_max, win.yp_max,
		       lb_gr_12RGB(COLOR_WHITE), COPYMODE_COPY);

  lg_gr_draw_axis_2d(NULL, win, &my_font,
		     lb_gr_12RGB(COLOR_BLUE), 5, 15,
		     lb_gr_12RGB(COLOR_GREEN), 10,
		     lb_gr_12RGB(COLOR_YELLOW), 1e3, 2.0, 
		     AXIS_DRAW_X | 0*AXIS_DRAW_X_ARROWS | 
		     AXIS_DRAW_Y | AXIS_DRAW_Y_ARROWS | AXIS_DRAW_Y_GRID | 
		     AXIS_DRAW_X_GRID_LOG | 0*AXIS_DRAW_Y_GRID_LOG,
		     COPYMODE_BLEND, LINEMODE_FILTERED);

  UINT16_T i;
  REAL_T xr, yr, xp, yp;
  xr=1;
  //  yr=1;
  for (i=0;i<=16;i++)
    {
      //xr=(i%9+1)*pow(10.0,(SINT16_T)i/9);
      //printf("a=%d b=%d c=%f\r\n",(i%9+1),(SINT16_T)i/9,xr); 
      //printf("xr=%f, yr=%f\t\n",xr,yr); 
      yr=xr;
      lb_gr_project_2d_x_log(win, xr,  &xp);
      lb_gr_project_2d_y(win, yr,  &yp);
      printf("xr=%f, yr=%f\t\n",xr,yr); 
      
      lb_gr_draw_rectangle_solid(NULL, xp-6, yp-6, xp+6, yp+6, lb_gr_12RGB(i*15/27));
      xr=xr*pow(10.0,1.0/4.0);
    } 
     
  lb_gr_refresh(&ty_screen);
  lb_gr_BMPfile_save("axis_2d_semilog.bmp", NULL);

  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
 	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif

  /******************************************************************************/
  /* Demo: Building a polar chart, with angular and radial grids.               */
  /* to_do: to elaborate an anti-aliased version of this chart.                 */
  /* hint: it isn't as easy as one would think...                               */
  /******************************************************************************/

  //#define DEMO_POLAR_AXIS
#ifdef DEMO_POLAR_AXIS
  SDL_Event event;
  VIEWPORT_2D_T win;
  FONT_T my_font;

  lb_gr_SDL_init("DEMO_POLAR_AXIS", SDL_INIT_VIDEO, 1200, 800, 0, 0, 0);
  lb_gr_clear_picture(NULL, lb_gr_12RGB(COLOR_SOLID | COLOR_WHITE));

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
  win.xp_max=ty_screen.w-20;
  win.yp_min=20;
  win.yp_max=ty_screen.h-20;

  win.xr_min= 0.2*-1.024*3;
  win.xr_max=  1.024*3;
  win.yr_min= 0.2*-0.768*3;
  win.yr_max= 0.768*3; 

  lb_gr_draw_rectangle(NULL, win.xp_min, win.yp_min, win.xp_max, win.yp_max, lb_gr_12RGB(COLOR_WHITE | COLOR_SOLID),
		       COPYMODE_COPY );
  lg_gr_draw_axis_2d_polar(NULL, win, &my_font, 0, 4, 0.1, lb_gr_12RGB(COLOR_BLUE),
			   0, 2*M_PI, 30*M_PI/180,
			   lb_gr_12RGB(COLOR_RED | COLOR_SOLID), 0, COPYMODE_COPY );
  lb_gr_refresh(&ty_screen);

  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
 	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif

  /******************************************************************************/
  /* Demo: Plot_continuous                                                      */
  /* Background: one challenge when plotting parametric curves is choosing the  */
  /* right step: if it is too small, there will be gaps in the curve.  If it is */
  /* too small, the curve will take too long to be drawn.                       */
  /* This experimental method can adjust the step keeping a continuous curve    */
  /* provided (this is important!) the curve is continous and differentiable    */
  /* (smooth).                                                                  */
  /* The demo also reminds us how to pass functions as parameters in good old C */
  /******************************************************************************/
  
  //#define DEMO_PLOT_CONTINUOUS
#ifdef DEMO_PLOT_CONTINUOUS
  REAL_T fn_x(REAL_T t, MATHERROR_T *error)
  {
    return 0.75*sin(5*t)*cos(t);
  }

  REAL_T fn_y(REAL_T t, MATHERROR_T *error)
  {
    return 0.75*sin(5*t)*sin(t);
  }

  SDL_Event event;
  VIEWPORT_2D_T win;
  FONT_T my_font;

  lb_gr_SDL_init("DEMO_PLOT_CONTINUOUS", SDL_INIT_VIDEO, 800, 600, 0, 0, 0);

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
  win.xp_max=ty_screen.w-5;
  win.yp_min=5;
  win.yp_max=ty_screen.h-5;

  win.xr_min= -1.024;
  win.xr_max=  1.024;
  win.yr_min= -0.768;
  win.yr_max=  0.768; 

  lb_gr_draw_rectangle(NULL, win.xp_min, win.yp_min, win.xp_max, win.yp_max,
		       lb_gr_12RGB(COLOR_DIMGRAY), COPYMODE_COPY);
      
  lg_gr_draw_axis_2d(NULL, win, &my_font,
		     lb_gr_12RGB(COLOR_WHITE), 2.5, 5,
		     lb_gr_12RGB(COLOR_GREEN), 0.1,
		     lb_gr_12RGB(COLOR_BLUE), 0.1, 3,
		     AXIS_DRAW_X | AXIS_DRAW_X_ARROWS | AXIS_DRAW_X_GRID |
		     AXIS_DRAW_Y | AXIS_DRAW_Y_ARROWS | AXIS_DRAW_Y_GRID,
		     COPYMODE_BLEND, LINEMODE_FILTERED); 


  lb_gr_plot_continuous_fn_2d(NULL, win, fn_x, fn_y,
			      0, 2*10*M_PI, 1,
			      20, lb_gr_12RGB(COLOR_BLUE | COLOR_SOLID), COPYMODE_COPY);

  lb_gr_refresh(&ty_screen);
  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
 	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif


  /******************************************************************************/
  /* Demo: Plot_continuous with anti-aliasing                                   */
  /* This experimental method can adjust the step keeping plots a continuous    */
  /* curve with antialiasing at the expense of much longer processing time.     */
  /* The neighborhood of each dot is examined to determine whether is solid,    */
  /* if it shuld be blent with the backgroun, or not drawn at all.              */
  /* The curve must be continous and differentiable (smooth).                   */
  /******************************************************************************/

  //#define DEMO_PLOT_CONTINUOUS_ANTIALIASING
#ifdef DEMO_PLOT_CONTINUOUS_ANTIALIASING
  REAL_T fn_x(REAL_T t, MATHERROR_T *error)
  {
    return 0.75*sin(5*t)*cos(t);
  }

  REAL_T fn_y(REAL_T t, MATHERROR_T *error)
  {
    return 0.75*sin(5*t)*sin(t);
  }

  SDL_Event event;
  VIEWPORT_2D_T win;
  FONT_T my_font;

  lb_gr_SDL_init("EMO_PLOT_CONTINUOUS_ANTIALIASING", SDL_INIT_VIDEO, 800, 600, 0, 0, 0);

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
  win.xp_max=ty_screen.w-5;
  win.yp_min=5;
  win.yp_max=ty_screen.h-5;
     
  win.xr_min= -1.024;
  win.xr_max=  1.024;
  win.yr_min=  0.768;
  win.yr_max= -0.768; 

  lb_gr_draw_rectangle(NULL, win.xp_min, win.yp_min, win.xp_max, win.yp_max,
		       lb_gr_12RGB(COLOR_WHITE | COLOR_SOLID), COPYMODE_COPY);

  lg_gr_draw_axis_2d(NULL, win, &my_font,
		     lb_gr_12RGB(COLOR_BLACK), 3, 1,
		     lb_gr_12RGB(COLOR_GREEN), 0.1,
		     lb_gr_12RGB(COLOR_BLUE), 0.1, 2,
		     AXIS_DRAW_X | AXIS_DRAW_X_ARROWS | AXIS_DRAW_X_GRID |
		     AXIS_DRAW_Y | AXIS_DRAW_Y_ARROWS | AXIS_DRAW_Y_GRID,
		     COPYMODE_COPY, LINEMODE_SOLID); 

  
  lb_gr_plot_continuous_fn_2d_antialiasing(NULL, win, fn_x, fn_y,
					   0, 2*M_PI, 0.1, 32,
					   16, lb_gr_12RGB(COLOR_BLUE|COLOR_SOLID));
  lb_gr_refresh(&ty_screen);
  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
 	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif


  /******************************************************************************/
  /* Demo: Plotting the Mandlelbrot set.                                        */
  /* This was one of the most memorable tests carried out during the project.   */
  /* Once the first graphic engine became functional, I looked for something    */
  /* cool to test it. Never having plot a Mandelbrot Set before, I consulted    */
  /* its theory and, by using the complex-type functions already built,         */
  /* got it within 15 minutes and worked fine at the very first try.            */
  /* Ironically, the graphic engine took MUCH longer!                           */
  /* Later on, zoom and shifts were added to explore any section of the Set.    */
  /******************************************************************************/

  //#define DEMO_MANDELBROT
#ifdef DEMO_MANDELBROT
  SDL_Event event;
  SINT32_T xp, yp, iterations;
  REAL_T xr, yr;
  COMPLEX_T z, p;
  VIEWPORT_2D_T win;
  PICTURE_T Pic;
  long max_iterations;
  REAL_T k=1.5, x_center, y_center, size;
  
  lb_gr_SDL_init("DEMO_MANDELBROT", SDL_INIT_VIDEO, 1920,1080, 0, 0, 0);
  lb_gr_clear_picture(NULL, lb_gr_12RGB(COLOR_SOLID | COLOR_BLACK));
  
  win.xp_min=0;
  win.yp_min=0;
  
  win.xp_max=ty_screen.w;
  win.yp_max=ty_screen.h;

  //win.yp_max=11020;

  
  Pic.w=win.xp_max;
  Pic.h=win.yp_max;

  lb_gr_create_picture(&Pic,lb_gr_12RGB(COLOR_BLACK | COLOR_SOLID));

  x_center =0.358406523810;
  y_center =0.647235573237;
  size = 0.00001;
  max_iterations=2048;


  //x_center =0.358406523810;
  //y_center =0.647235573237;
  //size = 0.000000009289;
  //max_iterations=32768;


  
  //x_center =0.352477;
  //y_center =0.582358;
  //size = 0.000001;

  
  //x_center =-1.155176;
  //y_center =0.279934;
  //size = 0.011531;

  //x_center =-1.158879;
  //y_center =0.285082;
  //size = 0.001552;

  //x_center =0.337700;
  //y_center =0.573170;
  //size = 0.009610;

  SINT8_T flag_exit, flag_changed;
  PIXEL_T pix;

  flag_exit=FALSE;
  flag_changed=TRUE;
  
  while (!flag_exit)
    {
      if (flag_changed)
	{
	  flag_changed=FALSE;
	  win.xr_min=x_center-size;
	  win.xr_max=x_center+size;

	  win.yr_min=y_center-size/k;
	  win.yr_max=y_center+size/k;
	  for(xp=0;xp<win.xp_max;xp++)
	    {
	      if ((xp % 100)==0)
		{
		  printf("\rProcessing: %0.2f %\r",100.0*xp/win.xp_max);
		  fflush(stdout);
		}
	      for(yp=0;yp<win.yp_max;yp++)
		{
		  lb_gr_project_2d_inv(win, xp, yp, &xr, &yr);
		  iterations=0;
		  z.r=xr;
		  z.i=yr;
		  while ((lb_cp_abs(z)<=2.0) && (iterations<max_iterations)) 
		    {
		      p.r=xr;
		      p.i=yr;
		      z=lb_cp_add(lb_cp_multiply(z,z),p);
		      iterations++;
		    }
		  if (iterations>=max_iterations)
		    {
		      pix.b=0;
		      pix.g=0;
		      pix.r=0;
		    }
		  else
		    {
		      pix.b=0;
		      pix.g=127;
		      pix.r=0;
		    }
		    
		  lb_gr_draw_pixel(&Pic, xp, yp, pix, COPYMODE_COPY);
		}
	    }
	  printf("\rProcessing: %0.2f\r",100.0);
	  fflush(stdout);
	  lb_gr_render_picture(&Pic, &ty_screen, 0, 0, COPYMODE_COPY, 0);
	  lb_gr_refresh(&ty_screen);
	}

      while (SDL_PollEvent(&event))
	{
	  if (event.type == SDL_QUIT)
	    flag_exit=TRUE;
	  if (event.type== SDL_KEYDOWN)
	    {
	      switch(event.key.keysym.sym)
		{
		case SDLK_F1:
		  max_iterations*=2;
		  flag_changed=TRUE;
		  break;
		case SDLK_F2:
		  max_iterations/=2;
		  if(max_iterations<4)
		    max_iterations=4;
		  flag_changed=TRUE;
		  break;
		case SDLK_PAGEUP:
		  size/=powf(10.0,1.0/4.0);
		  flag_changed=TRUE;
		  break;
		case SDLK_PAGEDOWN:
		  size*=powf(10.0,1.0/4.0);;
		  flag_changed=TRUE;
		  break;
		case SDLK_LEFT:
		  x_center-=size/8.0;
		  flag_changed=TRUE;
		  break;
		case SDLK_RIGHT:
		  x_center+=size/8.0;
		  flag_changed=TRUE;
		  break;
		case SDLK_UP:
		  y_center+=size/8.0;
		  flag_changed=TRUE;
		  break;
		case SDLK_DOWN:
		  y_center-=size/8.0;
		  flag_changed=TRUE;
		  break;
		case SDLK_ESCAPE:
		  flag_exit=TRUE;
		  break;
		case SDLK_b:
		  printf("Saving file as BMP\r\n");
		  lb_gr_BMPfile_save("./media/images/mandelbrot2.bmp",&Pic);
		  printf("File saved\r\n");
		  break;
		case SDLK_j:
		  printf("Saving file as JPG\r\n");
		  lb_gr_JPGfile_save("./media/images/mandelbrot2.jpg", &Pic, 100);
		  printf("File saved\r\n");
		  break;
		}
	      printf("x_center =%.12f;\r\n",x_center);
	      printf("y_center =%.12f;\r\n",y_center);
	      printf("size = %.12f;\r\n",size);
	      printf("max_iterations=%ld;\r\n",max_iterations);
	     
	      
	      printf("x: [%.12f ; %.12f]\r\n",x_center-size/2,x_center+size/2);
	      printf("y: [%.12f ; %.12f]\r\n",y_center-size/2,y_center+size/2);
	    }
	}
    }
#endif

  /******************************************************************************/
  /* Demo: generating videos                                                    */
  /* Videos can be generated by first writing a series of images, then using    */
  /* a external tool such as ffmpeg. More information is available at:          */
  /* https://trac.ffmpeg.org/wiki/Slideshow                                     */
  /* ffmpeg -framerate 24 -i man%06d.jpg output.mp4                             */
  /******************************************************************************/

  //#define DEMO_VIDEO_MANDELBROT
#ifdef DEMO_VIDEO_MANDELBROT
  int xp, yp, iterations;
  int k;
  REAL_T xr, yr, z_zoom;
  char filename[32];
  COMPLEX_T z, p;
  VIEWPORT_2D_T win;
  PIXEL_T pix;

  lb_gr_SDL_init("DEMO_VIDEO_MANDELBROT", SDL_INIT_VIDEO, 1920, 1080, 0, 0, 0);

  z_zoom=0.5;
  win.xp_min=0;
  win.yp_min=0;
  win.xp_max=ty_screen.w;
  win.yp_max=ty_screen.h;

  
  /* 24 frames per second, 3 minutes long */
  //for(k=0;k<3*60*24;k++)
  for(k=0;k<10*24;k++)
    {
      win.xr_min=0.25-1.00*(320.0/200.0)/z_zoom;
      win.xr_max=0.25+1.00*(320.0/200.0)/z_zoom;
      win.yr_min=-1/z_zoom; 
      win.yr_max=1/z_zoom;

      for(xp=0;xp<win.xp_max;xp++)
	for(yp=0;yp<win.yp_max;yp++)
	  {
	    lb_gr_project_2d_inv(win, xp, yp, &xr, &yr);
	    iterations=0;
	    z.r=xr;
	    z.i=yr;
	    while ((lb_cp_abs(z)<2.0) && (iterations<255)) 
	      {
		p.r=xr;
		p.i=yr;
		z=lb_cp_add(lb_cp_multiply(z,z),p);
		iterations++;
	      }
	    pix.r=0;
	    pix.g=0;
	    pix.b=iterations;

	    lb_gr_draw_pixel(NULL, xp, yp, pix, COPYMODE_COPY);
	  }
      lb_gr_refresh(&ty_screen);
      z_zoom*=1.01;
      sprintf(filename,"./media/videos/man%06d.jpg",k);
      lb_gr_JPGfile_save(filename, NULL, 100);
    }
  lb_gr_SDL_close();
  SDL_Quit();
  return EXIT_SUCCESS;
#endif


  /******************************************************************************/
  /* Demo: generating videos using ffmpeg                                       */
  /* This shows how to open a pipe in ffmpeg to buffer data from our programs!  */
  /* Writing to files to concatenate is no longer needed !!!                    */
  /******************************************************************************/

  //#define DEMO_VIDEO_MANDELBROT2
#ifdef DEMO_VIDEO_MANDELBROT2
  int xp, yp, iterations, max_iterations=2048;
  int k;
  REAL_T xr, yr, z_zoom;
  COMPLEX_T z, p;
  VIEWPORT_2D_T win;
  
  PIXEL_T pix;
 
  lb_gr_SDL_init("DEMO_VIDEO_MANDELBROT", SDL_INIT_VIDEO, 1920, 1080, 0, 0, 0);
  FILE *pipeout = popen("ffmpeg -y -f rawvideo -vcodec rawvideo -pix_fmt rgb24 -s 1920x1080 -r 30 -i - -f mp4 -q:v 0 -an -vcodec mpeg4 output.mp4", "w");
  unsigned char frame[1080][1920][3] = {0};

  z_zoom=0.5;
  win.xp_min=0;
  win.yp_min=0;
  win.xp_max=ty_screen.w;
  win.yp_max=ty_screen.h;
  
  /* 30 frames per second, 1 minutes long */
  for(k=0;k<3*60*30;k++)
    {
      win.xr_min=0.25-1.00*(320.0/200.0)/z_zoom;
      win.xr_max=0.25+1.00*(320.0/200.0)/z_zoom;
      win.yr_min=-1/z_zoom; 
      win.yr_max=1/z_zoom;

      for(xp=0;xp<win.xp_max;xp++)
	for(yp=0;yp<win.yp_max;yp++)
	  {
	    lb_gr_project_2d_inv(win, xp, yp, &xr, &yr);
	    iterations=0;
	    z.r=xr;
	    z.i=yr;
	    while ((lb_cp_abs(z)<2.0) && (iterations<max_iterations)) 
	      {
		
		p.r=xr;
		p.i=yr;
		z=lb_cp_add(lb_cp_multiply(z,z),p);
		iterations++;
	      }
	    pix.r=0;
	    pix.g=127*(max_iterations-iterations)/max_iterations;
	    pix.b=160*iterations/max_iterations;
	    
	    lb_gr_draw_pixel(NULL, xp, yp, pix, COPYMODE_COPY);

	    frame[yp][xp][0]=pix.r;
	    frame[yp][xp][1]=pix.g;
	    frame[yp][xp][2]=pix.b;
	  }
      lb_gr_refresh(&ty_screen);
      z_zoom*=1.005;
      fwrite(frame, 1, ty_screen.w*ty_screen.h*3, pipeout);
    }
    fflush(pipeout);
    pclose(pipeout);
  lb_gr_SDL_close();
  SDL_Quit();
  return EXIT_SUCCESS;
#endif


  /******************************************************************************/
  /* Demo: Mandelbrot sets with multi-threading                                 */
  /* New !!!! This program plots the Mandelbrot set performing multi-threaded   */
  /* processing AND records a video using ffmpeg.                               */
  /******************************************************************************/

  //#define DEMO_MANDELBROT_THREADS
#ifdef DEMO_MANDELBROT_THREADS
  #define N_THREADS 4
 
  typedef struct 
  {
    int _yp_a, _yp_b;
  } ARGS_T;

  int i, k, max_iterations=16;
  REAL_T z_zoom;
  VIEWPORT_2D_T win;
  pthread_t threads[N_THREADS];
  ARGS_T arguments[N_THREADS];
  unsigned char frame[720][1280][3] = {0};
  clock_t begin, end;
 
  
  void *mandelbrot(void *args)
  {
    UINT16_T xp, yp, yp_a, yp_b;
    REAL_T xr, yr;
    COMPLEX_T z, p;
    PIXEL_T pix;
    int iterations;

    /* It is simpler to first de-reference the arguments from the structure, then use them */
    yp_a=(*(ARGS_T *)args)._yp_a;
    yp_b=(*(ARGS_T *)args)._yp_b;

    for(yp=yp_a;yp<=yp_b;yp++)
      for(xp=0;xp<win.xp_max;xp++)
	{
	  lb_gr_project_2d_inv(win, xp, yp, &xr, &yr);
	  iterations=0;
	  z.r=xr;
	  z.i=yr;
	  while ((lb_cp_abs(z)<2.0) && (iterations<max_iterations)) 
	    {
	      p.r=xr;
	      p.i=yr;
	      z=lb_cp_add(lb_cp_multiply(z,z),p);
	      iterations++;
	    }
	  REAL_T L;
	  L=iterations/max_iterations;
	  pix.r=255*(1.0-L);
	  pix.g=255*(1.0-L);
	  pix.b=127+127*(1.0-L);
	  
	  lb_gr_draw_pixel(NULL, xp, yp, pix, COPYMODE_COPY);

	  frame[yp][xp][0]=pix.r;
	  frame[yp][xp][1]=pix.g;
	  frame[yp][xp][2]=pix.b;
	}
    return 0;
  }
    
  lb_gr_SDL_init("DEMO_VIDEO_MANDELBROT", SDL_INIT_VIDEO, 1280, 720, 0, 0, 0);
  FILE *pipeout = popen("ffmpeg -y -f rawvideo -vcodec rawvideo -pix_fmt rgb24 -s 1280x720 -r 30 -i - -f mp4 -q:v 0 -an -vcodec mpeg4 output.mp4", "w");

  z_zoom=1.0;
  win.xp_min=0;
  win.yp_min=0;
  win.xp_max=ty_screen.w;
  win.yp_max=ty_screen.h;

  begin=clock();
  /* 30 frames per second, 1 minutes long */
  for(k=0;k<1*60*30;k++)
    {
      //      max_iterations=1.6*pow(10.0,z_zoom);
      max_iterations=32*pow(256.0/32.0,k/(1.0*60*30));
      printf("\r\nmax_iterations = %d",(int)max_iterations);

      win.xr_min=0.25-2.00*((double)ty_screen.w/ty_screen.h)/z_zoom;
      win.xr_max=0.25+2.00*((double)ty_screen.w/ty_screen.h)/z_zoom;
      win.yr_min=-2.0/z_zoom; 
      win.yr_max=2.0/z_zoom;

      for (i=0;i<N_THREADS;i++)
	{
	  arguments[i]._yp_a=i*win.yp_max/N_THREADS;
	  arguments[i]._yp_b=(i+1)*win.yp_max/N_THREADS-1;
	  //printf("a=%d, b=%d\r\n\r\n",arguments[i]._yp_a,arguments[i]._yp_b);
	  fflush(stdout);
	  pthread_create(&threads[i],NULL, mandelbrot, (void *) &arguments[i]);
	}
      for (i=0;i<N_THREADS;i++)
	pthread_join(threads[i],NULL);

      lb_gr_refresh(&ty_screen);
      z_zoom*=1.003;
      fwrite(frame, 1, ty_screen.w*ty_screen.h*3, pipeout);
      end=clock();
      if (k!=0)
	printf("\r\nFPS = %f\r\n", (double)k*CLOCKS_PER_SEC/(end-begin));
      //oxo
    }
    fflush(pipeout);
    pclose(pipeout);
  lb_gr_SDL_close();
  SDL_Quit();
  return EXIT_SUCCESS;

  printf("Ended\r\n");
#endif

  /******************************************************************************/
  /* Demo: ACTIVE_SHUTTER                                                       */
  /******************************************************************************/
  #define DEMO_ACTIVE_SHUTTER
#ifdef DEMO_ACTIVE_SHUTTER

  /* 8: Marked as CE0 in breakout */
  /* 9: Marked as MISO in breakout */
  /* 10: Marked as MOSI in breakout */
  /* 11: Marked as SCLK in breakout */
#define PIN_RA     8 
#define PIN_RB     9
#define PIN_LA     10
#define PIN_LB     11
  SDL_Event event;
  PICTURE_T Pic_L, Pic_R;
  SCREEN_T screen2;
  
  
  /* Load Left and Right Images */
  SINT16_T width, height;
  SINT8_T channels;

  lb_gr_JPGfile_getsize("./media/images/stereo_left.jpg", &width, &height, &channels);
  Pic_L.w=width;
  Pic_L.h=height;
  lb_gr_JPGfile_getsize("./media/images/stereo_right.jpg", &width, &height, &channels);
  Pic_R.w=width;
  Pic_R.h=height;
  if ( (Pic_L.w!=Pic_R.w) || (Pic_L.h!=Pic_R.h) )
    {
      printf("Error: Left and Right images have different sizes\r\n");
      exit(1);
    }
  
  lb_gr_SDL_init("DEMO_ACTIVE_SHUTTER",SDL_INIT_VIDEO, Pic_L.w, Pic_L.h, 0, 0, 0);
  
  lb_gr_create_picture(&Pic_L,lb_gr_12RGB(COLOR_BLACK | COLOR_SOLID));
  lb_gr_create_picture(&Pic_R,lb_gr_12RGB(COLOR_BLACK | COLOR_SOLID));

  lb_gr_JPGfile_load("./media/images/stereo_left.jpg",&Pic_L);
  lb_gr_JPGfile_load("./media/images/stereo_right.jpg",&Pic_R);

  int k;

  lb_gr_render_picture(&Pic_L, &ty_screen, 0, 0, COPYMODE_COPY, 0);


  screen2.w=width;
  screen2.h=height;
  lb_gr_create_screen(&screen2,0,0,0,0);
  
  lb_gr_render_picture(&Pic_R, &screen2, 0, 0, COPYMODE_COPY, 0);

  //  SDL_SetHint(SDL_HINT_RENDER_VSYNC,"1");
  clock_t begin, end;
 
  lb_gp_gpio_open();
  lb_gp_gpio_setup_pin(PIN_LA, GPIO_OUTPUT);
  lb_gp_gpio_setup_pin(PIN_LB, GPIO_OUTPUT);
  lb_gp_gpio_setup_pin(PIN_RA, GPIO_OUTPUT);
  lb_gp_gpio_setup_pin(PIN_RB, GPIO_OUTPUT);

  k=0;
  while (k<300)
    {
      /* Positive Polarity Left*/
      lb_gp_gpio_wr(PIN_LA,   GPIO_HIGH);
      lb_gp_gpio_wr(PIN_LB,   GPIO_LOW);

      /* Blank Right */
      lb_gp_gpio_wr(PIN_RA,   GPIO_LOW);
      lb_gp_gpio_wr(PIN_RB,   GPIO_LOW);

      lb_gr_refresh(&ty_screen);
      lb_ti_delay_ms(1);

      /* Blank Left */
      lb_gp_gpio_wr(PIN_LA,   GPIO_LOW);
      lb_gp_gpio_wr(PIN_LB,   GPIO_LOW);

      /* Positive Polarity Right */
      lb_gp_gpio_wr(PIN_RA,   GPIO_HIGH);
      lb_gp_gpio_wr(PIN_RB,   GPIO_LOW);

      lb_gr_refresh(&screen2);
      lb_ti_delay_ms(1);

      /* Same, with opposite polarity */

      /* Positive Polarity Left*/
      lb_gp_gpio_wr(PIN_LA,   GPIO_LOW);
      lb_gp_gpio_wr(PIN_LB,   GPIO_HIGH);

      /* Blank Right */
      lb_gp_gpio_wr(PIN_RA,   GPIO_LOW);
      lb_gp_gpio_wr(PIN_RB,   GPIO_LOW);

      lb_gr_refresh(&ty_screen);
      lb_ti_delay_ms(1);

      /* Blank Left */
      lb_gp_gpio_wr(PIN_LA,   GPIO_LOW);
      lb_gp_gpio_wr(PIN_LB,   GPIO_LOW);

      /* Positive Polarity Right */
      lb_gp_gpio_wr(PIN_RA,   GPIO_LOW);
      lb_gp_gpio_wr(PIN_RB,   GPIO_HIGH);

      lb_gr_refresh(&screen2);
      lb_ti_delay_ms(1);
      k++;
    }
  /* Blank Left */
  lb_gp_gpio_wr(PIN_LA,   GPIO_LOW);
  lb_gp_gpio_wr(PIN_LB,   GPIO_LOW);

  /* Blank Right */
  lb_gp_gpio_wr(PIN_RA,   GPIO_LOW);
  lb_gp_gpio_wr(PIN_RB,   GPIO_LOW);
      
  while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT)
	{
	  /* Blank Left */
	  lb_gp_gpio_wr(PIN_LA,   GPIO_LOW);
	  lb_gp_gpio_wr(PIN_LB,   GPIO_LOW);

	  /* Blank Right */
	  lb_gp_gpio_wr(PIN_RA,   GPIO_LOW);
	  lb_gp_gpio_wr(PIN_RB,   GPIO_LOW);
	  
	  lb_gp_gpio_close();
	  lb_gr_SDL_close();
	  lb_gr_release_screen(&screen2);

	  SDL_Quit();
	}
    }
  return EXIT_SUCCESS;
#endif


  /******************************************************************************/
  /* Demo: ACTIVE_SHUTTER_THREADS                                               */
  /******************************************************************************/
  //#define DEMO_ACTIVE_SHUTTER_THREADS
#ifdef DEMO_ACTIVE_SHUTTER_THREADS

  /* 8: Marked as CE0 in breakout */
  /* 9: Marked as MISO in breakout */
  /* 10: Marked as MOSI in breakout */
  /* 11: Marked as SCLK in breakout */
#define PIN_LA     8 
#define PIN_LB     9
#define PIN_RA     10
#define PIN_RB     11
  SDL_Event event;
  SINT8_T flag_exit=FALSE;
  pthread_t thread_id; 
  PICTURE_T Pic_L, Pic_R;
  SCREEN_T screen2;
  
   /* Load Left and Right Images */
  SINT16_T width, height;
  SINT8_T channels;

  /* There are different methods to attempt to keep a constant frame rate.  Here is one: */

    
#define METHOD_ONE
#ifdef METHOD_ONE
  void *shutter_thread(void *vargp) 
  {
    REAL_T r_start, r_end, r_time;
    REAL_T FPS=60;
    struct timespec ts_time;
    
     while(!flag_exit)
      {
	/* Positive Polarity Left*/
	lb_gp_gpio_wr(PIN_LA,   GPIO_HIGH);
	lb_gp_gpio_wr(PIN_LB,   GPIO_LOW);

	/* Blank Right */
	lb_gp_gpio_wr(PIN_RA,   GPIO_LOW);
	lb_gp_gpio_wr(PIN_RB,   GPIO_LOW);

      	lb_gr_refresh(&ty_screen);
	lb_ti_delay_ms(10);

	/* Blank Left */
	lb_gp_gpio_wr(PIN_LA,   GPIO_LOW);
	lb_gp_gpio_wr(PIN_LB,   GPIO_LOW);

	/* Positive Polarity Right */
	lb_gp_gpio_wr(PIN_RA,   GPIO_HIGH);
	lb_gp_gpio_wr(PIN_RB,   GPIO_LOW);

       	lb_gr_refresh(&screen2);
	lb_ti_delay_ms(10);

	/* Same, with opposite polarity */

	/* Positive Polarity Left*/
	lb_gp_gpio_wr(PIN_LA,   GPIO_LOW);
	lb_gp_gpio_wr(PIN_LB,   GPIO_HIGH);

      /* Blank Right */
	lb_gp_gpio_wr(PIN_RA,   GPIO_LOW);
	lb_gp_gpio_wr(PIN_RB,   GPIO_LOW);

	lb_gr_refresh(&ty_screen);
	lb_ti_delay_ms(10);

	/* Blank Left */
	lb_gp_gpio_wr(PIN_LA,   GPIO_LOW);
	lb_gp_gpio_wr(PIN_LB,   GPIO_LOW);
	
	/* Positive Polarity Right */
	lb_gp_gpio_wr(PIN_RA,   GPIO_LOW);
	lb_gp_gpio_wr(PIN_RB,   GPIO_HIGH);

	lb_gr_refresh(&screen2);
	lb_ti_delay_ms(10);

	
      //lb_gr_refresh(&ty_screen);
      //clock_gettime(CLOCK_MONOTONIC, &ts_time);
      //r_start= ts_time.tv_sec + ts_time.tv_nsec/1000000000.0;
      //r_end =  r_start + 1/FPS;

      //do
      //  {
      //    clock_gettime(CLOCK_MONOTONIC, &ts_time);
      //    r_time= ts_time.tv_sec + ts_time.tv_nsec/1000000000.0;
      //  } while (r_time<r_end);

      }
    return NULL;
  }
#endif


  
  lb_gr_JPGfile_getsize("./media/images/stereo_left.jpg", &width, &height, &channels);
  Pic_L.w=width;
  Pic_L.h=height;
  lb_gr_JPGfile_getsize("./media/images/stereo_right.jpg", &width, &height, &channels);
  Pic_R.w=width;
  Pic_R.h=height;
  if ( (Pic_L.w!=Pic_R.w) || (Pic_L.h!=Pic_R.h) )
    {
      printf("Error: Left and Right images have different sizes\r\n");
      exit(1);
    }
  
  lb_gr_SDL_init("DEMO_ACTIVE_SHUTTER",SDL_INIT_VIDEO, Pic_L.w, Pic_L.h, 0, 0, 0);
  
  lb_gr_create_picture(&Pic_L,lb_gr_12RGB(COLOR_BLACK | COLOR_SOLID));
  lb_gr_create_picture(&Pic_R,lb_gr_12RGB(COLOR_BLACK | COLOR_SOLID));

  lb_gr_JPGfile_load("./media/images/stereo_left.jpg",&Pic_L);
  lb_gr_JPGfile_load("./media/images/stereo_right.jpg",&Pic_R);

  
  screen2.w=width;
  screen2.h=height;
  lb_gr_create_screen(&screen2,0,0,0,0);
  
  
  //  SDL_SetHint(SDL_HINT_RENDER_VSYNC,"1");

    lb_gp_gpio_open();
  lb_gp_gpio_setup_pin(PIN_LA, GPIO_OUTPUT);
  lb_gp_gpio_setup_pin(PIN_LB, GPIO_OUTPUT);
  lb_gp_gpio_setup_pin(PIN_RA, GPIO_OUTPUT);
  lb_gp_gpio_setup_pin(PIN_RB, GPIO_OUTPUT);

  pthread_create(&thread_id, NULL, shutter_thread, NULL); 
  
  printf("Here\r\n");
  fflush(stdout);


  while(TRUE)
    {
      while (SDL_PollEvent(&event))
	{
	  if (event.type == SDL_QUIT)
	    {
	      flag_exit=TRUE;
	      pthread_join(thread_id, NULL);
	      lb_gr_release_screen(&screen2);

	      /* Blank Left */
	      lb_gp_gpio_wr(PIN_LA,   GPIO_LOW);
	      lb_gp_gpio_wr(PIN_LB,   GPIO_LOW);

	      /* Blank Right */
	      lb_gp_gpio_wr(PIN_RA,   GPIO_LOW);
	      lb_gp_gpio_wr(PIN_RB,   GPIO_LOW);

	      lb_gp_gpio_close();
	      lb_gr_SDL_close();
	      SDL_Quit();
	      exit(1);
	    }
	}
    }
  



  

  lb_gp_gpio_open();
  lb_gp_gpio_setup_pin(PIN_LA, GPIO_OUTPUT);
  lb_gp_gpio_setup_pin(PIN_LB, GPIO_OUTPUT);
  lb_gp_gpio_setup_pin(PIN_RA, GPIO_OUTPUT);
  lb_gp_gpio_setup_pin(PIN_RB, GPIO_OUTPUT);

  
  while (1)
    {
      /* Positive Polarity */
      lb_gp_gpio_wr(PIN_LA,   GPIO_HIGH);
      lb_gp_gpio_wr(PIN_LB,   GPIO_LOW);
      lb_ti_delay_ms(1000);

      /* Blank */
      lb_gp_gpio_wr(PIN_LA,   GPIO_LOW);
      lb_gp_gpio_wr(PIN_LB,   GPIO_LOW);
      lb_ti_delay_ms(1000);

      /* Negative Polarity */
      lb_gp_gpio_wr(PIN_LA,   GPIO_LOW);
      lb_gp_gpio_wr(PIN_LB,   GPIO_HIGH);
      lb_ti_delay_ms(1000);
      
      /* Blank */
      lb_gp_gpio_wr(PIN_LA,   GPIO_LOW);
      lb_gp_gpio_wr(PIN_LB,   GPIO_LOW);
      lb_ti_delay_ms(1000);

      /**************************************/
      
      /* Positive Polarity */
      lb_gp_gpio_wr(PIN_RA,   GPIO_HIGH);
      lb_gp_gpio_wr(PIN_RB,   GPIO_LOW);
      lb_ti_delay_ms(1000);

      /* Blank */
      lb_gp_gpio_wr(PIN_RA,   GPIO_LOW);
      lb_gp_gpio_wr(PIN_RB,   GPIO_LOW);
      lb_ti_delay_ms(1000);

      /* Negative Polarity */
      lb_gp_gpio_wr(PIN_RA,   GPIO_LOW);
      lb_gp_gpio_wr(PIN_RB,   GPIO_HIGH);
      lb_ti_delay_ms(1000);
      
      /* Blank */
      lb_gp_gpio_wr(PIN_RA,   GPIO_LOW);
      lb_gp_gpio_wr(PIN_RB,   GPIO_LOW);
      lb_ti_delay_ms(1000);

      
     
    }
  return EXIT_SUCCESS;
#endif

  /******************************************************************************/
  /* Demo: GPIO                                                                 */
  /* This is a complete application which was put together to evaluate the      */
  /* feasibility of the system within an industrial setting.                    */
  /* A Raspberry Pi 3 is required.                                              */
  /* The program was used to control a pneumatic jig to perform shock/vibration */
  /* analysis of certain electronic sensors.  Data is being read via SPI from   */
  /* the sensors, and also from an accelerometer attached to them.              */
  /* Note this demo is rather specific, and may not be the best choice for      */
  /* trying out the GPIO library for the first time.                            */
  /* However, it can be used for doing a general overview of the GPIO functions.*/
  /******************************************************************************/
  
  //#define DEMO_GPIO
#ifdef DEMO_GPIO

#define PIN_CS_0   17
#define PIN_CS_1   18
#define PIN_CS_ADC 21
#define PIN_MOSI   10
#define PIN_CLK    11
#define PIN_MISO    9
#define PIN_RELAY  13

  typedef enum { Status_auto=0,
		 Status_single=1,
		 Status_paused=2,
		 Status_confirm=3,
		 Status_end=4 } STATUS_T;

  STATUS_T status;
  
  UINT64_T time_begin, time_end;
  char text[20];
  SINT8_T relay_status=0;
  SINT32_T shock_counter=1, counter_bad=0, counter_good=0;
  REAL_T accel, accel_max;
  char c;
  FILE *fp;
    

  union Record
  {
    float  f;
    unsigned char uc[4];
  };
      
  int i;

  union Record temp0, press0, temp1, press1;

  SPI_PORT_T my_port;
  SINT32_T time_stroke=4000, time_sensor=250;
  UINT16_T value;

  lb_co_reset_attributes();
  lb_co_color(TEXT_MODE_NEGATIVE);
    
  if (argc==2)
    printf("\r\nShock test running.  Logging to: %s\r\n",argv[1]);
    
  else
    printf("Shock test running.  No log file was specified.  Data is not being recorded\r\n");
  printf("MODE: Manual: press space bar to trigger shock.\r");

  lb_co_reset_attributes();
  lb_co_reset_colors();
  printf("\r\n");
    
  if (argc==2)
    fp = fopen(argv[1],"w"); 
  
  my_port.MOSI=PIN_MOSI;
  my_port.CLK=PIN_CLK;
  my_port.MISO=PIN_MISO;
  my_port.delay_clk=100;
  my_port.delay_byte=100;

  lb_gp_gpio_open();
  lb_gp_gpio_setup_pin(PIN_CS_0,   GPIO_OUTPUT);
  lb_gp_gpio_setup_pin(PIN_CS_1,   GPIO_OUTPUT);
  lb_gp_gpio_setup_pin(PIN_CS_ADC, GPIO_OUTPUT);
  lb_gp_gpio_setup_pin(PIN_MOSI, GPIO_OUTPUT);
  lb_gp_gpio_setup_pin(PIN_CLK,  GPIO_OUTPUT);


  //#define SPEED_TEST
#ifdef SPEED_TEST
  volatile unsigned int *my_gpio_high =lb_gp_gpio + 7;
  volatile unsigned int *my_gpio_low = lb_gp_gpio + 10;
  //extern volatile unsigned int *lb_gp_gpio;  // They have to be found somewhere, but can't be i
  UINT32_T my_bit = 1<<22;

  lb_gp_gpio_setup_pin(22,  GPIO_OUTPUT);
  while (1)
    {
      //*(lb_gp_gpio+7)=1<22;
      //printf("+");
      //*(lb_gp_gpio+10)=1<22;
      //printf("-");

      *(my_gpio_high) = my_bit;
      *(my_gpio_low) = my_bit;
      //*(lb_gp_gpio + 10) =my_bit;

      //      *(lb_gp_gpio + 7) = my_bit;
      //*(lb_gp_gpio + 10) =my_bit;
      //lb_gp_gpio_wr(22,GPIO_HIGH);
      //lb_gp_gpio_wr(22,GPIO_LOW);
    }
#endif
  lb_gp_gpio_setup_pin(PIN_RELAY,  GPIO_OUTPUT);
  lb_gp_gpio_setup_pin(PIN_MISO, GPIO_INPUT);

  lb_gp_gpio_wr(PIN_CS_0,   GPIO_HIGH);
  lb_gp_gpio_wr(PIN_CS_1,   GPIO_HIGH);
  lb_gp_gpio_wr(PIN_CS_ADC, GPIO_HIGH);

  status=Status_paused;
  while (status!=Status_end)
    {
      if ( (status==Status_auto) || (status==Status_single) )
	{
	  /* Schedule:
	     Log P, T, accel_max 
	     Trigger device
	     accel_max=0
	     Log acceleration, save accel_max
	     wait until t= 5.0 */

	  /* First, we set the required parameters for transducer 0 */

	  my_port.CPOL=GPIO_LOW;
	  my_port.CPHA=GPIO_LOW;
	  lb_gp_gpio_wr(PIN_CLK, my_port.CPOL); /* Always remember to set the default Clock idle state prior to selecting the SPI device */

	  time_begin = lb_ti_clock64();
	  time_end = time_begin + (time_stroke-2*time_sensor)*1000;
	  relay_status = !relay_status;
	  lb_gp_gpio_wr(PIN_RELAY,   relay_status);

	  accel_max = 0.0;
	   
	  while (lb_ti_clock64() < time_end)
	    {
	      /* This section reads a low-cost , 4 channel ADC: MPC3204 */ 

	      lb_gp_gpio_wr(PIN_CS_ADC, GPIO_LOW);
	      lb_ti_delay_us(100);
      
	      lb_gp_gpio_SPI_rw(&my_port, 0b110);
	      lb_ti_delay_us(100);
	      value=lb_gp_gpio_SPI_rw_nbits(&my_port, 0,16);
	      //lb_gp_print_u32_as_binary(value, 16);
	      lb_gp_gpio_wr(PIN_CS_ADC, GPIO_HIGH);
	      lb_ti_delay_us(100);

	      value = value & 0xFFF;
	      accel = (((value & 0xFFF)*3.3/0xFFF)-3.3/2.0)/1.52e-3 - 5.601;
	      if (accel>accel_max)
		accel_max = accel;
	    }
	  if (accel_max>525.0)
	    {
	      counter_bad++;
	      if (counter_bad>=3)
		{
		  counter_bad=0;
		  counter_good=0;
		  printf("Event: system has been PAUSED\r\n");
		  status=Status_paused;
		}
	    }
	  else
	    {
	      counter_good++;
	      if ((counter_good>=10) && (counter_bad>0))
		{
		  counter_good=0;
		  counter_bad--;
		}
	    }

	  my_port.CPOL=GPIO_HIGH;
	  lb_gp_gpio_wr(PIN_CLK, my_port.CPOL); /* Always remember to set the default Clock idle state prior to selecting the SPI device */
	  my_port.CPHA=GPIO_HIGH;

	  //#define RESTORE_CELSIUS ==>  This block works
#ifdef RESTORE_CELSIUS
	  lb_gp_gpio_wr(PIN_CS_1, GPIO_LOW);
	  lb_ti_delay_us(500);
	  /* Enter Level 1 */
	  lb_gp_gpio_SPI_rw(&my_port, 0x3C);
	  for (i=1;i<8;i++)
	    lb_gp_gpio_SPI_rw(&my_port, 0x00);
	  lb_gp_gpio_wr(PIN_CS_1, GPIO_HIGH);
	  lb_ti_delay_us(500);
	  lb_gp_gpio_wr(PIN_CS_1, GPIO_LOW);
	  
	  /* Command to set to Celsius */
	  lb_gp_gpio_SPI_rw(&my_port, 0x21);
	  lb_gp_gpio_SPI_rw(&my_port, 0x00);
	  for (i=2;i<8;i++)
	    lb_gp_gpio_SPI_rw(&my_port, 0x00);
#endif

	  //#define RESTORE_DEFAULT
#ifdef RESTORE_DEFAULT
	  lb_gp_gpio_wr(PIN_CS_1, GPIO_LOW);
	  lb_ti_delay_us(500);
	  /* Enter Level 1 */
	  lb_gp_gpio_SPI_rw(&my_port, 0x3C);
	  for (i=1;i<8;i++)
	    lb_gp_gpio_SPI_rw(&my_port, 0x00);
	  lb_gp_gpio_wr(PIN_CS_1, GPIO_HIGH);
	  lb_ti_delay_us(500);
	  lb_gp_gpio_wr(PIN_CS_1, GPIO_LOW);
	  
	  /* Command to restore the defaults */
	  lb_gp_gpio_SPI_rw(&my_port, 0xBB);
	  lb_gp_gpio_SPI_rw(&my_port, 0x00);
	  for (i=2;i<8;i++)
	    lb_gp_gpio_SPI_rw(&my_port, 0x00);
#endif

	  
	  
#define FIRST_SENSOR
#ifdef FIRST_SENSOR
	  /* First sensor */
	  lb_gp_gpio_wr(PIN_CS_0, GPIO_LOW);
	  lb_ti_delay_us(500);
	  lb_gp_gpio_SPI_rw(&my_port, 0x03);
	  for (i=1;i<8;i++)
	    lb_gp_gpio_SPI_rw(&my_port, 0x00);

	  time_end = time_begin + (time_stroke-time_sensor)*1000;
	  while (lb_ti_clock64() < time_end)
	    {
	      ; /* Heat up the room which is cold */
	    }

	  for (i=0;i<4;i++)
	    temp0.uc[3-i]=lb_gp_gpio_SPI_rw(&my_port, 0x00);

	  for (i=0;i<4;i++)
	    press0.uc[3-i]=lb_gp_gpio_SPI_rw(&my_port, 0x00);

	  lb_gp_gpio_wr(PIN_CS_0, GPIO_HIGH);
#endif

	 
#define SECOND_SENSOR
#ifdef SECOND_SENSOR
 
	  /* Second sensor */
	  lb_gp_gpio_wr(PIN_CS_1, GPIO_LOW);
	  lb_ti_delay_us(500);
	  lb_gp_gpio_SPI_rw(&my_port, 0x03);
	  for (i=1;i<8;i++)
	    lb_gp_gpio_SPI_rw(&my_port, 0x00);

	  time_end = time_begin + time_stroke*1000;
	  while (lb_ti_clock64() < time_end)
	    {
	      ; /* Heat up the room which is cold */
	    }
	    
	  for (i=0;i<4;i++)
	    temp1.uc[3-i]=lb_gp_gpio_SPI_rw(&my_port, 0x00);

	  for (i=0;i<4;i++)
	    press1.uc[3-i]=lb_gp_gpio_SPI_rw(&my_port, 0x00);

	  lb_gp_gpio_wr(PIN_CS_1, GPIO_HIGH);
#endif

	  /* We measure the acceleration */

	  if (relay_status)
	    sprintf(text,"UP");
	  else
	    sprintf(text,"DO");

	  lb_co_color(TEXT_MODE_NEGATIVE);
	  if (argc==2)
	    printf("[File: %s]",argv[1]);
	  if (argc==1)
	    printf("[NO FILE]");
	  lb_co_reset_attributes();
	    
	    
	  printf(" n=%ld  Dir=%s  T0=%4.2f  P0=%4.2f T1=%4.2f P1=%4.2f a=%4.1f, Exceeded: [%ld, %ld]\r\n",
		 shock_counter,text,temp0.f, press0.f,temp1.f, press1.f, accel_max,counter_bad, counter_good);

	  if (argc==2)
	    {
	      fprintf(fp,"n=;%ld;Dir=;%s;T0=;%4.2f;P0=;%4.2f;T1=;%4.2f;P1=;%4.2f;a=;%4.1f\r\n",
		      shock_counter,text,temp0.f, press0.f,temp1.f, press1.f, accel_max);
	      fflush(fp);
	    }
	  shock_counter++;
	}

      if ( status==Status_single )
	status=Status_paused;

      //      printf("status = %d \r\n",status);

      while (lb_co_kbhit())
	{
	  c=lb_co_getch_pc();
	  printf("lb_co_kbhit(), c=%c\r\n",c);
	  
	  switch(c)
	    {
	    case PCKEY_PAGE_UP:
	      time_stroke-=250;
	      if (time_stroke<(2*time_sensor+500))
		time_stroke=2*time_sensor+500;
	      printf("Event: stroke time has  been set to %lu ms\r\n",time_stroke);
	      break;
	    case PCKEY_PAGE_DOWN:
	      time_stroke+=250;
	      printf("Event: stroke time has  been set to %lu ms\r\n",time_stroke);
	      break;
	    case 'a':
	    case 'A':
	      status=Status_auto;
	      printf("Event: mode has been set to AUTOMATIC.  Total time per stroke is: %lu ms\r\n",time_stroke);
	      break;
	      
	    case 'p':
	    case 'P':
	      status=Status_paused;
	      printf("Event: system has been PAUSED\r\n");
	      break;
	    case 'r':
	    case 'R':
	      shock_counter=0;
	      printf("Event: counter has been reset\r\n");
	      break;
	    case ' ':
	      if (status==Status_paused)
		{
		  status=Status_single;
		  printf("Event: a single STROKE command has been issued\r\n");
		}
	      if (status==Status_auto)
		{
		  status=Status_paused;
		  printf("Event: mode has been set to PAUSED\r\n");
		}
	      break;
	    case 'q':
	    case 'Q':
	      status=Status_confirm;
	      printf("Event: Quitting program, press Y if you're sure\r\n");
	      break;
	    case 'y':
	    case 'Y':
	      if (status==Status_confirm)
		{
		  status=Status_end;
		  printf("Event: Closing program\r\n");
		}
	      break;
	    default:
	      if (status==Status_confirm)
		{
		  status=Status_paused;
		  printf("Event: Cancelling quit\r\n");
		}
	      break;
	    }
	}
    }
  if (argc==2)
    fclose(fp);
  
  lb_gp_gpio_close();
  return EXIT_SUCCESS;
#endif

  /******************************************************************************/
  /* Case  Study: Lever Mechanism                                               */
  /* This case is presented with detail in the Thesis Document.                 */
  /******************************************************************************/

  //#define CASE_LEVER
#ifdef CASE_LEVER
#define N_DISK 4000
  /* Graphical variables */
  REAL_T bar_radius=0.05;
  REAL_T bar_length=1.5;
  REAL_T disk_distance=0.7;
  REAL_T disk_max_radius=0.0;
  REAL_T ellipse_a, ellipse_b, tetha1, tetha2, xt_r_1, yt_r_1, xt_p_1, yt_p_1, xt_p_2, yt_p_2, xp_1, yp_1, xp_2, yp_2, max_angle;

  /* General use variables */
  REAL_T xr, yr;
  
  /* Dynamics variables */
  REAL_T Pos_x, Pos_y, Pos_x_prev, Pos_y_prev, Vel_x, Vel_y, Vel_x_prev, Vel_y_prev, Acc_x, Acc_y;
  REAL_T Pos_xp, Pos_yp;
  
  REAL_T RPM, t, dt, t_max, w,  xp, yp, xp2, yp2;
  SINT32_T i;
  VIEWPORT_2D_T win_v, win_a, win_sim;
  SDL_Event event;

  VECTOR_R_T R;
  VECTOR_R_T T;

  PICTURE_T Pic_Vel, Pic_Acc;
  FONT_T my_font;
  char text[80];

  lb_ft_load_GLCDfont("./fonts/Font_hp48G_large.lcd", &my_font);
  my_font.scale_x=2;
  my_font.scale_y=2;
  my_font.gap_x=2;
  my_font.gap_y=1;
  my_font.max_x=40;
  my_font.angle=0;
  my_font.flag_fg=TRUE;
  my_font.flag_bg=TRUE;
  my_font.color_fg=lb_gr_12RGB(COLOR_BLACK);
  my_font.color_bg=lb_gr_12RGB(COLOR_WHITE);

  R.items=N_DISK;
  T.items=N_DISK;
  lb_al_create_vector_r(&R);
  lb_al_create_vector_r(&T);
  
  lb_gr_SDL_init("CASE_LEVER", SDL_INIT_VIDEO, 1900,1000, 0, 0, 0);
  lb_gr_clear_picture(NULL, lb_gr_12RGB(COLOR_SOLID | COLOR_WHITE));

  Pic_Vel.w = ty_screen.w/2-15;
  Pic_Vel.h = ty_screen.h*1/3-10;
  lb_gr_create_picture(&Pic_Vel,lb_gr_12RGB(COLOR_WHITE | COLOR_SOLID));

  //sprintf(text,"dt: %02.2f [s]",dt);
  lb_gr_draw_rectangle_line(&Pic_Vel, 0, 0, Pic_Vel.w-2, Pic_Vel.h-2, 2,
			    lb_gr_12RGB(COLOR_BLACK), COPYMODE_COPY);
 
  Pic_Acc.w = ty_screen.w/2-15;
  Pic_Acc.h = ty_screen.h*1/3-10;
  lb_gr_create_picture(&Pic_Acc,lb_gr_12RGB(COLOR_WHITE | COLOR_SOLID));
  lb_gr_draw_rectangle_line(&Pic_Acc, 0, 0, Pic_Acc.w-2, Pic_Acc.h-2, 2,
			    lb_gr_12RGB(COLOR_BLACK), COPYMODE_COPY);
  
  RPM=1200.0;
  w=RPM*2*M_PI/60.0; /* rad per sec */

  t_max=8*60/RPM;
  dt=t_max/(1200.0);

  /* Load the disk */
  for (i=0;i<N_DISK; i++)
    {
      T.array[i] = i*2*M_PI/N_DISK;
      R.array[i] = 0.1+0.05*sin(T.array[i]);
      if (R.array[i]>disk_max_radius)
	disk_max_radius=R.array[i];
    }

  win_sim.xp_min=0;
  win_sim.xp_max=ty_screen.w;
  win_sim.yp_min=0;
  win_sim.yp_max=ty_screen.h;

  win_sim.xr_min= -5*bar_radius;
  win_sim.xr_max=  bar_length+disk_max_radius+5*bar_radius;
  win_sim.yr_min=  -0.5*(win_sim.yp_max-win_sim.yp_min)*(win_sim.xr_max-win_sim.xr_min)/(win_sim.xp_max-win_sim.xp_min);
  win_sim.yr_max=   -win_sim.yr_min; 

  win_v.xp_min=0;
  win_v.xp_max=Pic_Vel.w;
  win_v.yp_min=0;
  win_v.yp_max=Pic_Vel.h;

  win_v.xr_min= 0.0;
  win_v.xr_max=  t_max;
  win_v.yr_min=  -30;
  win_v.yr_max=  30; 


  win_a.xp_min=0;
  win_a.xp_max=Pic_Acc.w;
  win_a.yp_min=0;
  win_a.yp_max=Pic_Acc.h;

  win_a.xr_min= 0.0;
  win_a.xr_max=  t_max;
  win_a.yr_min=  -2500;
  win_a.yr_max=  2500; 

  /* simulation */  
  t=0;
  while (t<=t_max)
    {
      my_font.color_fg=lb_gr_12RGB(COLOR_BLACK);
      lb_gr_draw_rectangle(NULL, 0,0, ty_screen.w, ty_screen.h*2/3,lb_gr_12RGB(COLOR_WHITE), COPYMODE_COPY);
      sprintf(text,"Velocidad: componente y");
      lb_ft_draw_text(NULL, &my_font, 30, ty_screen.h*2/3-20, text, COPYMODE_COPY);
      sprintf(text,"Aceleracion: componente y");
      lb_ft_draw_text(NULL, &my_font, ty_screen.w/2+30, ty_screen.h*2/3-20, text, COPYMODE_COPY);
  
      /* First circle */
      lb_gr_project_2d(win_sim, 0, 0, &xp, &yp);
      lb_gr_project_2d(win_sim, bar_radius, 0, &xp2, &yp2);
      ellipse_a=fabs(xp2-xp);
      lb_gr_project_2d(win_sim, 0, bar_radius, &xp2, &yp2);
      ellipse_b=fabs(yp2-yp);
      lb_gr_draw_ellipse_antialiasing3(NULL, xp, yp, ellipse_a, ellipse_b, lb_gr_12RGB(COLOR_ORANGE));

      /* Motor axis */
      lb_gr_project_2d(win_sim, -1.2*bar_radius, 0, &xp, &yp);
      lb_gr_project_2d(win_sim, disk_distance+1.2*disk_max_radius, 0, &xp2, &yp2);
      lb_gr_draw_line(NULL, xp, yp, xp2, yp2, 1, lb_gr_12RGB(COLOR_GRAY), COPYMODE_COPY, LINEMODE_SOLID);

      lb_gr_project_2d(win_sim, disk_distance, -1.2*disk_max_radius, &xp, &yp);
      lb_gr_project_2d(win_sim, disk_distance,  1.2*disk_max_radius, &xp2, &yp2);
      lb_gr_draw_line(NULL, xp, yp, xp2, yp2, 1, lb_gr_12RGB(COLOR_GRAY), COPYMODE_COPY, LINEMODE_SOLID);

      lb_gr_project_2d(win_sim, 0, -1.2*bar_radius, &xp, &yp);
      lb_gr_project_2d(win_sim, 0,  1.2*bar_radius, &xp2, &yp2);
      lb_gr_draw_line(NULL, xp, yp, xp2, yp2, 1, lb_gr_12RGB(COLOR_GRAY), COPYMODE_COPY, LINEMODE_SOLID);

        
      max_angle=-M_PI;
      SINT8_T flag_first=TRUE;
      for (i=0;i<N_DISK; i++)
	{
	  xr = disk_distance + R.array[i]*cos(T.array[i] + w*t);
	  yr =                 R.array[i]*sin(T.array[i] + w*t);
	  
	  lb_gr_project_2d(win_sim, xr, yr, &xp_1, &yp_1);

	  if (!flag_first)
	    lb_gr_draw_line_antialiasing3(NULL, xp_1, yp_1, xp_2, yp_2, lb_gr_12RGB(COLOR_BLACK));
	  flag_first = FALSE;
	  
	  xp_2 = xp_1;
	  yp_2 = yp_1;
	  
	  //temp=atan2(R.array[i]*sin(T.array[i] + w*t),d + R.array[i]*cos(T.array[i] + w*t));
	  lb_ge_tangents_to_circle_point(0, 0, bar_radius,
					 xr, yr, &tetha1, &tetha2);
	  //printf("tetha2 = %f\r\n",tetha2);
	  if (tetha2>max_angle)
	    max_angle=tetha2;
	}
      /* The last segment of the disk is incomplete.  We assume a closed shape */
      xr = disk_distance + R.array[0]*cos(T.array[0] + w*t);
      yr =                 R.array[0]*sin(T.array[0] + w*t);
      lb_gr_project_2d(win_sim, xr, yr, &xp_1, &yp_1);
      lb_gr_draw_line_antialiasing3(NULL, xp_1, yp_1, xp_2, yp_2, lb_gr_12RGB(COLOR_BLACK));

      /* Second circle */
      xr=bar_length*cos(max_angle+M_PI_2);
      yr=bar_length*sin(max_angle+M_PI_2);
      lb_gr_project_2d(win_sim, xr, yr, &xp, &yp);
      lb_gr_draw_ellipse_antialiasing3(NULL, xp, yp, ellipse_a, ellipse_b, lb_gr_12RGB(COLOR_ORANGE));
      
      /* First line of the bar */
      xt_r_1=bar_radius*cos(max_angle);
      yt_r_1=bar_radius*sin(max_angle);
      lb_gr_project_2d(win_sim, xt_r_1, yt_r_1, &xt_p_1, &yt_p_1);
 
      lb_gr_project_2d(win_sim,
		       xt_r_1 + bar_length*cos(max_angle+M_PI_2),
		       yt_r_1 + bar_length*sin(max_angle+M_PI_2), &xt_p_2, &yt_p_2);
      lb_gr_draw_line_antialiasing3(NULL,xt_p_1, yt_p_1, xt_p_2, yt_p_2, lb_gr_12RGB(COLOR_ORANGE));


      /* Second line of the bar */
      xt_r_1=bar_radius*cos(max_angle+M_PI);
      yt_r_1=bar_radius*sin(max_angle+M_PI);
      lb_gr_project_2d(win_sim, xt_r_1, yt_r_1, &xt_p_1, &yt_p_1);
  
     
      lb_gr_project_2d(win_sim,
		       xt_r_1 + bar_length*cos(max_angle+M_PI_2),
		       yt_r_1 + bar_length*sin(max_angle+M_PI_2), &xt_p_2, &yt_p_2);
      lb_gr_draw_line_antialiasing3(NULL,xt_p_1, yt_p_1, xt_p_2, yt_p_2, lb_gr_12RGB(COLOR_ORANGE));

      //      lb_gr_delay(10);

      Pos_x=bar_length*cos(max_angle+M_PI_2);
      Pos_y=bar_length*sin(max_angle+M_PI_2);
      
      Vel_x=(Pos_x-Pos_x_prev)/dt;
      Vel_y=(Pos_y-Pos_y_prev)/dt;

      lb_gr_project_2d(win_sim, Pos_x, Pos_y, &Pos_xp, &Pos_yp);
      lb_gr_draw_arrow(NULL, Pos_xp, Pos_yp, Pos_xp + 10*Vel_x, Pos_yp-10*Vel_y, 5, 10, lb_gr_12RGB(COLOR_RED), COPYMODE_BLEND, LINEMODE_FILTERED);
      my_font.color_fg=lb_gr_12RGB(COLOR_RED);
      sprintf(text,"V");
      lb_ft_draw_text(NULL, &my_font, Pos_xp + 10*Vel_x+20, Pos_yp-10*Vel_y, text, COPYMODE_COPY);

      Acc_x=(Vel_x-Vel_x_prev)/dt;
      Acc_y=(Vel_y-Vel_y_prev)/dt;
      lb_gr_draw_arrow(NULL, Pos_xp, Pos_yp, Pos_xp + 0.05*Acc_x, Pos_yp-0.05*Acc_y, 5, 10, lb_gr_12RGB(COLOR_BLUE), COPYMODE_BLEND, LINEMODE_FILTERED);
      my_font.color_fg=lb_gr_12RGB(COLOR_BLUE);
      sprintf(text,"A");
      lb_ft_draw_text(NULL, &my_font, Pos_xp + 0.05*Acc_x + 20, Pos_yp-0.05*Acc_y, text, COPYMODE_COPY);		       
      //lb_gr_project_2d(win_sim, Pos_x, Pos_y, &Pos_xp, &Pos_yp);
      //lb_gr_draw_arrow(NULL, Pos_xp, Pos_yp, Pos_xp + Acc_x, Pos_yp-Acc_y, 1, 10, lb_gr_12RGB(COLOR_GREEN), COPYMODE_COPY, LINEMODE_SOLID);

      //Vel_mag=sqrt(Vel_x*Vel_x + Vel_y*Vel_y);
      //Acc_mag=sqrt(Acc_x*Acc_x + Acc_y*Acc_y);

      lb_gr_project_2d(win_v, t, Vel_y, &xp, &yp);
      //lb_gr_draw_pixel(&Pic_Vel,round(xp),round(yp),lb_gr_12RGB(COLOR_RED),COPYMODE_COPY);
      lb_gr_draw_circle_filled_antialiasing(&Pic_Vel, round(xp), round(yp), 2, lb_gr_12RGB(COLOR_RED));
      lb_gr_render_picture(&Pic_Vel, &ty_screen, 10, ty_screen.h*2/3+5, COPYMODE_COPY, 0);
		    

      lb_gr_project_2d(win_a, t, Acc_y, &xp, &yp);
      //lb_gr_draw_pixel(&Pic_Acc,round(xp),round(yp),lb_gr_12RGB(COLOR_BLUE),COPYMODE_COPY);
      lb_gr_draw_circle_filled_antialiasing(&Pic_Acc, round(xp), round(yp), 2, lb_gr_12RGB(COLOR_BLUE));
      lb_gr_render_picture(&Pic_Acc, &ty_screen, ty_screen.w/2+5, ty_screen.h*2/3+5, COPYMODE_COPY,0);

      Pos_x_prev = Pos_x;
      Pos_y_prev = Pos_y;

      Vel_x_prev = Vel_x;
      Vel_y_prev = Vel_y;

      t+=dt;

      lb_gr_refresh(&ty_screen);
    }
  lb_gr_BMPfile_save("leva.bmp", NULL);
  
  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
		      
	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
  lb_al_release_vector_r(&R);
  lb_al_release_vector_r(&T);
  lb_gr_release_picture(&Pic_Vel);
  lb_gr_release_picture(&Pic_Acc);
#endif

  
  /******************************************************************************/
  /* Case  Study: CASE_N_BODY_PROBLEM                                           */
  /* This case is presented with detail in the Thesis Document.                 */
  /******************************************************************************/

  //#define CASE_N_BODY_PROBLEM
#ifdef  CASE_N_BODY_PROBLEM
#define G 6.674e-11
#define N_OBJECTS 2
    
  typedef struct
  {
    REAL_T x;
    REAL_T y;
    REAL_T z;
  } VECTOR_3D_T;

  typedef struct
  {
    REAL_T m;
    VECTOR_3D_T p;
    VECTOR_3D_T v;
    VECTOR_3D_T v_temp;
    VECTOR_3D_T a_temp;
  } ASTRO_T;

  ASTRO_T M_euler[N_OBJECTS];
  ASTRO_T M_rk4[N_OBJECTS];

  VECTOR_3D_T calc_acceleration(const ASTRO_T M[], int i)
  {
    VECTOR_3D_T accel;
    REAL_T denom;
    int j;
  
    accel.x=0;
    accel.y=0;
    accel.z=0;
  
    for (j=0;j<N_OBJECTS;j++)
      {
	if (i!=j)
	  {
	    denom=1/pow( (M[j].p.x-M[i].p.x)*(M[j].p.x-M[i].p.x) + 
			 (M[j].p.y-M[i].p.y)*(M[j].p.y-M[i].p.y) +
			 (M[j].p.z-M[i].p.z)*(M[j].p.z-M[i].p.z), 1.5);

	    accel.x += (M[j].p.x - M[i].p.x)*G*M[j].m*denom;
	    accel.y += (M[j].p.y - M[i].p.y)*G*M[j].m*denom;
	    accel.z += (M[j].p.z - M[i].p.z)*G*M[j].m*denom;
	  }
      }
    return accel;
  }
 
  VECTOR_3D_T future_acc(ASTRO_T M[], int i, REAL_T delta_t, REAL_T delta_v_x,REAL_T delta_v_y, REAL_T delta_v_z)
  {
    VECTOR_3D_T tmp_pos, tmp_vel, acceleration;
  
    /* we must back-up the current position and velocity */
    tmp_pos=M[i].p;
    tmp_vel=M[i].v;

    M[i].v.x += delta_v_x;
    M[i].v.y += delta_v_y;
    M[i].v.z += delta_v_z;

    M[i].p.x +=  M[i].v.x*delta_t;
    M[i].p.y +=  M[i].v.y*delta_t;
    M[i].p.z +=  M[i].v.z*delta_t;

    acceleration = calc_acceleration(M,i);

    /* We restore the previous values, since we just wanted to get an estimate for the acceleration at (t+dt, vel+dv)*/
    M[i].p = tmp_pos;
    M[i].v = tmp_vel;
    return acceleration;
  }

  VECTOR_3D_T k1, k2, k3, k4, temp_acc;
  SDL_Event event;
  FONT_T my_font;
  char text[40];
  REAL_T t;
  REAL_T dt;
  VIEWPORT_2D_T win;
  REAL_T xp, yp;
  SINT32_T step_counter=0;
  SINT8_T flag_paused=FALSE;
  const SINT8_T skip_steps=50;

  lb_gr_SDL_init("CASE_N_BODY_PROBLEM", SDL_INIT_VIDEO, 1600, 1200, 0xFF, 0xFF, 0xFF);
   
  win.xp_min=0;
  win.yp_min=0;
  win.xp_max=ty_screen.w;
  win.yp_max=ty_screen.h;
  win.xr_max= 1.5*1.496e11;
  win.xr_min=-win.xr_max;
  win.yr_min=-win.xr_max*ty_screen.h/ty_screen.w;
  win.yr_max=-win.yr_min; 


  lb_ft_load_GLCDfont("./fonts/Font_hp48G_large.lcd", &my_font);
  my_font.scale_x=3;
  my_font.scale_y=3;
  my_font.gap_x=2;
  my_font.gap_y=1;
  my_font.max_x=40;
  my_font.angle=0;
  my_font.flag_fg=TRUE;
  my_font.flag_bg=TRUE;
  my_font.color_fg=lb_gr_12RGB(COLOR_BLACK);
  my_font.color_bg=lb_gr_12RGB(COLOR_WHITE);

      
  dt=60.0;  /* seconds */
  t=0;      /* tracks the elapsed time */

  
  /* Initialize values for Euler Matrix */
  M_euler[0].m=1.9891e30;
 
  M_euler[0].p.x=0;
  M_euler[0].p.y=0;
  M_euler[0].p.z=0;

  M_euler[0].v.x=0;
  M_euler[0].v.y=0;
  M_euler[0].v.z=0;

  M_euler[0].v_temp.x=0;
  M_euler[0].v_temp.y=0;
  M_euler[0].v_temp.z=0;

  M_euler[0].a_temp.x=0;
  M_euler[0].a_temp.y=0;
  M_euler[0].a_temp.z=0;


  /* Second object */
  M_euler[1].m=5.9737e24;

  M_euler[1].p.x=1.496e11;
  M_euler[1].p.y=0;
  M_euler[1].p.z=0;

  M_euler[1].v.x=0;
  //M_euler[1].v.y=29785.6783; /* m/s */;
  M_euler[1].v.y=sqrt(G*(M_euler[0].m+M_euler[1].m)/1.496e11);
  M_euler[1].v.z=0;

  M_euler[1].v_temp.x=0;
  M_euler[1].v_temp.y=0;
  M_euler[1].v_temp.z=0;

  M_euler[1].a_temp.x=0;
  M_euler[1].a_temp.y=0;
  M_euler[1].a_temp.z=0;


  /* Initialize values for Runge-Kutta-4 Matrix (make them the same) */
  
  for (i=0;i<N_OBJECTS;i++)
    {
      M_rk4[i].p.x = M_euler[i].p.x;
      M_rk4[i].p.y = M_euler[i].p.y;
      M_rk4[i].p.z = M_euler[i].p.z;

      M_rk4[i].v.x = M_euler[i].v.x;
      M_rk4[i].v.y = M_euler[i].v.y;
      M_rk4[i].v.z = M_euler[i].v.z;

      M_rk4[i].v_temp.x = M_euler[i].v_temp.x;
      M_rk4[i].v_temp.y = M_euler[i].v_temp.y;
      M_rk4[i].v_temp.z = M_euler[i].v_temp.z;

      M_rk4[i].v_temp.x = M_euler[i].v_temp.x;
      M_rk4[i].v_temp.y = M_euler[i].v_temp.y;
      M_rk4[i].v_temp.z = M_euler[i].v_temp.z;

      M_rk4[i].m = M_euler[i].m;
    }

  flag_paused=TRUE;
  while (1)
    {
      if (!flag_paused)
	{
	  for (i=0;i<N_OBJECTS;i++)
	    {
	      M_euler[i].v_temp = M_euler[i].v;
	      M_euler[i].a_temp = calc_acceleration(M_euler,i);

	      
	      M_rk4[i].v_temp = M_rk4[i].v;

	      temp_acc = calc_acceleration(M_rk4, i);
	      //temp_acc = future_acc(M_rk4, i , 0,0,0,0);
	      k1.x = temp_acc.x*dt;
	      k1.y = temp_acc.y*dt;
	      k1.z = temp_acc.z*dt;
	      
	      temp_acc = future_acc(M_rk4, i ,0.5*dt , 0.5*k1.x, 0.5*k1.y, 0.5*k1.z);
	      k2.x = temp_acc.x*dt;
	      k2.y = temp_acc.y*dt;
	      k2.z = temp_acc.z*dt;
	      
	      temp_acc = future_acc(M_rk4, i ,0.5*dt ,0.5*k2.x, 0.5*k2.y, 0.5*k2.z);
	      k3.x = temp_acc.x*dt;
	      k3.y = temp_acc.y*dt;
	      k3.z = temp_acc.z*dt;

	      temp_acc = future_acc(M_rk4, i ,dt, k3.x, k3.y, k3.z);
	      k4.x = temp_acc.x*dt;
	      k4.y = temp_acc.y*dt;
	      k4.z = temp_acc.z*dt;

	      M_rk4[i].a_temp.x = (k1.x + 2*k2.x + 2*k3.x + k4.x)/(dt*6.0);
	      M_rk4[i].a_temp.y = (k1.y + 2*k2.y + 2*k3.y + k4.y)/(dt*6.0);
	      M_rk4[i].a_temp.z = (k1.z + 2*k2.z + 2*k3.z + k4.z)/(dt*6.0);
	    }
    
	  for (i=0;i<N_OBJECTS;i++)
	    {
	      /* Euler Integration */
	      M_euler[i].p.x += M_euler[i].v_temp.x*dt;
	      M_euler[i].p.y += M_euler[i].v_temp.y*dt;
	      M_euler[i].p.z += M_euler[i].v_temp.z*dt;
 
	      M_euler[i].v.x += M_euler[i].a_temp.x*dt;
	      M_euler[i].v.y += M_euler[i].a_temp.y*dt;
	      M_euler[i].v.z += M_euler[i].a_temp.z*dt;

	      /* Runge-Kutta 4 Integration */  
	      M_rk4[i].p.x += M_rk4[i].v_temp.x*dt;  
	      M_rk4[i].p.y += M_rk4[i].v_temp.y*dt; 
	      M_rk4[i].p.z += M_rk4[i].v_temp.z*dt; 

	      M_rk4[i].v.x += M_rk4[i].a_temp.x*dt;
	      M_rk4[i].v.y += M_rk4[i].a_temp.y*dt;
	      M_rk4[i].v.z += M_rk4[i].a_temp.z*dt;
	  
	      if (i==1) 
		{
		  if ( !(step_counter % (UINT64_T)(365.0*24.0*3600.0/dt)) && (step_counter >= (UINT64_T)(300*365.0*24.0*3600.0/dt))   )
		    {
		      flag_paused=TRUE;

		      REAL_T angle;
		      angle=2*M_PI*t/(365.0*24.0*3600.0);
		      lb_gr_project_2d(win, 1.496e11*cos(angle),1.496e11*sin(angle), &xp, &yp);
		      lb_gr_draw_circle_antialiasing(NULL, xp, yp, 10, 3, lb_gr_12RGB(0x0000));

		      
		      lb_gr_project_2d(win, M_euler[i].p.x, M_euler[i].p.y, &xp, &yp);
		      lb_gr_draw_rectangle_line(NULL, xp-15, yp-15, xp+15, yp+15, 2, lb_gr_12RGB(0xf07f),COPYMODE_COPY); 
		
		      lb_gr_project_2d(win, M_rk4[i].p.x, M_rk4[i].p.y, &xp, &yp);
		      lb_gr_draw_line(NULL, xp-12, yp-12, xp+12, yp+12, 4, lb_gr_12RGB(0xFF33),COPYMODE_COPY, LINEMODE_FILTERED); 
		      lb_gr_draw_line(NULL, xp+12, yp-12, xp-12, yp+12, 4, lb_gr_12RGB(0xFF33),COPYMODE_COPY, LINEMODE_FILTERED); 

		      lb_gr_BMPfile_save("orbits.bmp", NULL);

		      //lb_gr_project_2d(win, 1.496e11, 0, &xp, &yp);
		      //lb_gr_draw_circle_antialiasing(NULL, ty_screen.w/2, ty_screen.h/2,
		      //				     fabs(ty_screen.w/2-xp), 2, lb_gr_12RGB(COLOR_BLACK));
		    }
		  if ((step_counter % skip_steps) == 0)
		    {
		      sprintf(text,"dt: %02.2f [s]",dt);
		      lb_ft_draw_text(NULL, &my_font, 20, 30, text, COPYMODE_COPY);

		      sprintf(text,"t: %02.2f [a]",t/(24.0*365.0*3600.0));
		      lb_ft_draw_text(NULL, &my_font, 20, 80, text, COPYMODE_COPY);

		      sprintf(text,"n: %0ld",step_counter);
		      lb_ft_draw_text(NULL, &my_font, 20, 130, text, COPYMODE_COPY);

		      //printf("EU: t=%4.2f: %4.5e, %4.5e, %4.5e",t/(3600*24), M_euler[i].p.x, M_euler[i].p.y, M_euler[i].p.z);
		      //printf("\nRK: t=%4.2f: %4.5e, %4.5e, %4.5e",t/(3600*24), M_rk4[i].p.x,   M_rk4[i].p.y,   M_rk4[i].p.z);
		      //printf("\n");

		      SINT32_T flag_dashed;
		      flag_dashed= (step_counter/2000) % 8;

		      REAL_T angle;
		      angle=2*M_PI*t/(365.0*24.0*3600.0);
		      lb_gr_project_2d(win, 1.496e11*cos(angle),1.496e11*sin(angle), &xp, &yp);
		      //	      lb_gr_draw_circle_filled(NULL, xp, yp, 3, lb_gr_12RGB(0xF33F), COPYMODE_BLEND);
		      lb_gr_draw_circle_filled(NULL, xp, yp, 1, lb_gr_12RGB(0xF000), COPYMODE_COPY);

		      if( flag_dashed <=1)
			{
			  //lb_gr_plot2d(NULL, win, M_euler[i].p.x, M_euler[i].p.y, 3, lb_gr_12RGB(0xFF33), COPYMODE_BLEND, LINEMODE_DOTS_SOLID);
			  lb_gr_project_2d(win, M_euler[i].p.x, M_euler[i].p.y, &xp, &yp);
			  //lb_gr_draw_circle_filled(NULL, xp, yp, 2, lb_gr_12RGB(COLOR_BLUE), COPYMODE_COPY);
			  lb_gr_draw_circle_filled(NULL, xp, yp, 4, lb_gr_12RGB(0xf07f),COPYMODE_COPY); 
			}

		      if( (flag_dashed == 4) || (flag_dashed ==5) )
			{
			  //lb_gr_plot2d(NULL, win, M_rk4[i].p.x, M_rk4[i].p.y, 8, lb_gr_12RGB(COLOR_GREEN), COPYMODE_COPY, LINEMODE_DOTS_FILTERED);
			  lb_gr_project_2d(win, M_rk4[i].p.x, M_rk4[i].p.y, &xp, &yp);
			  //lb_gr_draw_circle_filled(NULL, xp, yp, 3, lb_gr_12RGB(0xF3F3), COPYMODE_BLEND);
			  lb_gr_draw_circle_filled(NULL, xp, yp, 4, lb_gr_12RGB(0xFf33), COPYMODE_COPY);
			}

		      if (!(step_counter % (UINT64_T)(0.25*365.0*24.0*3600.0/dt)))
			{
			  REAL_T xreal, yreal;
			  
			  angle=2*M_PI*t/(365.0*24.0*3600.0);
			  xreal=1.496e11*cos(angle);
			  yreal=1.496e11*sin(angle);

			  printf("t=%f EU: e= %4.9f  RK4: e=%f%% \r\n",
				 t/(365.0*24.0*3600.0),
				 100.0*fabs(sqrt(   (xreal-M_euler[i].p.x)*(xreal-M_euler[i].p.x) + (yreal-M_euler[i].p.y)*(yreal-M_euler[i].p.y)))/sqrt(xreal*xreal+yreal*yreal),
				 100.0*fabs(sqrt(   (xreal-M_rk4[i].p.x)*(xreal-M_rk4[i].p.x) + (yreal-M_rk4[i].p.y)*(yreal-M_rk4[i].p.y)))/sqrt(xreal*xreal+yreal*yreal));
			}
		      //printf("\n RK: r= %4.9f %%", 100*fabs(sqrt(M_rk4[i].p.x*M_rk4[i].p.x+M_rk4[i].p.y*M_rk4[i].p.y)-1.496e11)/1.496e11);
		      //printf("\n\n");
		      //delay(200);
		      lb_gr_refresh(&ty_screen);
		    }
		}
	    }
	  t=t+dt;
	  step_counter++;
	}
      while (SDL_PollEvent(&event))
	{
	  if (event.type == SDL_QUIT)
	    {
	      SDL_Quit();
	      return EXIT_SUCCESS;
	    }
	  if (event.type== SDL_KEYDOWN)
	    {
	      if (event.key.keysym.sym == SDLK_SPACE)
		{
		  if (flag_paused)
		    {
		      flag_paused=FALSE;
		      lb_gr_clear_picture(NULL, lb_gr_12RGB(COLOR_WHITE));
		    }

		  else
		    flag_paused=TRUE;
		}
	    }
	}
    }
  lb_gr_SDL_close();
#endif

  /******************************************************************************/
  /* Demo: Basic 3D plotting                                                    */
  /* This example show how to generate simple wireframe graphics.               */
  /******************************************************************************/

  //#define DEMO_PLOT3D_BASIC
#ifdef DEMO_PLOT3D_BASIC
  SDL_Event event;

  SINT16_T j;
  VIEWPORT_3D_T vp3d;
  REAL_T u_a, u_b, v_a, v_b;
  REAL_T Rot[3][3], Rx_p[3][3], Rx_n[3][3], Ry_p[3][3], Ry_n[3][3], Rz_p[3][3], Rz_n[3][3], RotTranspose[3][3], temp[3][3];
  SINT8_T flag_exit, flag_paused=FALSE;
  MATRIX_POINT_3D_REAL_T S;
  FONT_T my_font;

  flag_exit=FALSE;

  lb_gr_SDL_init("DEMO_PLOT3D_BASIC", SDL_INIT_VIDEO, 0,0, 0, 0, 0);

  lb_ft_load_GLCDfont("fonts/Font_hp48G_large.lcd", &my_font);
  //lb_ft_load_GLCDfont("fonts/Font_hp48G_small.lcd", &my_font);
  my_font.scale_x=4;
  my_font.scale_y=3;
  my_font.gap_x=2;
  my_font.gap_y=1;
  my_font.max_x=40;
  my_font.angle=0;
  my_font.flag_fg=TRUE;
  my_font.flag_bg=FALSE;
  my_font.color_fg=lb_gr_12RGB(COLOR_SOLID | COLOR_BLACK);
  my_font.color_bg=lb_gr_12RGB(COLOR_BLACK);
 
  vp3d.xp_min=0;
  vp3d.yp_min=0;
  vp3d.xp_max=ty_screen.w;
  vp3d.yp_max=ty_screen.h;
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
  S.rows=20;
  S.cols=20;
  lb_al_create_matrix_p3d(&S);
  u_a=0.0;
  u_b= M_PI/4;
  v_a=0;
  v_b=5.0;

  for (i=0;i<S.rows;i++)
    for (j=0;j<S.cols;j++)
      {
	S.array[i][j].x=(j*(v_b-v_a)/(S.cols-1))*cos(u_a + i*(u_b-u_a)/(S.rows-1));
	S.array[i][j].y=(j*(v_b-v_a)/(S.cols-1))*sin(u_a + i*(u_b-u_a)/(S.rows-1));;
	//S.array[i][j].z=sin(S.array[i][j].x*S.array[i][j].x+S.array[i][j].y*S.array[i][j].y)/5;
	S.array[i][j].z=(S.array[i][j].x*S.array[i][j].x+S.array[i][j].y*S.array[i][j].y)/10.0;
	//printf("[%f\t%f\t%f]\r\n",S.array[i][j].x,S.array[i][j].y,S.array[i][j].z); 
      }

  while (!flag_exit)
    {
      lb_gr_clear_picture(NULL,lb_gr_12RGB(COLOR_WHITE));
      lb_gr_plot3d_surface(NULL, vp3d, Rot, &S,
			   3.5, lb_gr_12RGB(COLOR_BLUE), COPYMODE_BLEND, LINEMODE_FILTERED);

      //lb_ft_draw_text(NULL, &my_font, 20, 20, "Hello", COPYMODE_COPY);


      vp3d.cam_d=0;

      lg_gr_draw_axis_3d(NULL, vp3d, Rot, &my_font,
			 5.0, 5.0, lb_gr_12RGB(COLOR_SOLID | COLOR_BLACK),
			 0, 5.0, 1.0,
			 0, 5.0, 1.0,
			 0, 1.0, 1.0,
			 3.0, lb_gr_12RGB(0xF000),
			 0*AXIS_DRAW_X_GRID | 0*AXIS_DRAW_Y_GRID | 0*AXIS_DRAW_Z_GRID,
			 "O","X","Y","Z",
			 COPYMODE_BLEND, LINEMODE_FILTERED);
      lb_gr_refresh(&ty_screen);
	
      // Wait for the user to press a character.
      flag_paused=TRUE;
      while (flag_paused && !flag_exit)
	{
	  while (SDL_PollEvent(&event))
	    {
	      if (event.type == SDL_QUIT)
		flag_exit=TRUE;
	      if (event.type== SDL_KEYDOWN)
		{
		  flag_paused=FALSE;
		   
		  switch(event.key.keysym.sym)
		    {
		    case SDLK_p:
		      lb_gr_BMPfile_save("3d_plot.bmp", NULL);
		      break;
		    case SDLK_r:
		      vp3d.xp_min=0;
		      vp3d.yp_min=0;
		      vp3d.xp_max=ty_screen.w;
		      vp3d.yp_max=ty_screen.h;
		      vp3d.scale =100.0;    /* Zoom */
		      vp3d.cam_d= 0.0;   /* Stereoscopic */
		      vp3d.cam_h=10.0;    /* Depth */
		      vp3d.cam.x=0.0;   /* Camera's location */
		      vp3d.cam.y=0.0;   /* Camera's location */
		      vp3d.cam.z=20.0;   /* Camera's location */
		      break;
		    case SDLK_h:
		      if (event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT) )
			vp3d.cam_h*=1.1;
		      else
			vp3d.cam_h/=1.1;
		      break;
		    case SDLK_x:
		      if (event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT) )
			lb_al_multiply_matrix33_r_copy(Rot,Rx_p,Rot);
		      else
			lb_al_multiply_matrix33_r_copy(Rot,Rx_n,Rot);

		      lb_al_print_matrix33_r(Rot,"Rot",FLOAT_FORMAT_MATRIX);
		      lb_al_transpose_matrix33_r(Rot,RotTranspose);
		      lb_al_multiply_matrix33_r(Rot,RotTranspose,temp);
		      lb_al_print_matrix33_r(temp,"temp",FLOAT_FORMAT_MATRIX);
		      printf("Determinant = %f\r\n",lb_al_determinant_matrix33_r(Rot));
		      break;
		    case SDLK_y:
		      if (event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT) )
			lb_al_multiply_matrix33_r_copy(Rot,Ry_p,Rot);
		      else
			lb_al_multiply_matrix33_r_copy(Rot,Ry_n,Rot);
		      lb_al_print_matrix33_r(Rot,"Rot",FLOAT_FORMAT_MATRIX);
		      break;
		    case SDLK_z:
		      if (event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT) )
			lb_al_multiply_matrix33_r_copy(Rot,Rz_p,Rot);
		      else
			lb_al_multiply_matrix33_r_copy(Rot,Rz_n,Rot);
		      lb_al_print_matrix33_r(Rot,"Rot",FLOAT_FORMAT_MATRIX);
		      break;
		    case SDLK_PAGEUP:
		      vp3d.scale*=1.1;
		      break;
		    case SDLK_PAGEDOWN:
		      vp3d.scale/=1.1;
		      break;
		    case SDLK_UP:
		      vp3d.cam.y+=1.0;
		      break;
		    case SDLK_DOWN:
		      vp3d.cam.y-=1.0;
		      break;
		    case SDLK_LEFT:
		      vp3d.cam.x+=1.0;
		      break;
		    case SDLK_RIGHT:
		      vp3d.cam.x-=1.0;
		      break;
		    case SDLK_INSERT:
		      vp3d.cam.z+=1.0;
		      break;
		    case SDLK_DELETE:
		      vp3d.cam.z-=1.0;
		      break;
		    case SDLK_ESCAPE:
		      flag_exit=TRUE;
		      break;
		    case SDLK_SPACE:
		      if (flag_paused)
			flag_paused=FALSE;
		      else
			flag_paused=TRUE;
		      break;
		    }
		}
	    }
	}
    }
  SDL_Quit();
  lb_al_release_matrix_p3d(&S);
  lb_ft_release_GLCDfont(&my_font);

#endif

  /******************************************************************************/
  /* Experiment: z-buffer 3D graphics                                           */
  /* The z-buffer method is the basis for a number of methods for hidden        */
  /* surface removal.                                                           */
  /* No complex polyedra have yet been implemented and there are limitations:   */
  /* for example, only triangles can be drawn. No complex polyedra have yet     */
  /* been implemented.  There are are other limitations: if any of the vertices */
  /* of a triangle leaves the window, the equation of the plane defined by them */
  /* will be invalid, and since no error checks are made, it can give any error */
  /* including segmentation faults.                                             */
  /* This can be taken as an unfinished experiment.                             */
  /******************************************************************************/

  //#define DEMO_PLOT3D
#ifdef DEMO_PLOT3D
  SDL_Event event;
  PICTURE_T Pic;
  FONT_T my_font;
    
  VIEWPORT_3D_T vp3d;
  //REAL_T u_a, u_b, v_a, v_b;
  REAL_T Rot[3][3], Rx_p[3][3], Rx_n[3][3], Ry_p[3][3], Ry_n[3][3], Rz_p[3][3], Rz_n[3][3];
  SINT8_T flag_exit;
    
  //MATRIX_POINT_3D_REAL_T S;
  MATRIX_R_T Z;
 
  FONT_T font_console;
  SINT8_T flag_paused;

  lb_gr_SDL_init("DEMO_PLOT3D", SDL_INIT_VIDEO, 800,600, 0, 0, 0);

  lb_ft_load_GLCDfont("fonts/Font_hp48G_large.lcd", &my_font);
  //lb_ft_load_GLCDfont("fonts/Font_hp48G_small.lcd", &my_font);
  my_font.scale_x=4;
  my_font.scale_y=3;
  my_font.gap_x=2;
  my_font.gap_y=1;
  my_font.max_x=40;
  my_font.angle=0;
  my_font.flag_fg=TRUE;
  my_font.flag_bg=FALSE;
  my_font.color_fg=lb_gr_12RGB(COLOR_SOLID | COLOR_BLACK);
  my_font.color_bg=lb_gr_12RGB(COLOR_BLACK);
   
  Pic.w=ty_screen.w;
  Pic.h=ty_screen.h;
  lb_gr_create_picture(&Pic,lb_gr_12RGB(COLOR_RED | COLOR_SOLID));
    
  flag_exit=FALSE;
      
  vp3d.xp_min=0;
  vp3d.yp_min=0;
  vp3d.xp_max=Pic.w;
  vp3d.yp_max=Pic.h;
  vp3d.scale =200.0;    /* Zoom */
  vp3d.cam_d= 0.0;      /* Stereoscopic */
  vp3d.cam_h=10.0;      /* Depth */
  vp3d.cam.x=0.0;       /* Camera's location */
  vp3d.cam.y=0.0;       /* Camera's location */
  vp3d.cam.z=40.0;      /* Camera's location */
      
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

      lb_gr_plot_zbuffer_line_1(&Pic, vp3d, Rot, &Z, (POINT_3D_REAL_T){0,0,0}, (POINT_3D_REAL_T){2,2,2},
				lb_gr_12RGB(COLOR_WHITE), COPYMODE_COPY);

      lb_gr_plot_zbuffer_line_1(&Pic, vp3d, Rot, &Z, (POINT_3D_REAL_T){1,0,0}, (POINT_3D_REAL_T){0,1,0},
				lb_gr_12RGB(COLOR_BLUE), COPYMODE_COPY);

      lb_gr_plot_zbuffer_triangle(&Pic, vp3d, Rot, &Z, (POINT_3D_REAL_T){0,0,1}, (POINT_3D_REAL_T){0,3,1},
				  (POINT_3D_REAL_T){3,0,1}, lb_gr_12RGB(COLOR_SOLID | COLOR_FORESTGREEN), COPYMODE_BLEND);

      lb_gr_plot_zbuffer_triangle(&Pic, vp3d, Rot, &Z, (POINT_3D_REAL_T){1,0,0}, (POINT_3D_REAL_T){1,2,0},
				  (POINT_3D_REAL_T){1,0,2}, lb_gr_12RGB(COLOR_SOLID | COLOR_YELLOW), COPYMODE_BLEND);

      lb_gr_plot_zbuffer_triangle(&Pic, vp3d, Rot, &Z, (POINT_3D_REAL_T){0,0,0}, (POINT_3D_REAL_T){0,5,0},
				  (POINT_3D_REAL_T){5,0,0}, lb_gr_12RGB(COLOR_SOLID | 0xa00), COPYMODE_BLEND);

      lb_gr_plot_zbuffer_triangle(&Pic, vp3d, Rot, &Z, (POINT_3D_REAL_T){5,5,0}, (POINT_3D_REAL_T){0,5,0},
				  (POINT_3D_REAL_T){5,0,0}, lb_gr_12RGB(COLOR_SOLID | 0xa00), COPYMODE_BLEND);

      lb_gr_plot_zbuffer_triangle(&Pic, vp3d, Rot, &Z, (POINT_3D_REAL_T){0,0,0}, (POINT_3D_REAL_T){1,1,1},
				  (POINT_3D_REAL_T){2,2,2}, lb_gr_12RGB(COLOR_SOLID | 0xF00), COPYMODE_BLEND);

	
      vp3d.cam_d=0;

      lg_gr_draw_axis_3d(&Pic, vp3d, Rot, &font_console,
			 5.0, 4.0, lb_gr_12RGB(COLOR_WHITE),
			 0, 5.0, 1.0,
			 0, 5.0, 1.0,
			 0, 1.0, 1.0,
			 2.0, lb_gr_12RGB(0xF911),
			 0*AXIS_DRAW_X_GRID | 0*AXIS_DRAW_Y_GRID | 0*AXIS_DRAW_Z_GRID,
			 "O","X","Y","Z",
			 COPYMODE_BLEND, LINEMODE_FILTERED);

      lb_gr_render_picture(&Pic, &ty_screen, 0, 0, COPYMODE_COPY, 0);
      lb_gr_refresh(&ty_screen);

      flag_paused=TRUE;
      while (flag_paused && !flag_exit)
	{
	  while (SDL_PollEvent(&event))
	    {
	      if (event.type == SDL_QUIT)
		flag_exit=TRUE;
	      if (event.type== SDL_KEYDOWN)
		{
		  flag_paused=FALSE;
		   
		  switch(event.key.keysym.sym)
		    {
		    case SDLK_p:
		      lb_gr_BMPfile_save("3d_plot.bmp", NULL);
		      break;
		    case SDLK_r:
		      vp3d.xp_min=0;
		      vp3d.yp_min=0;
		      vp3d.xp_max=ty_screen.w;
		      vp3d.yp_max=ty_screen.h;
		      vp3d.scale =100.0;    /* Zoom */
		      vp3d.cam_d= 0.0;   /* Stereoscopic */
		      vp3d.cam_h=10.0;    /* Depth */
		      vp3d.cam.x=0.0;   /* Camera's location */
		      vp3d.cam.y=0.0;   /* Camera's location */
		      vp3d.cam.z=20.0;   /* Camera's location */
		      break;
		    case SDLK_h:
		      if (event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT) )
			vp3d.cam_h*=1.1;
		      else
			vp3d.cam_h/=1.1;
		      break;
		    case SDLK_x:
		      if (event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT) )
			lb_al_multiply_matrix33_r_copy(Rot,Rx_p,Rot);
		      else
			lb_al_multiply_matrix33_r_copy(Rot,Rx_n,Rot);

		      lb_al_print_matrix33_r(Rot,"Rot",FLOAT_FORMAT_MATRIX);
		      break;
		    case SDLK_y:
		      if (event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT) )
			lb_al_multiply_matrix33_r_copy(Rot,Ry_p,Rot);
		      else
			lb_al_multiply_matrix33_r_copy(Rot,Ry_n,Rot);
		      lb_al_print_matrix33_r(Rot,"Rot",FLOAT_FORMAT_MATRIX);
		      break;
		    case SDLK_z:
		      if (event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT) )
			lb_al_multiply_matrix33_r_copy(Rot,Rz_p,Rot);
		      else
			lb_al_multiply_matrix33_r_copy(Rot,Rz_n,Rot);
		      lb_al_print_matrix33_r(Rot,"Rot",FLOAT_FORMAT_MATRIX);
		      break;
		    case SDLK_PAGEUP:
		      vp3d.scale*=1.1;
		      break;
		    case SDLK_PAGEDOWN:
		      vp3d.scale/=1.1;
		      break;
		    case SDLK_UP:
		      vp3d.cam.y+=1.0;
		      break;
		    case SDLK_DOWN:
		      vp3d.cam.y-=1.0;
		      break;
		    case SDLK_LEFT:
		      vp3d.cam.x+=1.0;
		      break;
		    case SDLK_RIGHT:
		      vp3d.cam.x-=1.0;
		      break;
		    case SDLK_INSERT:
		      vp3d.cam.z+=1.0;
		      break;
		    case SDLK_DELETE:
		      vp3d.cam.z-=1.0;
		      break;
		    case SDLK_ESCAPE:
		      flag_exit=TRUE;
		      break;
		    case SDLK_SPACE:
		      if (flag_paused)
			flag_paused=FALSE;
		      else
			flag_paused=TRUE;
		      break;
		    }
		}
	    }
	}
    }
  lb_gr_release_picture(&Pic);
  lb_ft_release_GLCDfont(&my_font);
  SDL_Quit();
#endif

  /******************************************************************************/
  /* Demo: Fonts                                                                */
  /* Fonts are used to print data to a graphic.                                 */
  /******************************************************************************/

  //#define DEMO_NEW_FONT
#ifdef DEMO_NEW_FONT
  SDL_Event event;
  FONT_T my_font;
  char text[40];
  REAL_T x;

  lb_gr_SDL_init("DEMO_NEW_FONT", SDL_INIT_VIDEO, 800,600, 0, 0, 0);
  
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
      lb_ft_draw_text(NULL, &my_font, ty_screen.w/2, ty_screen.h/2, text, COPYMODE_COPY);
      lb_gr_refresh(&ty_screen);
      lb_ti_delay_ms(200);
    } 

  strcpy(text,"hola \r\namigo");
  lb_ft_draw_text(NULL, &my_font, ty_screen.w/2, ty_screen.h/2, text, COPYMODE_COPY);
  lb_ft_draw_text_centered(NULL, &my_font, 20, 70, 200, 50,  text, COPYMODE_COPY);
  lb_gr_draw_rectangle(NULL, 20, 70, 200, 50, lb_gr_12RGB(COLOR_WHITE), COPYMODE_COPY);
  lb_ft_release_GLCDfont(&my_font);
  lb_gr_refresh(&ty_screen);
    
  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
 	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif

    
  /******************************************************************************/
  /* Demo: Financial                                                            */
  /* Las tasas de interés para hipotecas en Canadá las especifican en una forma */
  /* curiosa: los bancos no la anuncian como una tasa efectiva anual ni como    */
  /* una tasa nominal.  Se trata de una tasa semestral sin reinversión de       */
  /* intereses.                                                                 */
  /* Por ejemplo, si el banco dice: la tasa APR (Advanced Percentage Rate)      */
  /* es del 6%.  Quiere decir: Tasa efectiva anual = (1.0 + (1/2)*6/100)^2      */                                    /*  =  1.0609                                                                 */                                    /******************************************************************************/
  
  //#define DEMO_FINANCIAL
#ifdef DEMO_FINANCIAL
  MATHERROR_T e_code;
  REAL_T apr, monthly;
  apr=6.00;
  monthly=lb_re_APR_to_monthly(apr, &e_code);
  printf("APR = %.4f %% half yearly, not in advanced corresponds to %.4f %% monthly\r\n",
	 apr,monthly);
  printf("monthly = %.4f %% corresponds to %.4f %% effective yearly\r\n",
	 monthly,lb_re_monthly_to_effective(monthly,&e_code));
  printf("monthly = %.4f %% corresponds to %.4f %% APR\r\n",
	 monthly,lb_re_monthly_to_APR(monthly,&e_code));
  exit(1);
#endif

  /******************************************************************************/
  /* Demo: Function parsing - Benchmarking                                      */
  /******************************************************************************/

  //#define DEMO_PARSER_BENCHMARK
#ifdef DEMO_PARSER_BENCHMARK
  REAL_T vector[3], result, time_compiler, time_parser;
  clock_t begin, end;
  MATHERROR_T error=e_none;

  FN_RECORD_T fnrec;
  SINT16_T n_vars;


  /* Comparison with compiled code */
  REAL_T i;
  REAL_T sum;
  begin=clock();
  sum=0;
  for (i=1;i<10000000;i++)
    {
      sum+=pow(-1.0,i+1)/(2*i-1.0);
    }
  sum*=4;
  end=clock();
  time_compiler=(double)(end - begin) / CLOCKS_PER_SEC;

  printf("Result [Compiled code]= %f\r\n",sum);
  printf("Time elapsed [Compiled code]= %.4f seconds\n",time_compiler);
 
  
  vector[0]=0;
  vector[1]=0;
  vector[2]=0;

  lb_pa_parse("4*SIGMA(1,10000000,(-1)^(i+1)/(2*i-1),i,1)", &fnrec, "i", &n_vars);
  lb_pa_shunting_yard(&fnrec);
  
  begin=clock();
  result=lb_pa_eval_real(&fnrec, vector, &error);
  end=clock();
  time_parser=(double)(end - begin) / CLOCKS_PER_SEC;


  printf("Result [Parsed code]= %f\r\n",result);
  printf("Time elapsed [Parsed code]= %.4f seconds\n",time_parser);
  printf("Ratio parser/compiler = %.4f times\n",time_parser/time_compiler);
#endif

  /******************************************************************************/
  /* Demo: Function parsing (Real Variable)                                     */
  /* Any of the expressions commenetd our below can be tried                    */
  /******************************************************************************/

  //  #define DEMO_PARSER_REAL
#ifdef DEMO_PARSER_REAL
  MATHERROR_T e_code;
  REAL_T x,y,z,result;
  char variables[]="x,y,i"; 
 
   x=1;
  y=M_PI*60/180;
  z=3;  
    
      
  /* Example: pi as an infinite addition - Machin */
  char expression[]="4*SIGMA(1,10^7,(-1)^(i+1)/(2*i-1),i,1)";   
   e_code=e_none;
  printf("Formula: %s\r\n",expression);
  printf(" Variables: %s\r\n", variables);
  printf(" Values: %0.4f y=%0.4f z=%0.4f\r\n", x,y,z);

  result=lb_pa_formula(expression,variables,x,y,z,&e_code);
  printf("Error: %s\r\n", errors[e_code].msg);
  printf("Result=%0.9f\r\n",result);
 
  
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
  //char expression[]="idef2(1,5,0,2,x^2*y^3,x,y,5)"; 

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
  // char expression[]="sin(1+y*cos(3+pi/4))";   
  //char expression[]="1+2-avg(x,2,3,4,5,6,7,8)+10*sin(pi/4)+1000+2000+1+2+3";   
  //char expression[]="100+250+10*sin(pi/4)+1000+2000+sin(pi/4)";   
  //char expression[]="sin(sin(1,2,3,4,5)+2+3,sin(1,2,3,4,5,6),sin(1))"; 
  //char expression[]="avg(10,20)";
#endif

  /******************************************************************************/
  /* Demo: Function parsing (Complex Variable)                                  */
  /* Any of the expressions commented our below can be tried                    */
  /******************************************************************************/
  
  //#define DEMO_PARSER_COMPLEX
#ifdef DEMO_PARSER_COMPLEX 
  MATHERROR_T e_code;
  COMPLEX_T var1,var2,imag,result;
  var1.r=0;
  var1.i=0;
  var2.r=0;
  var2.i=0;
  imag.r=0;
  imag.i=1;

  /* Example: pi as a product */
  char variables[]="x,k,i";
  char expression[]="2*PROD(1,10000,4*k^2/(4*k^2-1),k,1)";   
    
  /* Example: Gauss'childhood sum, using complex numbers  */
  //  char expression[]="sigma(100,1,0.5*k+k*i,K,-1)";   
  
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
  srand(0xabcdef);
  printf("Expression: %s\r\n",expression);
  printf(" Variables: %s\r\n", variables);
  printf(" Values: x=[%0.4f , %0.4f]  y=[%0.4f , %0.4f]\r\n", var1.r,var1.i,var2.r,var2.i);
     
  result=lb_pa_formula_complex(expression,variables,var1,var2,imag,&e_code);
  printf("Error: %s\r\n", errors[e_code].msg);
  printf("Result= [%0.8f , %0.8f] \r\n",result.r, result.i);
  exit(1);
#endif

  /******************************************************************************/
  /* Demo: Statistics: the ubiquitous Gauss bell from an Histogram              */
  /* Notes: In spite of its widespread use, it is very hard to find             */
  /* properly-drawn Histograms nor tools to correctly generate them.            */
  /* Even Excel won't generate by default a good Histogram and one needs to     */
  /* follow a number of special steps to create one.                            */
  /******************************************************************************/

  //#define DEMO_HISTOGRAM
#ifdef DEMO_HISTOGRAM
  SDL_Event event;
  VIEWPORT_2D_T win;
  VECTOR_R_T Data, Bins;
  FONT_T my_font;
  char text[40];
  SINT16_T i;
  REAL_T mu, sigma2;

  lb_gr_SDL_init("DEMO_HISTOGRAM", SDL_INIT_VIDEO, 1800, 1000, 0xFF, 0xFF, 0xFF);

  win.xp_min=0;
  win.xp_max=ty_screen.w;
  win.yp_min=0;
  win.yp_max=ty_screen.h;
  win.xr_min=-80;
  win.xr_max=80;
  win.yr_min=-30; 
  win.yr_max=300;
  
  /* Data generation */
  Data.items=6000;
  lb_al_create_vector_r(&Data);
  for(i=0;i<Data.items;i++)
    {
      Data.array[i]=lb_st_marsaglia_polar(30);
      //printf("Data[%d]=%f\r\n",i,Data.array[i]);
      //lb_ti_delay_ms(2);
    }
  Bins.items=50;
  lb_al_create_vector_r(&Bins);
  lb_st_histogram(&Data,&Bins, win.xr_min, win.xr_max,&mu, &sigma2);
  lb_al_print_vector_r(&Bins, "Bin", "%04.2f\r\n");

  printf("mu=%f,  sigma2=%f\r\n",mu,sigma2);
  lb_gr_draw_histogram(NULL, win, &Bins, 
		       lb_gr_12RGB(COLOR_DIMGRAY), lb_gr_12RGB(COLOR_WHITE),
		       lb_gr_12RGB(COLOR_BLACK), lb_gr_12RGB(COLOR_LIGHTBLUE));

  LINE_2D_REAL_T L;
  L.items=Bins.items;

  lb_gr_create_line2d_r(&L);

  REAL_T area_bins=0, area_normal=0, xr, yr;
  for (i=0;i<Bins.items;i++)
    {
      xr = win.xr_min+(REAL_T)i*(win.xr_max-win.xr_min)/Bins.items;
      yr = exp(-(xr-mu)*(xr-mu)/(2*sigma2))/sqrt(2*M_PI*sigma2);
      area_normal +=yr*(win.xr_max-win.xr_min)/Bins.items;
      area_bins += Bins.array[i]*(win.xr_max-win.xr_min)/Bins.items;
    }
  printf("Area_normal = %f\r\n",area_normal);
  printf("Area_bins/area_normal = %f\r\n",area_bins/area_normal);
  printf("Area_bins = %f\r\n",area_bins);
 
      
  for (i=0;i<Bins.items;i++)
    {
      xr = win.xr_min+i*(win.xr_max-win.xr_min)/Bins.items;
      yr = (area_bins/area_normal)*exp(-(xr-mu)*(xr-mu)/(2*sigma2))/sqrt(2*M_PI*sigma2);
      L.array[i].x=xr;
      L.array[i].y=yr;
    }

  lb_gr_plot2d_line(NULL, win, &L, 4, lb_gr_12RGB(0xF00a), COPYMODE_BLEND, LINEMODE_FILTERED);

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

  lb_gr_BMPfile_save("histogram.bmp", NULL);

  lb_gr_refresh(&ty_screen);
  
  lb_ft_release_GLCDfont(&my_font);
  lb_al_release_vector_r(&Data);

  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif

  /******************************************************************************/
  /* Demo: Prime numbers                                                        */
  /* There is nothing special about this demo. This is just a log-log           */ 
  /* plot of the first primes                                                   */
  /* Some off things about primes:                                              */
  /* - Other than 2 or 5, all primes end in 1, 3, 7 or 9.                       */
  /* - Prime numbers don't like repeating their last digit                      */
  /******************************************************************************/

  //#define DEMO_PRIMES
#ifdef DEMO_PRIMES
  SDL_Event event;
  VIEWPORT_2D_T win;
  FONT_T my_font;
  VECTOR_SINT16_T P;
  SINT32_T k,j;
  REAL_T x, y, xp, yp;

  lb_gr_SDL_init("DEMO_PRIMES", SDL_INIT_VIDEO, 800, 600, 0x1F, 0x1F, 0x1F);

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
  win.xp_max=ty_screen.w-5;
  win.yp_min=5;
  win.yp_max=ty_screen.h-5;

  win.xr_min= 0.0;
  win.xr_max=  10.0;
  win.yr_min=  0.0;
  win.yr_max= 15.0; 

  P.items=100;
  lb_al_create_vector_si16(&P);

  lb_sieve_erathostenes_2(&P, 32500);
  lb_al_print_vector_si16(&P,"Primes");

  j=0;
  for(k=1;k<10000000;k++)
    {
      if(lb_in_is_perfectsquare(k)==TRUE)
	{
	  printf("%ld\r\n",k);
	  j++;
	}
    }

  k=10000000000;
  printf("isqrt(%ld)=%ld\r\n",k,lb_in_isqrt(k));

  for (k=1;k<P.items;k++)
    {
      x=log(k);
      y=log(P.array[k]);
      lb_gr_project_2d(win, x, y, &xp, &yp);
      lb_gr_draw_pixel(NULL,round(xp),round(yp),lb_gr_12RGB(COLOR_BLUE),COPYMODE_COPY);
    }
  lb_gr_refresh(&ty_screen);
  lb_al_release_vector_si16(&P);

  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    lb_gr_SDL_close();
	    SDL_Quit();
	    return EXIT_SUCCESS;
	  }
      }
#endif

  /******************************************************************************/
  /* Numerical Methods                                                          */
  /******************************************************************************/

  /******************************************************************************/
  /* Demo: 2nd order Euler method  skeleton                                     */
  /* This is just a clean implementation of the Euler method for a 2nd order    */
  /* differential equations. No special features are used. This demo is         */
  /* being kept since it could be reused or adapted to build something else.    */
  /******************************************************************************/

  //  #define EULER_2ND_ORDER
#ifdef EULER_2ND_ORDER

  REAL_T f_x0_div_dt(REAL_T t, REAL_T _x0, REAL_T _x1)
  {
    return _x1;
  }
  
  REAL_T f_x1_div_dt(REAL_T t, REAL_T _x0, REAL_T _x1)
  {
    return -9.8;
  }

  REAL_T t, x0, x1, x0_0, x1_0, t_max, h, x0_copy, x1_copy;

  h=0.01;
  t_max=10;

  x0_0 = 10; /* Initial condition: position */
  x1_0 = 10; /* Initial condition: speed */

  x0 = x0_0;
  x1 = x1_0;
  
  t=0;
  while (t<t_max)
    {
      printf("t=%4.4f Euler [x, x']= [%4.4f , %4.4f], Analytic [x, x']= [%4.4f , %4.4f]\r\n",
	     t, x0, x1, x0_0+x1_0*t-0.5*9.8*t*t, x1_0-9.8*t);  
     
      x0_copy=f_x0_div_dt(t+h, x0, x1);
      x1_copy=f_x1_div_dt(t+h, x0, x1);
      x0 += h*x0_copy;
      x1 += h*x1_copy;

      t+=h;
      lb_ti_delay_ms(500);
    }
#endif

  /******************************************************************************/
  /* Demo: 1st order RK4 skeleton                                               */
  /* This is just a clean implementation of the RK4 method for a 1st order      */
  /* differential equation. No special features are used.                       */
  /******************************************************************************/

  //#define DEMO_RK4_FIRST_ORDER
#ifdef DEMO_RK4_FIRST_ORDER
  SDL_Event event;
  REAL_T fn(REAL_T x, REAL_T y, MATHERROR_T *error)
  {
    return -0.1*x;
  }
  
  REAL_T t_n, y_rk4, y_euler, xp, yp;
  VIEWPORT_2D_T win;
  MATHERROR_T error;

  lb_gr_SDL_init("DEMO_RK4_FIRST_ORDER", SDL_INIT_VIDEO, 800, 600, 0xfF, 0xfF, 0xfF);

  win.xp_min=0;
  win.yp_min=0;
  win.xp_max=ty_screen.w;
  win.yp_max=ty_screen.h;
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
      rk4(&t_n, &y_rk4, 0.1, &error, fn);
      rk4(&t_n, &y_euler, 0.1, &error, fn);
    }
  lb_gr_refresh(&ty_screen);
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


  /******************************************************************************/
  /* Demo: 2nd order RK4 skeleton                                               */
  /* This is just a clean implementation of the RK4 method for a 2nd order      */
  /* differential equations. No special features are used.  This demo is        */
  /* being kept since it could be reused or adapted to build something else.    */
  /* The example below solves the equation d2x/dt2 = -g   [g=9.8 m/s^2]         */
  /* This problem has an analytical solution, whose results are printed as well */
  /* for comparison.                                                            */
  /******************************************************************************/

  //#define RK4_2ND_ORDER
#ifdef RK4_2ND_ORDER

  REAL_T t, x0, x1, x0_0, x1_0, t_max, h, x0_copy, x1_copy, k1, k2, k3, k4;
  h=0.01;
  t_max=15;
  
  x0_0 = 10; /* Initial condition: position */
  x1_0 = 10; /* Initial condition: speed */

  x0 = x0_0;
  x1 = x1_0;
 
  REAL_T f_x0_div_dt(REAL_T t, REAL_T _x0, REAL_T _x1)
  {
    return _x1;
  }
  
  REAL_T f_x1_div_dt(REAL_T t, REAL_T _x0, REAL_T _x1)
  {
    return -9.8;
  }

  t=0;
  while (t<t_max)
    {
      printf("t=%4.4f RK4 [x, x']= [%4.4f , %4.4f], Analytic [x, x']= [%4.4f , %4.4f]\r\n",
	     t, x0, x1, x0_0+x1_0*t-0.5*9.8*t*t, x1_0-9.8*t);  
     
      x0_copy=x0;
      x1_copy=x1;

      k1 = h*f_x0_div_dt(t,         x0_copy,          x1_copy);
      k2 = h*f_x0_div_dt(t + 0.5*h, x0_copy + 0.5*k1, x1_copy);
      k3 = h*f_x0_div_dt(t + 0.5*h, x0_copy + 0.5*k2, x1_copy);
      k4 = h*f_x0_div_dt(t + h,     x0_copy + k3,     x1_copy);

      x0 = x0 + (k1 + 2*k2 + 2*k3 + k4)/6;  

      k1 = h*f_x1_div_dt(t,         x0_copy,  x1_copy          );
      k2 = h*f_x1_div_dt(t + 0.5*h, x0_copy , x1_copy + 0.5*k1 );
      k3 = h*f_x1_div_dt(t + 0.5*h, x0_copy , x1_copy + 0.5*k2 );
      k4 = h*f_x1_div_dt(t + h,     x0_copy , x1_copy + k3     );

      x1 = x1 + (k1 + 2*k2 + 2*k3 + k4)/6;  
      t+=h;
      lb_ti_delay_ms(500);
    }
#endif

  /******************************************************************************/
  /* Demo: 1st order RK4 General Method                                         */
  /* Long story made short: all the RK4 methods can be defined by a Matrix      */
  /* containing its coefficients.                                               */
  /* This skeletton can be improved, e.g, the function to be solved is          */
  /* and could be instead passed as a parameter.                                */
  /******************************************************************************/

  //#define DEMO_GENERAL_RUNGE_KUTTA
#ifdef DEMO_GENERAL_RUNGE_KUTTA
  SDL_Event event;
  VECTOR_R_T C, B, K;
  MATRIX_R_T A;
  SINT16_T i, j;
  REAL_T t_n, y_n, h, temp_t, temp_y, y_next, xp, yp;
  VIEWPORT_2D_T win;

  lb_gr_SDL_init("DEMO_GENERAL_RUNGE_KUTTA", SDL_INIT_VIDEO, 800, 600, 0x1F, 0x1F, 0x1F);
  
  win.xp_min=0;
  win.yp_min=0;
  win.xp_max=ty_screen.w;
  win.yp_max=ty_screen.h;
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
  lb_gr_refresh(&ty_screen);
  lb_al_release_matrix_r(&A);
  lb_al_release_vector_r(&B);
  lb_al_release_vector_r(&C);

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

  /******************************************************************************/
  /* Demo: Characterization of a Demodulation using a Montecarlo Analysis       */
  /* This Case Study is explained in detail in the Thesis Document              */
  /******************************************************************************/

  //#define CASE_DEMODULATION_MONTECARLO
#ifdef CASE_DEMODULATION_MONTECARLO
  VECTOR_R_T Signal, Noise_mag, Noise_phase;
  SDL_Event event;
  
  /* User-defined simulation parameters */
  const REAL_T freq_0=2000;        /* Frequency associated to one of the symbols */
  const REAL_T freq_1=3000;         /* Frequency associated to the other symbolsymbol */
  SINT32_T n_experiments; /* Maximum number of experiments. Not declared as a const to allow a possible
			     upgrade to variable-step size */
  REAL_T noise_variance=0.1;        /* The variance of the noise in a Gaussian model */
  REAL_T f_max_noise=6.0e3;  /* Maximum noise frequency */
  REAL_T N_exp_max=1.0;            /* Maximum (initial) amound of noise tried out during a series of experiments */
  REAL_T N_exp_min;         /* Minimum (final) amound of noise to be reached during a series of experiments */;
   
  /* Simulation variables */
  SINT32_T experiment;       /* Keeps track of the current experiment */
  SINT32_T errors_count_threshold_00, errors_count_threshold_01, errors_count_threshold_02;     /* Keeps track of the number of errors detected during a series of experiments */
  REAL_T N_experiment;        /* Current value of the noise energy during a series of experiments */
  REAL_T N_total;             /* Total energy of a frequency vector  */
  SINT8_T flag_running=TRUE; /* This variable will be true until it reaches any of the following conditions:
				a) The last series of experiments with the minimum noise level was completed.
				b) The maximum time allowed for running the simulation has been exceeded. 
				c) The user interrupted the simulation. */
  SINT8_T flag_draw_wave;   /* There isn't really any use to plot the waveform, except to give the user a rough
			       visual idea of the amount of noise and its effects. 
			       The waveform will be only ploted in a subset of the experiments. */

  REAL_T t_min=-0.25/freq_0;             /* Initial time for parsing the waveform */
  REAL_T t_max=0.5/freq_0+0.75/freq_1 ;  /* Final time for parsing the waveform */
  REAL_T t;
  REAL_T T_base;

  VIEWPORT_2D_T win_wave, win_BER;
  REAL_T xp, yp;                        /* Graphic variables */
  SINT16_T i, k;                       /* General-use counters */
 
  FONT_T my_font;
  char file_name[24];
  SINT8_T file_counter=0;

  
  REAL_T wave(REAL_T t, REAL_T f0, REAL_T f1) 
  {
    if (t<1.0/(2.0*f0)) 
      return sin(2*M_PI*f0*t);
    else
      return sin(2*M_PI*f1*(t-1.0/(2.0*f0)+1.0/(2.0*f1)));
  }

  REAL_T energy_signal(VECTOR_R_T *V)
  {
    REAL_T temp;
    UINT16_T j;

    temp=0.0;
    for(j=0;j<(*V).items;j++)
      temp+=(*V).array[j]*(*V).array[j];
    return 0.5*temp;
  } 

  SINT8_T parse_vector(VECTOR_R_T *buffer, REAL_T threshold_h, REAL_T threshold_l)
  {
    UINT16_T time_stamp[4];
   
    UINT16_T i, crossings_counter=0;
    REAL_T delta_0, delta_1, margin;

    delta_0=(*buffer).items*freq_1/(1.5*freq_0+1.5*freq_1);
    delta_1=(*buffer).items*freq_0/(1.5*freq_0+1.5*freq_1);
    margin=0.5*(delta_0-delta_1);
    
    
    //printf("delta_0=%f delta_1=%f\r\n",delta_0,delta_1);
    crossings_counter=0;
    i=0;
    while ((i<(*buffer).items-1) && (crossings_counter<=3))
      {
	if ( ( ((*buffer).array[i]<threshold_h) && ((*buffer).array[i+1]>=threshold_h) ) ||
	     ( ((*buffer).array[i]>threshold_l) && ((*buffer).array[i+1]<=threshold_l)) )
	  {
	    time_stamp[crossings_counter]=i;
	    crossings_counter++;
	    //	    printf("crossing # %d, crossing at = %i\r\n",crossings_counter,i);
	  }
	i++;
      }
    if ( ((time_stamp[1]-time_stamp[0])>= (delta_0-margin)) &&
	 ((time_stamp[1]-time_stamp[0])<  (delta_0+margin)) &&
	 ((time_stamp[2]-time_stamp[1])>  (delta_1-margin)) &&
	 ((time_stamp[2]-time_stamp[1])<  (delta_1+margin)) &&
	 (crossings_counter==3) )
      return 0; /* Success: bit 0 and bit 1 are valid */
    return 1; /* Failure: one of the bits was incorrectly decoded */
  }

  printf("Size of SINT64_T is =%d\r\n",(SINT8_T)sizeof(SINT64_T));
  printf("Size of UINT64_T is =%d\r\n",(SINT8_T)sizeof(UINT64_T));
  SINT64_T numero;
  numero=0xffffffffffffffff;
  printf("T is =%lld\r\n",numero);
  numero=0xffffffffffffffff;
  numero++;
  printf("T is =%lld\r\n",numero);

  lb_gr_SDL_init("BER vs Noise using a Montecarlo simulation", SDL_INIT_VIDEO, 1800, 1000, 0xFF, 0xFF, 0xFF);

  win_wave.xp_min=ty_screen.w*0.75+20;
  win_wave.xp_max=ty_screen.w-20;
  win_wave.yp_min=20;
  win_wave.yp_max=ty_screen.h-20;

  
  win_wave.xr_min=  t_min;
  win_wave.xr_max=  t_max;
  win_wave.yr_min= -2*sqrt(2);
  win_wave.yr_max=  2*sqrt(2);


  win_BER.xp_min=20;
  win_BER.xp_max=ty_screen.w*0.75-20;
  win_BER.yp_min=0+20;
  win_BER.yp_max=ty_screen.h-20;
      
  win_BER.xr_min=   0;
  win_BER.xr_max=  18;
  N_exp_min = 1.0/pow(10.0,18.0/10.0);
  win_BER.yr_min=  1.0;
  win_BER.yr_max=  1.0e-8;

  lg_gr_draw_axis_2d(NULL, win_BER, &my_font,
		     lb_gr_12RGB(COLOR_YELLOW), 3, 25,
		     lb_gr_12RGB(COLOR_BLACK), 1.0,
		     lb_gr_12RGB(COLOR_BLACK), 10, 1.0,
		     AXIS_DRAW_X | AXIS_DRAW_X_GRID |   
		     AXIS_DRAW_Y_GRID_LOG | AXIS_DRAW_COLORVALUES_Y_1,
		     COPYMODE_COPY, LINEMODE_SOLID); 


  //lb_gr_project_2d_y_log(win_BER, 0.1, &yp);
  //lb_gr_draw_circle_filled(NULL, 21, (int)yp, 8, lb_gr_12RGB(COLOR_YELLOW), COPYMODE_COPY);

      
  printf("t_mon = %f, t_max=%f\r\n", t_min, t_max);
      
  
  /* In order to nicely draw the waveform, a f(t) per pixel is desired */ 
  Signal.items=win_wave.xp_max - win_wave.xp_min;

  /* We determine the number of frequency coefficients which are required, given the bandwidth */

  T_base=1.0/(2.0*freq_0)+1.0/(2.0*freq_1);
  Noise_mag.items=(SINT16_T)ceil(f_max_noise*T_base);
  
  /* We make sure the size of the signal in the time domain is at least twice as large as the vector 
     holding the noise frequencies, due to the Shannon-Nyquist sampling theorem. */

  
  if (Signal.items<=2*Noise_mag.items)
    {
      printf("Error: more time samples are needed...\r\n");
      exit(EXIT_FAILURE);
    }
  printf("Noise vector has %d items\r\n",Noise_mag.items);
  lb_al_create_vector_r(&Signal);
  lb_al_create_vector_r(&Noise_mag);
  Noise_phase.items=Noise_mag.items;
  lb_al_create_vector_r(&Noise_phase);
 

  N_experiment=N_exp_max;
  n_experiments=10;
  while (flag_running)
    {
      errors_count_threshold_00 = 0;
      errors_count_threshold_01 = 0 ;
      errors_count_threshold_02 = 0;
      for (experiment=0;experiment<n_experiments;experiment++)
	{
	  if (!(experiment % 1000))
	    flag_draw_wave = TRUE;
	  else 
	    flag_draw_wave = FALSE;
	      
	  for (k=0; k<Noise_mag.items; k++)
	    {
	      Noise_mag.array[k]=lb_st_marsaglia_polar(noise_variance);
	      Noise_phase.array[k]=rand()/(REAL_T)RAND_MAX;
	    }
	  N_total=energy_signal(&Noise_mag);
	  REAL_T scale_factor;
	  scale_factor=sqrt(N_experiment/N_total);

	  for (k=0; k<Noise_mag.items; k++)
	    Noise_mag.array[k]=scale_factor*Noise_mag.array[k];
	  N_total=energy_signal(&Noise_mag);

	  //	      flag_draw_wave=FALSE;
	  if (flag_draw_wave)
	    lb_gr_draw_rectangle_solid(NULL, ty_screen.w*0.75, 0, ty_screen.w, ty_screen.h, lb_gr_12RGB(COLOR_WHITE));

	  lg_gr_draw_axis_2d(NULL, win_wave, &my_font,
			     lb_gr_12RGB(COLOR_BLUE), 3, 25,
			     lb_gr_12RGB(COLOR_BLACK), 0,
			     lb_gr_12RGB(COLOR_BLACK), 1.0, 1.0,
			     AXIS_DRAW_X | AXIS_DRAW_Y_GRID,
			     COPYMODE_COPY, LINEMODE_SOLID); 

	      
	  for (k=0;k<Signal.items;k++)
	    {
	      t=t_min+k*(t_max-t_min)/Signal.items;
		  
	      REAL_T temp;
	      temp=0;

	      for (i=0;i<Noise_mag.items;i++)
		temp+=Noise_mag.array[i]*sin(2*M_PI*(i*t/T_base+Noise_phase.array[i]));
	      Signal.array[k]=wave(t,freq_0, freq_1)+temp;
	      if (flag_draw_wave)
		{
		  lb_gr_project_2d(win_wave, t, Signal.array[k], &xp, &yp);
		  //lb_gr_draw_pixel(NULL, xp, yp, lb_gr_12RGB(COLOR_BLACK), COPYMODE_COPY);
		  lb_gr_draw_circle(NULL, xp, yp, 2, lb_gr_12RGB(COLOR_RED), COPYMODE_COPY);
		}
	    }
	  if (parse_vector(&Signal, 0, 0))
	    errors_count_threshold_00++;
		
	  if (parse_vector(&Signal, -0.025, 0.025))
	    errors_count_threshold_01++;

	  if (parse_vector(&Signal, -0.05, 0.05))
	    errors_count_threshold_02++;


	  lb_gr_refresh(&ty_screen);
	  while (SDL_PollEvent(&event))
	    {
	      if (event.type == SDL_QUIT)
		{
		  SDL_Quit();
		  exit(EXIT_SUCCESS);
		}
	    }
	}
      printf("N_experiment=%f\tN_total=%f\tE(0,0)=%lu, E(0.025, -0.025)=%lu, E(0.05, -0.05)=%lu\r\n",
	     N_experiment,N_total, errors_count_threshold_00, errors_count_threshold_01, errors_count_threshold_02);

      REAL_T temp_00, temp_01, temp_02, temp;
      if (N_experiment>0)
	{
	  lb_gr_project_2d_x(win_BER, -10*log10(N_experiment), &xp);

	  temp_00=(REAL_T)errors_count_threshold_00/n_experiments;
	  if (temp_00>0)
	    {
	      lb_gr_project_2d_y_log(win_BER, temp_00, &yp);
	      lb_gr_draw_circle_antialiasing(NULL, xp, yp, 10, 3, lb_gr_12RGB(0x0000));

	    }

	  temp_01=(REAL_T)errors_count_threshold_01/n_experiments;
	  if (temp_01>0)
	    {
	      lb_gr_project_2d_y_log(win_BER, temp_01, &yp);
	      lb_gr_draw_rectangle_line(NULL, xp-15, yp-15, xp+15, yp+15, 2, lb_gr_12RGB(0xf07f),COPYMODE_COPY); 
	    }
	  temp_02=(REAL_T)errors_count_threshold_02/n_experiments;

	  if (temp_02>0)
	    {
	      lb_gr_project_2d_y_log(win_BER, temp_02, &yp);
	      lb_gr_draw_line(NULL, xp-12, yp-12, xp+12, yp+12, 4, lb_gr_12RGB(0xFF33),COPYMODE_COPY, LINEMODE_FILTERED); 
	      lb_gr_draw_line(NULL, xp+12, yp-12, xp-12, yp+12, 4, lb_gr_12RGB(0xFF33),COPYMODE_COPY, LINEMODE_FILTERED); 
	    }
	    
	  MATHERROR_T error;
	  temp = 0.5*lb_st_erfc(lb_re_sqrt(0.5*(REAL_T)1.0/N_experiment, &error));
	  if (temp>0)
	    {
	      lb_gr_project_2d_y_log(win_BER, temp, &yp);
	      lb_gr_draw_circle_filled(NULL, (int)xp, (int)yp, 8, lb_gr_12RGB(COLOR_RED), COPYMODE_COPY);
	    }
	    
	  printf("Noise=%f x=%f dB, BER_00=%f, BER_01=%f, BER_02=%f, BER_FSK=%f\r\n",
		 N_experiment, 10.0*log10(N_experiment), temp_00, temp_01, temp_02, temp);
	      
	  lb_gr_refresh(&ty_screen);
	}
      sprintf(file_name,"zero_mod_%03d",file_counter);
      lb_gr_BMPfile_save(file_name, NULL);
      file_counter++;
       
      N_experiment/=pow(10.0,1.0/10.0); /* plot in 1.0 dB increments */
      
      n_experiments*=2;
      printf("n_experiments=%lu\r\n",n_experiments);
      if (N_experiment<N_exp_min)
	flag_running=FALSE;
    }

 
  printf("Program completed: waiting to be closed by user\r\n");
  while (1)
    while (SDL_PollEvent(&event))
      {
	if (event.type == SDL_QUIT)
	  {
	    SDL_Quit();
	    exit(EXIT_SUCCESS);
	  }
      }
  lb_al_release_vector_r(&Signal);
  lb_al_release_vector_r(&Noise_mag);
  lb_al_release_vector_r(&Noise_phase);

#endif


  /******************************************************************************/
  /* Demo: Time Delay Functions                                                 */
  /* These functions provide reasonably accurate delays                         */
  /******************************************************************************/

  /* Test for the time delay funcions with milli and micro-second resolution */
  /* Status: Pass */
  //#define DEMO_TIME
#ifdef DEMO_TIME
  SINT16_T i=0;
  while (1)
    {
      printf("Sec: %d, P=%02.1f\r\n",i,100.0*(REAL_T)clock()/(REAL_T)0xFFFFFFFF);
      lb_ti_delay_us(1000000);
      
      //lb_ti_delay_ms(1000);
      i++;
    }
#endif

  /******************************************************************************/
  /* Demo: Keyboard support functions                                           */
  /* The idea is to make easier reading the keyboard, including the special     */
  /* keys under Linux/Unix. We do so by using the non-printable ASCII           */
  /* as codes for the most important special keys (PCKEY_XXX).                  */
  /* There are two support functions, which are not part of the standard 'c':   */
  /*    ln_co_kbhit()    --> checks whether a key has been pressed              */
  /*    ln_co_getch_pc() --> gets the first key which was pressed               */
  /* to_do: this would be a nice place to include the "cancel" (CRL-C)          */
  /* redirection code which written in the now-deprecated 'framebuffer' section */
  /******************************************************************************/
  
  //#define DEMO_KEYBOARD
#ifdef DEMO_KEYBOARD
  unsigned char c;
  SINT8_T flag;

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

  /******************************************************************************/
  /* Demo: Console support functions                                            */
  /* These functions aim to provide utilities for console-only programs,        */
  /* under Linux/Unix such as printing in colors, moving the cursor,            */
  /* setting the background color, clearing screen, a row, etc.                 */
  /******************************************************************************/

  //#define DEMO_CONSOLE
#ifdef DEMO_CONSOLE
   char text[40];
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
  lb_co_printf_block("DIEGO ALBERTO VELEZ HENAO", 1, 3);
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

  lb_ti_delay_ms(10000);
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
	lb_ti_delay_ms(1000);
      }
  printf("TEXT_COLOR_0x00f\r\n");
  printf("\a");

  //lb_co_cls();
#endif


  /******************************************************************************/
  /* Demo: Audio Demos                                                          */
  /* Audio is still an experimental matter.  Audio support relies  heavily on   */
  /* SDL's capabilities.                                                        */
  /* Additional work is needed to simplify it and make it friendlier.           */
  /* One of the items than need to be reexamined is how to handle the           */
  /* wide range of formats with different number of bits, frequencies, and      */
  /* Another one is how to explore the recording capabilities and, better,      */
  /* simultaneous recording and playback.                                       */
  /* Nevertheless, several applications can be implemented right now.           */
  /******************************************************************************/


  /******************************************************************************/
  /* Demo: DTMF                                                                 */
  /* Shows an application where Dual-Tone-Multi-Frequency signals are           */
  /* synthetized in real-time.                                                  */
  /******************************************************************************/
  
  //#define DEMO_DTMF
#ifdef DEMO_DTMF
  lb_au_SDL_audio_init_DTMF();
  lb_au_SDL_audio_start();

  lb_au_freq_DTMF('4', &_lb_au_f0, &_lb_au_f1);
  lb_ti_delay_ms(200);
  _lb_au_f0=0.0;
  _lb_au_f1=0.0;
  lb_ti_delay_ms(200);
  
  lb_au_freq_DTMF('0', &_lb_au_f0, &_lb_au_f1);
  lb_ti_delay_ms(200);
  _lb_au_f0=0.0;
  _lb_au_f1=0.0;
  lb_ti_delay_ms(200);
  
  lb_au_freq_DTMF('3', &_lb_au_f0, &_lb_au_f1);
  lb_ti_delay_ms(200);
  _lb_au_f0=0.0;
  _lb_au_f1=0.0;
  lb_ti_delay_ms(200);
  
  lb_au_freq_DTMF('5', &_lb_au_f0, &_lb_au_f1);
  lb_ti_delay_ms(200);
  _lb_au_f0=0.0;
  _lb_au_f1=0.0;
  lb_ti_delay_ms(200);
  
  lb_au_freq_DTMF('3', &_lb_au_f0, &_lb_au_f1);
  lb_ti_delay_ms(200);
  _lb_au_f0=0.0;
  _lb_au_f1=0.0;
  lb_ti_delay_ms(200);
  
  lb_au_freq_DTMF('1', &_lb_au_f0, &_lb_au_f1);
  lb_ti_delay_ms(200);
  _lb_au_f0=0.0;
  _lb_au_f1=0.0;
  lb_ti_delay_ms(200);
  
  lb_au_freq_DTMF('2', &_lb_au_f0, &_lb_au_f1);
  lb_ti_delay_ms(200);
  _lb_au_f0=0.0;
  _lb_au_f1=0.0;
  lb_ti_delay_ms(200);
  
  lb_au_freq_DTMF('6', &_lb_au_f0, &_lb_au_f1);
  lb_ti_delay_ms(200);
  _lb_au_f0=0.0;
  _lb_au_f1=0.0;
  lb_ti_delay_ms(200);
  
  lb_au_freq_DTMF('1', &_lb_au_f0, &_lb_au_f1);
  lb_ti_delay_ms(200);
  _lb_au_f0=0.0;
  _lb_au_f1=0.0;
  lb_ti_delay_ms(200);
  
  lb_au_freq_DTMF('6', &_lb_au_f0, &_lb_au_f1);
  lb_ti_delay_ms(200);
  _lb_au_f0=0.0;
  _lb_au_f1=0.0;
  lb_ti_delay_ms(200);
  
  _lb_au_f0=0.0;
  _lb_au_f1=0.0;

  lb_au_SDL_audio_close_DTMF();

#endif

  /******************************************************************************/
  /* Demo: Writting a standard Wave-File                                        */
  /* A synthetized audio signal is saved to a file.                             */
  /******************************************************************************/

  //#define DEMO_WAV
#ifdef DEMO_WAV
  VECTOR_R_T V;
  UINT16_T i;
  V.items=VECTOR_MAX_ITEMS;
  lb_al_create_vector_r(&V);

  for (i=0;i<V.items;i++)
    V.array[i]=sin(2*M_PI*500.0*i/11025);
  lb_au_wave_write_from_vector_r("./media/audio/wav2.wav",&V, 11025,8);

  lb_al_release_vector_r(&V);
#endif

  
  /******************************************************************************/
  /* Demo: Writting a standard Wave-File                                        */
  /* A synthetized audio signal is saved to a file.  If the file already        */
  /* exists, data gets appended to the file.                                    */
  /******************************************************************************/

  //#define DEMO_WAV_WRITE
#ifdef DEMO_WAV_WRITE
#define SAMPLES 22050
  VECTOR_R_T V;
  UINT16_T i;
  REAL_T f1,f2;
  
  V.items=SAMPLES;
  lb_al_create_vector_r(&V);

  lb_au_freq_DTMF('3', &f1, &f2);
  for (i=0;i<SAMPLES;i++)
    V.array[i]=0.5*sin(2*M_PI*f1*i/SAMPLES) + 0.5*sin(2*M_PI*f2*i/SAMPLES);
      
  lb_au_wave_write_or_append_from_vector_r("./media/audio/test2.wav", &V, SAMPLES, 8);
  lb_al_release_vector_r(&V);
#endif

  /******************************************************************************/
  /* Demo: Playing a Wave-format audio file                                     */
  /* A synthetized audio signal is saved to a file.                             */
  /* This is really more an SDL example than anything else.  It is kept as it   */
  /* may become handy as more progress is made in the audio module.             */
  /******************************************************************************/
  
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
  if( SDL_LoadWAV("./media/audio/toto.wav", &wav_spec, &wav_buffer, &wav_length) == NULL )
    {
      printf("Error loading wav file\r\n");
      exit(1);
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
      exit(-1);
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

  /******************************************************************************/
  /* Demo: Exploring Unicode Support                                            */
  /* This test shows, if supported by the current console, how to print Unicode */
  /* characters.                                                                */
  /******************************************************************************/

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
	  lb_ti_delay_ms(250);
	}
      printf("%lc", i);
 	
      //sprintf(text,"\\x%x\\x%x\\x%x\r\n" ,(x>>16) & 0xff, (x>>8) & 0xff, x & 0xff);
      //printf(text);
    }

#endif

  /******************************************************************************/
  /* Demo: Using RS-232 serial ports                                            */
  /* I'm currently working on this.  Because of that, it is not yet functional  */
  /******************************************************************************/
  
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
      lb_co_color(TEXT_COLOR_WHITE);
      fflush(stdout);

      lb_se_print_buffer(&port1);
      lb_co_color(TEXT_COLOR_YELLOW);
      fflush(stdout);
      lb_se_print_buffer(&port2);
      a++;
      if (a==128)
	a=0;
    }
  lb_se_close(&port1);
  lb_se_close(&port2);
#endif

}

