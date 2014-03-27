#include "camera.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "vector.h"
#include "ray.h"
#include "bmp.h"
#include "config.h"


camera_t *camera_init(vector_t origin, vector_t forward, vector_t up, double f, double aspect)
{
	camera_t *out = malloc(sizeof(camera_t));

	vector_copy(origin,  out->origin);
	vector_copy(forward, out->forward);
	vector_copy(up     , out->up);

	vector_cross(up, forward, out->right);

	out->f = f;
	return out;
}

void camera_create_initial_ray( camera_t *camera, int x, int y, int subx, int suby, ray_t *ray)
{
	//Map the pixel to the range -1 to 1
	double dx, dy;
	int n = g_config.samples;
	dx = (2.0 * (x + (subx + 0.5) / n)) / g_config.width  - 1.0;
	dy = (2.0 * (y + (suby + 0.5) / n)) / g_config.height - 1.0;
	ray->normal[0] =  dx;
	ray->normal[1] =  dy;
	ray->normal[2] = -1.5;
	ray->origin[0] =  0.0;
	ray->origin[1] =  0.0;
	ray->origin[2] =  2.0;
	vector_normal(ray->normal, ray->normal);
}

camera_t *camera_default(void)
{
	double origin[]  = {0.0, 0.0, -1.0};
	double forward[] = {0.0, 0.0,  1.0};
	double up[]      = {0.0, 1.0,  0.0};
	return camera_init(origin, forward, up, 1.0, 1.0);
}

void camera_delete(camera_t *camera)
{
	free(camera);
}
