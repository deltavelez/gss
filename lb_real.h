#include "lb_types.h"

#ifndef LB_REAL_H
#define LB_REAL_H

/* Basic operations */

REAL_T lb_re_divide(REAL_T x, REAL_T y, MATHERROR_T *error);
REAL_T lb_re_pow(REAL_T x, REAL_T y,  MATHERROR_T *error);
REAL_T lb_re_sqr(REAL_T x, MATHERROR_T *error);
REAL_T lb_re_sqrt(REAL_T x, MATHERROR_T *error);
REAL_T lb_re_xroot(REAL_T x, REAL_T y, MATHERROR_T *error);

/* Euler */

REAL_T lb_re_exp(REAL_T x,MATHERROR_T *error);
REAL_T lb_re_ln(REAL_T x, MATHERROR_T *error);
REAL_T lb_re_log(REAL_T x, MATHERROR_T *error);
REAL_T lb_re_log2(REAL_T x, MATHERROR_T *error);

/* Trigonometric - circular */

REAL_T lb_re_tan(REAL_T x, MATHERROR_T *error);
REAL_T lb_re_asin(REAL_T x,MATHERROR_T *error);
REAL_T lb_re_acos(REAL_T x,MATHERROR_T *error);
REAL_T lb_re_atan2(REAL_T y, REAL_T x);

/* Trigonometric - hyperbolic*/

REAL_T lb_re_sinh(REAL_T x, MATHERROR_T *error);
REAL_T lb_re_cosh(REAL_T x, MATHERROR_T *error);
REAL_T lb_re_acosh(REAL_T x,MATHERROR_T *error);
REAL_T lb_re_atanh(REAL_T x,MATHERROR_T *error);

/* Comparison */

SINT8_T lb_re_equal(REAL_T a, REAL_T b);
SINT8_T lb_re_is_int(REAL_T x);
SINT8_T lb_re_larger_or_equal(REAL_T a, REAL_T b);
SINT8_T lb_re_smaller_or_equal(REAL_T a, REAL_T b);
REAL_T lb_re_max(REAL_T x, REAL_T y);
REAL_T lb_re_min(REAL_T x, REAL_T y);

/* Singular functions */
REAL_T lb_re_frac(REAL_T x);
SINT8_T lb_re_ispos(REAL_T x);
REAL_T lb_re_ramp(REAL_T x);

SINT8_T lb_re_sign(REAL_T x);
REAL_T lb_re_pulse(REAL_T a, REAL_T b, REAL_T x);
REAL_T lb_re_pulse_triangle(REAL_T a, REAL_T b, REAL_T x);
REAL_T lb_re_step(REAL_T x);
REAL_T lb_re_step_practical(REAL_T t, REAL_T x);

/* Other functions */
SINT16_T lb_re_ndigits(REAL_T x);
REAL_T lb_re_factorial(REAL_T n,MATHERROR_T *error);
REAL_T lb_re_normed_exponent(REAL_T x);
REAL_T lb_re_normed_significand(REAL_T x);
REAL_T lb_re_test_interval_closed(REAL_T a, REAL_T b, REAL_T x);
REAL_T lb_re_test_interval_open(REAL_T a, REAL_T b, REAL_T x);



/* Financial */

REAL_T lb_re_APR_to_monthly(REAL_T apr, MATHERROR_T *error);
REAL_T lb_re_monthly_to_APR(REAL_T monthly, MATHERROR_T *error);
REAL_T lb_re_monthly_to_effective(REAL_T monthly, MATHERROR_T *error);

/* Interpolation */
/*
    y|  (x0,y1)=Q01    (x1,y1)=Q11
     |      
     |  (x0,y0)=Q00    (x1,y0)=Q10
     |_____________________________x 
*/

void lb_re_inter_linear(REAL_T x0, REAL_T Q0, REAL_T x1, REAL_T Q1,
			REAL_T x, REAL_T *Q, MATHERROR_T *error);

void lb_re_inter_bilinear(REAL_T x0, REAL_T y0, REAL_T x1, REAL_T y1,
			  REAL_T Q00, REAL_T Q01, REAL_T Q10, REAL_T Q11,
			  REAL_T x, REAL_T y, REAL_T *Q, MATHERROR_T *error);

/* Rotation */
void lb_re_rotate(REAL_T x, REAL_T y, REAL_T angle, REAL_T *xr, REAL_T *yr);

#endif /* LB_REAL_H */
