#ifndef LB_INTEGER_H
#define LB_INTEGER_H

void lb_sieve_erathostenes(VECTOR_S_INT16_T *P, S_INT16_T n);
void lb_sieve_erathostenes_2(VECTOR_S_INT16_T *P, S_INT16_T n);
S_INT8_T lb_in_pos_in_sorted_vector(VECTOR_S_INT16_T *P, S_INT16_T n);
U_INT16_T lb_in_invert_bits(U_INT16_T x, U_INT16_T n_bits);
S_INT8_T lb_in_is_even(S_INT16_T x);
//S_INT8_T  lb_in_is_integer(FLOAT_T n);
//S_INT8_T  lb_in_is_fibo(S_INT32_T n);
S_INT8_T lb_in_is_odd(S_INT16_T x);
S_INT8_T  lb_in_is_perfectsquare(S_INT32_T n);
S_INT8_T lb_in_is_power_of_two(U_INT32_T x);
S_INT8_T  lb_in_is_prime(S_INT32_T n);
S_INT32_T  lb_in_isqrt(S_INT32_T n);
S_INT16_T lb_in_littleS16_to_bigS16(S_INT16_T x);
S_INT16_T lb_in_max(S_INT16_T x, S_INT16_T y);
S_INT16_T lb_in_max_n(S_INT16_T n,...);

S_INT16_T lb_in_min(S_INT16_T x, S_INT16_T y);
S_INT16_T lb_in_pow(S_INT16_T k, S_INT16_T n);
S_INT16_T lb_in_round_div_up(S_INT32_T a, S_INT32_T b);
 
S_INT16_T lb_in_simpson2d_coef(S_INT16_T i, S_INT16_T j, S_INT16_T m, S_INT16_T n);

#endif /* LB_INTEGER_H */
