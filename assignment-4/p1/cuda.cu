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

static __global__ void cuda_1(float C[MATRIX_SIZE][MATRIX_SIZE], float A[MATRIX_SIZE][MATRIX_SIZE], float B[MATRIX_SIZE][MATRIX_SIZE]);
static __global__ void cuda_2(float C[MATRIX_SIZE][MATRIX_SIZE], float A[MATRIX_SIZE][MATRIX_SIZE], float B[MATRIX_SIZE][MATRIX_SIZE]);
static __global__ void cuda_3(float C[MATRIX_SIZE][MATRIX_SIZE], float A[MATRIX_SIZE][MATRIX_SIZE], float B[MATRIX_SIZE][MATRIX_SIZE]);

extern "C" void cuda_init()
{
    CHECK_TIME_INIT_GPU();
}

extern "C" void cuda_destroy()
{
    CHECK_TIME_DEST_GPU();
}

extern "C" void cuda_version_1(float C[MATRIX_SIZE][MATRIX_SIZE], float A[MATRIX_SIZE][MATRIX_SIZE], float B[MATRIX_SIZE][MATRIX_SIZE])
{
    void *A_device, *B_device, *C_device;
    double start, finish;

    GET_TIME(start);

    CUDA_CALL(cudaMalloc((void**)&A_device, sizeof(float) * MATRIX_SIZE * MATRIX_SIZE));
    CUDA_CALL(cudaMalloc((void**)&B_device, sizeof(float) * MATRIX_SIZE * MATRIX_SIZE));
    CUDA_CALL(cudaMalloc((void**)&C_device, sizeof(float) * MATRIX_SIZE * MATRIX_SIZE));

    // transfer data from host to device.
    CUDA_CALL(cudaMemcpy(A_device, A, sizeof(float) * MATRIX_SIZE * MATRIX_SIZE, cudaMemcpyHostToDevice));
    CUDA_CALL(cudaMemcpy(B_device, B, sizeof(float) * MATRIX_SIZE * MATRIX_SIZE, cudaMemcpyHostToDevice));

    // execute kernel.
    dim3 block(32,32);
    dim3 grid(MATRIX_SIZE/32, MATRIX_SIZE/32);
    float device_time;

    printf("[CUDA 1] Start Launching Kernel (cuda_1).\n");
    CHECK_TIME_START_GPU();
    cuda_1<<<grid, block>>>((float(*)[MATRIX_SIZE])C_device, (float(*)[MATRIX_SIZE])A_device, (float(*)[MATRIX_SIZE])B_device);
    cuda_error_check( "ERROR: ", " when cuda_1 was launched.\n" );
    CHECK_TIME_END_GPU(device_time);
    printf("[CUDA 1] Only Processing Elapsed Time : %.3f (sec).\n", device_time/1000);

    // transfer result from device to host.
    CUDA_CALL(cudaMemcpy(C, C_device, sizeof(float) * MATRIX_SIZE * MATRIX_SIZE, cudaMemcpyDeviceToHost));
    CUDA_CALL( cudaDeviceSynchronize() );

    GET_TIME(finish);

    printf("[CUDA 1] All Elapsed Time : %.3f (sec).\n", finish-start);
}

extern "C" void cuda_version_2(float C[MATRIX_SIZE][MATRIX_SIZE], float A[MATRIX_SIZE][MATRIX_SIZE], float B[MATRIX_SIZE][MATRIX_SIZE])
{
    void *A_device, *B_device, *C_device;
    double start, finish;

    GET_TIME(start);

    CUDA_CALL(cudaMalloc((void**)&A_device, sizeof(float) * MATRIX_SIZE * MATRIX_SIZE));
    CUDA_CALL(cudaMalloc((void**)&B_device, sizeof(float) * MATRIX_SIZE * MATRIX_SIZE));
    CUDA_CALL(cudaMalloc((void**)&C_device, sizeof(float) * MATRIX_SIZE * MATRIX_SIZE));

    // transfer data from host to device.
    CUDA_CALL(cudaMemcpy(A_device, A, sizeof(float) * MATRIX_SIZE * MATRIX_SIZE, cudaMemcpyHostToDevice));
    CUDA_CALL(cudaMemcpy(B_device, B, sizeof(float) * MATRIX_SIZE * MATRIX_SIZE, cudaMemcpyHostToDevice));

    // execute kernel.
    dim3 block(32,32);
    dim3 grid(MATRIX_SIZE/32, MATRIX_SIZE/32);
    float device_time;

    printf("[CUDA 2] Start Launching Kernel (cuda_2).\n");
    CHECK_TIME_START_GPU();
    cuda_2<<<grid, block, sizeof(float)*32*32>>>((float(*)[MATRIX_SIZE])C_device, (float(*)[MATRIX_SIZE])A_device, (float(*)[MATRIX_SIZE])B_device);
    cuda_error_check( "ERROR: ", " when cuda_2 was launched.\n" );
    CHECK_TIME_END_GPU(device_time);
    printf("[CUDA 2] Only Processing Elapsed Time : %.3f (sec).\n", device_time/1000);

    // transfer result from device to host.
    CUDA_CALL(cudaMemcpy(C, C_device, sizeof(float) * MATRIX_SIZE * MATRIX_SIZE, cudaMemcpyDeviceToHost));
    CUDA_CALL( cudaDeviceSynchronize() );

    GET_TIME(finish);

    printf("[CUDA 2] All Elapsed Time : %.3f (sec).\n", finish-start);
}

