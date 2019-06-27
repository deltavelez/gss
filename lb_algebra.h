#include "lb_real.h"

#ifndef LB_ALGEBRA_H
#define LB_ALGEBRA_H

void      lb_al_add_vector_c(VECTOR_C_T *A, VECTOR_C_T *B, VECTOR_C_T *C);
void      lb_al_add_vector_r(VECTOR_R_T *A, VECTOR_R_T *B, VECTOR_R_T *C);
S_INT_8_T lb_al_assert_dimensions_array_r(ARRAY_R_T *M);

S_INT_8_T lb_al_assert_dimensions_matrix_p2d(MATRIX_POINT_2D_T *S);
S_INT_8_T lb_al_assert_dimensions_matrix_p3d(MATRIX_POINT_3D_T *S);
S_INT_8_T lb_al_assert_dimensions_matrix_r(MATRIX_R_T *M);
S_INT_8_T lb_al_assert_dimensions_matrix_si8(MATRIX_S_INT_8_T *M);
S_INT_8_T lb_al_assert_dimensions_vector_c(VECTOR_C_T *V);
S_INT_8_T lb_al_assert_dimensions_vector_r(VECTOR_R_T *V);
S_INT_8_T lb_al_assert_dimensions_vector_si8(VECTOR_S_INT_8_T *V);
S_INT_8_T lb_al_assert_dimensions_vector_si16(VECTOR_S_INT_16_T *V);
void      lb_al_copy_matrix_r_col_to_vector_r(MATRIX_R_T *M, VECTOR_R_T *V, U_INT_16_T j);
void      lb_al_copy_matrix_r_row_to_vector_r(MATRIX_R_T *M, VECTOR_R_T *V, U_INT_16_T i);
void      lb_al_copy_matrix_r(MATRIX_R_T *M1, MATRIX_R_T *M2);
void      lb_al_copy_matrix33_r(FLOAT_T M1[3][3], FLOAT_T M2[3][3]);
void      lb_al_copy_minor_matrix_r(MATRIX_R_T *M1, MATRIX_R_T *M2, U_INT_16_T io, U_INT_16_T jo);
void      lb_al_copy_vector_c(VECTOR_C_T *V1, VECTOR_C_T *V2);
void      lb_al_copy_vector_r(VECTOR_R_T *V1, VECTOR_R_T *V2);

void      lb_al_create_array_r(ARRAY_R_T *M);

void      lb_al_create_matrix_p2d(MATRIX_POINT_2D_T *S);
void      lb_al_create_matrix_p3d(MATRIX_POINT_3D_T *S);
void      lb_al_create_matrix_r(MATRIX_R_T *M);
void      lb_al_create_matrix_si8(MATRIX_S_INT_8_T *M);
void      lb_al_create_vector_c(VECTOR_C_T *V);
void      lb_al_create_vector_r(VECTOR_R_T *V);
void      lb_al_create_vector_si8(VECTOR_S_INT_8_T *V);
void      lb_al_create_vector_si16(VECTOR_S_INT_16_T *V);
void      lb_al_cross_product_vector_r(VECTOR_R_T *V1, VECTOR_R_T *V2, VECTOR_R_T *V3);
void      lb_al_cross_product_vector3_r(FLOAT_T V1[3], FLOAT_T V2[3], FLOAT_T V3[3]);
void      lb_al_delete_item_vector_r(VECTOR_R_T *V, U_INT_16_T pos);
void      lb_al_delete_n_items_vector_r(VECTOR_R_T *V, U_INT_16_T pos, U_INT_16_T n);
FLOAT_T   lb_al_determinant_matrix_r(MATRIX_R_T *M);
FLOAT_T   lb_al_determinant_matrix_r_gauss(MATRIX_R_T *M);
FLOAT_T   lb_al_determinant_matrix22_r(FLOAT_T M[2][2]);
FLOAT_T   lb_al_determinant_matrix33_r(FLOAT_T M[3][3]);
FLOAT_T   lb_al_dot_product_vector_r(VECTOR_R_T *V1, VECTOR_R_T *V2);
void      lb_al_fill_identity_matrix_r(MATRIX_R_T *M);
void      lb_al_fill_rotation_matrix_tait_bryan_ZYX(MATRIX_R_T *R, FLOAT_T yaw, FLOAT_T pitch, FLOAT_T roll);
void      lb_al_fill_rotation_matrix_vector(MATRIX_R_T *R, FLOAT_T x, FLOAT_T y, FLOAT_T z, FLOAT_T angle);
void      lb_al_fill_rotation_matrix_X(MATRIX_R_T *R, FLOAT_T angle_x);
void      lb_al_fill_rotation_matrix_Y(MATRIX_R_T *R, FLOAT_T angle_y);
void      lb_al_fill_rotation_matrix_Z(MATRIX_R_T *R, FLOAT_T angle_z);
void      lb_al_fill_rotation_matrix33_tait_bryan_ZYX(FLOAT_T R[3][3], FLOAT_T yaw, FLOAT_T pitch, FLOAT_T roll);
void      lb_al_fill_rotation_matrix33_X(FLOAT_T R[3][3], FLOAT_T angle_x);
void      lb_al_fill_rotation_matrix33_Y(FLOAT_T R[3][3], FLOAT_T angle_y);
void      lb_al_fill_rotation_matrix33_Z(FLOAT_T R[3][3], FLOAT_T angle_z);
void      lb_al_fill_zeros_matrix_r(MATRIX_R_T *M);
void      lb_al_load_CSV_file(MATRIX_R_T *M, const char filename);
void      lb_al_insert_item_vector_r(VECTOR_R_T *V, FLOAT_T x, U_INT_16_T pos);
void      lb_al_inter_matrix(MATRIX_R_T *M, FLOAT_T ir, FLOAT_T jr, FLOAT_T *Q);
void      lb_al_multiply_matrix_r(MATRIX_R_T *A, MATRIX_R_T *B, MATRIX_R_T *C);
void      lb_al_multiply_matrix_r_copy(MATRIX_R_T *A, MATRIX_R_T *B, MATRIX_R_T *C);
void      lb_al_multiply_matrix_r_vector_r(MATRIX_R_T *A, VECTOR_R_T *B, VECTOR_R_T *C);
void      lb_al_multiply_matrix33_r(FLOAT_T A[3][3], FLOAT_T B[3][3], FLOAT_T C[3][3]);
void      lb_al_multiply_matrix33_r_copy(FLOAT_T A[3][3], FLOAT_T B[3][3], FLOAT_T C[3][3]);
void      lb_al_multiply_matrix33_r_vector_r(FLOAT_T A[3][3], FLOAT_T B[3], FLOAT_T C[3]);
void      lb_al_multiply_vector_c_real(VECTOR_C_T *V1, FLOAT_T k);
void      lb_al_multiply_vector_c_real_copy(VECTOR_C_T *V1, FLOAT_T k, VECTOR_C_T *V2);
void      lb_al_multiply_vector_r_real(VECTOR_R_T *V1, FLOAT_T k);
void      lb_al_multiply_vector_r_real_copy(VECTOR_R_T *V1, FLOAT_T k, VECTOR_R_T *V2);
FLOAT_T   lb_al_norm_matrix_r(MATRIX_R_T *M, ERR_T *error);
FLOAT_T   lb_al_norm_vector_r(VECTOR_R_T *V, ERR_T *error);
void      lb_al_normalize_matrix_r(MATRIX_R_T *M, MATRIX_R_T *Mnorm, ERR_T *error);
void      lb_al_normalize_vector_r(VECTOR_R_T *V, VECTOR_R_T *Vnorm, ERR_T *error);

