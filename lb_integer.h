#ifndef LB_INTEGER_H
#define LB_INTEGER_H

void lb_sieve_erathostenes(VECTOR_SINT16_T *P, SINT16_T n);
void lb_sieve_erathostenes_2(VECTOR_SINT16_T *P, SINT16_T n);
SINT8_T lb_in_pos_in_sorted_vector(VECTOR_SINT16_T *P, SINT16_T n);
UINT16_T lb_in_invert_bits(UINT16_T x, UINT16_T n_bits);
SINT8_T lb_in_is_even(SINT16_T x);
//SINT8_T  lb_in_is_integer(REAL_T n);
//SINT8_T  lb_in_is_fibo(SINT32_T n);
SINT8_T lb_in_is_odd(SINT16_T x);
SINT8_T  lb_in_is_perfectsquare(SINT32_T n);
SINT8_T lb_in_is_power_of_two(UINT32_T x);
SINT8_T  lb_in_is_prime(SINT32_T n);
SINT32_T  lb_in_isqrt(SINT32_T n);
SINT16_T lb_in_littleS16_to_bigS16(SINT16_T x);
SINT16_T lb_in_max(SINT16_T x, SINT16_T y);
SINT16_T lb_in_max_n(SINT16_T n,...);

SINT16_T lb_in_min(SINT16_T x, SINT16_T y);
SINT16_T lb_in_pow(SINT16_T k, SINT16_T n);
SINT16_T lb_in_round_div_up(SINT32_T a, SINT32_T b);
 
SINT16_T lb_in_simpson2d_coef(SINT16_T i, SINT16_T j, SINT16_T m, SINT16_T n);

#endif /* LB_INTEGER_H */
