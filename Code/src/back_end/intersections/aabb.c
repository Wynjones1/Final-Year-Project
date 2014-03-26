#include "ray.h"
#include "intersection.h"
#include "utils.h"

int intersection_ray_aabb_axis(ray_t *ray, double *min, double *max, int axis,intersection_t *info)
{
	double t1,t2;
	info->type = intersection_type_aabb;
	info->aabb.tmax = INFINITY;
	info->aabb.tmin = -INFINITY;
	t1 = (min[axis] - ray->origin[axis]) / ray->normal[axis];
	t2 = (max[axis] - ray->origin[axis]) / ray->normal[axis];
	if(t1 > t2) dswap(&t1,&t2);
	if(t1 > info->aabb.tmin) info->aabb.tmin = t1;
	if(t2 < info->aabb.tmax) info->aabb.tmax = t2;
	if(info->aabb.tmin > info->aabb.tmax || info->aabb.tmax < EPSILON) return 0;
	return 1;
}

int intersection_ray_aabb(ray_t *ray,
						  double *min,   double *max,
						 intersection_t *info)
{
#if 1
	info->aabb.tmax = INFINITY;
	info->aabb.tmin = -INFINITY;
	double t1,t2;
	int i;
	info->type = intersection_type_aabb;
	for(i = 0;i < 3;i++)
	{
		t1 = (min[i] - ray->origin[i]) / ray->normal[i];
		t2 = (max[i] - ray->origin[i]) / ray->normal[i];
		if(t1 > t2) dswap(&t1,&t2);
		if(t1 > info->aabb.tmin) info->aabb.tmin = t1;
		if(t2 < info->aabb.tmax) info->aabb.tmax = t2;
		if(info->aabb.tmin > info->aabb.tmax || info->aabb.tmax < EPSILON) return 0;
	}
	return 1;
#else
	double tmin, tmax, tymin, tymax, tzmin, tzmax;
	double divx = 1 / ray_normal[0];
	double divy = 1 / ray_normal[1];
	double divz = 1 / ray_normal[2];
	if(divx >= 0)
	{
		tmin = (min[0] - ray_origin[0]) * divx;
		tmax = (max[0] - ray_origin[0]) * divx;
	}
	else
	{
		tmin = (max[0] - ray_origin[0]) * divx;
		tmax = (min[0] - ray_origin[0]) * divx;
	}
	if(divy >= 0)
	{
		tymin = (min[1] - ray_origin[1]) * divy;
		tymax = (max[1] - ray_origin[1]) * divy;
	}
	else
	{
		tymin = (max[1] - ray_origin[1]) * divy;
		tymax = (min[1] - ray_origin[1]) * divy;
	}
	if((tmin > tymax) || (tymin > tmax)) return 0;
	if(tymin > tmin)
		tmin = tymin;
	if(tymax < tmax)
		tmax = tymax;
	if(divz >= 0)
	{
		tzmin = (min[2] - ray_origin[2]) * divz;
		tzmax = (max[2] - ray_origin[2]) * divz;
	}
	else
	{
		tzmin = (max[2] - ray_origin[2]) * divz;
		tzmax = (min[2] - ray_origin[2]) * divz;
	}
	if((tmin > tzmax) || (tzmin > tmax)) return 0;
	if(tzmin > tmin) tmin = tzmin;
	if(tzmin < tmax) tmax = tzmax;
	double t1 = 1000.0;
	double t0 = 0.0;
	return (tmin < t1) && (tmax > t0);
#endif
}
