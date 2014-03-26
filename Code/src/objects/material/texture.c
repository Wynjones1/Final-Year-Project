#include "texture.h"
#include "tga.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "maths.h"

struct texture
{
	enum texture_mode   mode;
	enum texture_repeat repeat;
	tga_t              *image;
};


double repeat(double f)
{
	if(f < 0.0)
	{
		while(f < 0.0) f += 1.0;
		return f;
	}
	else
	{
		while(f > 1.0) f -= 1.0;
		return f;
	}
}
void texture_sample(texture_t *tex, double u, double v, double colour_out[3])
{
	uint32_t texel;
	if(tex == NULL)
	{
		/* Nice pink colour :) */
		colour_out[0] = 1.0;
		colour_out[1] = 0.412;
		colour_out[2] = 0.706;
	}
	else
	{
		double u_wrap = 0.0;
		double v_wrap = 0.0;
		switch(tex->repeat)
		{
			case texture_repeat_repeat:
				u_wrap = repeat(u);
				v_wrap = repeat(v);
				break;
			case texture_repeat_clip:
				u_wrap = min(1, max(0, u));
				v_wrap = min(1, max(0, v));
				break;
			default:
				ERROR("Invalid Texture.\n");
				break;
		}
		int x = u_wrap * (tex->image->width  - 1);
		int y = v_wrap * (tex->image->height - 1);
		Assert(x >= 0 && x < tex->image->width,  "Invalid x");
		Assert(y >= 0 && y < tex->image->height, "Invalid y");
		tga_get_pixel(tex->image, x, y,(char*) &texel);
		//The colours are stored in the int as bgr
		colour_out[0] = (float)((uint8_t*)&texel)[2] / 255;
		colour_out[1] = (float)((uint8_t*)&texel)[1] / 255;
		colour_out[2] = (float)((uint8_t*)&texel)[0] / 255;
	}
}

texture_t *texture_read(const char *filename)
{
	texture_t *out = malloc(sizeof(texture_t));
	out->mode      = texture_mode_linear;
	out->repeat    = texture_repeat_repeat;
	out->image     = tga_read(filename);
	return out;
}

void texture_delete(texture_t *texture)
{
	tga_delete(texture->image);
	free(texture);
}
