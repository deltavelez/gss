#include <stdio.h>
#include <string.h>
#include "math.h"
#include "lb_types.h"
#include "lb_real.h"
#include "lb_complex.h"

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

COMPLEX_T lb_cp_add(COMPLEX_T a, COMPLEX_T b)
{
  COMPLEX_T temp;
  temp.r = a.r + b.r;
  temp.i = a.i + b.i;
  return temp;
}

COMPLEX_T lb_cp_substract(COMPLEX_T a, COMPLEX_T b)
{
  COMPLEX_T temp;
  temp.r = a.r - b.r;
  temp.i = a.i - b.i;
  return temp;
}

COMPLEX_T lb_cp_neg(COMPLEX_T a)
{
  COMPLEX_T temp;
  temp.r = -a.r;
  temp.i = -a.i;
  return temp;
}

COMPLEX_T lb_cp_multiply(COMPLEX_T a, COMPLEX_T b)
{
  COMPLEX_T temp;
  temp.r = a.r*b.r - a.i*b.i;
  temp.i = a.r*b.i + a.i*b.r;
  return temp;
}

COMPLEX_T lb_cp_multiply_real(COMPLEX_T c, FLOAT_T r)
{
  COMPLEX_T temp;
  temp.r = r*c.r;
  temp.i = r*c.i;
  return temp;
}

COMPLEX_T lb_cp_divide_real(COMPLEX_T c, FLOAT_T r, ERR_T *error)
{
  COMPLEX_T temp;
  if (lb_re_equal(r,0.0))
    {
      temp.r=0.0;
      temp.i=0.0;
      *error=e_div_zero;
    }
  else
    {
      temp.r = c.r/r;
      temp.i = c.i/r;
    }
  return temp;
}

COMPLEX_T lb_cp_divide(COMPLEX_T a, COMPLEX_T b, ERR_T *error)
{
  COMPLEX_T temp;
  FLOAT_T denom;
  denom=lb_re_sqr(b.r,error)+lb_re_sqr(b.i, error);
  temp.r=lb_re_divide(a.r*b.r+a.i*b.i,denom, error);
  temp.i=lb_re_divide(a.i*b.r-a.r*b.i,denom, error);
  return temp;
}

COMPLEX_T lb_cp_conjugate(COMPLEX_T a)
{
  COMPLEX_T temp;
  temp.r =  a.r;
  temp.i = -a.i;
  return temp;
}

/* Power */

COMPLEX_T lb_cp_sqr(COMPLEX_T z, ERR_T *error)
{
  COMPLEX_T temp;
  temp.r=lb_re_sqr(z.r,error)-lb_re_sqr(z.i,error);
  temp.i=2*z.r*z.i;
  return temp;
}

COMPLEX_T lb_cp_sqrt(COMPLEX_T z, ERR_T *error)
{
  COMPLEX_T temp;
  FLOAT_T h;
  h=lb_re_sqrt(z.r*z.r + z.i*z.i,error);
  temp.r=M_SQRT1_2*lb_re_sqrt(h+z.r,error);
  temp.i=M_SQRT1_2*lb_re_sign(z.i)*lb_re_sqrt(h-z.r,error);
  return temp;
}

COMPLEX_T lb_cp_power(COMPLEX_T z1, COMPLEX_T z2, ERR_T *error)
{
 COMPLEX_T temp;
 FLOAT_T mag_z1_sqr, mag_z1, angle_z1, t1, t2; 
 
 if (lb_re_equal(z1.r,0) && lb_re_equal(z1.i,0))
   if (lb_re_equal(z2.r,0) && lb_re_equal(z2.i,0))
     {
       temp.r=1.0;
       temp.i=0.0;
     } 
   else
     {
       temp.r=0.0;
       temp.i=0.0;
     }
 else
   {
     mag_z1_sqr=z1.r*z1.r+z1.i*z1.i;
     mag_z1=sqrt(mag_z1_sqr);
     
     angle_z1=lb_cp_arg(z1);
     t1=lb_re_pow(mag_z1,z2.r,error)*lb_re_exp(-z2.i*angle_z1,error);
     t2=z2.r*angle_z1+0.5*z2.i*log(mag_z1_sqr);
 
     temp.r=t1*cos(t2);
     temp.i=t1*sin(t2);
   }
 return temp;
}

/* Euler */

COMPLEX_T lb_cp_exp(COMPLEX_T z, ERR_T *error)
{
  COMPLEX_T temp;
  temp.r = lb_re_exp(z.r,error)*cos(z.i);
  temp.i = lb_re_exp(z.r,error)*sin(z.i);
  return temp;
}

