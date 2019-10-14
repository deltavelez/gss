#include <stdio.h>
#include <stdlib.h>
#include "lb_complex.h"
#include "lb_types.h"
#include "lb_integer.h"
#include "lb_parser.h"
#include "lb_real.h"
#include "lb_numer.h"


REAL_T lb_nu_diff(FN_RECORD_T *function, INDEX_TYPE var, REAL_T *values, REAL_T xo, REAL_T delta, MATHERROR_T *error)
{
  REAL_T vec_var[3];
  REAL_T a, b;
  SINT16_T i;
 
  if(lb_re_equal(delta,0))
    *error=e_div_zero;
    else
      if (delta<0)
	*error=e_arg_negative;
  
  for(i=0;i<3;i++)
    vec_var[i]=values[i];
  
  /* Left */
  vec_var[var]=xo-delta;
  a=lb_pa_eval_real(function, vec_var, error);

  /* Right */
  vec_var[var]=xo+delta;
  b=lb_pa_eval_real(function, vec_var, error);
	      
  if ((*error)!=e_none)
    return 0.0;
  
  return 0.5*(b-a)/delta;
}

REAL_T lb_nu_diff2(FN_RECORD_T *function, INDEX_TYPE var, REAL_T *values, REAL_T xo, REAL_T delta, MATHERROR_T *error)
{
  REAL_T vec_var[3];
  REAL_T a, middle, b;
  SINT16_T i;
 
  if(lb_re_equal(delta*delta,0))
    *error=e_div_zero;
    else
      if (delta<0)
	*error=e_arg_negative;
  
  for(i=0;i<3;i++)
    vec_var[i]=values[i];
  
  /* Left */
  vec_var[var]=xo-2.0*delta;
  a=lb_pa_eval_real(function, vec_var, error);

  /* Middle */
  vec_var[var]=xo;
  middle=lb_pa_eval_real(function, vec_var, error);
  
  /* Right */
  vec_var[var]=xo+2.0*delta;
  b=lb_pa_eval_real(function, vec_var, error);
	      
  if ((*error)!=e_none)
    return 0.0;
  
  return 0.25*(a+b-2.0*middle)/(delta*delta);
}

COMPLEX_T lb_nu_diff_c(FN_RECORD_T *function, INDEX_TYPE var, COMPLEX_T *values, COMPLEX_T xo, REAL_T delta, MATHERROR_T *error)
{
  COMPLEX_T vec_var[3];
  COMPLEX_T a, b;
  SINT16_T i;
 
  if(lb_re_equal(delta,0))
    *error=e_div_zero;
    else
      if (delta<0)
	*error=e_arg_negative;
  
  for(i=0;i<3;i++)
    vec_var[i]=values[i];
  
  vec_var[var].i=0.0;
  /* Left */
  vec_var[var].r=xo.r - delta;
  a=lb_pa_eval_complex(function, vec_var, error);

  /* Right */
  vec_var[var].r=xo.r + delta;
  b=lb_pa_eval_complex(function, vec_var, error);
	      
  if ((*error)!=e_none)
    return ZERO_C;
  
  return lb_cp_multiply_real(lb_cp_substract(b,a),0.5/delta);
}


COMPLEX_T lb_nu_diff2_c(FN_RECORD_T *function, INDEX_TYPE var, COMPLEX_T *values, COMPLEX_T xo, REAL_T delta, MATHERROR_T *error)
{
  COMPLEX_T vec_var[3];
  COMPLEX_T a, b, middle, temp;
  SINT16_T i;
 
  if(lb_re_equal(delta*delta,0))
    *error=e_div_zero;
    else
      if (delta<0)
	*error=e_arg_negative;
  
  for(i=0;i<3;i++)
    vec_var[i]=values[i];
  
  vec_var[var].i=0.0;
  /* Left */
  vec_var[var].r=xo.r - 2.0*delta;
  a=lb_pa_eval_complex(function, vec_var, error);

  /* Middle */
  vec_var[var].r=xo.r;
  middle=lb_pa_eval_complex(function, vec_var, error);

  /* Right */
  vec_var[var].r=xo.r + 2.0*delta;
  b=lb_pa_eval_complex(function, vec_var, error);
	      
  if ((*error)!=e_none)
    return ZERO_C;
  
  temp.r=0.25*(a.r+b.r-2.0*middle.r)/(delta*delta);
  temp.i=0.25*(a.i+b.i-2.0*middle.i)/(delta*delta);

  return temp;
}

