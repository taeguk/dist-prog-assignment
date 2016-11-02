#include "ppm.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int EX_WIDTH = atoi(argv[1]);
    int EX_HEIGHT = atoi(argv[2]);
    char* INPUT_FILENAME = argv[3];
    char* OUTPUT_FILENAME = argv[4];

    PPMImage img;
    fnReadPPM(INPUT_FILENAME, &img);

    int old_width = img.width, old_height = img.height;
    int new_width = img.width * EX_WIDTH, new_height = img.height * EX_HEIGHT;

    RGB *new_pixels = malloc(sizeof(RGB) * new_width * new_height);

    for(int i=0; i<EX_HEIGHT; ++i) {
        for (int j=0; j<EX_WIDTH; ++j) {
            for(int a=0; a<old_height; ++a) {
                for (int b=0; b<old_width; ++b) {
                    new_pixels[(i * old_height + a) * new_width + (j * old_width + b)]
                            = img.pixels[a * old_width + b];
                }
            }
        }
    }

    img.pixels = new_pixels;
    img.width = new_width;
    img.height = new_height;

    fnWritePPM(OUTPUT_FILENAME, &img);
    fnClosePPM(&img);

    return 0;
}