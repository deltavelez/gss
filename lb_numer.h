#include "lb_types.h"
#include "lb_parser.h"

#ifndef LB_NUMER_H
#define LB_NUMER_H

FLOAT_T lb_nu_diff(FN_RECORD_T *function, INDEX_TYPE var, FLOAT_T *values, FLOAT_T xo, FLOAT_T delta, ERR_T *error);

FLOAT_T lb_nu_diff2(FN_RECORD_T *function, INDEX_TYPE var, FLOAT_T *values, FLOAT_T xo, FLOAT_T delta, ERR_T *error);

COMPLEX_T lb_nu_diff_c(FN_RECORD_T *function, INDEX_TYPE var, COMPLEX_T *values, COMPLEX_T xo, FLOAT_T delta, ERR_T *error);

COMPLEX_T lb_nu_diff2_c(FN_RECORD_T *function, INDEX_TYPE var, COMPLEX_T *values, COMPLEX_T xo, FLOAT_T delta, ERR_T *error);

S_INT_8_T lb_nu_find_zero(FLOAT_T x0, FLOAT_T y0, FLOAT_T x1, FLOAT_T y1, FLOAT_T *x_root);

FLOAT_T lb_nu_sigma(FLOAT_T a, FLOAT_T b, FN_RECORD_T *function, INDEX_TYPE var, FLOAT_T delta, FLOAT_T *values, ERR_T *error);

COMPLEX_T lb_nu_sigma_c(FLOAT_T a, FLOAT_T b, FN_RECORD_T *function, INDEX_TYPE var, FLOAT_T delta, COMPLEX_T *values, ERR_T *error);

FLOAT_T lb_nu_prod(FLOAT_T a, FLOAT_T b, FN_RECORD_T *function, INDEX_TYPE var, FLOAT_T delta, FLOAT_T *values, ERR_T *error);

COMPLEX_T lb_nu_prod_c(FLOAT_T a, FLOAT_T b, FN_RECORD_T *function, INDEX_TYPE var, FLOAT_T delta, COMPLEX_T *values, ERR_T *error);

void rk4(FLOAT_T *x_n, FLOAT_T *y_n, FLOAT_T h, ERR_T *error, FUNCTION_X_Y f_xy);
void euler(FLOAT_T *x_n, FLOAT_T *y_n, FLOAT_T h, ERR_T *error, FUNCTION_X_Y f_xy);

FLOAT_T lb_nu_simpson(FLOAT_T a, FLOAT_T b, FN_RECORD_T *function, INDEX_TYPE var, S_INT_16_T n, FLOAT_T *values, ERR_T *error);

COMPLEX_T lb_nu_simpson_c(COMPLEX_T a, COMPLEX_T b, FN_RECORD_T *function, INDEX_TYPE var, S_INT_16_T n, COMPLEX_T *values, ERR_T *error);

FLOAT_T lb_nu_simpson2d(FLOAT_T c, FLOAT_T d, FLOAT_T a, FLOAT_T b, FN_RECORD_T *function, INDEX_TYPE var1, INDEX_TYPE var2, S_INT_16_T m, S_INT_16_T n, FLOAT_T *values, ERR_T *error);

COMPLEX_T lb_nu_simpson2d_c(COMPLEX_T c, COMPLEX_T d, COMPLEX_T a, COMPLEX_T b, FN_RECORD_T *function, INDEX_TYPE var1, INDEX_TYPE var2, S_INT_16_T m, S_INT_16_T n, COMPLEX_T *values, ERR_T *error);

#endif /* LB_NUMER_H */


