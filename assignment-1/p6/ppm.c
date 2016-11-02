#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ppm.h"

#define TRUE	1
#define FALSE	0		

#define PPMREADBUFLEN 256

int fnReadPPM(char* fileNm, PPMImage* img)
{
	FILE* fp;
	char buf[PPMREADBUFLEN], *t;

	if(fileNm == NULL){
		fprintf(stderr, "fnReadPPM 호출 에러\n");
		return FALSE;
	}
	
	fp = fopen(fileNm, "rb");	// binary mode
	if(fp == NULL){
		fprintf(stderr, "파일을 열 수 없습니다 : %s\n", fileNm);
		return FALSE;
	}

	fscanf(fp, "%c%c\n", &img->M, &img->N);	// 매직넘버 읽기

	if(img->M != 'P' || img->N != '6'){
		fprintf(stderr, "PPM 이미지 포멧이 아닙니다 : %c%c\n", img->M, img->N);
		return FALSE;
	}

	do
	{ /* Px formats can have # comments after first line */
		t = fgets(buf, PPMREADBUFLEN, fp);
		if ( t == NULL ) return FALSE;
	} while ( strncmp(buf, "#", 1) == 0 );
	
	sscanf(buf, "%d %d\n", &img->width, &img->height);	// 가로, 세로 읽기
	fscanf(fp, "%d\n"   , &img->max                );	// 최대명암도 값

	if(img->max != 255){
		fprintf(stderr, "올바른 이미지 포멧이 아닙니다. : (%d,%d), %d\n", img->width, img->height, img->max);
		return FALSE;
	}


	// <-- 메모리 할당
	img->pixels = calloc(img->height * img->width, sizeof(*img->pixels));

	// -->


	// <-- ppm 파일로부터 픽셀값을 읽어서 할당한 메모리에 load
	for(int i=0; i<img->height; i++){
		for(int j=0; j<img->width; j++){
			fread(&img->pixels[i * img->width + j], sizeof(unsigned char), 3, fp);
		}
	}
	// -->


	fclose(fp);	// 더 이상 사용하지 않는 파일을 닫아 줌

	return TRUE;
}

int fnWritePPM(char* fileNm, PPMImage* img)
{
	FILE* fp;

	fp = fopen(fileNm, "wb");
	if(fp == NULL){
		fprintf(stderr, "파일 생성에 실패하였습니다.\n");
		return FALSE;
	}

	fprintf(fp, "%c%c\n", 'P', '6');
	fprintf(fp, "%d %d\n" , img->width, img->height);
	fprintf(fp, "%d\n", 255);


	for(int i=0; i<img->height; i++){
		for(int j=0; j<img->width; j++){
			fwrite(&img->pixels[i * img->width + j], sizeof(unsigned char), 3, fp);
		}
	}

	fclose(fp);
	
	return TRUE;
}

void fnClosePPM(PPMImage* img)
{
	free(img->pixels);
}
