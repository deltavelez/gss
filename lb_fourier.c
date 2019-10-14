#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "lb_types.h"
#include "lb_algebra.h"
#include "lb_complex.h"
#include "lb_fourier.h"
#include "lb_real.h"
#include "lb_integer.h"

void lb_fo_FFT_C_recursive(VECTOR_C_T *T,VECTOR_C_T *F, SINT8_T sign)
{
  UINT16_T i,size;
  VECTOR_C_T T_even, T_odd, F_even, F_odd, W;
  REAL_T temp_f;

  /* We assert the size */
  if(!lb_al_assert_dimensions_vector_c(T))
    {
      printf("invalid dimension --> lb_fo_FFT_recursive [T]\r\n");
      exit(1);
    }
  if (!lb_al_assert_dimensions_vector_c(F))
    {
      printf("invalid dimension --> lb_fo_FFT_recursive [F]\r\n");
      exit(1);
    }
  
  /* Vectors should have the same size */
  if ((*T).items != (*F).items)
    {
      printf("invalid dimension --> lb_fo_FFT_recursive [n1!=n2]\r\n");
      exit(1);
    }
  
  size=(*T).items;  /* n makes the code clearer */
  
  /* Vectors must be a power of two */
  if (!lb_in_is_power_of_two(size))
    {
      printf("invalid dimension --> lb_fo_FFT_recursive [n!=2^m]\r\n");
      exit(1);
    }

  /* Transform sign can only be -1 (FFT) or +1 (IFFT) */
  if (!((sign==1) || (sign==-1)))
    {
      printf("invalid argument --> lb_fo_FFT_recursive [sign!=+1 or -1]\r\n");
      exit(1);
    }
  
  if (size==1)
    {
      (*F).array[0].r=(*T).array[0].r;
      (*F).array[0].i=(*T).array[0].i;
      return;
    }

  if (size==2)
    {
      (*F).array[0]=  lb_cp_add((*T).array[0],(*T).array[1]);
      (*F).array[1]=lb_cp_substract((*T).array[0],(*T).array[1]);
   
      if (sign==1)
	{
	  (*F).array[0]=lb_cp_multiply_real((*F).array[0],0.5);
	  (*F).array[1]=lb_cp_multiply_real((*F).array[1],0.5);
	}
      return;
    }

  W.items=size;
  
  T_even.items=size/2;
  T_odd.items=size/2;
  F_even.items=size/2;
  F_odd.items=size/2;
 
  lb_al_create_vector_c(&W);
  lb_al_create_vector_c(&T_even);
  lb_al_create_vector_c(&T_odd);
  lb_al_create_vector_c(&F_even);
  lb_al_create_vector_c(&F_odd);

  for(i=0;i<size/2;i++)
    {
      T_even.array[i]=(*T).array[2*i]; 
      T_odd.array[i] =(*T).array[2*i+1]; 
    }
  
  lb_fo_FFT_C_recursive(&T_even,&F_even,sign);
  lb_fo_FFT_C_recursive(&T_odd,&F_odd,sign);
     
  for(i=0;i<size;i++)
    {  
      temp_f=sign*2*M_PI*i/size;
      W.array[i].r = cos(temp_f);
      W.array[i].i = sin(temp_f);
    }

  for(i=0;i<size/2;i++)
    {  
      (*F).array[i]    =lb_cp_add(F_even.array[i],lb_cp_multiply(W.array[i],    F_odd.array[i]));
      (*F).array[size/2+i]=lb_cp_add(F_even.array[i],lb_cp_multiply(W.array[size/2+i],F_odd.array[i]));
    }
 
  if (sign==1)
    for (i=0;i<size;i++)
      (*F).array[i]=lb_cp_multiply_real((*F).array[i],0.5);
  
  lb_al_release_vector_c(&W);
  lb_al_release_vector_c(&T_even);
  lb_al_release_vector_c(&T_odd);
  lb_al_release_vector_c(&F_even);
  lb_al_release_vector_c(&F_odd);
  return;
}

