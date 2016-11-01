#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int *integers;
int N;

void init_integers(void)
{
	int i;
	integers = malloc(sizeof(int) * N);
	for (i = 0; i < N; ++i)
		integers[i] = i;
}

int collapse(int num)
{
	int res = 0;
	while (num) {
		res += num % 10;
		num /= 10;
	}
	return res;
}

int ultimate_collapse(int num)
{
	while (num / 10 > 0)
		num = collapse(num);
	return num;
}

int main(int argc, char **argv)
{
	int rank, K;
	int root = 0;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &K);
	
	if (argc < 2)
		return 1;

	N = atoi(argv[1]);

	init_integers();
	
	int cnt = N / K;
	int si, ei;

	if (N % K == 0)
	{
		si = rank * cnt;
	}
	else if (rank < N % K)
	{
		++cnt;
		si = rank * cnt;
	}
	else
	{
		si = (N % K) * (cnt+1);
		si += (rank - N % K) * cnt;
	}
	
	ei = si + cnt;
		
	int local_collapse = 0;
	int i;
	for (i = si; i < ei; ++i)
		local_collapse += integers[i];
	local_collapse = collapse(local_collapse);

	int *arr;
	if (rank == root)
		arr = malloc(sizeof(int) * K);
	else
		arr = NULL;
	
	int err = MPI_Gather(&local_collapse, 1, MPI_INT, arr, 1, MPI_INT, root, MPI_COMM_WORLD);

	if (err != MPI_SUCCESS)
		printf("something goes wrong!\n");

	// master
	if (rank == root)
	{
		int result = 0;
		for (i = 0; i < K; ++i)
			result += arr[i];

		free(arr);

		result = ultimate_collapse(result);

		printf("Result = %d\n", result);
	}

	MPI_Finalize();

	free(integers);
	
	return 0;
}
