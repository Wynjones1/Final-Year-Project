#ifndef BMP_H
#define BMP_H
#include <stdint.h>
/*
 *	32bit pixel bmp
 */

typedef struct bmp bmp_t;

bmp_t *bmp_init(const char *filename, int width, int height);
void   bmp_write(bmp_t *bmp, int width, int height, uint32_t data);
void   bmp_pixel_to_camera(bmp_t *bmp, int x, int y, double *xnorm, double *ynorm);
int    bmp_width(bmp_t *bmp);
int    bmp_height(bmp_t *bmp);
void   bmp_delete(bmp_t *bmp);

void bmp_diff(const char *file1, const char *file2, int *x, int *y);

#define BMP_COLOUR(red, green, blue)    ((((char)red   & 0xff) << 16)) |\
					((((char)green & 0xff) << 8))|\
					((((char)blue  & 0xff)))

#endif
