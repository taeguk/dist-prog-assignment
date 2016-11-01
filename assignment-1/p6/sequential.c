#include "ppm.h"
#include <stdio.h>
#include <stdlib.h>

//#define INPUT_FILE_NAME "example/small/boxes_1.ppm"
//#define OUTPUT_FILE_NAME "output/small/boxes_1.ppm"
#define INPUT_FILE_NAME "example/large/falls_2.ppm"
#define OUTPUT_FILE_NAME "output/large/falls_2.ppm"

RGB* alloc_pixels(int width, int height)
{
	RGB *pixels;

	pixels = malloc(sizeof(*pixels) * height * width);

	return pixels;
}

void flip_horizontally(PPMImage *img)
{
	for(int i=0, ri=img->height-1; i < img->height/2; ++i, --ri)
	{
		for(int j=0; j<img->width; ++j)
		{
			RGB tmp = img->pixels[i * img->width + j];
			img->pixels[i * img->width + j] = img->pixels[ri * img->width + j];
			img->pixels[ri * img->width + j] = tmp;
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

int main(void)
{
	PPMImage img;

	fnReadPPM(INPUT_FILE_NAME, &img);

	flip_horizontally(&img);
	grayscale(&img);

	fnWritePPM(OUTPUT_FILE_NAME, &img);
	fnClosePPM(&img);

	return 0;
}
