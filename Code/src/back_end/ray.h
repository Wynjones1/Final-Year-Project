#ifndef RAY_H
#define RAY_H
#include <stdbool.h>

struct ray
{
	double origin[3];
	double normal[3];
	int    depth;
};

typedef struct ray    ray_t;
typedef struct camera camera_t;

void ray_generate(camera_t *camera, double x, double y, ray_t *ray_out);
ray_t ray_copy(ray_t *ray);

#endif
