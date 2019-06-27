#include "lb_real.h"

#ifndef LB_COMPLEX_H
#define LB_COMPLEX_H

/* Basic operations */
COMPLEX_T lb_cp_add(COMPLEX_T a, COMPLEX_T b);
COMPLEX_T lb_cp_substract(COMPLEX_T a, COMPLEX_T b);
COMPLEX_T lb_cp_neg(COMPLEX_T a);
COMPLEX_T lb_cp_multiply(COMPLEX_T a, COMPLEX_T b);
COMPLEX_T lb_cp_multiply_real(COMPLEX_T c, FLOAT_T r);
COMPLEX_T lb_cp_divide_real(COMPLEX_T c, FLOAT_T r, ERR_T *error);
COMPLEX_T lb_cp_divide(COMPLEX_T a, COMPLEX_T b, ERR_T *error);
COMPLEX_T lb_cp_conjugate(COMPLEX_T a);

/* Power */
COMPLEX_T lb_cp_sqr(COMPLEX_T z, ERR_T *error);
COMPLEX_T lb_cp_sqrt(COMPLEX_T z, ERR_T *error);
COMPLEX_T lb_cp_power(COMPLEX_T z1, COMPLEX_T z2, ERR_T *error);

/* Euler */
COMPLEX_T lb_cp_exp(COMPLEX_T z, ERR_T *error);
COMPLEX_T lb_cp_ln(COMPLEX_T z, ERR_T *error);
COMPLEX_T lb_cp_log(COMPLEX_T z, ERR_T *error);
COMPLEX_T lb_cp_log2(COMPLEX_T z, ERR_T *error);
COMPLEX_T lb_cp_logb(COMPLEX_T b, COMPLEX_T z, ERR_T *error);

/* Trigonometric - circular */
COMPLEX_T lb_cp_sin(COMPLEX_T z, ERR_T *error);
COMPLEX_T lb_cp_cos(COMPLEX_T z, ERR_T *error);
COMPLEX_T lb_cp_tan(COMPLEX_T z, ERR_T *error);
COMPLEX_T lb_cp_asin(COMPLEX_T z, ERR_T *error);
COMPLEX_T lb_cp_acos(COMPLEX_T z, ERR_T *error);
COMPLEX_T lb_cp_atan(COMPLEX_T z, ERR_T *error);

/* Trigonometric - hyperbolic */
COMPLEX_T lb_cp_sinh(COMPLEX_T z, ERR_T *error);
COMPLEX_T lb_cp_cosh(COMPLEX_T z, ERR_T *error);
COMPLEX_T lb_cp_tanh(COMPLEX_T z, ERR_T *error);
COMPLEX_T lb_cp_asinh(COMPLEX_T z, ERR_T *error);
COMPLEX_T lb_cp_acosh(COMPLEX_T z, ERR_T *error);
COMPLEX_T lb_cp_atanh(COMPLEX_T z, ERR_T *error);

/* Conversion or data assembly */
FLOAT_T lb_cp_abs(COMPLEX_T z);
FLOAT_T lb_cp_arg(COMPLEX_T z);
COMPLEX_T lb_cp_complex(FLOAT_T r, FLOAT_T i);
FLOAT_T lb_cp_re(COMPLEX_T z);
FLOAT_T lb_cp_im(COMPLEX_T z);
COMPLEX_T lb_cp_polar_to_rect(FLOAT_T mag, FLOAT_T angle);
COMPLEX_T lb_cp_rect_to_polar(FLOAT_T x, FLOAT_T y);

/* Other */

S_INT_8_T lb_cp_equal(COMPLEX_T z1, COMPLEX_T z2);
void lb_cp_print_c(COMPLEX_T z, const char *format);

#endif /* LB_COMPLEX_H */
