#ifndef LB_STATISTICS
#define LB_STATISTICS

REAL_T lb_st_erf(REAL_T x);
REAL_T lb_st_erfc(REAL_T x);
REAL_T lb_st_frand(void);
REAL_T lb_st_marsaglia_polar(REAL_T variance);
void lb_st_marsaglia_polar2(REAL_T variance, REAL_T *r1, REAL_T *r2);
REAL_T lb_st_gauss_area(REAL_T x);
REAL_T lb_st_average(VECTOR_R_T *Data);
REAL_T lb_st_stddev2(VECTOR_R_T *Data, REAL_T mu);
void lb_st_histogram(VECTOR_R_T *Data, VECTOR_R_T *Bins, REAL_T a, REAL_T b, REAL_T *mu, REAL_T *sigma2 );


#endif /* LB_STATISTICS */