SINT8_T lb_nu_find_zero(REAL_T x0, REAL_T y0, REAL_T x1, REAL_T y1, REAL_T *x_root)
{
  if(lb_re_equal(y0,0) && lb_re_equal(y1,0))
      return -1; /* All points in interval are a solution */
   if(lb_re_sign(y0)==lb_re_sign(y1))
      return 0; /* No solution in interval */
   *x_root=x1-y1*(x1-x0)/(y1-y0);
   return 1;
}

REAL_T lb_nu_sigma(REAL_T a, REAL_T b, FN_RECORD_T *function, INDEX_TYPE var, REAL_T delta, REAL_T *values, MATHERROR_T *error)
{
  REAL_T vec_var[3];
  REAL_T start, end, step, sum;
  SINT16_T i;
  
  if ((b>a) && (delta>0)) 
    {
      start=a;
      end=b;
      step=delta;
    }
  else
    if ((b<a) && (delta<0))
    {
      start=b;
      end=a; 
      step=-delta;
    }
    else
      {
	*error=e_arg_inf_loop;
	return 0.0;
      }
  
  for(i=0;i<3;i++)
    vec_var[i]=values[i];

  sum=0.0;
  vec_var[var]=start;
  while(lb_re_smaller_or_equal(vec_var[var],end))
    {
      sum+=lb_pa_eval_real(function, vec_var, error);
      vec_var[var]+=step;
    }
  return sum;
}

COMPLEX_T lb_nu_sigma_c(REAL_T a, REAL_T b, FN_RECORD_T *function, INDEX_TYPE var, REAL_T delta, COMPLEX_T *values, MATHERROR_T *error)
{
  COMPLEX_T vec_var[3], sum;
  REAL_T start, end, step;
  SINT16_T i;
  
  if ((b>a) && (delta>0)) 
    {
      start=a;
      end=b;
      step=delta;
    }
  else
    if ((b<a) && (delta<0))
    {
      start=b;
      end=a; 
      step=-delta;
    }
    else
      {
	*error=e_arg_inf_loop;
	return ZERO_C;
      }
  
  for(i=0;i<3;i++)
    vec_var[i]=values[i];

  sum=ZERO_C;
  vec_var[var].r=start;
  vec_var[var].i=0.0;
  while(lb_re_smaller_or_equal(vec_var[var].r,end))
    {
      sum=lb_cp_add(sum,lb_pa_eval_complex(function, vec_var, error));
      vec_var[var].r+=step;
    }
  return sum;
}

REAL_T lb_nu_prod(REAL_T a, REAL_T b, FN_RECORD_T *function, INDEX_TYPE var, REAL_T delta, REAL_T *values, MATHERROR_T *error)
{
  REAL_T vec_var[3];
  REAL_T start, end, step, prod;
  SINT16_T i;

  if ((b>a) && (delta>0)) 
    {
      start=a;
      end=b;
      step=delta;
    }
  else
    if ((b<a) && (delta<0))
      {
	start=b;
	end=a; 
	step=-delta;
      }
    else
      {
	*error=e_arg_inf_loop;
	return 0.0;
      }
  
  for(i=0;i<3;i++)
    vec_var[i]=values[i];

  prod=1;
  vec_var[var]=start;
  while(vec_var[var]<=end)
    {
      //printf("x=%f\r\n",vec_var[var]);
      prod*=lb_pa_eval_real(function, vec_var, error);
      vec_var[var]+=step;
    }
  return prod;
}

COMPLEX_T lb_nu_prod_c(REAL_T a, REAL_T b, FN_RECORD_T *function, INDEX_TYPE var, REAL_T delta, COMPLEX_T *values, MATHERROR_T *error)
{
  COMPLEX_T vec_var[3], prod;
  REAL_T start, end, step;
  SINT16_T i;
  
  if ((b>a) && (delta>0)) 
    {
      start=a;
      end=b;
      step=delta;
    }
  else
    if ((b<a) && (delta<0))
    {
      start=b;
      end=a; 
      step=-delta;
    }
    else
      {
	*error=e_arg_inf_loop;
	return ZERO_C;
      }
  
  for(i=0;i<3;i++)
    vec_var[i]=values[i];

  prod.r=1.0;
  prod.i=0.0;
  vec_var[var].r=start;
  vec_var[var].i=0.0;
  while(lb_re_smaller_or_equal(vec_var[var].r,end))
    {
      prod=lb_cp_multiply(prod,lb_pa_eval_complex(function, vec_var, error));
      vec_var[var].r+=step;
    }
  return prod;
}

