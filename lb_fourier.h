#ifndef LB_FOURIER_H
#define LB_FOURIER_H

void lb_fo_FFT_C_recursive(VECTOR_C_T *T,VECTOR_C_T *F, S_INT_8_T sign);
void lb_fo_calculate_W(VECTOR_C_T *W, S_INT_8_T sign);
void lb_fo_DFT_C(VECTOR_C_T *T, VECTOR_C_T *F, S_INT_8_T sign, ERR_T *error); 
void lb_fo_DFT_R(VECTOR_R_T *T, VECTOR_C_T *F); 
void lb_fo_FFT_C(VECTOR_C_T *T, VECTOR_C_T *F, VECTOR_C_T *W, S_INT_8_T sign); 
void lb_fo_FFT_R(VECTOR_R_T *T, VECTOR_C_T *F, VECTOR_C_T *W); 
void lb_fo_IFFT_R(VECTOR_C_T *T, VECTOR_C_T *F, VECTOR_C_T *W); 

#endif /* LB_FOURIER_H */
