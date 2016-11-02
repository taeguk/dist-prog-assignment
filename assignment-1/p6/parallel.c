#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "ppm.h"

typedef struct {
	int offset;
	RGB rgb;
} Pixel;

int rank, K;
int root = 0;
MPI_Datatype RGB_Type, Pixel_Type, RGB_RowGroup_Type;

void derivedRGBType()
{
	RGB sample;

	int blklen[3];
	MPI_Aint displ[3], off, base;
	MPI_Datatype types[3];

	blklen[0] = blklen[1] = blklen[2] = 1;

	types[0] = types[1] = types[2] = MPI_UNSIGNED_CHAR;

	displ[0] = 0;
	MPI_Get_address(&sample.R, &base);
	MPI_Get_address(&sample.G, &off);
	displ[1] = off - base;
	MPI_Get_address(&sample.B, &off);
	displ[2] = off - base;

	MPI_Type_create_struct(3, blklen, displ, types, &RGB_Type);
	MPI_Type_commit(&RGB_Type);
}

void derivedPixelType()
{
	Pixel sample;

	int blklen[2];
	MPI_Aint displ[2], off, base;
	MPI_Datatype types[2];

	blklen[0] = blklen[1] = 1;

	types[0] = MPI_INT;
	types[1] = RGB_Type;

	displ[0] = 0;
	MPI_Get_address(&sample.offset, &base);
	MPI_Get_address(&sample.rgb, &off);
	displ[1] = off - base;

	MPI_Type_create_struct(2, blklen, displ, types, &Pixel_Type);
	MPI_Type_commit(&Pixel_Type);
}

void scatter_img_block(RGB *sendbuf, int send_block_cnt,
					   int **p_sendcounts, int **p_displs,
					   RGB **p_recvbuf, int *p_recv_block_cnt, int block_size)
{
	int i;

	if (rank == root) {
		*p_sendcounts = malloc(sizeof(int) * K);
		*p_displs = malloc(sizeof(int) * K);

		int idx = 0;
		for(i = 0; i < K; ++i) {
			(*p_displs)[i] = idx;

			if (i < send_block_cnt % K)
				(*p_sendcounts)[i] = send_block_cnt / K + 1;
			else
				(*p_sendcounts)[i] = send_block_cnt / K;

			idx += (*p_sendcounts)[i];
		}
	}

	if (rank < send_block_cnt % K)
		*p_recv_block_cnt = send_block_cnt / K + 1;
	else
		*p_recv_block_cnt = send_block_cnt / K;

	*p_recvbuf = malloc(sizeof(RGB) * block_size * (*p_recv_block_cnt));
	MPI_Scatterv(sendbuf, *p_sendcounts, *p_displs, RGB_RowGroup_Type,
				 *p_recvbuf, *p_recv_block_cnt, RGB_RowGroup_Type, root, MPI_COMM_WORLD);
}

void merge_results(RGB *merged, int *recvcounts, int *displs,
				   RGB *results, int block_cnt)
{
	int err = MPI_Gatherv(results, block_cnt, RGB_RowGroup_Type,
						  merged, recvcounts, displs,
						  RGB_RowGroup_Type, root, MPI_COMM_WORLD);

	if (err != MPI_SUCCESS)
		printf("something goes wrong!\n");
}

void process(RGB *data, int block_cnt, int block_size)
{
	RGB *blk_ptr = data;

	for(int i=0; i<block_cnt; ++i, blk_ptr += block_size) {

		// flip block.
		for(int j=0; j<block_size/2; ++j) {
			RGB tmp = blk_ptr[j];
			blk_ptr[j] = blk_ptr[block_size - 1 - j];
			blk_ptr[block_size - 1 - j] = tmp;
		}

		// grayscale
		for(int j=0; j<block_size; ++j) {
			blk_ptr[j].R = blk_ptr[j].G = blk_ptr[j].B =
					((int)blk_ptr[j].R + blk_ptr[j].G + blk_ptr[j].B) / 3;
		}
	}
}

int main(int argc, char **argv)
{
	double start_time, after_read_time, before_write_time, end_time;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &K);

	if (argc < 3) {
		printf("ERROR!\n");
		return 1;
	}

	PPMImage img;
	int width, height;

	if (rank == root) {
		printf("READY!\n");
		start_time = MPI_Wtime();
	}

	if (rank == root) {
		fnReadPPM(argv[1], &img);
		width = img.width;
		height = img.height;
	}
	after_read_time = MPI_Wtime();

	MPI_Bcast(&width, 1, MPI_INT, root, MPI_COMM_WORLD);
	MPI_Bcast(&height, 1, MPI_INT, root, MPI_COMM_WORLD);

	derivedRGBType();
	derivedPixelType();
	MPI_Type_vector(1, width, 1, RGB_Type, &RGB_RowGroup_Type);
	MPI_Type_commit(&RGB_RowGroup_Type);

	RGB *data = NULL;
	int data_block_cnt;
	int *counts = NULL, *displs = NULL;
	scatter_img_block(img.pixels, height,
					  &counts, &displs,
					  &data, &data_block_cnt, width);

	printf("[%d] received data block cnt = %d\n", rank, data_block_cnt);

	process(data, data_block_cnt, width);
	merge_results(img.pixels, counts, displs, data, data_block_cnt);

	if (!counts) free(counts);
	if (!displs) free(displs);
	if (!data) free(data);

	if (rank == root) {
		before_write_time = MPI_Wtime();
		fnWritePPM(argv[2], &img);
		fnClosePPM(&img);

		end_time = MPI_Wtime();
		printf("[T] Elapsed time = %lf seconds.\n", end_time - start_time);
		printf("[T] Elapsed time without reading/writing PPM = %lf seconds.\n", before_write_time - after_read_time);
	}

	MPI_Finalize();

	return 0;
}