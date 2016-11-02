#include <mpi.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define RANDOM_X_RANGE 1000

void scan(int *in, int *inout, int *len, MPI_Datatype *dptr)
{
	int i;
	for (i = 0; i < *len; ++i) {
		*inout = *in + *inout;
		in++;
		inout++;
	}
}

int main(int argc, char **argv)
{
	double start_time, end_time;
	int rank, num;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num);

	if (rank == 0) {
		printf("READY!\n");
		start_time = MPI_Wtime();
	}

	srand((rank+1) * time(NULL));
	
	MPI_Op myOp;
	int val = rand() % RANDOM_X_RANGE, result;

	/*
	 * noncommutative - set commute (arg 2) to 0
	 */
	MPI_Op_create((MPI_User_function *)scan, 0, &myOp);
	MPI_Scan(&val, &result, 1, MPI_INT, myOp, MPI_COMM_WORLD);

	printf("I'm %d of %d. Val = %d, Result = %d\n", rank, num, val, result);

	MPI_Barrier(MPI_COMM_WORLD);

	if (rank == 0) {
		end_time = MPI_Wtime();
		printf("[T] Elapsed time = %lf seconds.\n", end_time - start_time);
	}

	MPI_Finalize();

	return 0;
}

