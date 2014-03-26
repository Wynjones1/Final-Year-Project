#include "tga.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include "utils.h"
#define HEADER_SIZE 18

void write_header(tga_t *tga, const char *filename)
{
	short height = tga->height;
	short width  = tga->width;
	char header[HEADER_SIZE];
	memset(header,0x0,18);
	header[2] = 2;
	header[7] = tga->bytes_per_pixel * 8;
	memcpy(&header[12],&width,2);
	memcpy(&header[14],&height,2);
	header[16] = tga->bytes_per_pixel * 8;
	header[17] |= 0x0 << 4; //Left to right
	header[17] |= 0x0 << 5; //Top to bottom

	FILE *fp = OPEN(filename, "w");
	fwrite(header,18,1,fp);
	if(errno)
	{
		ERROR("Failed to write data: %s.\n",strerror(errno));
	}
	fseek(fp,18 + width * height * tga->bytes_per_pixel,SEEK_SET);
	fputc('0',fp);
	fclose(fp);
}

tga_t *tga_new(int width,int height,int bytes_per_pixel, const char *filename)
{
	tga_t *tga  = malloc(sizeof(tga_t));
	tga->width  = width;
	tga->height = height;
	tga->bytes_per_pixel = bytes_per_pixel;
	write_header(tga, filename);
	return tga;
}

void pixel_swap(char *p1,char *p2,size_t size)
{
	char temp[4];
	assert(size <= 4); //Pixel width of more than 4 bytes are not supported.
	memcpy(temp,p1,size);
	memcpy(p1,p2,size);
	memcpy(p2,temp,size);
}

void convert_to_left_to_right(tga_t *tga)
{
	int   width  = tga->width;
	int   height = tga->height;
	char p1[4],p2[4];
	int   i, j;

	for(i = 0;i < width / 2;i++)
	{
		for(j = 0;j < height;j++)
		{
			tga_get_pixel(tga,i,j,p1);
			tga_get_pixel(tga,width - i - 1,j,p2);
			pixel_swap(p1,p2,tga->bytes_per_pixel);
		}
	}
}

void convert_to_bottom_to_top(tga_t *tga)
{
	int   width  = tga->width;
	int   height = tga->height;
	int i,j;
	char p1[4],p2[4];
	for(i = 0; i < width; i++)
	{
		for(j = 0; j < height / 2; j++)
		{
			tga_get_pixel(tga,i,j,p1);
			tga_get_pixel(tga,i,height - j - 1,p2);
			pixel_swap(p1,p2,tga->bytes_per_pixel);
		}
	}
}

char *read_file_to_data(const char *filename)
{
	FILE *fp = OPEN(filename, "r");
	fseek(fp, 0, SEEK_END);
	size_t allocation_size = ftell(fp);
	rewind(fp);
	char *out = malloc(allocation_size);
	if(!fread(out, 1, allocation_size, fp))
	{
		ERROR("Could not read file %s: %s\n", filename, strerror(errno));
	}
	fclose(fp);
	return out;
}

/*!
 * Read in an TGA tga, the tga will be read in
 * with the pixel 0,0 in the bottom left.
 */
tga_t *tga_read(const char *filename)
{
	tga_t  *tga          = malloc(sizeof(tga_t));
	char *data           = read_file_to_data(filename);
	char  id_size        = data[0];
	short width          = *(short*)&data[12];
	short height         = *(short*)&data[14];
	char  bits_per_pixel = data[16];
	char  left_to_right  = !((data[17] >> 4) & 0x1);
	char  top_to_bottom  = (data[17] >> 5) & 0x1;

	tga->offset          = 18 + id_size;
	tga->height          = height;
	tga->width           = width;
	tga->bytes_per_pixel = bits_per_pixel / 8;
	tga->data            = data;
	tga->data += tga->offset;


	if(!left_to_right)
	{
		convert_to_left_to_right(tga);	
	}
	if(top_to_bottom)
	{
		convert_to_bottom_to_top(tga);
	}
	return tga;
}

void tga_write_tga(tga_t *image,const char *filename)
{
	short height = image->height;
	short width  = image->width;
	char header[18];
	memset(header,0x0,18);
	header[2] = 2;
	header[7] = image->bytes_per_pixel * 8;
	memcpy(&header[12],&width,2);
	memcpy(&header[14],&height,2);
	header[16] = image->bytes_per_pixel * 8;
	header[17] |= 0x0 << 4; //Left to right
	header[17] |= 0x0 << 5; //Top to bottom

	FILE *fp = OPEN(filename, "w");
	errno = 0;
	fwrite(header,18,1,fp);
	if(errno)
	{
		ERROR("Could not write to %s : %s\n.",filename, strerror(errno));
	}
	errno = 0;
	fwrite(image->data,image->height * image->width,image->bytes_per_pixel,fp);
	if(errno)
	{
		ERROR("Could not write to %s : %s\n.",filename, strerror(errno));
	}
	fclose(fp);
}

void tga_get_pixel(tga_t *tga,int x,int y,char *pix)
{
	memcpy(pix, tga->data + tga->bytes_per_pixel * (x + y * tga->width), tga->bytes_per_pixel);
}

void   tga_set_pixel(tga_t *tga,int x,int y, ...)
{
	va_list ap;
	char temp;
	va_start(ap,y);
	int i;
	char *pix = tga->data + tga->bytes_per_pixel * (x + y * tga->width);
	for(i = 0;i < tga->bytes_per_pixel;i++)
	{
		temp = (char)va_arg(ap,int);
		pix[i] = temp;

	}
	va_end(ap);
}

void tga_delete(tga_t *tga)
{
	free(tga->data - tga->offset);
	free(tga);
}