extern "C" void cuda_version_3(float C[MATRIX_SIZE][MATRIX_SIZE], float A[MATRIX_SIZE][MATRIX_SIZE], float B[MATRIX_SIZE][MATRIX_SIZE])
{
    void *A_device, *B_device, *C_device;
    double start, finish;

    GET_TIME(start);

    CUDA_CALL(cudaMalloc((void**)&A_device, sizeof(float) * MATRIX_SIZE * MATRIX_SIZE));
    CUDA_CALL(cudaMalloc((void**)&B_device, sizeof(float) * MATRIX_SIZE * MATRIX_SIZE));
    CUDA_CALL(cudaMalloc((void**)&C_device, sizeof(float) * MATRIX_SIZE * MATRIX_SIZE));

    // transfer data from host to device.
    CUDA_CALL(cudaMemcpy(A_device, A, sizeof(float) * MATRIX_SIZE * MATRIX_SIZE, cudaMemcpyHostToDevice));
    CUDA_CALL(cudaMemcpy(B_device, B, sizeof(float) * MATRIX_SIZE * MATRIX_SIZE, cudaMemcpyHostToDevice));

    // execute kernel.
    dim3 block(32,32);
    dim3 grid(MATRIX_SIZE/32, MATRIX_SIZE/32);
    float device_time;

    printf("[CUDA 3] Start Launching Kernel (cuda_3).\n");
    CHECK_TIME_START_GPU();
    cuda_3<<<grid, block, 2*sizeof(float)*32*32>>>((float(*)[MATRIX_SIZE])C_device, (float(*)[MATRIX_SIZE])A_device, (float(*)[MATRIX_SIZE])B_device);
    cuda_error_check( "ERROR: ", " when cuda_3 was launched.\n" );
    CHECK_TIME_END_GPU(device_time);
    printf("[CUDA 3] Only Processing Elapsed Time : %.3f (sec).\n", device_time/1000);

    // transfer result from device to host.
    CUDA_CALL(cudaMemcpy(C, C_device, sizeof(float) * MATRIX_SIZE * MATRIX_SIZE, cudaMemcpyDeviceToHost));
    CUDA_CALL( cudaDeviceSynchronize() );

    GET_TIME(finish);

    printf("[CUDA 3] All Elapsed Time : %.3f (sec).\n", finish-start);
}

static __global__ void cuda_1(float C[MATRIX_SIZE][MATRIX_SIZE], float A[MATRIX_SIZE][MATRIX_SIZE], float B[MATRIX_SIZE][MATRIX_SIZE])
{
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    float result = 0.0f;
    for(int k=0; k<MATRIX_SIZE; ++k)
        result += A[row][k] * B[k][col];
    C[row][col] = result;
}

static __global__ void cuda_2(float C[MATRIX_SIZE][MATRIX_SIZE], float A[MATRIX_SIZE][MATRIX_SIZE], float B[MATRIX_SIZE][MATRIX_SIZE])
{
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    extern __shared__ float B_shared[/* blockDim.y * blockDim.x */];

    float result = 0.0f;
    int y_base = 0;
    while (y_base < MATRIX_SIZE) {
        // load a chunk from global memory to shared memory.
        B_shared[threadIdx.y * blockDim.x + threadIdx.x] = B[y_base + threadIdx.y][col];
        //B_shared[threadIdx.x * blockDim.y + threadIdx.y] = B[y_base + threadIdx.y][col];
        //B_shared[threadIdx.x * (blockDim.y+1) + threadIdx.y] = B[y_base + threadIdx.y][col];
        __syncthreads();

        for(int k=0; k<blockDim.y; ++k)
            //result += A[row][y_base + k] * B[y_base + k][col];
            result += A[row][y_base + k] * B_shared[k * blockDim.x + threadIdx.x];    // most fastest.
        //result += A[row][y_base + k] * B_shared[threadIdx.x * blockDim.y + k];  // bank conflict.
        //result += A[row][y_base + k] * B_shared[threadIdx.x * (blockDim.y+1) + k];  // no bank conflict.

        // * THIS BARRIER MUST BE NEEDED! because of B_shared.
        __syncthreads();

        y_base += blockDim.y;
    }
    C[row][col] = result;
}

static __global__ void cuda_3(float C[MATRIX_SIZE][MATRIX_SIZE], float A[MATRIX_SIZE][MATRIX_SIZE], float B[MATRIX_SIZE][MATRIX_SIZE])
{
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    extern __shared__ float shared_memory[/* 2 * blockDim.y * blockDim.x */];
    float *A_shared = &shared_memory[0];
    float *B_shared = &shared_memory[blockDim.y * blockDim.x];

    float result = 0.0f;
    int k_base = 0;
    while (k_base < MATRIX_SIZE) {
        // load a chunk from global memory to shared memory.
        A_shared[threadIdx.y * blockDim.x + threadIdx.x] = A[row][k_base + threadIdx.x];
        B_shared[threadIdx.y * blockDim.x + threadIdx.x] = B[k_base + threadIdx.y][col];
        __syncthreads();

        for(int k=0; k<blockDim.y; ++k)
            result += A_shared[threadIdx.y * blockDim.x + k] * B_shared[k * blockDim.x + threadIdx.x];

        __syncthreads();

        k_base += blockDim.y;
    }
    C[row][col] = result;
}
