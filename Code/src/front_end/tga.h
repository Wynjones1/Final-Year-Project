#ifndef IMAGE_H
#define IMAGE_H
#include <stdio.h>

struct tga
{
	int   height;
	int   width;
	int   bytes_per_pixel;
	int   offset;
	char  *data;
};

typedef struct tga tga_t;

/*!
	Reads in a tga file into an image struct.
	the image will be stored with pixel 0,0
	in the top left.
*/

tga_t *tga_test1();

tga_t *tga_read(const char *filename);
tga_t *tga_new(int width,int height,int bytes_per_pixel,const char *filename);
void   tga_write_tga(tga_t *tga,const char *filename);
void   tga_get_pixel(tga_t *tga,int x,int y,char *pix);
void   tga_set_pixel(tga_t *tga,int x,int y, ...);
void   tga_delete(tga_t *tga);

#endif
