#include <graphics.h>
#include "dv_types.h"
#include "dv_fourier.h"
#include "dv_graphics.h"

#define N_EXPERIMENT_MAX 100000
#define FREQ_1 4291.0
#define FREQ_2 5464.0

#define SNR_MIN 2.0
#define SNR_MAX 4.0
#define SNR_DELTA 2.0

void fill_vector_real_gaussian_noise(COMPLEX_T vector[], FLOAT_T variance, U_INT_16_T size)
{
  U_INT_16_T k;
  for(k=0;k<size;k++)
    {
      vector[k].r=marsaglia_polar(variance);
      vector[k].i=0.0;
    } 
}

void escale_complex_vector(COMPLEX_T vector[], FLOAT_T factor, U_INT_16_T size)
{
  U_INT_16_T k;
  for(k=0;k<size;k++)
    {
      vector[k].r*=factor;
      vector[k].i*=factor;
    }
} 

FLOAT_T RMS_noise_time_complex(COMPLEX_T signal[], U_INT_16_T a, U_INT_16_T b)
{
  FLOAT_T temp;
  U_INT_16_T i;

  temp=0.0;
  for(i=a;i<b;i++)
    temp+=signal[i].r*signal[i].r + signal[i].i + signal[i].i;
  return sqrt(temp/((FLOAT_T)b-(FLOAT_T)a));
} 

FLOAT_T RMS_noise_time_real(FLOAT_T signal[], U_INT_16_T a, U_INT_16_T b)
{
  FLOAT_T temp;
  U_INT_16_T i;

  temp=0.0;
  for(i=a;i<b;i++)
    temp+=signal[i]*signal[i];
  return sqrt(temp/((FLOAT_T)b-(FLOAT_T)a));
} 