void lb_fo_calculate_W(VECTOR_C_T *W, SINT8_T sign)
{
  UINT16_T k;
  REAL_T arg;
   if(!lb_al_assert_dimensions_vector_c(W))
    {
      printf("Error:  lb_fo_calculate_W() --> invalid dimension\r\n");
      exit(1);
    }

   if (!((sign==1) || (sign==-1)))
    {
      printf("Error: lb_fo_calculate_W() --> invalid argument [sign!=+1 or -1]\r\n");
      exit(1);
    }


  /* We pre-calculate half of all the  W_k_n.*/
 
  for(k=0;k<(*W).items;k++)
    {
      arg=sign*M_PI*k/(*W).items;
      (*W).array[k].r = cos(arg);
      (*W).array[k].i = sin(arg);
    }
}

void lb_fo_DFT_C(VECTOR_C_T *T, VECTOR_C_T *F, SINT8_T sign, MATHERROR_T *error) 
{
  UINT16_T k, n, size;
  COMPLEX_T a;
  REAL_T temp_f;

  /* We assert the size */
  if(!lb_al_assert_dimensions_vector_c(T))
    {
      printf("invalid dimension --> lb_fo_DFT_C [T]\r\n");
      exit(1);
    }
  if (!lb_al_assert_dimensions_vector_c(F))
    {
      printf("invalid dimension --> lb_fo_DFT_C [F]\r\n");
      exit(1);
    }
  
  /* Vectors should have the same size */
  if ((*T).items != (*F).items)
    {
      printf("invalid dimension --> lb_fo_DFT_C [n1!=n2]\r\n");
      exit(1);
    }
 
  size=(*T).items;
 
  /* Transform sign can only be -1 (FFT) or +1 (IFFT) */
  if (!((sign==1) || (sign==-1)))
    {
      printf("invalid argument --> lb_fo_DFT_C [sign!=+1 or -1]\r\n");
      exit(1);
    }
  
    temp_f=1/(REAL_T)size;
   for (k=0;k<size;k++)
    {
      (*F).array[k].r=0.0;
      (*F).array[k].i=0.0;
      for (n=0;n<size;n++)
	{
	  a.r=0;
	  a.i=sign*2*M_PI*k*n*temp_f;
	  (*F).array[k]=lb_cp_add((*F).array[k],lb_cp_multiply((*T).array[n],lb_cp_exp(a,error)));    
	}
      if(sign==1)
	(*F).array[k]=lb_cp_multiply_real((*F).array[k],temp_f);
    }
}

void lb_fo_DFT_R(VECTOR_R_T *T, VECTOR_C_T *F) 
{
  UINT16_T k, n, size;
 
  /* We assert the size */
  if(!lb_al_assert_dimensions_vector_r(T))
    {
      printf("invalid dimension --> lb_fo_DFT_R [T]\r\n");
      exit(1);
    }
  if (!lb_al_assert_dimensions_vector_c(F))
    {
      printf("invalid dimension --> lb_fo_DFT_R [F]\r\n");
      exit(1);
    }
  
  /* Vectors should have the same size */
  if ((*T).items != (*F).items)
    {
      printf("invalid dimension --> lb_fo_DFT_R [n1!=n2]\r\n");
      exit(1);
    }
   
   size=(*T).items;
   
   for (k=0;k<=size/2;k++)
    {
      (*F).array[k].r=0.0;
      (*F).array[k].i=0.0;
      for (n=0;n<size;n++)
      {
	(*F).array[k].r = (*F).array[k].r + (*T).array[n]*cos(-2*M_PI*k*n/size);
	(*F).array[k].i = (*F).array[k].i + (*T).array[n]*sin(-2*M_PI*k*n/size);
      }
    }
   /* We "fill-out" the other "half minus one" of the spectrum which is symetrical for real signals */
   for (k=0;k<size/2;k++)
     {
       (*F).array[size/2+k].r= (*F).array[size/2-k].r;
       (*F).array[size/2+k].i=-(*F).array[size/2-k].i;
     }
}

