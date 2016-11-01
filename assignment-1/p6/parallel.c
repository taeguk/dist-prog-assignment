#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "ppm.h"

#define INPUT_FILE_NAME "example/small/boxes_1.ppm"
#define OUTPUT_FILE_NAME "output/small/boxes_1.ppm"
//#define INPUT_FILE_NAME "example/large/falls_2.ppm"
//#define OUTPUT_FILE_NAME "output/large/falls_2.ppm"

typedef struct {
	int offset;
	RGB rgb;
} Pixel;

int rank, K;
int root = 0;
MPI_Datatype RGB_Type, Pixel_Type;

void derivedRGBType (MPI_Datatype *t)
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

	MPI_Type_create_struct(3, blklen, displ, types, t);
	MPI_Type_commit(t);
}

void derivedPixelType (MPI_Datatype *t)
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

	MPI_Type_create_struct(2, blklen, displ, types, t);
	MPI_Type_commit(t);
}

void parallel_flip(PPMImage *img)
{
	int N = img->width * img->height / 2;
	int cnt = N / K;
	bool has_dummy;
	int si, ei;

	if (N % K == 0)
	{
		has_dummy = false;
		si = rank * cnt;
	}
	else if (rank < N % K)
	{
		++cnt;
		has_dummy = false;
		si = rank * cnt;
	}
	else
	{
		has_dummy = true;
		si = (N % K) * (cnt+1);
		si += (rank - N % K) * cnt;
	}

	ei = si + cnt;

	printf("[%d] N = %d, cnt = %d\n", rank, N, cnt);
	printf("[%d] %d, %d\n", rank, si, ei);

	Pixel* pixels = malloc(sizeof(Pixel) * ((cnt+1)*2));
	int pcnt = 0;

	for(int i=si; i<ei; ++i)
	{
		int y = i / img->width;
		int x = i % img->width;
		int ry = img->height-1-y;

		printf("[%d] y = %d, ry = %d, x = %d\n", rank, y, ry, x);
		
		//RGB tmp = img->pixels[y * img->width + x];
		//img->pixels[y * img->width + x] = img->pixels[ry * img->width + x];
		//img->pixels[ry * img->width + x] = tmp;
		
		pixels[pcnt].offset = y * img->width + x;
		pixels[pcnt++].rgb = img->pixels[ry * img->width + x];
		pixels[pcnt].offset = ry * img->width + x;
		pixels[pcnt++].rgb = img->pixels[y * img->width + x];
	}

	if (has_dummy)
	{
		pixels[pcnt++].offset = -1;
		pixels[pcnt++].offset = -1;
	}

	Pixel* recv_pixels = NULL;
	if (rank == root)
		recv_pixels = malloc(sizeof(Pixel) * pcnt * K);

	int err = MPI_Gather(pixels, pcnt, Pixel_Type, recv_pixels, pcnt, Pixel_Type, root, MPI_COMM_WORLD);

	if (err != MPI_SUCCESS)
		printf("something goes wrong!\n");

	// master
	if (rank == root)
	{
		for(int i=0; i<pcnt*K; ++i)
		{
			int offset = recv_pixels[i].offset;
			
			if (offset < 0)
			{
				printf("213r13r2fsadfasdgg\n");
				return;
			}

			img->pixels[offset] = recv_pixels[i].rgb;
		}
		
		free(recv_pixels);
	}

	free(pixels);
}

void parallel_grayscale(PPMImage *img)
{
}

int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &K);

	derivedRGBType(&RGB_Type);
	derivedPixelType(&Pixel_Type);

	PPMImage img;

	fnReadPPM(INPUT_FILE_NAME, &img);
	
	parallel_flip(&img);
	parallel_grayscale(&img);

	if (rank == root)
	{
		fnWritePPM(OUTPUT_FILE_NAME, &img);
	}

	fnClosePPM(&img);

	MPI_Finalize();

	return 0;
}
