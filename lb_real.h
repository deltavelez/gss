#include "lb_types.h"

#ifndef LB_REAL_H
#define LB_REAL_H

/* Basic operations */

FLOAT_T lb_re_divide(FLOAT_T x, FLOAT_T y, ERR_T *error);
FLOAT_T lb_re_pow(FLOAT_T x, FLOAT_T y,  ERR_T *error);
FLOAT_T lb_re_sqr(FLOAT_T x, ERR_T *error);
FLOAT_T lb_re_sqrt(FLOAT_T x, ERR_T *error);
FLOAT_T lb_re_xroot(FLOAT_T x, FLOAT_T y, ERR_T *error);

/* Euler */

FLOAT_T lb_re_exp(FLOAT_T x,ERR_T *error);
FLOAT_T lb_re_ln(FLOAT_T x, ERR_T *error);
FLOAT_T lb_re_log(FLOAT_T x, ERR_T *error);
FLOAT_T lb_re_log2(FLOAT_T x, ERR_T *error);

/* Trigonometric - circular */

FLOAT_T lb_re_tan(FLOAT_T x, ERR_T *error);
FLOAT_T lb_re_asin(FLOAT_T x,ERR_T *error);
FLOAT_T lb_re_acos(FLOAT_T x,ERR_T *error);
FLOAT_T lb_re_atan2(FLOAT_T y, FLOAT_T x);

/* Trigonometric - hyperbolic*/

FLOAT_T lb_re_sinh(FLOAT_T x, ERR_T *error);
FLOAT_T lb_re_cosh(FLOAT_T x, ERR_T *error);
FLOAT_T lb_re_acosh(FLOAT_T x,ERR_T *error);
FLOAT_T lb_re_atanh(FLOAT_T x,ERR_T *error);

/* Comparison */

S_INT_8_T lb_re_equal(FLOAT_T a, FLOAT_T b);
S_INT_8_T lb_re_is_int(FLOAT_T x);
S_INT_8_T lb_re_larger_or_equal(FLOAT_T a, FLOAT_T b);
S_INT_8_T lb_re_smaller_or_equal(FLOAT_T a, FLOAT_T b);
FLOAT_T lb_re_max(FLOAT_T x, FLOAT_T y);
FLOAT_T lb_re_min(FLOAT_T x, FLOAT_T y);

/* Singular functions */
FLOAT_T lb_re_frac(FLOAT_T x);
FLOAT_T lb_re_ramp(FLOAT_T x);

S_INT_8_T lb_re_sign(FLOAT_T x);
FLOAT_T lb_re_pulse(FLOAT_T a, FLOAT_T b, FLOAT_T x);
FLOAT_T lb_re_pulse_triangle(FLOAT_T a, FLOAT_T b, FLOAT_T x);
FLOAT_T lb_re_step(FLOAT_T x);
FLOAT_T lb_re_step_practical(FLOAT_T t, FLOAT_T x);

/* Other functions */
S_INT_16_T lb_re_ndigits(FLOAT_T x);
FLOAT_T lb_re_factorial(FLOAT_T n,ERR_T *error);
FLOAT_T lb_re_normed_exponent(FLOAT_T x);
FLOAT_T lb_re_normed_significand(FLOAT_T x);
FLOAT_T lb_re_test_interval_closed(FLOAT_T a, FLOAT_T b, FLOAT_T x);
FLOAT_T lb_re_test_interval_open(FLOAT_T a, FLOAT_T b, FLOAT_T x);



/* Financial */

FLOAT_T lb_re_APR_to_monthly(FLOAT_T apr, ERR_T *error);
FLOAT_T lb_re_monthly_to_APR(FLOAT_T monthly, ERR_T *error);
FLOAT_T lb_re_monthly_to_effective(FLOAT_T monthly, ERR_T *error);

/* Interpolation */
/*
    y|  (x0,y1)=Q01    (x1,y1)=Q11
     |      
     |  (x0,y0)=Q00    (x1,y0)=Q10
     |_____________________________x 
*/

void lb_re_inter_linear(FLOAT_T x0, FLOAT_T Q0, FLOAT_T x1, FLOAT_T Q1,
			FLOAT_T x, FLOAT_T *Q, ERR_T *error);

void lb_re_inter_bilinear(FLOAT_T x0, FLOAT_T y0, FLOAT_T x1, FLOAT_T y1,
			  FLOAT_T Q00, FLOAT_T Q01, FLOAT_T Q10, FLOAT_T Q11,
			  FLOAT_T x, FLOAT_T y, FLOAT_T *Q, ERR_T *error);

/* Rotation */
void lb_re_rotate(FLOAT_T x, FLOAT_T y, FLOAT_T angle, FLOAT_T *xr, FLOAT_T *yr);

#endif /* LB_REAL_H */
