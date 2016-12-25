#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

#include "cuda.cuh"
#include "timer.h"

// #define N /* Must be defined in compile progress */

void init_array(int arr[N])
{
    for(int i=0; i<N; ++i)
        arr[i] = rand();
}

void serial_version(int *res, int arr[N])
{
    double start, finish;

    GET_TIME(start);
    *res = arr[0];
    for(int i=1; i<N; ++i) {
        if (*res < arr[i])
            *res = arr[i];
    }
    GET_TIME(finish);
    printf("[Serial] Elapsed Time = %.3f (sec)\n", finish-start);
}

int arr[N];
int res_serial, res_cuda_1, res_cuda_2, res_cuda_3, res_cuda_4;

int main(int argc, char *argv[])
{
    cuda_init();

    init_array(arr);

    serial_version(&res_serial, arr);

    cuda_version_1(&res_cuda_1, arr);
    if (res_serial != res_cuda_1)
        printf("[CUDA 1] ERROR! (%d vs %d)\n", res_serial, res_cuda_1);

    cuda_version_2(&res_cuda_2, arr);
    if (res_serial != res_cuda_2)
        printf("[CUDA 2] ERROR! (%d vs %d)\n", res_cuda_1, res_cuda_2);

    cuda_version_3(&res_cuda_3, arr);
    if (res_serial != res_cuda_3)
        printf("[CUDA 3] ERROR! (%d vs %d)\n", res_cuda_1, res_cuda_3);

    cuda_version_4(&res_cuda_4, arr);
    if (res_serial != res_cuda_4)
        printf("[CUDA 4] ERROR! (%d vs %d)\n", res_cuda_1, res_cuda_4);

    cuda_destroy();

    return 0;
}
