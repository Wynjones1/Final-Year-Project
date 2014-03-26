#include "ray.h"
#include <string.h>

void ray_generate(camera_t *camera, double x, double y, ray_t *ray_out)
{}

ray_t ray_copy(ray_t *ray)
{
	ray_t out;
	memcpy(&out, ray, sizeof(ray_t));
	return out;
}
