#include "ray.h"
#include "intersection.h"
#include "vector.h"

int intersection_ray_tri(
		ray_t *ray, double **triangle,
		intersection_t *info)
{
	info->type = intersection_type_triangle;
	double *v0 = triangle[0];
	double *v1 = triangle[1];
	double *v2 = triangle[2];

	double e1[] = {0,0,0};
	double e2[] = {0,0,0};
	double t[]  = {0,0,0};
	double p[]  = {0,0,0};
	double q[]  = {0,0,0};

	vector_sub(v1,v0,e1);
	vector_sub(v2,v0,e2);
	vector_sub(ray->origin,v0,t);
	vector_cross(ray->normal,e2,p);
	vector_cross(t,e1,q);

	double det = vector_dot(p,e1);
	if(det > -EPSILON && det < EPSILON) return 0;
	double inv_det = 1.0 / det;
	info->t = inv_det * vector_dot(q,e2);

	if(info->t < EPSILON)
		return 0;

	info->triangle.u = vector_dot(p,t) * inv_det;
	info->triangle.v = vector_dot(q,ray->normal) * inv_det;

	if(info->triangle.u < -EPSILON || info->triangle.u > (1.0 + EPSILON)) return 0;
	if(info->triangle.v < -EPSILON || info->triangle.v > (1.0 + EPSILON)) return 0;
	if(info->triangle.v + info->triangle.u > (1.0 + EPSILON))             return 0;

	if(inv_det < 0.0) return -1;
	return 1;
}
