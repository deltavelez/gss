#ifndef LB_STATISTICS
#define LB_STATISTICS

FLOAT_T lb_st_erf(FLOAT_T x);
FLOAT_T lb_st_erfc(FLOAT_T x);
FLOAT_T lb_st_frand(void);
FLOAT_T lb_st_marsaglia_polar(FLOAT_T variance);
void lb_st_marsaglia_polar2(FLOAT_T variance, FLOAT_T *r1, FLOAT_T *r2);
FLOAT_T lb_st_gauss_area(FLOAT_T x);
FLOAT_T lb_st_average(VECTOR_R_T *Data);
FLOAT_T lb_st_stddev2(VECTOR_R_T *Data, FLOAT_T mu);
void lb_st_histogram(VECTOR_R_T *Data, VECTOR_R_T *Bins, FLOAT_T a, FLOAT_T b, FLOAT_T *mu, FLOAT_T *sigma2 );


#endif /* LB_STATISTICS */
