#ifndef __PPM_H__
#define __PPM_H__

typedef struct {
	unsigned char R, G, B;
} RGB;

typedef struct {
	char M, N;
	int width;
	int height;
	int max;
	RGB *pixels; // height*width
} PPMImage;		

int fnReadPPM(char* fileNm, PPMImage* img);
int fnWritePPM(char* fileNm, PPMImage* img);
void fnClosePPM(PPMImage* img);

#endif
