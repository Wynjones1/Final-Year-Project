#ifndef TEXTURE_H
#define TEXTURE_H
#include <stdint.h>

typedef struct texture texture_t;

enum texture_mode
{
	texture_mode_linear,
	texture_mode_bilinear,
	texture_mode_bicubic
};

enum texture_repeat
{
	texture_repeat_clip,
	texture_repeat_repeat,
};

texture_t *texture_read(const char *filename);
void       texture_delete(texture_t *tex);

void texture_sample(texture_t *tex, double u, double v, double colour_out[3]);
texture_t *texture_test1();

#endif
