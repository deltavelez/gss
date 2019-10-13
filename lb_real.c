#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <fenv.h>
#include "lb_types.h"
#include "lb_real.h"

/* 
M_E        The base of natural logarithms.
M_LOG2E    The logarithm to base 2 of M_E.
M_LOG10E   The logarithm to base 10 of M_E.
M_LN2      The natural logarithm of 2.
M_LN10     The natural logarithm of 10.
M_PI       Pi, the ratio of a circle’s circumference to its diameter.
M_PI_2     Pi divided by two.
M_PI_4     Pi divided by four.
M_1_PI     The reciprocal of pi (1/pi)
M_2_PI     Two times the reciprocal of pi.
M_2_SQRTPI Two times the reciprocal of the square root of pi.
M_SQRT2    The square root of two.
M_SQRT1_2  The reciprocal of the square root of two (also the square root of 1/2). */

/******************************************************************************************/
/* Basic operations */
/******************************************************************************************/

FLOAT_T lb_re_divide(FLOAT_T x, FLOAT_T y, ERR_T *error)
{
 int raised;
 FLOAT_T result;

 feclearexcept(FE_ALL_EXCEPT);
 result=x/y;
 raised=fetestexcept(FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW);
 /* Error handling */
 if (raised & FE_INVALID)
   *error=e_complex;
 else
   if (raised & FE_DIVBYZERO)
     *error=e_div_zero;
   else
     if (raised & FE_OVERFLOW)
       *error=e_too_big;
 /* This case will be ommited 
    if (raised & FE_UNDERFLOW) 
    printf("number is too small\r\n"); */
 if(raised)
   return 0;
 return result;
}

FLOAT_T lb_re_pow(FLOAT_T x, FLOAT_T y,  ERR_T *error)
{
 int raised;
 FLOAT_T result;

 feclearexcept(FE_ALL_EXCEPT);
 result=pow(x,y);
 raised=fetestexcept(FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW);
 /* Error handling */
 if (raised & FE_INVALID)
   *error=e_complex;
 else
   if (raised & FE_DIVBYZERO)
     *error=e_div_zero;
   else
     if (raised & FE_OVERFLOW)
       *error=e_too_big;
 /* This case will be ommited 
    if (raised & FE_UNDERFLOW) 
    printf("number is too small\r\n"); */
 if(raised)
   return 0;
 return result;
}

FLOAT_T lb_re_sqr(FLOAT_T x, ERR_T *error)
{
    FLOAT_T max_argument;
    if (sizeof(FLOAT_T)==4)
      max_argument=1.8446743524e19;
    else
      if (sizeof(FLOAT_T)==8)
	max_argument=1.8446743524e19;
    
    if (x<max_argument)
    return x*x;
    else	
      {
	*error=e_too_big;
	return 0;
    }
}

FLOAT_T lb_re_sqrt(FLOAT_T x, ERR_T *error)
{
  if (x<0)
    {
      *error=e_complex;
      return 0.0;
    }
  return sqrt(x);
}

FLOAT_T lb_re_xroot(FLOAT_T x, FLOAT_T y, ERR_T *error)
{
  if (lb_re_equal(x,0.0))
    {
      *error=e_div_zero;
      return 0.0;
    }
  return lb_re_pow(y,1/x,error);
}

/******************************************************************************************/
/* Euler */
/******************************************************************************************/

FLOAT_T lb_re_exp(FLOAT_T x, ERR_T *error)
{
  FLOAT_T max_argument;
  if (sizeof(FLOAT_T)==4)
    max_argument=88.722839;
  else
    if (sizeof(FLOAT_T)==8)
      max_argument=709.782712;
      else
	{
	  printf("Error: lb_re_exp(): invalid data type: sizeof(FLOAT_T)=%zd\r\n",sizeof(FLOAT_T));
	  exit(1);
	}
  if (x<max_argument)
    return exp(x);
  else	
    {
      *error=e_too_big;
      return 0;
    }
}

FLOAT_T lb_re_ln(FLOAT_T x, ERR_T *error)
{
 int raised;
 FLOAT_T result;

 feclearexcept(FE_ALL_EXCEPT);
 result=log(x);
 raised=fetestexcept(FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW);
 /* Error handling */
 if (raised & FE_INVALID)
   *error=e_complex;
 else
   if (raised & FE_DIVBYZERO)
     *error=e_div_zero;
   else
     if (raised & FE_OVERFLOW)
       *error=e_too_big;
 /* This case will be ommited 
    if (raised & FE_UNDERFLOW) 
    printf("number is too small\r\n"); */
 if(raised)
   return 0;
 return result;
}

