#include <cstdio>
#include <cstdlib>
#include <climits>
#include <omp.h>

#define CACHE_LINE_SIZE 64
#define ALIGNMENT_LEN (CACHE_LINE_SIZE/sizeof(long long))

int main(int argc, char **argv)
{
    const int thread_cnt = atoi(argv[1]);
    const int N = atoi(argv[2]);
    const int MAX_M = atoi(argv[3]);
    double start, finish;

    start = omp_get_wtime();

    long long (*local_result)[ALIGNMENT_LEN] = (long long (*)[ALIGNMENT_LEN])
            new long long[ALIGNMENT_LEN * thread_cnt];
    for (int i=0; i<thread_cnt; ++i) {
        local_result[i][0] = 1;
    }

    int **rand_num = new int*[N];
    int *cnt = new int[N];
    for(int i=0; i<N; ++i) {
        cnt[i] = rand() % MAX_M + 1;
        rand_num[i] = new int[cnt[i]];
        for (int j = 0; j < cnt[i]; ++j)
            rand_num[i][j] = rand();
    }

    int schd_boundary = N / 2;
    #pragma omp parallel num_threads(thread_cnt)
    {
        int tid = omp_get_thread_num();

        #pragma omp for schedule(static) nowait
        for (int i = 0; i < schd_boundary; ++i) {
            long long sum = 1;
            for(int j=0; j<cnt[i]; ++j) {
                sum += rand_num[i][j];
            }
            sum %= INT_MAX;
            local_result[tid][0] = (local_result[tid][0] * sum) % INT_MAX;
        }

        #pragma omp for schedule(dynamic)
        for (int i = schd_boundary; i < N; ++i) {
            long long sum = 1;
            for(int j=0; j<cnt[i]; ++j) {
                sum += rand_num[i][j];
            }
            sum %= INT_MAX;
            local_result[tid][0] = (local_result[tid][0] * sum) % INT_MAX;
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