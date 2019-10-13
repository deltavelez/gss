#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "lb_real.h"
#include "lb_types.h"

/* Calculates the distance between a point P(x, y) to a Line passing through points P1(x1,y1) and P2(x2,y2) */ 
FLOAT_T lb_ge_distance_point_to_line_coordinates(FLOAT_T x1, FLOAT_T y1, FLOAT_T x2, FLOAT_T y2, FLOAT_T x, FLOAT_T y, ERR_T *error)
{
  if ((x1 != x2) || (y1 != y2))
    return fabs(x*(y2-y1)-y*(x2-x1)+x2*y1-y2*x1)/sqrt(lb_re_sqr(y2-y1,error)+lb_re_sqr(x2-x1,error));
  else
    {
      printf("Error: lb_ge_distance_point_to_line_coordinates() --> invalid line --> \r\n");
      return 0;
    }
} 

/* This is an approximation to the length of an ellipse x^2/a^2 + y^2/b^2 = 1  */
FLOAT_T lb_ge_angle_from_length_ellipse_q1(FLOAT_T a, FLOAT_T b, FLOAT_T length, FLOAT_T R)
{
  FLOAT_T yb;
  ERR_T error;
  if (lb_re_equal(a,b))
    if (lb_re_equal(a,0.0))
      return 0.0;
    else
      return length/a;
  else
    {
      if (length>R)
	{
	  printf("Error: lb_ge_angle_from_length_ellipse_q1() --> length > R\r\n");
	  exit(1);
	}
      yb=b*(R-M_PI_2*a)/(b-a);
      return M_PI_2*(-yb+lb_re_sqrt(yb*yb+(R-2*yb)*length,&error))/(R-2*yb);
    }
}


/* Returns 1/4 of the arc length of an ellipse using Ramanujan's second approximation */
FLOAT_T lb_ge_arclength_ellipse(FLOAT_T a, FLOAT_T b)
{
  FLOAT_T h;
  ERR_T error;
  if (lb_re_equal(a,b))
    return 0.5*a*M_PI;
  else
    {
      h=(a-b)/(a+b);
      h*=h;
      return 0.25*M_PI*(a+b)*(1.0+3*h/(10.0+lb_re_sqrt(4.0-3.0*h,&error)));
    }
} 

/* Returns an approximation to the arc length of an ellipse x^2/a^2 + y^2/b^2 = 1 covering an angle in the range [0.0 pi/2] */

FLOAT_T lb_ge_arclength_ellipse_angle_q1(FLOAT_T a, FLOAT_T b, FLOAT_T t, FLOAT_T R)
{
  FLOAT_T temp1, temp2; 
  //ERR_T error;

  if (lb_re_equal(a,b))
    return a*t;
  else
    {
      temp1=t*2.0/M_PI;
      temp2=temp1*temp1;
      return (2.0*a*R-M_PI*a*b)*(temp1-temp2)/(a-b) + R*temp2;
    }
} 

/* Returns an approximation to the arc length of an ellipse covering any angle including those larger than pi/2 */
FLOAT_T lb_ge_arclength_ellipse_angle(FLOAT_T a, FLOAT_T b, FLOAT_T t)
{
  FLOAT_T R, length;
  S_INT16_T n_q;

  R=lb_ge_arclength_ellipse(a,b);
    
  n_q=fabs(t)*M_2_PI;  /* angle/(pi/2) */
  if (n_q%2 == 0) /*even number of quadrants */
    length=lb_ge_arclength_ellipse_angle_q1(a, b, fabs(t)-n_q*M_PI_2, R);
  else
    length=R-lb_ge_arclength_ellipse_angle_q1(a, b, M_PI_2-(fabs(t)-n_q*M_PI_2), R);

  if (t>=0)
    return n_q*R+length;
  else
    return -n_q*R-length;
}
   		      
/* Returns the arc length of an ellipse between any two angles */
FLOAT_T lb_ge_arclength_ellipse_angles(FLOAT_T a, FLOAT_T b, FLOAT_T t1, FLOAT_T t2)
{
  return lb_ge_arclength_ellipse_angle(a,b,t2) - lb_ge_arclength_ellipse_angle(a,b,t1);
}

/* Given a Point Po(xo,yo), and a circle with r radius centered at Pc(xc,yc), 
   gives the two angles (relative to the circle) to the tangency points which pass through Po(xo,yo) */
void lb_ge_tangents_to_circle_point(FLOAT_T xc, FLOAT_T yc, FLOAT_T r, FLOAT_T xo, FLOAT_T yo, FLOAT_T *tetha1, FLOAT_T *tetha2)
{
  FLOAT_T temp;
  temp = (xo-xc)*(xo-xc) + (yo-yc)*(yo-yc) - r*r;
  if (temp>=0)
    temp=sqrt(temp);
  *tetha1=2*atan2( (yo-yc)+temp , (xo-xc)+r);
  *tetha2=2*atan2( (yo-yc)-temp , (xo-xc)+r);
}