FLOAT_T lb_re_log(FLOAT_T x, ERR_T *error)
{
 int raised;
 FLOAT_T result;

 feclearexcept(FE_ALL_EXCEPT);
 result=log10(x);
 raised=fetestexcept(FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW);
 /* Error handling */
 if (raised & FE_INVALID)
   *error=e_complex;
 else
   if (raised & FE_DIVBYZERO)
     *error=e_div_zero;
   else
     if (raised & FE_OVERFLOW)
       *error=e_too_big;
 /* This case will be ommited 
    if (raised & FE_UNDERFLOW) 
    printf("number is too small\r\n"); */
 if(raised)
   return 0;
 return result;
}

FLOAT_T lb_re_log2(FLOAT_T x, ERR_T *error)
{
 int raised;
 FLOAT_T result;

 feclearexcept(FE_ALL_EXCEPT);
 result=log2(x);
 raised=fetestexcept(FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW);
 /* Error handling */
 if (raised & FE_INVALID)
   *error=e_complex;
 else
   if (raised & FE_DIVBYZERO)
     *error=e_div_zero;
   else
     if (raised & FE_OVERFLOW)
       *error=e_too_big;
 /* This case will be ommited 
    if (raised & FE_UNDERFLOW) 
    printf("number is too small\r\n"); */
 if(raised)
   return 0;
 return result;
}

/******************************************************************************************/
/* Trigonometric - circular */
/******************************************************************************************/

FLOAT_T lb_re_tan(FLOAT_T x, ERR_T *error)
{
 int raised;
 FLOAT_T result;

 feclearexcept(FE_ALL_EXCEPT);
 result=tan(x);
 raised=fetestexcept(FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW);
 /* Error handling */
 if (raised & FE_INVALID)
   *error=e_complex;
 else
   if (raised & FE_DIVBYZERO)
     *error=e_div_zero;
   else
     if (raised & FE_OVERFLOW)
       *error=e_too_big;
 /* This case will be ommited 
    if (raised & FE_UNDERFLOW) 
    printf("number is too small\r\n"); */
 if(raised)
   return 0;
 return result;
}

FLOAT_T lb_re_asin(FLOAT_T x,ERR_T *error)
{
  if (fabs(x)>1)
    {
      *error=e_arg_larger_one;
      return 0;
    }
  return asin(x);
}

FLOAT_T lb_re_acos(FLOAT_T x,ERR_T *error)
{
  if (fabs(x)>1)
    {
      *error=e_arg_larger_one;
      return 0;
    }
  return acos(x);
}

/******************************************************************************************/
/* Trigonometric - hyperbolic */
/******************************************************************************************/

FLOAT_T lb_re_sinh(FLOAT_T x, ERR_T *error)
{
  if (sizeof(FLOAT_T)==4)
    if (x<88.722839)    
      return sinh(x);
    else ;
  else
    if (sizeof(FLOAT_T)==8)
      if (x<709.782712)
	return sinh(x);
      else ;
    else
      {
	printf("Error: lb_re_sinh(): invalid data type: sizeof(FLOAT_T)=%zd\r\n",sizeof(FLOAT_T));
	exit(1);
      }
  *error=e_too_big;
  return 0;
}

FLOAT_T lb_re_cosh(FLOAT_T x, ERR_T *error)
{
  if (sizeof(FLOAT_T)==4)
    if (x<88.722839)    
      return cosh(x);
    else ;
  else
    if (sizeof(FLOAT_T)==8)
      if (x<709.782712)
	return cosh(x);
      else ;
    else
      {
	printf("Error: lb_re_cosh(): invalid data type: sizeof(FLOAT_T)=%zd\r\n",sizeof(FLOAT_T));
	exit(1);
      }
  *error=e_too_big;
  return 0;
}

FLOAT_T lb_re_acosh(FLOAT_T x,ERR_T *error)
{
  if (x<1)
    {
      *error=e_arg_smaller_one;
      return 0;
    }
  return acosh(x);
}

FLOAT_T lb_re_atanh(FLOAT_T x,ERR_T *error)
{
  if (fabs(x)>1)
    {
      *error=e_arg_larger_one;
      return 0;
    }
  return atanh(x);
}