void lb_fo_FFT_C(VECTOR_C_T *T, VECTOR_C_T *F, VECTOR_C_T *W, SINT8_T sign) 
{
  UINT8_T stage, bit_size;
  UINT16_T k, l, size, addr_1, addr_2, two_pow_stage, two_pow_stage_plus_one, w_index_factor;
  REAL_T temp_f;
  COMPLEX_T temp1, temp2, W_temp;

#ifdef DEBUGGING_FFT
  char ss[20];
#endif

  /* We assert the size */
  if(!lb_al_assert_dimensions_vector_c(T))
    {
      printf("Error: lb_fo_FFT_C() --> invalid dimension [T]\r\n");
      exit(1);
    }
  if (!lb_al_assert_dimensions_vector_c(F))
    {
      printf("Error: lb_fo_FFT_C() --> invalid dimension [F]\r\n");
      exit(1);
    }
  
  /* Vectors should have the same size */
  if ((*T).items != (*F).items)
    {
      printf("Error: lb_fo_FFT_C() --> invalid dimension [n1!=n2]\r\n");
      exit(1);
    }

  /* W must be half the size as the T and F vectors */
  if ((2*(*W).items) != (*F).items)
    {
      printf("Error: lb_fo_FFT_C() --> invalid dimension [size(W)!=size(T)/2]\r\n");
      exit(1);
    }

  size=(*T).items;  /* makes the code clearer */
  
  /* Vectors must be a power of two */
  bit_size=lb_in_is_power_of_two(size);
  if (!bit_size)
    {
      printf("Error: lb_fo_FFT_C() --> invalid dimension [n!=2^m]\r\n");
      exit(1);
    }
  
  /* Transform sign can only be -1 (FFT) or +1 (IFFT) */
  if (!((sign==1) || (sign==-1)))
    {
      printf("Error: lb_fo_FFT_C() --> invalid argument [sign!=+1 or -1]\r\n");
      exit(1);
    }

  /* This implementation of the FFT stores the temporary results in the same output buffer, and works recursively on it */
  for(k=0;k<size;k++)
    (*F).array[k]=(*T).array[lb_in_invert_bits(k,bit_size)];

  for(stage=0;stage<bit_size;stage++)
    {
      two_pow_stage          = 0x01<<stage;
      two_pow_stage_plus_one = 2*two_pow_stage;
      w_index_factor         = 0x01<<(bit_size-1-stage);
        
      for (k=0;k<w_index_factor;k++)
	{
	  for (l=0;l<two_pow_stage;l++)
	    {
	      addr_1=k*two_pow_stage_plus_one + l;
	      addr_2=k*two_pow_stage_plus_one + two_pow_stage + l;
	      
	      temp1=(*F).array[addr_1];
	      temp2=(*F).array[addr_2];
	      W_temp=(*W).array[l*w_index_factor];
	      
	      /* Elegant */
	      // (*F).array[addr_1]=lb_cp_add(  temp1,lb_cp_complex_prod(W_temp,temp2));
	      // (*F).array[addr_2]=lb_cp_complex_minus(temp1,lb_cp_complex_prod(W_temp,temp2));
	      
	      /* Less elegant but 40% faster */
	      (*F).array[addr_1].r = temp1.r + W_temp.r*temp2.r - W_temp.i*temp2.i; 
	      (*F).array[addr_1].i = temp1.i + W_temp.r*temp2.i + W_temp.i*temp2.r; 
	      (*F).array[addr_2].r = temp1.r - W_temp.r*temp2.r + W_temp.i*temp2.i; 
	      (*F).array[addr_2].i = temp1.i - W_temp.r*temp2.i - W_temp.i*temp2.r;
	    }
	}

#ifdef DEBUGGING_FFT
      if (sign==1)
	{
	  sprintf(ss,"stage= %i F",stage);
	  lb_al_print_vector_c(F,ss,"%0.4f");
	}
#endif

    } /* stage*/
  if (sign==1)
    {
      temp_f=1/(REAL_T)size;
      for (l=0;l<size;l++)
	{
	  (*F).array[l].r=(*F).array[l].r*temp_f;
	  (*F).array[l].i=(*F).array[l].i*temp_f;
	}
    }
}
    