/* RK4 performs a single iteration using the 4th order Runge-Kutta method */
void rk4(REAL_T *x_n, REAL_T *y_n, REAL_T h, MATHERROR_T *error, FUNCTION_X_Y f_xy)
{
  REAL_T k1, k2, k3, k4;
 
  k1 = f_xy(*x_n         , *y_n          , error);
  k2 = f_xy(*x_n + 0.5*h , *y_n + 0.5*k1 , error);
  k3 = f_xy(*x_n + 0.5*h , *y_n + 0.5*k2 , error);
  k4 = f_xy(*x_n + h     , *y_n + k3     , error);

  *x_n += h;
  *y_n += h*((k1+k4)*0.16666666666666666666 + (k2+k3)*0.33333333333333333333); 
}

void euler(REAL_T *x_n, REAL_T *y_n, REAL_T h, MATHERROR_T *error, FUNCTION_X_Y f_xy)
{
  *x_n += h;
  *y_n += h*f_xy(*x_n, *y_n, error);
}

REAL_T lb_nu_simpson(REAL_T a, REAL_T b, FN_RECORD_T *function, INDEX_TYPE var, SINT16_T n, REAL_T *values, MATHERROR_T *error)
/* a, b: Integration interval for var1
   m, n: Number of divisions in var1 and var2, respectively.  Both must be odd nuymbers.
   *fnrec: Address of a function as an RPN list of values
   *values: Address of a vector with all the variables values.  
   The values for var1 and var2 will be ignored as these become integration variables
   *error: output error code, if it occurs. */
{
  REAL_T vec_var[3];
  REAL_T sum, delta;
  SINT16_T i;
  
  if (!lb_in_is_even(n))
    *error=e_arg_non_even;  
  
  if (n<0)
    *error=e_arg_negative;

  if (n>65535.0)
    *error=e_arg_too_big;

  if (n<2)
    *error=e_arg_less_two;
 
  if ((*error)!=e_none)
    return 0.0; 
  
  for(i=0;i<3;i++)
    vec_var[i]=values[i];

  delta=(b-a)/(REAL_T)n;
  
  sum=0.0;
  for(i=1;i<=n/2;i++)
    {
      vec_var[var]=a+delta*(2*i-2);
      sum+=lb_pa_eval_real(function, vec_var, error);
      
      vec_var[var]=a+delta*(2*i-1);
      sum+=4.0*lb_pa_eval_real(function, vec_var, error);
      
      vec_var[var]=a+delta*(2*i);
      sum+=lb_pa_eval_real(function, vec_var, error);
    }
  printf("sum=%f\r\n",sum*delta/3.0);
  return sum*delta/3.0;
}

COMPLEX_T lb_nu_simpson_c(COMPLEX_T a, COMPLEX_T b, FN_RECORD_T *function, INDEX_TYPE var, SINT16_T n, COMPLEX_T *values, MATHERROR_T *error)
{
  COMPLEX_T vec_var[3];
  COMPLEX_T sum, delta;
  SINT16_T i;
  
  if (!lb_in_is_even(n))
    *error=e_arg_non_even;  
  
  if (n<0)
    *error=e_arg_negative;

  if (n>65535.0)
    *error=e_arg_too_big;

  if (n<2)
    *error=e_arg_less_two;
 
  if ((*error)!=e_none)
    return ZERO_C; 
  
  for(i=0;i<3;i++)
    vec_var[i]=values[i];
  
  sum=ZERO_C;
  delta=lb_cp_divide_real(lb_cp_substract(b,a),n,error);

  for(i=1;i<=n/2;i++)
    {
      vec_var[var]=lb_cp_add(a,lb_cp_multiply_real(delta,2*i-2));
      sum=lb_cp_add(sum,lb_pa_eval_complex(function, vec_var, error));
		  
      vec_var[var]=lb_cp_add(a,lb_cp_multiply_real(delta,2*i-1));
      sum=lb_cp_add(sum,lb_cp_multiply_real(lb_pa_eval_complex(function, vec_var, error),4.0));
	      
      vec_var[var]=lb_cp_add(a,lb_cp_multiply_real(delta,2*i));
      sum=lb_cp_add(sum,lb_pa_eval_complex(function, vec_var, error));
    }
  
  return lb_cp_divide_real(lb_cp_multiply(sum,delta),3.0,error);
}


