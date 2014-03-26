#include "intersection.h"
#include "object.h"

int intersection_ray_object(ray_t *ray, object_t *object, intersection_t *info)
{
	return CALL(object, intersection, ray, info);
}
