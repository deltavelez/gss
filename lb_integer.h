#ifndef LB_INTEGER_H
#define LB_INTEGER_H

void lb_sieve_erathostenes(VECTOR_S_INT_16_T *P, S_INT_16_T n);
void lb_sieve_erathostenes_2(VECTOR_S_INT_16_T *P, S_INT_16_T n);
S_INT_8_T lb_in_pos_in_sorted_vector(VECTOR_S_INT_16_T *P, S_INT_16_T n);
U_INT_16_T lb_in_invert_bits(U_INT_16_T x, U_INT_16_T n_bits);
S_INT_8_T lb_in_is_even(S_INT_16_T x);
//S_INT_8_T  lb_in_is_integer(FLOAT_T n);
//S_INT_8_T  lb_in_is_fibo(S_INT_32_T n);
S_INT_8_T lb_in_is_odd(S_INT_16_T x);
S_INT_8_T  lb_in_is_perfectsquare(S_INT_32_T n);
S_INT_8_T lb_in_is_power_of_two(U_INT_32_T x);
S_INT_8_T  lb_in_is_prime(S_INT_32_T n);
S_INT_32_T  lb_in_isqrt(S_INT_32_T n);
S_INT_16_T lb_in_littleS16_to_bigS16(S_INT_16_T x);
S_INT_16_T lb_in_max(S_INT_16_T x, S_INT_16_T y);
S_INT_16_T lb_in_max_n(S_INT_16_T n,...);

S_INT_16_T lb_in_min(S_INT_16_T x, S_INT_16_T y);
S_INT_16_T lb_in_pow(S_INT_16_T k, S_INT_16_T n);
S_INT_16_T lb_in_round_div_up(S_INT_32_T a, S_INT_32_T b);
 
S_INT_16_T lb_in_simpson2d_coef(S_INT_16_T i, S_INT_16_T j, S_INT_16_T m, S_INT_16_T n);

#endif /* LB_INTEGER_H */