void      lb_al_print_array_r(ARRAY_R_T *M, const char *text, S_INT_8_T len, S_INT_8_T dec);
void      lb_al_print_matrix_r(MATRIX_R_T *M, char *text, const char *format);
void      lb_al_print_matrix33_r(FLOAT_T M[3][3], char *text, const char *format);
void      lb_al_print_vector_c(VECTOR_C_T *V, char *text, const char *format);
void      lb_al_print_vector_r(VECTOR_R_T *V, char *text, const char *format);
void      lb_al_print_vector_si8(VECTOR_S_INT_8_T *V, char *text);
void      lb_al_print_vector_si16(VECTOR_S_INT_16_T *V, char *text);

void      lb_al_release_array_r(ARRAY_R_T *M);

void      lb_al_release_matrix_p2d(MATRIX_POINT_2D_T *S);
void      lb_al_release_matrix_p3d(MATRIX_POINT_3D_T *S);
void      lb_al_release_matrix_r(MATRIX_R_T *M);
void      lb_al_release_matrix_si8(MATRIX_S_INT_8_T *M);
void      lb_al_release_vector_c(VECTOR_C_T *V);
void      lb_al_release_vector_r(VECTOR_R_T *V);
void      lb_al_release_vector_si8(VECTOR_S_INT_8_T *V);
void      lb_al_release_vector_si16(VECTOR_S_INT_16_T *V);
void      lb_al_resize_vector_r(VECTOR_R_T *V, U_INT_16_T new_size);
void      lb_al_resize_vector_si16(VECTOR_S_INT_16_T *V, U_INT_16_T new_size);
void      lb_al_reverse_order_vector_r(VECTOR_R_T *V1, VECTOR_R_T *V2);
void      lb_al_solve_linear_system_r(MATRIX_R_T *M, VECTOR_R_T *X, VECTOR_R_T *S);
void      lb_al_sort_bubble_vector_r(VECTOR_R_T *V1, VECTOR_R_T *V2);
void      lb_al_sort_bubble_vector_r_unreg(FLOAT_T *V, U_INT_16_T n);
void      _lb_al_sort_quicksort_vector_r(VECTOR_R_T *V, U_INT_16_T a, U_INT_16_T b);
void      lb_al_sort_quicksort_vector_r(VECTOR_R_T *V1, VECTOR_R_T *V2);
void      lb_al_substract_vector_c(VECTOR_C_T *A, VECTOR_C_T *B, VECTOR_C_T *C);
void      lb_al_substract_vector_r(VECTOR_R_T *A, VECTOR_R_T *B, VECTOR_R_T *C);
FLOAT_T   lb_al_sum_abs_matrix_r(MATRIX_R_T *M);
FLOAT_T   lb_al_sum_abs_vector_c(VECTOR_C_T *V, ERR_T *error);
FLOAT_T   lb_al_sum_abs_vector_r(VECTOR_R_T *V);
//COMPLEX_T lb_al_sum_matrix_c(MATRIX_C_T *M);
FLOAT_T   lb_al_sum_matrix_r(MATRIX_R_T *M);
COMPLEX_T lb_al_sum_vector_c(VECTOR_C_T *V);
FLOAT_T   lb_al_sum_vector_r(VECTOR_R_T *V);
void      lb_al_swap_vector_r(VECTOR_R_T *A, VECTOR_R_T *B);
void      lb_al_transpose_matrix_r(MATRIX_R_T *M1, MATRIX_R_T *M2);


#endif /* LB_ALGEBRA_H */
