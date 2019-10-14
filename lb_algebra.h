#include "lb_real.h"

#ifndef LB_ALGEBRA_H
#define LB_ALGEBRA_H

void      lb_al_add_vector_c(VECTOR_C_T *A, VECTOR_C_T *B, VECTOR_C_T *C);
void      lb_al_add_vector_r(VECTOR_R_T *A, VECTOR_R_T *B, VECTOR_R_T *C);
SINT8_T lb_al_assert_dimensions_array_r(ARRAY_R_T *M);

SINT8_T lb_al_assert_dimensions_matrix_p2d(MATRIX_POINT_2D_REAL_T *S);
SINT8_T lb_al_assert_dimensions_matrix_p3d(MATRIX_POINT_3D_REAL_T *S);
SINT8_T lb_al_assert_dimensions_matrix_r(MATRIX_R_T *M);
SINT8_T lb_al_assert_dimensions_matrix_si8(MATRIX_SINT8_T *M);
SINT8_T lb_al_assert_dimensions_vector_c(VECTOR_C_T *V);
SINT8_T lb_al_assert_dimensions_vector_r(VECTOR_R_T *V);
SINT8_T lb_al_assert_dimensions_vector_si8(VECTOR_SINT8_T *V);
SINT8_T lb_al_assert_dimensions_vector_si16(VECTOR_SINT16_T *V);
void      lb_al_copy_matrix_r_col_to_vector_r(MATRIX_R_T *M, VECTOR_R_T *V, UINT16_T j);
void      lb_al_copy_matrix_r_row_to_vector_r(MATRIX_R_T *M, VECTOR_R_T *V, UINT16_T i);
void      lb_al_copy_matrix_r(MATRIX_R_T *M1, MATRIX_R_T *M2);
void      lb_al_copy_matrix33_r(REAL_T M1[3][3], REAL_T M2[3][3]);
void      lb_al_copy_minor_matrix_r(MATRIX_R_T *M1, MATRIX_R_T *M2, UINT16_T io, UINT16_T jo);
void      lb_al_copy_vector_c(VECTOR_C_T *V1, VECTOR_C_T *V2);
void      lb_al_copy_vector_r(VECTOR_R_T *V1, VECTOR_R_T *V2);

void      lb_al_create_array_r(ARRAY_R_T *M);

void      lb_al_create_matrix_p2d(MATRIX_POINT_2D_REAL_T *S);
void      lb_al_create_matrix_p3d(MATRIX_POINT_3D_REAL_T *S);
void      lb_al_create_matrix_r(MATRIX_R_T *M);
void      lb_al_create_matrix_si8(MATRIX_SINT8_T *M);
void      lb_al_create_vector_c(VECTOR_C_T *V);
void      lb_al_create_vector_r(VECTOR_R_T *V);
void      lb_al_create_vector_si8(VECTOR_SINT8_T *V);
void      lb_al_create_vector_si16(VECTOR_SINT16_T *V);
void      lb_al_cross_product_vector_r(VECTOR_R_T *V1, VECTOR_R_T *V2, VECTOR_R_T *V3);
void      lb_al_cross_product_vector3_r(REAL_T V1[3], REAL_T V2[3], REAL_T V3[3]);
void      lb_al_delete_item_vector_r(VECTOR_R_T *V, UINT16_T pos);
void      lb_al_delete_n_items_vector_r(VECTOR_R_T *V, UINT16_T pos, UINT16_T n);
REAL_T   lb_al_determinant_matrix_r(MATRIX_R_T *M);
REAL_T   lb_al_determinant_matrix_r_gauss(MATRIX_R_T *M);
REAL_T   lb_al_determinant_matrix22_r(REAL_T M[2][2]);
REAL_T   lb_al_determinant_matrix33_r(REAL_T M[3][3]);
REAL_T   lb_al_dot_product_vector_r(VECTOR_R_T *V1, VECTOR_R_T *V2);
void      lb_al_fill_identity_matrix_r(MATRIX_R_T *M);
void      lb_al_fill_rotation_matrix_tait_bryan_ZYX(MATRIX_R_T *R, REAL_T yaw, REAL_T pitch, REAL_T roll);
void      lb_al_fill_rotation_matrix_vector(MATRIX_R_T *R, REAL_T x, REAL_T y, REAL_T z, REAL_T angle);
void      lb_al_fill_rotation_matrix_X(MATRIX_R_T *R, REAL_T angle_x);
void      lb_al_fill_rotation_matrix_Y(MATRIX_R_T *R, REAL_T angle_y);
void      lb_al_fill_rotation_matrix_Z(MATRIX_R_T *R, REAL_T angle_z);
void      lb_al_fill_rotation_matrix33_tait_bryan_ZYX(REAL_T R[3][3], REAL_T yaw, REAL_T pitch, REAL_T roll);
void      lb_al_fill_rotation_matrix33_X(REAL_T R[3][3], REAL_T angle_x);
void      lb_al_fill_rotation_matrix33_Y(REAL_T R[3][3], REAL_T angle_y);
void      lb_al_fill_rotation_matrix33_Z(REAL_T R[3][3], REAL_T angle_z);
void      lb_al_fill_zeros_matrix_r(MATRIX_R_T *M);
void      lb_al_load_CSV_file(MATRIX_R_T *M, const char filename);
void      lb_al_insert_item_vector_r(VECTOR_R_T *V, REAL_T x, UINT16_T pos);
void      lb_al_inter_matrix(MATRIX_R_T *M, REAL_T ir, REAL_T jr, REAL_T *Q);
void      lb_al_multiply_matrix_r(MATRIX_R_T *A, MATRIX_R_T *B, MATRIX_R_T *C);
void      lb_al_multiply_matrix_r_copy(MATRIX_R_T *A, MATRIX_R_T *B, MATRIX_R_T *C);
void      lb_al_multiply_matrix_r_vector_r(MATRIX_R_T *A, VECTOR_R_T *B, VECTOR_R_T *C);
void      lb_al_multiply_matrix33_r(REAL_T A[3][3], REAL_T B[3][3], REAL_T C[3][3]);
void      lb_al_multiply_matrix33_r_copy(REAL_T A[3][3], REAL_T B[3][3], REAL_T C[3][3]);
void      lb_al_multiply_matrix33_r_vector_r(REAL_T A[3][3], REAL_T B[3], REAL_T C[3]);
void      lb_al_multiply_vector_c_real(VECTOR_C_T *V1, REAL_T k);
void      lb_al_multiply_vector_c_real_copy(VECTOR_C_T *V1, REAL_T k, VECTOR_C_T *V2);
void      lb_al_multiply_vector_r_real(VECTOR_R_T *V1, REAL_T k);
void      lb_al_multiply_vector_r_real_copy(VECTOR_R_T *V1, REAL_T k, VECTOR_R_T *V2);
REAL_T   lb_al_norm_matrix_r(MATRIX_R_T *M, MATHERROR_T *error);
REAL_T   lb_al_norm_vector_r(VECTOR_R_T *V, MATHERROR_T *error);
void      lb_al_normalize_matrix_r(MATRIX_R_T *M, MATRIX_R_T *Mnorm, MATHERROR_T *error);
void      lb_al_normalize_vector_r(VECTOR_R_T *V, VECTOR_R_T *Vnorm, MATHERROR_T *error);