FLOAT_T RMS_noise_frequency(COMPLEX_T frequencies[], U_INT_16_T a, U_INT_16_T b)
{
  FLOAT_T temp;
  U_INT_16_T i;
  
  for(i=a;i<b;i++)
    temp+=frequencies[i].r*frequencies[i].r + frequencies[i].i*frequencies[i].i;
  return sqrt(temp)/((FLOAT_T)b-(FLOAT_T)a);
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

S_INT_8_T parse_vector(COMPLEX_T buffer[], U_INT_16_T size)
{
  S_INT_8_T sign;
  U_INT_16_T time_stamp[5];
  U_INT_16_T k, i, time_thereshold;
  time_thereshold=round(size*0.25*(1.0/FREQ_1+1.0/FREQ_2)/(1.0/FREQ_2+1.5/FREQ_1));
  //printf("Thereshold = %i\r\n",time_thereshold);
  k=0;
  i=0;
  sign=fsign(buffer[0].r);
  while ((i<N_SAMPLES) && (k<5))
    {
      if (fsign(buffer[i].r) != sign)
	{ 
	  sign=fsign(buffer[i].r);
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


void main()
{
  int gd, gm;
  U_INT_16_T i, j, k;
  U_INT_32_T n_experiment, errors_count;
  FLOAT_T snr;
  FLOAT_T variance;
  FLOAT_T  RMS_noise;
  FLOAT_T t_min=-0.25/FREQ_1, t_max=1/FREQ_2+1.25/FREQ_1, t; /* time */
  FLOAT_T w1=2*M_PI*FREQ_2, w2=2*M_PI*FREQ_1;
  COMPLEX_T signal_complex[N_SAMPLES];
  COMPLEX_T frequencies[N_SAMPLES];

  FFT_pre_calculate_W(N_SAMPLES);

  system("clear");

  printf("erf = %e\r\n",erf(-0.09));
  printf("erfc = %e\r\n",erfc(-0.09));
  
  MATRIX_T A;
  A.rows=3;
  A.cols=3;
  matrix_create(&A);

  VECTOR_T V;
  V.items=3;
  vector_create(&V);

  for (i=0;i<A.rows;i++)
      for (j=0;j<A.cols;j++)
	A.array[i][j]=10*i+j;

  for (i=0;i<V.items;i++)
    V.array[i]=10*i;
  
  printf("**************************\r\n");
  matrix_print(&A,"A");
  printf("**************************\r\n");

  vector_print(&V,"B");
  printf("**************************\r\n");
 
  update_rotation_matrix(&A,&V,0.1234);
 
  printf("**************************\r\n");
  matrix_print(&A,"A");
 
  matrix_release(&A);
  vector_release(&V);
  
  printf("**************************\r\n");
 
  printf("Simulation running... please wait... \r\n");
  srand(time(NULL));

  
  for(snr=SNR_MIN;snr<SNR_MAX;snr+=SNR_DELTA)
    {
      errors_count=0;
      for (n_experiment=0;(n_experiment<N_EXPERIMENT_MAX) && (errors_count<100);n_experiment++)
	{
	  fill_vector_real_gaussian_noise(signal_complex, 0.1, N_SAMPLES);
	  
	  /* Noise using the time's Domain */
	  // printf("RMS Noise (time domain method) = %f\r\n",RMS_noise_time_complex(signal_complex,0,N_SAMPLES));

	  /* Noise using the frequency's Domain */
	  FFT_R(signal_complex,frequencies,N_SAMPLES);
  
	  // printf("RMS Noise (frequency domain method) = %f\r\n", RMS_noise_frequency(frequencies,N_POINTS));
  
	  /* A perfect Bandpass filter: we're leaving the a fourth of the lower frequencies  */
	  /* Why 12 ?
	     The circuit being simulated is a 1-st order active Low Pass with R=27.4K and C=220p, hence f=26.403KHz
	     The DFT' base period is tmax = 1/5464 + 1/4291 = 4.1606*10^-4 seg   ==> d_freq=2403.5 Hz

	     To fiter using a DTF we need to "keep"the base frequencies:
	     f0  = DC
	     f1  = 2403.5
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
	  for(i=12;i<N_SAMPLES;i++)
	    {
	      frequencies[i].r=0;
	      frequencies[i].i=0;
	    } 

	  /* Digital filter */
	  FFT_C(1,frequencies,signal_complex,N_SAMPLES);
	  //RMS_noise=RMS_noise_time_complex(signal_complex,round(0.25*N_SAMPLES/FREQ_1),N_SAMPLES-round(0.25*N_SAMPLES/FREQ_1));
	  //printf("RMS Noise (time domain method) after filtering = %f\r\n",RMS_noise);
	  // printf("RMS Noise (frequency domain method) after filtering = %f\r\n",RMS_noise_frequency(frequencies,N_POINTS));
	  //	  escale_complex_vector(signal_complex,(1.0*(FLOAT_T)noise_step/(FLOAT_T)NOISE_STEPS_MAX)/RMS_noise,N_SAMPLES

	  for(i=0;i<N_SAMPLES;i++)
	    {
	      signal_complex[i].i=0;
	    }
	  RMS_noise=RMS_noise_time_complex(signal_complex,round(0.25*N_SAMPLES/FREQ_1),N_SAMPLES-round(0.25*N_SAMPLES/FREQ_1));
	  escale_complex_vector(signal_complex,pow(10,-0.1*snr)/RMS_noise,N_SAMPLES);
	  // printf("RMS Noise (time domain method) *** = %f\r\n",
	  // RMS_noise_time_complex(signal_complex,round(0.25*N_SAMPLES/FREQ_1),N_SAMPLES-round(0.25*N_SAMPLES/FREQ_1)));
	  // printf("RMS Noise (frequency domain method) after filtering = %f\r\n",RMS_noise_frequency(signal_complex,N_SAMPLES));
      
	  for(i=0;i<N_SAMPLES;i++)
	    {
	      t=t_min+i*(t_max-t_min)/N_SAMPLES;
	      signal_complex[i].r += sqrt(2)*f(t,2*M_PI*FREQ_2,2*M_PI*FREQ_1);
	    }
      
	  if (!parse_vector(signal_complex, N_SAMPLES))
	    errors_count++;
	  printf("Eb/No=%0.2f  Pg=%2.2f %% E=%i N=%i  BER=%0.8f  Log(BER)=%0.4f  PL=%0.2f %%\r",
		 snr, 
		 100.0*(n_experiment+1)/N_EXPERIMENT_MAX, 
		 errors_count,
		 N_EXPERIMENT_MAX, 
		 (FLOAT_T)errors_count/n_experiment,
		 log10((FLOAT_T)errors_count/n_experiment),
		 100-100*pow(1.0-(FLOAT_T)errors_count/(FLOAT_T)n_experiment,8.0*256.0));
	}
      printf("\r\n");
    }

#define GRAPHICS
#ifdef GRAPHICS
  /**************** GRAPHICS ******************************/
  gd = X11;
  gm = X11_1024x768;
  initgraph (&gd, &gm, "");

  VIEWPORT_T win;

  win.xp_min=25;
  win.xp_max=getmaxx()-25;
  win.yp_min=25;
  win.yp_max=getmaxy()-25;

  win.xr_min=t_min;
  win.xr_max=t_max;
  win.yr_min=-5.0;
  win.yr_max=5.0;

  setbkcolor(BLACK);
  setcolor(WHITE);
  cleardevice ();

  /* We draw a box and the axis */  
 
  //setlinestyle (SOLID_LINE, 0, THICK_WIDTH);
  rectangle (win.xp_min, win.yp_min, win.xp_max, win.yp_max);
  outtextxy (25, 10, "CHARACTERIZATION OF A ZERO-CROSSING MODULATION SCHEME");

  setcolor(YELLOW);
  line(win.xp_min,0.5*(win.yp_max+win.yp_min), win.xp_max,0.5*(win.yp_max+win.yp_min));


  for(i=0;i<N_SAMPLES;i++)
    plot(win,t_min+i*(t_max-t_min)/N_SAMPLES,signal_complex[i].r);
 
  while (!kbhit ()) ;

  closegraph ();
 #endif
}

/* Might be useful later

  while (!kbhit ()) {
    
    if (p = mouseclick ())
      if ( (mousex () < 200) && (mousey () < 20))
	break;
    else {
      setfillstyle (SOLID_FILL, 
		    COLOR (random(255), random(255), random(255)));
      setcolor (COLOR (random(255), random(255), random(255)));
      if (0 == random (2))
	fillellipse (mousex (), mousey (), p*10, p*10);
      else
	bar (mousex () - p*10, mousey () - p*10,
	     mousex () + p*10, mousey () + p*10);
    }
    
  } // while

  complex_t data_buffer[16] ={ {-9.21,0}, {0.28,0}, {-0.31,0}, {0.47,0}, {7.51,0}, {0.61,0}, {-2.71,0}, {10.81,0}, 
  {4.23,0}, {-2.07,0}, {3.80,0}, {2.10,0}, {-1.04,0}, {2.11,0}, {0.11,0}, {-9.26,0} };

   fill_vector_real_gaussian_noise(data_buffer,0.1,6);
  
  printf("**************************************\r\n");
  printf("DFT_complex:\r\n");
  DFT_C(data_buffer,X,N_SAMPLES);
  for (k=0;k<N_SAMPLES;k++)
  printf("real=%f  im=%f\r\n",X[k].r,X[k].i);

  printf("**************************************\r\n");
  printf("FFT version 2 Complex:\r\n");
  FFT_C(-1,data_buffer,X,N_SAMPLES);
  for (k=0;k<N_SAMPLES;k++)
  printf("real=%f  im=%f\r\n",X[k].r,X[k].i);

  printf("**************************************\r\n");
  printf("FFT  Complex inverse:\r\n");
  for (k=0;k<10000;k++)
  FFT_C(1,X,data_buffer,N_SAMPLES);
  printf("ready\r\n");
  for (k=0;k<N_SAMPLES;k++)
  printf("real=%f  im=%f\r\n",data_buffer[k].r,data_buffer[k].i);

  printf("**************************************\r\n");
  printf("IFFT (optimized Real):\r\n");
  for (k=0;k<10000;k++)
  IFFT_R(X,data_buffer,N_SAMPLES);
  printf("ready\r\n");
 
  printf("**************************************\r\n");
  for (k=0;k<N_SAMPLES;k++)
  printf("real=%f  im=%f\r\n",data_buffer[k].r,data_buffer[k].i);

*/

