#include "camera.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "utils.h"
#include "vector.h"
#include "ray.h"
#include "bmp.h"
#include "config.h"


camera_t *camera_init(double eye[3], double at[3], double up[3], double f)
{
	double x[3];
	double y[3];
	double z[3];

	//Calculate the z axis
	vector_sub(at, eye, z);
	vector_normal(z, z);
	//Calculate the x axis
	vector_cross(z, up, x);
	vector_normal(x, x);
	//Calculate the y axis
	vector_cross(x, z, y);

	camera_t *out = malloc(sizeof(camera_t));
	vector_copy(eye, out->origin);
	vector_copy(x, out->x);
	vector_copy(y, out->y);
	vector_copy(z, out->z);

	out->f = f;
	return out;
}

camera_t *camera_init_string(const char *string)
{
	double e[3];
	double l[3];
	double u[3];
	double f;
	if(sscanf(string, "camera %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
						e, e + 1, e + 2,
						l, l + 1, l + 2,
						u, u + 1, u + 2, &f) != 10)
	{
		ERROR("Ill formed camera input.\n");
	}
	return camera_init(e, l, u, f);
}

void camera_create_initial_ray( camera_t *camera, int x, int y, int subx, int suby, ray_t *ray)
{
	//Map the pixel to the range -1 to 1
	double dx, dy;
	int n = g_config.samples;
	dx = (2.0 * (x + (subx + 0.5) / n)) / g_config.width  - 1.0;
	dy = (2.0 * (y + (suby + 0.5) / n)) / g_config.height - 1.0;
	for(int i = 0; i < 3; i++)
	{
		ray->normal[i] =  camera->x[i] * dx +
						  camera->y[i] * dy +
						  camera->z[i] * camera->f;
	}
	vector_normal(ray->normal, ray->normal);
	ray->origin[0] =  camera->origin[0];
	ray->origin[1] =  camera->origin[1];
	ray->origin[2] =  camera->origin[2];
}

camera_t *camera_default(void)
{
	double origin[]  = {0.0, 0.0, -1.0};
	double forward[] = {0.0, 0.0,  1.0};
	double up[]      = {0.0, 1.0,  0.0};
	return camera_init(origin, forward, up, 1.0);
}

void camera_delete(camera_t *camera)
{
	free(camera);
}
