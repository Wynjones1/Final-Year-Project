#ifndef CAMERA_H
#define CAMERA_H
#include "matrix.h"
#include "vector.h"

struct camera
{
	vector_t origin;
	vector_t up;
	vector_t forward;
	vector_t right;
	double   f;
	double   aspect;
	matrix_t transform;
};

typedef struct camera camera_t;
typedef struct ray    ray_t;

camera_t *camera_init(vector_t origin, vector_t forward, 
				vector_t up, double f, double aspect);

void      camera_free(camera_t *camera);

void camera_create_initial_ray( camera_t *camera, int x, int y, int subx, int suby, ray_t *ray);

camera_t *camera_default(void);
void camera_delete(camera_t *camera);
#endif
