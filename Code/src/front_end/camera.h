#ifndef CAMERA_H
#define CAMERA_H
#include "matrix.h"

struct camera
{
	double   origin[3];
	double   up[3];
	double   forward[3];
	double   right[3];
	double   f;
	double   aspect;
	matrix_t transform;
};

typedef struct camera camera_t;
typedef struct ray    ray_t;

camera_t *camera_init(double *origin, double *forward, 
				double *up, double f, double aspect);

void      camera_free(camera_t *camera);

void camera_create_initial_ray( camera_t *camera, int x, int y, int subx, int suby, ray_t *ray);

camera_t *camera_default(void);
void camera_delete(camera_t *camera);
#endif