void lb_fo_FFT_R(VECTOR_R_T *T, VECTOR_C_T *F, VECTOR_C_T *W) 
{
  UINT8_T stage, bit_size;
  UINT16_T k, l, size, addr_1, addr_2, addr_3, two_pow_stage, two_pow_stage_plus_one, w_index_factor;
  COMPLEX_T temp1, temp2, W_temp;

  /* We assert the size */
  if(!lb_al_assert_dimensions_vector_r(T))
    {
      printf("invalid dimension --> lb_fo_FFT_R [T]\r\n");
      exit(1);
    }
  if (!lb_al_assert_dimensions_vector_c(F))
    {
      printf("invalid dimension --> lb_fo_FFT_R [F]\r\n");
      exit(1);
    }

  /* W must be half the size as the T and F vectors */
  if ((*W).items != ((*F).items/2))
    {
      printf("invalid dimension --> lb_al_vector_FFT_R [size(W)!=size(T)/2]\r\n");
      exit(1);
    }
  
  /* Vectors should have the same size */
  if ((*T).items != (*F).items)
    {
      printf("invalid dimension --> lb_fo_FFT_R [n1!=n2]\r\n");
      exit(1);
    }
  
  size=(*T).items;  /* makes the code clearer */
   
  /* Vectors must be a power of two */
  bit_size=lb_in_is_power_of_two(size);
  if (!bit_size)
    {
      printf("invalid dimension --> lb_al_vector_FFT_R [n!=2^m]\r\n");
      exit(1);
    }
  
 /* This implementation of the FFT stores the temporary results in the same output buffer, and works recursively on it */
  for(k=0;k<size;k++)
    {    
      (*F).array[k].r=(*T).array[lb_in_invert_bits(k,bit_size)];
      (*F).array[k].i=0.0;
    }
 
  for(stage=0;stage<bit_size;stage++)
    {
      two_pow_stage          = 0x01<<stage;
      two_pow_stage_plus_one = 2*two_pow_stage;
      w_index_factor         = 0x01<<(bit_size-1-stage);

      for (k=0;k<w_index_factor;k++)
      {
	addr_3=k*two_pow_stage_plus_one + two_pow_stage;
	for (l=0;l<two_pow_stage;l++)
	  {
	    addr_1=k*two_pow_stage_plus_one + l;
	    // correct, but can be made faster:  addr_2=k*two_pow_stage_plus_one + two_pow_stage + l;
	    addr_2=addr_1 + two_pow_stage;
	    
	    temp1=(*F).array[addr_1];
	    temp2=(*F).array[addr_2];
	    
	    W_temp=(*W).array[l*w_index_factor];

	    (*F).array[addr_1]=lb_cp_add(  temp1,lb_cp_multiply(W_temp,temp2));
	    if (l==0)
	      (*F).array[addr_2]=lb_cp_substract(temp1,lb_cp_multiply(W_temp,temp2));
	  }
	for (l=0;l<two_pow_stage;l++)
	  {
	    (*F).array[addr_3 + l].r =   (*F).array[addr_3 -l].r;
	    (*F).array[addr_3 + l].i =  -(*F).array[addr_3 -l].i;
	  }
      }
    }
 }


