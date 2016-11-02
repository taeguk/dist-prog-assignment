#include <mpi.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define RANDOM_X_RANGE 1000

int prefix_sum(int rank, int num, int *val)
{
	int jump;

	// step 1
	for (jump = 1; jump < num; jump*=2)
	{
		int size = jump * 2;
		
		// sender
		if (rank % size == jump-1)
		{
			if (rank + jump < num)
				MPI_Send(val, 1, MPI_INT, rank + jump, 0, MPI_COMM_WORLD);
		}
		// receiver
		else if (rank % size == size-1)
		{
			MPI_Status stat;
			int tmp;
			MPI_Recv(&tmp, 1, MPI_INT, rank - jump, 0, MPI_COMM_WORLD, &stat);
			*val += tmp;
		}
	}

	// step 2
	for (jump/=4; jump >= 1; jump/=2)
	{
		int size = jump * 2;

		// sender
		if (rank % size == size-1)
		{
			if (rank + jump < num)
				MPI_Send(val, 1, MPI_INT, rank + jump, 0, MPI_COMM_WORLD);
		}
		// receiver
		else if (rank % size == jump-1)
		{
			if (rank - jump >= 0)
			{
				MPI_Status stat;
				int tmp;
				MPI_Recv(&tmp, 1, MPI_INT, rank - jump, 0, MPI_COMM_WORLD, &stat);
				*val += tmp;
			}
		}	
	}
}

int main(int argc, char **argv)
{
    double start_time, end_time;
	int rank, num;

    /*
     *  [Cautious] Do nothing before MPI_Init and MPI_Comm_rank and MPI_Comm_size.
     */

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num);

    if (rank == 0) {
        printf("READY!\n");
        start_time = MPI_Wtime();
    }

    //system("ifconfig");

	srand((rank+1) * time(NULL));
	
	int val = rand() % RANDOM_X_RANGE;
	int org_val = val;

	prefix_sum(rank, num, &val);

	printf("I'm %d of %d. Val = %d, Result = %d\n", rank, num, org_val, val);

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        end_time = MPI_Wtime();
        printf("[T] Elapsed time = %lf seconds.\n", end_time - start_time);
    }

	MPI_Finalize();

	return 0;
}
