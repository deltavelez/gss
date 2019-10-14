#include "lb_types.h"
#include "lb_parser.h"

#ifndef LB_NUMER_H
#define LB_NUMER_H

REAL_T lb_nu_diff(FN_RECORD_T *function, INDEX_TYPE var, REAL_T *values, REAL_T xo, REAL_T delta, MATHERROR_T *error);

REAL_T lb_nu_diff2(FN_RECORD_T *function, INDEX_TYPE var, REAL_T *values, REAL_T xo, REAL_T delta, MATHERROR_T *error);

COMPLEX_T lb_nu_diff_c(FN_RECORD_T *function, INDEX_TYPE var, COMPLEX_T *values, COMPLEX_T xo, REAL_T delta, MATHERROR_T *error);

COMPLEX_T lb_nu_diff2_c(FN_RECORD_T *function, INDEX_TYPE var, COMPLEX_T *values, COMPLEX_T xo, REAL_T delta, MATHERROR_T *error);

SINT8_T lb_nu_find_zero(REAL_T x0, REAL_T y0, REAL_T x1, REAL_T y1, REAL_T *x_root);

REAL_T lb_nu_sigma(REAL_T a, REAL_T b, FN_RECORD_T *function, INDEX_TYPE var, REAL_T delta, REAL_T *values, MATHERROR_T *error);

COMPLEX_T lb_nu_sigma_c(REAL_T a, REAL_T b, FN_RECORD_T *function, INDEX_TYPE var, REAL_T delta, COMPLEX_T *values, MATHERROR_T *error);

REAL_T lb_nu_prod(REAL_T a, REAL_T b, FN_RECORD_T *function, INDEX_TYPE var, REAL_T delta, REAL_T *values, MATHERROR_T *error);

COMPLEX_T lb_nu_prod_c(REAL_T a, REAL_T b, FN_RECORD_T *function, INDEX_TYPE var, REAL_T delta, COMPLEX_T *values, MATHERROR_T *error);

void rk4(REAL_T *x_n, REAL_T *y_n, REAL_T h, MATHERROR_T *error, FUNCTION_X_Y f_xy);
void euler(REAL_T *x_n, REAL_T *y_n, REAL_T h, MATHERROR_T *error, FUNCTION_X_Y f_xy);

REAL_T lb_nu_simpson(REAL_T a, REAL_T b, FN_RECORD_T *function, INDEX_TYPE var, SINT16_T n, REAL_T *values, MATHERROR_T *error);

COMPLEX_T lb_nu_simpson_c(COMPLEX_T a, COMPLEX_T b, FN_RECORD_T *function, INDEX_TYPE var, SINT16_T n, COMPLEX_T *values, MATHERROR_T *error);

REAL_T lb_nu_simpson2d(REAL_T c, REAL_T d, REAL_T a, REAL_T b, FN_RECORD_T *function, INDEX_TYPE var1, INDEX_TYPE var2, SINT16_T m, SINT16_T n, REAL_T *values, MATHERROR_T *error);

COMPLEX_T lb_nu_simpson2d_c(COMPLEX_T c, COMPLEX_T d, COMPLEX_T a, COMPLEX_T b, FN_RECORD_T *function, INDEX_TYPE var1, INDEX_TYPE var2, SINT16_T m, SINT16_T n, COMPLEX_T *values, MATHERROR_T *error);

#endif /* LB_NUMER_H */