FLOAT_T lb_re_atan2(FLOAT_T y, FLOAT_T x)
{
  if (x==0) x=0;  // 0 has several representations.  
                  // atan2(1,-0.0)=3.1416 [OK]  atan2(1.0,-0.0)=-3.1416 [WRONG]
  if (y==0) y=0;
  return atan2(y,x);
}

/******************************************************************************************/
/* Comparison */
/******************************************************************************************/

S_INT8_T lb_re_equal(FLOAT_T a, FLOAT_T b)
{
  FLOAT_T max_diff, largest, diff;
  if (sizeof(FLOAT_T)==4)
    max_diff = 10.0*FLT_EPSILON;
  else
    if (sizeof(FLOAT_T)==8)
      max_diff = 10.0*DBL_EPSILON;
    else
      {
	printf("Error: lb_re_equal(): invalid floating data type: [size = %zd]\r\n",sizeof(FLOAT_T));
	exit(1);
      }
  //printf("Epsilon = %1.9e\r\n",max_diff);
  diff = fabs(a-b);
  if (diff <= max_diff) /* absolute comparison, good when values are close to zero */
    return TRUE;
  
  a = fabs(a);
  b = fabs(b);
  largest = (b > a) ? b : a;
 
  if (diff <= largest * max_diff)  /* relative comparison, good when values are larger than zero */
    return TRUE;
  return FALSE;
}

S_INT8_T lb_re_is_int(FLOAT_T x)
{
  if (lb_re_equal(x-(S_INT16_T)x,0))
    return 1;
  return 0;
}

S_INT8_T lb_re_larger_or_equal(FLOAT_T a, FLOAT_T b)
{
  if (a>b)
    return TRUE;

  if (lb_re_equal(a,b))
    return TRUE;

  return FALSE;
}

S_INT8_T lb_re_smaller_or_equal(FLOAT_T a, FLOAT_T b)
{
  if (a<b)
    return TRUE;

  if (lb_re_equal(a,b))
    return TRUE;

  return FALSE;
}

FLOAT_T lb_re_max(FLOAT_T x, FLOAT_T y)
{
  if (x>=y) return x;
  return y;
}

FLOAT_T lb_re_min(FLOAT_T x, FLOAT_T y)
{
  if (x<=y) return x;
  return y;
}

/******************************************************************************************/
/* Singular functions */
/******************************************************************************************/

FLOAT_T lb_re_frac(FLOAT_T x)
{
  return fmod(x,1.0);
}

S_INT8_T lb_re_ispos(FLOAT_T x)
{
  if (x>=0) return 1.0;
  return 0.0;
}

FLOAT_T lb_re_ramp(FLOAT_T x)
{
  if(x>=0) return x;
  return 0.0;
}

S_INT8_T lb_re_sign(FLOAT_T x)
{
  if (x>=0) return 1.0;
  if (x<0) return -1.0;
  return 0.0;
}

FLOAT_T lb_re_pulse(FLOAT_T a, FLOAT_T b, FLOAT_T x)
{
  if ((x==a) || (x==b)) return 0.5;
  if ((x<a)  || (x>b)) return 0.0;
  return 1;
}

FLOAT_T lb_re_pulse_triangle(FLOAT_T a, FLOAT_T b, FLOAT_T x)
{
  if (a==b)
    { 
      printf("invalid data --> lb_re_pulse_triangle\r\n");
      return 0;
    }
  if ((x<=a)  || (x>=b)) return 0.0;
  if (x<=0.5*(b+a)) return 2*(x-a)/(b-a);
  return 2*(b-x)/(b-a);
}

FLOAT_T lb_re_step(FLOAT_T x)
{
  if(x==0) return 0.5;
  if (x>0) return 1.0;
  return 0.0; /* if x<0 return 0.0 */
}

FLOAT_T lb_re_step_practical(FLOAT_T t, FLOAT_T x)
{
  if (x<=0) return 0.0;
  if (x>=t) return 1.0;
  return x/t;
}

/******************************************************************************************/
/* Other functions */
/******************************************************************************************/

/* Returns the minimum number of digits required to represent a number */
S_INT16_T lb_re_ndigits(FLOAT_T x)
{
  if (x>0)
    return log10(x)+1.0;
  return 1;
}


