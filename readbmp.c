
#include "readbmp.h"
#include "memory.h"
#include <stdio.h>
#include <string.h>

ClImage* clLoadImage(ClImage* bmpImg, unsigned char * bmpp){
	int channels = 1;
	int width = 0;
	int height = 0;
	int step = 0;
	int offset = 0;
	unsigned char pixVal;
	ClRgbQuad* quad;
	int i, j, k;

	unsigned short *fileType;
	ClBitMapFileHeader *bmpFileHeader;
	ClBitMapInfoHeader *bmpInfoHeader;
	bmpFileHeader = (ClBitMapFileHeader *)bmpp;
	bmpInfoHeader = (ClBitMapInfoHeader *)(bmpp+14);
	if (bmpFileHeader->bfType == 0x4D42){
		if (bmpInfoHeader->biBitCount == 8){
			channels = 1;
			width = bmpInfoHeader->biWidth;
			height = bmpInfoHeader->biHeight;
			offset = (channels*width)%4;

			if (offset != 0){
				offset = 4 - offset;
			}
			bmpImg->width = width;
			bmpImg->height = height;
			bmpImg->channels = 1;
			bmpImg->imageData = (unsigned char*)malloc(width*height);
			step = channels*width;
			bmpImg->quadTable = (ClRgbQuad*)malloc(1024);
			memcpy(bmpImg->quadTable, bmpp+54, 1024);
			k=1078;
			for (i=0; i<height; i++)
			{
				for (j=0; j<width; j++)
				{
					pixVal = *((unsigned char*)(bmpp+k));
					bmpImg->imageData[(height-1-i)*step+j] = pixVal;
					k++;
				}
				k+=offset;
			}
		}
	}

	return bmpImg;
}

/**
int main(){
	int i,j;
	ClImage bmpImg;
	FILE* pFile;
	unsigned char * bmpp;

	pFile = fopen("C:\\Users\\keepf\\Desktop\\操作系统课设\\mmmm.bmp", "rb");
	
	if(0==pFile){
		printf("open no good");
		return 0;
	}
	bmpp = (unsigned char *)malloc(29590);
	fread(bmpp, 1142, 1, pFile);
	fclose(pFile);


	printf("OK");
	clLoadImage(&bmpImg, bmpp);
		printf("\n");

	for(i=0; i<bmpImg.height; i++){
		printf("line %d ", i);
		for(j=0; j<bmpImg.width; j++){
			printf("%d ", j);
			int map = bmpImg.imageData[i*bmpImg.width+j];
			//printf("%2x,", bmpImg.quadTable[map].rgbBlue);
			//printf("%2x,", bmpImg.quadTable[map].rgbGreen);
			//printf("%2x   ", bmpImg.quadTable[map].rgbRed);
		}
		printf("\n");
	}
	return 0;
}

*/

