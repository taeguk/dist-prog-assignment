#include <cuda_runtime.h>
#include <device_launch_parameters.h>

#include <assert.h>
#include <stdio.h>

#include "cuda.cuh"
#include "timer.h"

#define CUDA_CALL(x) { const cudaError_t a = (x); if(a != cudaSuccess) { printf("\nCuda Error: %s (err_num=%d) at line:%d\n", cudaGetErrorString(a), a, __LINE__); cudaDeviceReset(); assert(0);}}
typedef float TIMER_T;

#define USE_GPU_TIMER 1

#if USE_GPU_TIMER == 1
cudaEvent_t cuda_timer_start, cuda_timer_stop;
#define CUDA_STREAM_0 (0)

void create_device_timer()
{
    CUDA_CALL(cudaEventCreate(&cuda_timer_start));
    CUDA_CALL(cudaEventCreate(&cuda_timer_stop));
}

void destroy_device_timer()
{
    CUDA_CALL( cudaEventDestroy( cuda_timer_start ) );
    CUDA_CALL( cudaEventDestroy( cuda_timer_stop ) );
}

inline void start_device_timer()
{
    cudaEventRecord(cuda_timer_start, CUDA_STREAM_0);
}

inline TIMER_T stop_device_timer()
{
    TIMER_T ms;
    cudaEventRecord(cuda_timer_stop, CUDA_STREAM_0);
    cudaEventSynchronize(cuda_timer_stop);

    cudaEventElapsedTime(&ms, cuda_timer_start, cuda_timer_stop);
    return ms;
}

#define CHECK_TIME_INIT_GPU() { create_device_timer(); }
#define CHECK_TIME_START_GPU() { start_device_timer(); }
#define CHECK_TIME_END_GPU(a) { a = stop_device_timer(); }
#define CHECK_TIME_DEST_GPU() { destroy_device_timer(); }
#else
#define CHECK_TIME_INIT_GPU()
#define CHECK_TIME_START_GPU()
#define CHECK_TIME_END_GPU(a)
#define CHECK_TIME_DEST_GPU()
#endif

__host__ void cuda_error_check(const char * prefix, const char * postfix)
{
    if (cudaPeekAtLastError() != cudaSuccess)
    {
        printf("%s%s%s", prefix, cudaGetErrorString(cudaGetLastError()), postfix);
        cudaDeviceReset();
        //wait_exit();
        exit(1);
    }
}

static __global__ void cuda_1(int *arr, int *output, int size);
static __global__ void cuda_2(int *arr, int *output, int size);
static __global__ void cuda_3(int *arr, int *output, int size);
static __global__ void cuda_4(int *arr, int *output, int size);

extern "C" void cuda_init()
{
    CHECK_TIME_INIT_GPU();
}

extern "C" void cuda_destroy()
{
    CHECK_TIME_DEST_GPU();
}

extern "C" void cuda_version_1(int *res, int arr[N])
{
    void *output_device, *arr_device;
    double start, finish;
    const int num_threads = 256;
    int size = N;
    int num_blocks = (size+num_threads-1) / num_threads;

    GET_TIME(start);

    CUDA_CALL(cudaMalloc((void**)&arr_device, sizeof(int) * N));
    CUDA_CALL(cudaMalloc((void**)&output_device, sizeof(int) * num_blocks));

    // transfer data from host to device.
    CUDA_CALL(cudaMemcpy(arr_device, arr, sizeof(int) * N, cudaMemcpyHostToDevice));

    float all_device_time = 0.f;

    do {
        float device_time;

        CHECK_TIME_START_GPU();
        cuda_1<<<num_blocks, num_threads>>>((int*)arr_device, (int*)output_device, size);
        cuda_error_check( "ERROR: ", " when cuda_1 was launched.\n" );
        CHECK_TIME_END_GPU(device_time);
        all_device_time += device_time;

        {
            void *tmp = arr_device;
            arr_device = output_device;
            output_device = tmp;
        }
        size = num_blocks;
        num_blocks = (size+num_threads-1) / num_threads;

    } while (size > 1);

    printf("[CUDA 1] Only Processing Elapsed Time : %.3f (sec).\n", all_device_time/1000);

    // transfer result from device to host.
    CUDA_CALL(cudaMemcpy(res, arr_device, sizeof(int), cudaMemcpyDeviceToHost));
    CUDA_CALL( cudaDeviceSynchronize() );

    CUDA_CALL(cudaFree(arr_device));
    CUDA_CALL(cudaFree(output_device));

    GET_TIME(finish);

    printf("[CUDA 1] All Elapsed Time : %.3f (sec).\n", finish-start);
}