FLOAT_T lb_re_factorial(FLOAT_T n,ERR_T *error)
{
  FLOAT_T temp_f;
  S_INT16_T n_int;

  if (n<0)
    *error=e_arg_negative;
  else
    if (!lb_re_equal(n,(S_INT16_T)n))
      *error=e_arg_non_int;
    else
      if (sizeof(FLOAT_T)==4)
	if (n>34) 
	  *error=e_too_big;
	else ;
      else
	if (sizeof(FLOAT_T)==8)
	  if (n>170)
	    *error=e_too_big;
	  else; 
	else
	  {
	    printf("Error: lb_re_factorial(): invalid data type: sizeof(FLOAT_T)=%zd\r\n",sizeof(FLOAT_T));
	    exit(1);
	  }
  if ((*error) != e_none)
    return 0;

  temp_f=1.0;
  n_int=n;
  while (n_int>1)
    {
      temp_f*=n_int;
      n_int--;
    }
  return temp_f;
}

/* Returns the normed significand, that is, a number between 0 and 0.9999 x 10^n */

FLOAT_T lb_re_normed_exponent(FLOAT_T x)
{
  if (x>0)
    return (S_INT16_T)floor(log10(fabs(x)));
  return 0;
}

FLOAT_T lb_re_normed_significand(FLOAT_T x)
{
  if (x>0)
    return x/pow(10.0,floor(log10(x)));
  if (x<0)
    return x/pow(10.0,floor(log10(-x)));
  return 0;
}

/* Returns 1 if x is in the closed interval, otherwise returns 0 */
FLOAT_T lb_re_test_interval_closed(FLOAT_T a, FLOAT_T b, FLOAT_T x)
{
  if  ( ((a<=x) && (x<=b)) || ((b<=x) && (x<=a)) )
    return 1;
  return 0;
}

/* Returns 1 if x is in the open interval, otherwise returns 0 */
FLOAT_T lb_re_test_interval_open(FLOAT_T a, FLOAT_T b, FLOAT_T x)
{
  if  ( ((a<x) && (x<b)) || ((b<x) && (x<a)) )
    return 1;
  return 0;
}


/******************************************************************************************/
/* Financial */
/******************************************************************************************/

FLOAT_T lb_re_APR_to_monthly(FLOAT_T apr, ERR_T *error)
{
  /* Converts a half-yearly, not in advanced rate to a simple monthly rate */
  return 100.0*lb_re_xroot(6,1+apr/200,error)-100.0;
}

FLOAT_T lb_re_monthly_to_APR(FLOAT_T monthly, ERR_T *error)
{
  /* Converts a simple monthly rate to an APR half-yearly, not in advanced rate */  
  return 200*lb_re_pow(1.0+monthly/100,6,error)-200.0;
}

FLOAT_T lb_re_monthly_to_effective(FLOAT_T monthly, ERR_T *error)
{
  /* Converts a simple montly rate to an effective yearly rate */
  return 100*lb_re_pow(1.0+monthly/100.0,12,error)-100.0;
}

/******************************************************************************************/
/* Interpolation */
/******************************************************************************************/

void lb_re_inter_linear(FLOAT_T x0, FLOAT_T Q0, FLOAT_T x1, FLOAT_T Q1,
			FLOAT_T x, FLOAT_T *Q, ERR_T *error)
{
  if(lb_re_equal(x1,x0))
    {
      *error=e_div_zero;
      *Q=0.0;
    }
  else
    *Q=Q0+(Q1-Q0)*(x-x0)/(x1-x0);
}

void lb_re_inter_bilinear(FLOAT_T x0, FLOAT_T y0, FLOAT_T x1, FLOAT_T y1,
			  FLOAT_T Q00, FLOAT_T Q01, FLOAT_T Q10, FLOAT_T Q11,
			  FLOAT_T x, FLOAT_T y, FLOAT_T *Q, ERR_T *error)
{
  FLOAT_T denom, temp;
  denom=(x1-x0)*(y1-y0);
  if(lb_re_equal(denom,0.0))
    {
      *error=e_div_zero;
      *Q=0.0;
    }
  else
    {
      temp=(y1-y)*((x1-x)*Q00+(x-x0)*Q10)+(y-y0)*((x1-x)*Q01+(x-x0)*Q11);
      *Q=temp/denom;
    }
}


/******************************************************************************************/
/* Rotation */
/******************************************************************************************/

void lb_re_rotate(FLOAT_T x, FLOAT_T y, FLOAT_T angle, FLOAT_T *xr, FLOAT_T *yr)
{
  FLOAT_T cos_val, sin_val;
  cos_val=cos(angle);
  sin_val=sin(angle);
  *xr=x*cos_val - y*sin_val;
  *yr=y*cos_val + x*sin_val;

}