COMPLEX_T lb_cp_ln(COMPLEX_T z, ERR_T *error)
{
  COMPLEX_T s;
  FLOAT_T temp;
  temp=lb_re_sqr(z.r,error)+lb_re_sqr(z.i,error);
  if (!lb_re_equal(temp,0))
    {
      s.r=0.5*log(temp);
      s.i=lb_cp_arg(z);
    }
  else
    {
      *error=e_div_zero;
      s.r=0;
      s.i=0;
    }
  return s;
}

COMPLEX_T lb_cp_log(COMPLEX_T z, ERR_T *error)
{
  COMPLEX_T temp_z;
  temp_z=lb_cp_divide_real(lb_cp_ln(z,error),M_LN10,error);
  return temp_z;
}

COMPLEX_T lb_cp_log2(COMPLEX_T z, ERR_T *error)
{
  COMPLEX_T temp_z;
  temp_z=lb_cp_divide_real(lb_cp_ln(z,error),M_LN2,error);
  return temp_z;
}

COMPLEX_T lb_cp_logb(COMPLEX_T b, COMPLEX_T z, ERR_T *error)
{
  COMPLEX_T temp_z;
  printf("b.r=%f , b.i=%f \r\n",b.r,b.i);
  printf("z.r=%f , z.i=%f \r\n",z.r,z.i);
  temp_z=lb_cp_ln(z,error);

  temp_z=lb_cp_divide(lb_cp_ln(z,error),lb_cp_ln(b,error),error);
  return temp_z;
}

/******************************************************************************************/
/* Trigonometric - circular*/
/******************************************************************************************/

COMPLEX_T lb_cp_sin(COMPLEX_T z, ERR_T *error)
{
  COMPLEX_T s;
  s.r=sin(z.r)*lb_re_cosh(z.i,error);
  s.i=cos(z.r)*lb_re_sinh(z.i,error);
  return s;
}

COMPLEX_T lb_cp_cos(COMPLEX_T z, ERR_T *error)
{
  COMPLEX_T s;
  s.r=  cos(z.r)*lb_re_cosh(z.i,error);
  s.i= -sin(z.r)*lb_re_sinh(z.i,error);
  return s;
} 

COMPLEX_T lb_cp_tan(COMPLEX_T z, ERR_T *error)
{
  COMPLEX_T s;
  FLOAT_T denom;
  denom=cos(2*z.r)+lb_re_cosh(2*z.i,error);
  s.r=sin(2*z.r)/denom;
  s.i=lb_re_sinh(2*z.i,error)/denom;
  return s;
}

COMPLEX_T lb_cp_asin(COMPLEX_T z, ERR_T *error)
{
  COMPLEX_T s;
  FLOAT_T alpha, betha;
  alpha=0.5*(sqrt(lb_re_sqr(z.r+1,error)+lb_re_sqr(z.i,error))+sqrt(lb_re_sqr(z.r-1,error)+lb_re_sqr(z.i,error)));
  betha=0.5*(sqrt(lb_re_sqr(z.r+1,error)+lb_re_sqr(z.i,error))-sqrt(lb_re_sqr(z.r-1,error)+lb_re_sqr(z.i,error)));
  s.r=asin(betha);
  s.i=log(alpha+sqrt(lb_re_sqr(alpha,error)-1));
  return s;
}

COMPLEX_T lb_cp_acos(COMPLEX_T z, ERR_T *error)
{
  COMPLEX_T s;
  FLOAT_T alpha, betha;
  alpha=0.5*(sqrt(lb_re_sqr(z.r+1,error)+lb_re_sqr(z.i,error))+sqrt(lb_re_sqr(z.r-1,error)+lb_re_sqr(z.i,error)));
  betha=0.5*(sqrt(lb_re_sqr(z.r+1,error)+lb_re_sqr(z.i,error))-sqrt(lb_re_sqr(z.r-1,error)+lb_re_sqr(z.i,error)));
  s.r=acos(betha);
  s.i=-log(alpha+sqrt(lb_re_sqr(alpha,error)-1));
  return s;
}

COMPLEX_T lb_cp_atan(COMPLEX_T z, ERR_T *error)
{
  COMPLEX_T s;
  FLOAT_T denom;
  denom=1-lb_re_sqr(z.r,error)-lb_re_sqr(z.i,error);
  if (!lb_re_equal(denom,0))
    {
      s.r=0.5*atan(2*z.r/denom);
      s.r=0.25*log((lb_re_sqr(z.r,error)+lb_re_sqr(z.i+1,error))/(lb_re_sqr(z.r,error)+lb_re_sqr(z.i-1,error)));
    }
  else
    {
      s.r=0.0;
      s.i=0.0;
      *error=e_div_zero;
    }
  return s;
}