void lb_fo_IFFT_R(VECTOR_C_T *T, VECTOR_C_T *F, VECTOR_C_T *W) 
{
  UINT8_T stage, bit_size;
  UINT16_T k, l, size, addr_1, addr_2, two_pow_stage, two_pow_stage_plus_one, w_index_factor;
  REAL_T temp_f;
  COMPLEX_T temp1, temp2, W_temp;

  /* We assert the size */
  if(!lb_al_assert_dimensions_vector_c(T))
    {
      printf("invalid dimension --> lb_al_vector_FFT_C [T]\r\n");
      exit(1);
    }
  if (!lb_al_assert_dimensions_vector_c(F))
    {
      printf("invalid dimension --> lb_al_vector_FFT_C [F]\r\n");
      exit(1);
    }
  
  /* Vectors should have the same size */
  if ((*T).items != (*F).items)
    {
      printf("invalid dimension --> lb_al_vector_FFT_C [n1!=n2]\r\n");
      exit(1);
    }

  /* W must be half the size as the T and F vectors */
  if ((*W).items != ((*F).items/2))
    {
      printf("invalid dimension --> lb_al_vector_FFT_C [size(W)!=size(T)/2]\r\n");
      exit(1);
    }

  size=(*T).items;  /* makes the code clearer */
  
  /* Vectors must be a power of two */
  bit_size=lb_in_is_power_of_two(size);
  if (!bit_size)
    {
      printf("invalid dimension --> lb_al_vector_FFT_C [n!=2^m]\r\n");
      exit(1);
    }
  
  /* This implementation of the FFT stores the temporary results in the same output buffer, and works recursively on it */
  for(k=0;k<size;k++)
    (*F).array[k]=(*T).array[lb_in_invert_bits(k,bit_size)];

  for(stage=0;stage<bit_size;stage++)
    {
      two_pow_stage          = 0x01<<stage;
      two_pow_stage_plus_one = 2*two_pow_stage;
      w_index_factor         = 0x01<<(bit_size-1-stage);
        
      for (k=0;k<w_index_factor;k++)
	{
	  for (l=0;l<two_pow_stage;l++)
	    {
	      addr_1=k*two_pow_stage_plus_one + l;
	      // correct, but can be made faster: addr_2=k*two_pow_stage_plus_one + two_pow_stage + l;
	       addr_2=addr_1 + two_pow_stage;
	   	      
	      temp1=(*F).array[addr_1];
	      temp2=(*F).array[addr_2];
	      W_temp=(*W).array[l*w_index_factor];
	    
	      (*F).array[addr_1].r = temp1.r + W_temp.r*temp2.r - W_temp.i*temp2.i; 
	      (*F).array[addr_2].r = temp1.r - W_temp.r*temp2.r + W_temp.i*temp2.i; 
	      
	      if (addr_2>two_pow_stage_plus_one)
		{
		  (*F).array[addr_2].i = temp1.i - W_temp.r*temp2.i - W_temp.i*temp2.r;
		  if (addr_1>two_pow_stage_plus_one)
		    (*F).array[addr_1].i = temp1.i + W_temp.r*temp2.i + W_temp.i*temp2.r;
		  else
		    (*F).array[addr_1].i = 0.0;
		}
	      else
		{
		  (*F).array[addr_1].i = 0.0;
		  (*F).array[addr_2].i = 0.0;
		} 
	      /*
	      if (addr_1<=two_pow_stage_plus_one)
		(*F).array[addr_1].i = 0.0;
	      else
		(*F).array[addr_1].i = temp1.i + W_temp.r*temp2.i + W_temp.i*temp2.r; 
	      
	      if (addr_2<=two_pow_stage_plus_one)
		(*F).array[addr_2].i = 0.0;
	      else
	      (*F).array[addr_2].i = temp1.i - W_temp.r*temp2.i - W_temp.i*temp2.r; */
	    }
	}
    } /* stage*/
  temp_f=1/(REAL_T)size;
  for (l=0;l<size;l++)
    (*F).array[l].r=(*F).array[l].r*temp_f;
}