void      lb_al_print_array_r(ARRAY_R_T *M, const char *text, SINT8_T len, SINT8_T dec);
void      lb_al_print_matrix_r(MATRIX_R_T *M, char *text, const char *format);
void      lb_al_print_matrix33_r(REAL_T M[3][3], char *text, const char *format);
void      lb_al_print_vector_c(VECTOR_C_T *V, char *text, const char *format);
void      lb_al_print_vector_r(VECTOR_R_T *V, char *text, const char *format);
void      lb_al_print_vector_si8(VECTOR_SINT8_T *V, char *text);
void      lb_al_print_vector_si16(VECTOR_SINT16_T *V, char *text);

void      lb_al_release_array_r(ARRAY_R_T *M);

void      lb_al_release_matrix_p2d(MATRIX_POINT_2D_REAL_T *S);
void      lb_al_release_matrix_p3d(MATRIX_POINT_3D_REAL_T *S);
void      lb_al_release_matrix_r(MATRIX_R_T *M);
void      lb_al_release_matrix_si8(MATRIX_SINT8_T *M);
void      lb_al_release_vector_c(VECTOR_C_T *V);
void      lb_al_release_vector_r(VECTOR_R_T *V);
void      lb_al_release_vector_si8(VECTOR_SINT8_T *V);
void      lb_al_release_vector_si16(VECTOR_SINT16_T *V);
void      lb_al_resize_vector_r(VECTOR_R_T *V, UINT16_T new_size);
void      lb_al_resize_vector_si16(VECTOR_SINT16_T *V, UINT16_T new_size);
void      lb_al_reverse_order_vector_r(VECTOR_R_T *V1, VECTOR_R_T *V2);
void      lb_al_solve_linear_system_r(MATRIX_R_T *M, VECTOR_R_T *X, VECTOR_R_T *S);
void      lb_al_sort_bubble_vector_r(VECTOR_R_T *V1, VECTOR_R_T *V2);
void      lb_al_sort_bubble_vector_r_unreg(REAL_T *V, UINT16_T n);
void      _lb_al_sort_quicksort_vector_r(VECTOR_R_T *V, UINT16_T a, UINT16_T b);
void      lb_al_sort_quicksort_vector_r(VECTOR_R_T *V1, VECTOR_R_T *V2);
void      lb_al_substract_vector_c(VECTOR_C_T *A, VECTOR_C_T *B, VECTOR_C_T *C);
void      lb_al_substract_vector_r(VECTOR_R_T *A, VECTOR_R_T *B, VECTOR_R_T *C);
REAL_T   lb_al_sum_abs_matrix_r(MATRIX_R_T *M);
REAL_T   lb_al_sum_abs_vector_c(VECTOR_C_T *V, MATHERROR_T *error);
REAL_T   lb_al_sum_abs_vector_r(VECTOR_R_T *V);
//COMPLEX_T lb_al_sum_matrix_c(MATRIX_C_T *M);
REAL_T   lb_al_sum_matrix_r(MATRIX_R_T *M);
COMPLEX_T lb_al_sum_vector_c(VECTOR_C_T *V);
REAL_T   lb_al_sum_vector_r(VECTOR_R_T *V);
void      lb_al_swap_vector_r(VECTOR_R_T *A, VECTOR_R_T *B);
void      lb_al_transpose_matrix_r(MATRIX_R_T *M1, MATRIX_R_T *M2);


#endif /* LB_ALGEBRA_H */
