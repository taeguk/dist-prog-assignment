#include <cstdio>
#include <cstdlib>
#include <climits>
#include <omp.h>

#define CACHE_LINE_SIZE 64
#define ALIGNMENT_LEN (CACHE_LINE_SIZE/sizeof(long long))

#define N 100000000

int main(int argc, char **argv)
{
    const int thread_cnt = atoi(argv[1]);
    const int chunk_size = atoi(argv[2]);
    double start, finish;

    start = omp_get_wtime();

    long long (*local_result)[ALIGNMENT_LEN] = (long long (*)[ALIGNMENT_LEN])
            new long long[ALIGNMENT_LEN * thread_cnt];
    for (int i=0; i<thread_cnt; ++i) {
        local_result[i][0] = 1;
    }

    int *rand_num = new int[N];
    for(int i=0; i<N; ++i)
        rand_num[i] = rand();

    #pragma omp parallel num_threads(thread_cnt)
    {
        int tid = omp_get_thread_num();
        int tnum = omp_get_num_threads();

        if (chunk_size > 0) {
            int jump = tnum * chunk_size;
            for (int i = tid; i < N; i += jump) {
                for (int j = 0; j < chunk_size; ++j)
                    local_result[tid][0] = (local_result[tid][0] * rand_num[i+j]) % INT_MAX;
            }
        }
        else { // max chunk size
            int start = tid * N / tnum;
            int end = (tid+1) * N / tnum;
            for(int i=start; i<end; ++i) {
                local_result[tid][0] = (local_result[tid][0] * rand_num[i]) % INT_MAX;
            }
        }
    }

    long long final_result = 1;
    for (int i=0; i<thread_cnt; ++i) {
        final_result = (final_result * local_result[i][0]) % INT_MAX;
    }

    finish = omp_get_wtime();

    printf("Result : %lld\n", final_result);
    printf("Elapsed Time : %.4f(s)\n", finish - start);
}