extern "C" void cuda_version_2(int *res, int arr[N])
{
    void *output_device, *arr_device;
    double start, finish;
    const int num_threads = 256;
    int size = N;
    int num_blocks = (size+num_threads-1) / num_threads;

    GET_TIME(start);

    CUDA_CALL(cudaMalloc((void**)&arr_device, sizeof(int) * N));
    CUDA_CALL(cudaMalloc((void**)&output_device, sizeof(int) * num_blocks));

    // transfer data from host to device.
    CUDA_CALL(cudaMemcpy(arr_device, arr, sizeof(int) * N, cudaMemcpyHostToDevice));

    float all_device_time = 0.f;

    do {
        float device_time;

        CHECK_TIME_START_GPU();
        cuda_2<<<num_blocks, num_threads>>>((int*)arr_device, (int*)output_device, size);
        cuda_error_check( "ERROR: ", " when cuda_2 was launched.\n" );
        CHECK_TIME_END_GPU(device_time);
        all_device_time += device_time;

        {
            void *tmp = arr_device;
            arr_device = output_device;
            output_device = tmp;
        }
        size = num_blocks;
        num_blocks = (size+num_threads-1) / num_threads;

    } while (size > 1);

    printf("[CUDA 2] Only Processing Elapsed Time : %.3f (sec).\n", all_device_time/1000);

    // transfer result from device to host.
    CUDA_CALL(cudaMemcpy(res, arr_device, sizeof(int), cudaMemcpyDeviceToHost));
    CUDA_CALL( cudaDeviceSynchronize() );

    CUDA_CALL(cudaFree(arr_device));
    CUDA_CALL(cudaFree(output_device));

    GET_TIME(finish);

    printf("[CUDA 2] All Elapsed Time : %.3f (sec).\n", finish-start);
}

extern "C" void cuda_version_3(int *res, int arr[N])
{
    void *output_device, *arr_device;
    double start, finish;
    const int num_threads = 256;
    int size = N;
    int num_blocks = (size+num_threads-1) / num_threads;

    GET_TIME(start);

    CUDA_CALL(cudaMalloc((void**)&arr_device, sizeof(int) * N));
    CUDA_CALL(cudaMalloc((void**)&output_device, sizeof(int) * num_blocks));

    // transfer data from host to device.
    CUDA_CALL(cudaMemcpy(arr_device, arr, sizeof(int) * N, cudaMemcpyHostToDevice));

    float all_device_time = 0.f;

    do {
        float device_time;

        CHECK_TIME_START_GPU();
        cuda_3<<<num_blocks, num_threads, sizeof(int) * num_threads>>>((int*)arr_device, (int*)output_device, size);
        cuda_error_check( "ERROR: ", " when cuda_3 was launched.\n" );
        CHECK_TIME_END_GPU(device_time);
        all_device_time += device_time;

        {
            void *tmp = arr_device;
            arr_device = output_device;
            output_device = tmp;
        }
        size = num_blocks;
        num_blocks = (size+num_threads-1) / num_threads;

    } while (size > 1);

    printf("[CUDA 3] Only Processing Elapsed Time : %.3f (sec).\n", all_device_time/1000);

    // transfer result from device to host.
    CUDA_CALL(cudaMemcpy(res, arr_device, sizeof(int), cudaMemcpyDeviceToHost));
    CUDA_CALL( cudaDeviceSynchronize() );

    CUDA_CALL(cudaFree(arr_device));
    CUDA_CALL(cudaFree(output_device));

    GET_TIME(finish);

    printf("[CUDA 3] All Elapsed Time : %.3f (sec).\n", finish-start);
}

