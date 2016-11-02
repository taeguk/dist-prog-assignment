#include "ppm.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define GET_TIME(now) { \
	struct timeval t; \
	gettimeofday(&t, NULL); \
	now = t.tv_sec + t.tv_usec / 1000000.0; \
}

RGB* alloc_pixels(int width, int height)
{
	RGB *pixels;

	pixels = malloc(sizeof(*pixels) * height * width);

	return pixels;
}

void flip_horizontally(PPMImage *img)
{
	for (int i = 0; i < img->height; ++i) {
		RGB *base = &img->pixels[i * img->width];
		for (int j = 0; j < img->width / 2; ++j) {
			RGB tmp = base[j];
			base[j] = base[img->width - 1 - j];
			base[img->width - 1 - j] = tmp;
		}
	}
}

void grayscale(PPMImage *img)
{
	for(int i=0; i<img->height; ++i)
	{
		for(int j=0; j<img->width; ++j)
		{
			unsigned char avg = ((int)img->pixels[i * img->width + j].R + 
				img->pixels[i * img->width + j].G + img->pixels[i * img->width + j].B) / 3;

			img->pixels[i * img->width + j].R = img->pixels[i * img->width + j].G
				= img->pixels[i * img->width + j].B = avg;
		}
	}
}

int main(int argc, char *argv[])
{
	double start_time, after_read_time, before_write_time, end_time;

	if (argc < 3) {
		printf("ERROR!\n");
		return 1;
	}

	GET_TIME(start_time);

	PPMImage img;
	fnReadPPM(argv[1], &img);

	GET_TIME(after_read_time);

	flip_horizontally(&img);
	grayscale(&img);

	GET_TIME(before_write_time);

	fnWritePPM(argv[2], &img);
	fnClosePPM(&img);

	GET_TIME(end_time);

	printf("[T] Elapsed time = %lf seconds.\n", end_time - start_time);
	printf("[T] Elapsed time without reading/writing PPM = %lf seconds.\n", before_write_time - after_read_time);

	return 0;
}
