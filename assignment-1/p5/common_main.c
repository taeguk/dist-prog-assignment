#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

extern void process(int *data, int cnt, int *p_local_result);
extern void merge(int *gathered_results, int cnt, int *p_final_result);

int rank, K;
int root = 0;

void read_integers(const char *filename, int *p_N, int **p_integers)
{
    FILE *fp = fopen(filename, "rt");
    int data_num;
    int i;

    fscanf(fp, "%d", p_N);
    *p_integers = malloc(sizeof(int) * (*p_N));
    for(i=0; i<*p_N; ++i)
        fscanf(fp, "%d", &(*p_integers)[i]);

    fclose(fp);
}

void scatter_data(int *sendbuf, int sendcnt, int **p_recvbuf, int *p_recvcnt)
{
    int *sendcounts = NULL;
    int *displs = NULL;
    int i;

    if (rank == root) {
        sendcounts = malloc(sizeof(int) * K);
        displs = malloc(sizeof(int) * K);

        int idx = 0;
        for(i = 0; i < K; ++i) {
            displs[i] = idx;

            if (i < sendcnt % K)
                sendcounts[i] = sendcnt / K + 1;
            else
                sendcounts[i] = sendcnt / K;

            idx += sendcounts[i];
        }
    }

    if (rank < sendcnt % K)
        *p_recvcnt = sendcnt / K + 1;
    else
        *p_recvcnt = sendcnt / K;

    *p_recvbuf = malloc(sizeof(int) * (*p_recvcnt));
    MPI_Scatterv(sendbuf, sendcounts, displs, MPI_INT,
                 *p_recvbuf, *p_recvcnt, MPI_INT, root, MPI_COMM_WORLD);

    if (rank == root) {
        free(sendcounts);
        free(displs);
    }
}

void gather_results(int local_result, int **p_gathered_results, int *p_cnt)
{
    if (rank == root) {
        *p_cnt = K;
        *p_gathered_results = malloc(sizeof(int) * K);
    }

    int err = MPI_Gather(&local_result, 1, MPI_INT, *p_gathered_results, 1, MPI_INT, root, MPI_COMM_WORLD);

    if (err != MPI_SUCCESS)
        printf("something goes wrong!\n");
}

int main(int argc, char *argv[])
{
    double start_time, after_read_time, end_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &K);

    if (argc < 2)
        return 1;

    int N;
    int *integers = NULL;

    if (rank == root) {
        printf("READY!\n");
        start_time = MPI_Wtime();
        read_integers(argv[1], &N, &integers);

        printf("AFTER READING INTEGERS!\n");
        after_read_time = MPI_Wtime();
    }

    MPI_Bcast(&N, 1, MPI_INT, root, MPI_COMM_WORLD);

    int *data, data_cnt;
    int local_result;
    scatter_data(integers, N, &data, &data_cnt);
    process(data, data_cnt, &local_result);

    int *gathered_results = NULL, result_cnt;
    int final_result;
    gather_results(local_result, &gathered_results, &result_cnt);

    if (rank == root) {
        merge(gathered_results, result_cnt, &final_result);
        printf("Result = %d\n", final_result);

        end_time = MPI_Wtime();
        printf("[T] Elapsed time = %lf seconds.\n", end_time - start_time);
        printf("[T] Elapsed time without reading integers = %lf seconds.\n", end_time - after_read_time);
    }

    MPI_Finalize();

    if (!integers) free(integers);
    if (!data) free(data);
    if (!gathered_results) free(gathered_results);

    return 0;
}