/******************************************************************************************/
/* Trigonometric - hyperbolic */
/******************************************************************************************/

COMPLEX_T lb_cp_sinh(COMPLEX_T z, ERR_T *error)
{
  COMPLEX_T s;
  s.r=lb_re_sinh(z.r,error)*cos(z.i);
  s.i=lb_re_cosh(z.r,error)*sin(z.i);
  return s;
}

COMPLEX_T lb_cp_cosh(COMPLEX_T z, ERR_T *error)
{
  COMPLEX_T s;
  s.r=lb_re_cosh(z.r,error)*cos(z.i);
  s.i=lb_re_sinh(z.r,error)*sin(z.i);
  return s;
}

COMPLEX_T lb_cp_tanh(COMPLEX_T z, ERR_T *error)
{
  COMPLEX_T s;
  FLOAT_T denom;
  denom=lb_re_cosh(2*z.r,error)+cos(2*z.i);
  s.r=lb_re_sinh(2*z.r,error)/denom;
  s.i=sin(2*z.i)/denom;
  return s;
}

COMPLEX_T lb_cp_asinh(COMPLEX_T z, ERR_T *error)
{
  COMPLEX_T temp_z, one;
  one.r=1.0;
  one.i=0.0;

  temp_z=lb_cp_sqr(z,error);
  temp_z=lb_cp_add(temp_z,one);
  temp_z=lb_cp_sqrt(temp_z,error);
  temp_z=lb_cp_add(temp_z,z);
  temp_z=lb_cp_ln(temp_z,error);
  return temp_z;
}

COMPLEX_T lb_cp_acosh(COMPLEX_T z, ERR_T *error)
{
  COMPLEX_T temp_z, one;
  one.r=1.0;
  one.i=0.0;

  temp_z=lb_cp_sqr(z,error);
  temp_z=lb_cp_substract(temp_z,one);
  temp_z=lb_cp_sqrt(temp_z,error);
  temp_z=lb_cp_add(temp_z,z);
  temp_z=lb_cp_ln(temp_z,error);
  return temp_z;
}

COMPLEX_T lb_cp_atanh(COMPLEX_T z, ERR_T *error)
{
  COMPLEX_T temp_z, num, denom, one;
  one.r=1.0;
  one.i=0.0;

  num=lb_cp_add(one,z);
  denom=lb_cp_substract(one,z);

  temp_z=lb_cp_divide(num,denom,error);
  printf("Z a/b="); 
  lb_cp_print_c(temp_z,FLOAT_FORMAT); 
  printf("\n");
  temp_z=lb_cp_ln(temp_z,error);
  temp_z=lb_cp_multiply_real(temp_z,0.5);
  return temp_z;
}

/******************************************************************************************/
/* Conversion or data assembly */
/******************************************************************************************/
 
FLOAT_T lb_cp_abs(COMPLEX_T z)
{
  return sqrt(z.r*z.r+z.i*z.i);
}

FLOAT_T lb_cp_arg(COMPLEX_T z)
{
  if (z.r==0)
    z.r=0;
  if (z.i==0)
    z.i=0;
  return atan2(z.i,z.r);
}

COMPLEX_T lb_cp_complex(FLOAT_T r, FLOAT_T i)
{
  COMPLEX_T temp;
  temp.r = r;
  temp.i = i;
  return temp;
}

FLOAT_T lb_cp_re(COMPLEX_T z)
{
  return z.r;
}

FLOAT_T lb_cp_im(COMPLEX_T z)
{
  return z.i;
}

COMPLEX_T lb_cp_polar_to_rect(FLOAT_T mag, FLOAT_T angle)
{
  COMPLEX_T temp;
  temp.r=mag*cos(angle);
  temp.i=mag*sin(angle);
  return temp;
}

COMPLEX_T lb_cp_rect_to_polar(FLOAT_T x, FLOAT_T y)
{
  COMPLEX_T z, temp;
  z.r=x;
  z.i=y;
  temp.r=lb_cp_abs(z);
  temp.i=lb_cp_arg(z);
  return temp;
}

/******************************************************************************************/
/* Other */
/******************************************************************************************/

S_INT8_T lb_cp_equal(COMPLEX_T z1, COMPLEX_T z2)
{
  if (lb_re_equal(z1.r,z2.r) && lb_re_equal(z1.i,z2.i))
    return 1.0;
  return 0.0;
}

void lb_cp_print_c(COMPLEX_T z, const char *format)
{
 char s[40];
 
 strcpy(s,"(");
 strcat(s,format);
 strcat(s,";");
 strcat(s,format);
 strcat(s,") ");
 printf(s,z.r,z.i);
}



