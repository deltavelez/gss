#include <stdio.h>
#include <math.h>
#include "lb_types.h"
#include "lb_real.h"


/* The function to be generated using PWM */

double f(double x)
{ 
  return sin(x);
}

/* Equation of a line given two points */
double f2(double x1, double y1, double x2, double y2, double x)
{
  return y1 + (y2-y1)*(x-x1)/(x2-x1);
}

double f_PWM(double x1, double y1, double x2, double y2, double x)
{
  return sin(x*2*M_PI*5464.0) - (y1 + (y2-y1)*(x-x1)/(x2-x1));
}

int lb_num_root_find_bisection(double a, double b, double tol, double *xo)
{
  double a_temp, b_temp, x_temp;

  if (fabs(f(a))<=tol)
    {
      *xo=a;
      return 1;
    }
  if (fabs(f(b))<=tol)
    {
      *xo=b;
      return 1;
    }
  if (lb_re_sign(f(a))*lb_re_sign(f(b))>0)
    {
      *xo=0;
      printf("Error: lb_num_root_find_bisection: No roots in interva\r\nl");
      return -1;
    }
  a_temp=a;
  b_temp=b;
  while (fabs(f(x_temp))>tol)
    {
      x_temp=(a_temp+b_temp)/2;
      if ((lb_re_sign(f(a_temp))*lb_re_sign(f(x_temp)))<0)
	b_temp=x_temp;
      else
	a_temp=x_temp;
      printf("x = %e, f(x)=%e \r\n",x_temp, f(x_temp));
    }
}				  

int lb_num_root_find_bisection_PWM(double x1, double y1, double x2, double y2, double tol, double *xo)
{
  double a_temp, b_temp, x_temp;
  
  if (fabs(f_PWM(x1,y1,x2,y2,x1))<=tol)
    {
      *xo=x1;
      //printf("x = %f\r\n",x_temp);
      return 1;
    }
  if (fabs(f_PWM(x1,y1,x2,y2,x2))<=tol)
    {
      *xo=x2;
      //printf("x = %f, f(x)=%f \r\n",x_temp, f_PWM(x1,y1,x2,y2,x2));
      return 1;
    }
  if (lb_re_sign(f_PWM(x1,y1,x2,y2,x1))*lb_re_sign(f_PWM(x1,y1,x2,y2,x2))>0)
    {
      *xo=0;
      //      printf("Error: lb_num_root_find_bisection: No roots in interval\rl");
      return 0;
    }
  a_temp=x1;
  b_temp=x2;
  x_temp=(a_temp+b_temp)/2;
  //  printf("x1=%f  y1=%f  x2=%f  y2=%f  f=%f \r\n",x1,y1,x2,y2,f_PWM(x1,y1,x2,y2,x1));
  while (fabs(f_PWM(x1,y1,x2,y2,x_temp))>tol)
    {
      x_temp=(a_temp+b_temp)/2;
      if ((lb_re_sign(f_PWM(x1,y1,x2,y2,a_temp))*lb_re_sign(f_PWM(x1,y1,x2,y2,x_temp)))<0)
	b_temp=x_temp;
      else
	a_temp=x_temp;
    }
  *xo=x_temp;
  //  printf("x = %f \r\n",rint(x_temp*5464.0*pow(2.0,12)));
  return 1;
}


void main()
{
  double T1 = 1/5464.0;
  double T2 = T1/32;
  double x;
  int x_n, x_n1;
  int i;
  int result;
  int first;
  int PWM_state=1;

  first=1;
  
  for (i=0;i<T1/T2;i++)
    {
      result = lb_num_root_find_bisection_PWM(i*T2        ,  0, i*T2+T2/4   ,  1, 1e-9, &x);
      if (result) 
	if (first)
	  {
	    first=0;
	    x_n1 = rint(x*5464.0*pow(2.0,12));
	  }
	else
	  {
	    x_n=rint(x*5464.0*pow(2.0,12));
	    printf(" %i %i %i\r\n",x_n,x_n-x_n1, PWM_state);
	    x_n1=x_n;
	    if (PWM_state) 
	      PWM_state=0;
	    else
	      PWM_state=1;
	  }
      result = lb_num_root_find_bisection_PWM(i*T2+T2/4   ,  1, i*T2+3*T2/4 , -1, 1e-9, &x);
      if (result) 
	if (first)
	  {
	    first=0;
	    x_n1 = rint(x*5464.0*pow(2.0,12));
	  }
	else
	  {
	    x_n=rint(x*5464.0*pow(2.0,12));
	    printf(" %i %i %i\r\n",x_n,x_n-x_n1, PWM_state);
	    x_n1=x_n;
	    if (PWM_state) 
	      PWM_state=0;
	    else
	      PWM_state=1;
	  }
      result = lb_num_root_find_bisection_PWM(i*T2+3*T2/4 , -1, (i+1)*T2    ,  0, 1e-9, &x);
      if (result) 
	if (first)
	  {
	    first=0;
	    x_n1 = rint(x*5464.0*pow(2.0,12));
	  }
	else
	  {
	    x_n=rint(x*5464.0*pow(2.0,12));
	    printf(" %i %i %i\r\n",x_n,x_n-x_n1, PWM_state);
	    x_n1=x_n;
	    if (PWM_state) 
	      PWM_state=0;
	    else
	      PWM_state=1;
	  }
      }
}