extern "C" void cuda_version_4(int *res, int arr[N])
{
    void *output_device, *arr_device;
    double start, finish;
    const int num_threads = 256;
    int size = N;
    int num_blocks = (size+num_threads-1) / num_threads;

    GET_TIME(start);

    CUDA_CALL(cudaMalloc((void**)&arr_device, sizeof(int) * N));
    CUDA_CALL(cudaMalloc((void**)&output_device, sizeof(int) * num_blocks));

    // transfer data from host to device.
    CUDA_CALL(cudaMemcpy(arr_device, arr, sizeof(int) * N, cudaMemcpyHostToDevice));

    float all_device_time = 0.f;

    do {
        float device_time;

        CHECK_TIME_START_GPU();
        cuda_4<<<num_blocks, num_threads, sizeof(int) * num_threads>>>((int*)arr_device, (int*)output_device, size);
        cuda_error_check( "ERROR: ", " when cuda_4 was launched.\n" );
        CHECK_TIME_END_GPU(device_time);
        all_device_time += device_time;

        {
            void *tmp = arr_device;
            arr_device = output_device;
            output_device = tmp;
        }
        size = num_blocks;
        num_blocks = (size+num_threads-1) / num_threads;

    } while (size > 1);

    printf("[CUDA 4] Only Processing Elapsed Time : %.3f (sec).\n", all_device_time/1000);

    // transfer result from device to host.
    CUDA_CALL(cudaMemcpy(res, arr_device, sizeof(int), cudaMemcpyDeviceToHost));
    CUDA_CALL( cudaDeviceSynchronize() );

    CUDA_CALL(cudaFree(arr_device));
    CUDA_CALL(cudaFree(output_device));

    GET_TIME(finish);

    printf("[CUDA 4] All Elapsed Time : %.3f (sec).\n", finish-start);
}

static __global__ void cuda_1(int *arr, int *output, int size)
{
    int tid = blockIdx.x * blockDim.x + threadIdx.x;

    for(int s=1; s<blockDim.x; s*=2) {
        if (threadIdx.x % (s*2) == 0 && tid+s < size)     // path divergence
            arr[tid] = max(arr[tid], arr[tid+s]);
        __syncthreads();
    }
    if(threadIdx.x == 0)
        output[blockIdx.x] = arr[tid];
}

static __global__ void cuda_2(int *arr, int *output, int size)
{
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    const int base = blockIdx.x * blockDim.x;

    for(int s=1; s<blockDim.x; s*=2) {
        int offset = 2*s*threadIdx.x;
        int idx = base + offset;
        if (offset < blockDim.x  && idx + s < size)    // no path divergence
            arr[idx] = max(arr[idx], arr[idx + s]);
        __syncthreads();
    }
    if(threadIdx.x == 0)
        output[blockIdx.x] = arr[tid];
}

static __global__ void cuda_3(int *arr, int *output, int size)
{
    int tid = blockIdx.x * blockDim.x + threadIdx.x;

    extern __shared__ int arr_shared[/* blockDim.x */];
    int shared_size = min(blockDim.x, size - blockIdx.x * blockDim.x);    // size of arr_shared.

    arr_shared[threadIdx.x] = arr[tid];
    __syncthreads();

    for(int s=1; s<blockDim.x; s*=2) {
        int idx = 2*s*threadIdx.x;
        if (idx < blockDim.x && idx + s < shared_size)
            arr_shared[idx] = max(arr_shared[idx], arr_shared[idx + s]);    // bank conflict
        __syncthreads();
    }
    if(threadIdx.x == 0)
        output[blockIdx.x] = arr_shared[0];
}

static __global__ void cuda_4(int *arr, int *output, int size)
{
    int tid = blockIdx.x * blockDim.x + threadIdx.x;

    extern __shared__ int arr_shared[/* blockDim.x */];
    int shared_size = min(blockDim.x, size - blockIdx.x * blockDim.x);    // size of arr_shared.

    arr_shared[threadIdx.x] = arr[tid];
    __syncthreads();

    for(int s=blockDim.x/2; s>0; s/=2) {
        if (threadIdx.x < s && threadIdx.x + s < shared_size)
            arr_shared[threadIdx.x] = max(arr_shared[threadIdx.x], arr_shared[threadIdx.x + s]);    // no bank conflict
        __syncthreads();
    }
    if(threadIdx.x == 0)
        output[blockIdx.x] = arr_shared[0];
}