REAL_T lb_nu_simpson2d(REAL_T c, REAL_T d, REAL_T a, REAL_T b, FN_RECORD_T *function, INDEX_TYPE var1, INDEX_TYPE var2, SINT16_T m, SINT16_T n, REAL_T *values, MATHERROR_T *error)
/* Parameters:
   c, d: Integration interval for var2
   a, b: Integration interval for var1
   m, n: Number of divisions in var1 and var2, respectively.  Both must be odd nuymbers.
   *fnrec: Address of a function as an RPN list of values
   *values: Address of a vector with all the variables values.  
   The values for var1 and var2 will be ignored as these become integration variables
   *error: output error code, if it occurs. */
{
  REAL_T vec_vars[MAX_NUMBER_VARIABLES];
  REAL_T delta_x, delta_y, sum;
  SINT16_T i, j;
 
  /* Asserting parameters */

  if (!lb_in_is_odd(m) || !lb_in_is_odd(n))
    *error=e_arg_non_odd;  
  
  if ((m<0) || (n<0))
    *error=e_arg_negative;

  if ((m>65535.0) || (n>65535.0))
    *error=e_arg_too_big;

  if ((m<3) || (n<3))
    *error=e_arg_too_small;

  if ((*error)!=e_none)
    return 0.0; 
  
  for(i=0;i<3;i++)
    vec_vars[i]=values[i];
   
  delta_x=(b-a)/(m-1);
  delta_y=(d-c)/(n-1);
  
  sum=0.0;
  for(j=0;j<n;j++)
    {
      vec_vars[var2]=c+j*delta_y;
      for(i=0;i<m;i++)
	{
	  vec_vars[var1]=a+i*delta_x;
#ifdef DEBUG_IDEF2
      	  printf("S[%d,%d]=%2d*%07.3f   ",i,j,lb_in_simpson2d_coef(i,j,m,n),lb_pa_eval_real(function, vec_vars, error));
#endif
	  sum+=lb_pa_eval_real(function, vec_vars, error)*lb_in_simpson2d_coef(i,j,m,n);
	}
#ifdef DEBUG_IDEF2
      printf("\n");
#endif
    }
  if ((*error)!=e_none)
    return 0.0; 
  
  return sum*delta_x*delta_y/9.0;   
}

COMPLEX_T lb_nu_simpson2d_c(COMPLEX_T c, COMPLEX_T d, COMPLEX_T a, COMPLEX_T b, FN_RECORD_T *function, INDEX_TYPE var1, INDEX_TYPE var2, SINT16_T m, SINT16_T n, COMPLEX_T *values, MATHERROR_T *error)
{
  COMPLEX_T vec_vars[MAX_NUMBER_VARIABLES];
  COMPLEX_T temp_z, sum;
  REAL_T delta_x, delta_y;
  SINT16_T i, j;
 
  /* Asserting parameters  */

  if (!lb_in_is_odd(m) || !lb_in_is_odd(n))
    *error=e_arg_non_odd;  
  
  if ((m<0) || (n<0))
    *error=e_arg_negative;

  if ((m>65535.0) || (n>65535.0))
    *error=e_arg_too_big;

  if ((m<3) || (n<3))
    *error=e_arg_too_small;

  if ((*error)!=e_none)
    return ZERO_C; 
  
  for(i=0;i<3;i++)
  vec_vars[i]=values[i];
   
  delta_x=(b.r-a.r)/(m-1);
  delta_y=(d.r-c.r)/(n-1);
  
  sum=(COMPLEX_T){0.0,0.0};
  for(j=0;j<n;j++)
    {
      vec_vars[var2].r=c.r+j*delta_y;
      vec_vars[var2].i=0.0;
      for(i=0;i<m;i++)
	{
	  vec_vars[var1].r=a.r+i*delta_x;
	  vec_vars[var1].i=0.0;
#define DEBUG_IDEF2
#ifdef DEBUG_IDEF2
      	  printf("S[%d,%d]=%2d*",i,j,lb_in_simpson2d_coef(i,j,m,n));
	  lb_cp_print_c(lb_pa_eval_complex(function, vec_vars, error),"%05.1f");
#endif
	  temp_z=lb_cp_multiply_real(lb_pa_eval_complex(function, vec_vars, error),lb_in_simpson2d_coef(i,j,m,n));
	  sum=lb_cp_add(sum,temp_z);
	}
#ifdef DEBUG_IDEF2
      printf("\n");
#endif
    }
  if ((*error)!=e_none)
    return (COMPLEX_T){0.0, 0.0}; 
  
  return lb_cp_multiply_real(sum,delta_x*delta_y/9.0);   
}

