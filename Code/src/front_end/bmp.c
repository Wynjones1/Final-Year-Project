#include "bmp.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "utils.h"

const uint32_t DIB_HEADER_SIZE = 50;
const uint32_t HEADER_SIZE     = 54;

struct bmp
{
	FILE *fp;
	int width;
	int height;
	int stride; //size in bytes of one line. (including padding)
	pthread_mutex_t mutex;
};

void fputdw(uint32_t dw, FILE *fp)
{
	for(int i = 0; i < 4; i++)
		fputc((dw >> 8 * i) & 0xff, fp);
}

void fputw(uint16_t dw, FILE *fp)
{
	for(int i = 0; i < 2; i++)
		fputc((dw >> 8 * i) & 0xff, fp);
}

bmp_t *bmp_init(const char *filename, int width, int height)
{
	bmp_t *out  = malloc(sizeof(bmp_t));
	out->fp     = OPEN(filename , "w");
	out->width  = width;
	out->height = height;
	//calculate the stride.
	out->stride = width * 3;
	if(out->stride % 4)
	{
		out->stride += 4 - out->stride % 4;
	}

	uint32_t size = HEADER_SIZE + (width * height * 4);
	//write the BMP header.
	fputc('B'          , out->fp); //magic number. 
	fputc('M'          , out->fp);
	fputdw(size        , out->fp); //file of the bmp.
	fputdw(0           , out->fp); //reserved.
	fputdw(HEADER_SIZE , out->fp); //offset to the data.
	//write the DIB header.
	fputdw(40       , out->fp); //size of the DIB header.
	fputdw(width    , out->fp); //width.
	fputdw(height   , out->fp); //height.
	fputw( 1        , out->fp); //number of colour planes.
	fputw( 24       , out->fp); //bits per pixel.	
	fputdw(0        , out->fp); //compression. (none)
	fputdw(40 + size, out->fp); //image size.
	fputdw(0        , out->fp); //horizontal resolution.
	fputdw(0        , out->fp); //vertical resolution.
	fputdw(0        , out->fp); //number of colours.
	fputdw(0        , out->fp); //number of important colours.
	//Write the end of the file
	fseek(out->fp, HEADER_SIZE + (width * height * 4) - 1, SEEK_SET);
	fputc(0, out->fp);
		
	pthread_mutex_init(&out->mutex, NULL);
	return out;
}

void bmp_write(bmp_t *bmp, int width, int height, uint32_t data)
{
	data &= 0x00ffffff;
	pthread_mutex_lock(&bmp->mutex);
	fseek(bmp->fp, HEADER_SIZE + height * bmp->stride + 3 * width, SEEK_SET);
	fwrite(&data, 1, 3, bmp->fp);
	pthread_mutex_unlock(&bmp->mutex);
}

void bmp_pixel_to_camera(bmp_t *bmp, int x, int y, double *xnorm, double *ynorm)
{
	*xnorm = ((double)x - bmp->width  / 2 + 0.5) / bmp->width;
	*ynorm = ((double)y - bmp->height / 2 + 0.5) / bmp->height;
}

int bmp_width(bmp_t *bmp)
{
	return bmp->width;
}
int bmp_height(bmp_t *bmp)
{
	return bmp->height;
}

void bmp_delete(bmp_t *bmp)
{
	fclose(bmp->fp);
	free(bmp);
}
