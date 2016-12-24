#ifndef __CUDA_CUH__
#define __CUDA_CUH__

#ifdef __cplusplus
extern "C" {
#endif

void cuda_init();
void cuda_destroy();
void cuda_version_1(float C[MATRIX_SIZE][MATRIX_SIZE], float A[MATRIX_SIZE][MATRIX_SIZE], float B[MATRIX_SIZE][MATRIX_SIZE]);
void cuda_version_2(float C[MATRIX_SIZE][MATRIX_SIZE], float A[MATRIX_SIZE][MATRIX_SIZE], float B[MATRIX_SIZE][MATRIX_SIZE]);
void cuda_version_3(float C[MATRIX_SIZE][MATRIX_SIZE], float A[MATRIX_SIZE][MATRIX_SIZE], float B[MATRIX_SIZE][MATRIX_SIZE]);

#ifdef __cplusplus
}
#endif

#endif
