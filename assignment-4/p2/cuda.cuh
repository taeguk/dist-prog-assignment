#ifndef __CUDA_CUH__
#define __CUDA_CUH__

#ifdef __cplusplus
extern "C" {
#endif

void cuda_init();
void cuda_destroy();
void cuda_version_1(int *res, int arr[N]);
void cuda_version_2(int *res, int arr[N]);
void cuda_version_3(int *res, int arr[N]);
void cuda_version_4(int *res, int arr[N]);

#ifdef __cplusplus
}
#endif

#endif
