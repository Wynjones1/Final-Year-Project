#include "scene.h"
#include "intersection.h"
#include <string.h>

int intersection_ray_scene(ray_t *ray, scene_t *scene, intersection_t *info)
{
	object_t **objects= (object_t**) list_data(scene->objects);
	intersection_t temp;
	int num_objects = list_size(scene->objects);
	int retval      = 0;
	info->t         = INFINITY;

	for(int i = 0; i < num_objects; i++)
	{
		temp.pmedia = false;
		if(CALL(objects[i], intersection, ray, &temp)
			&& temp.t < info->t
			&& (temp.t < -EPSILON || temp.t > EPSILON))
		{
			*info = temp;
			info->scene.object = objects[i];
			retval             = 1;	
		}
	}

	info->incident  = *ray;
	memset(info->scene.colour, 0x00, sizeof(double) * 3);
	if(retval) //Calculate the output colour.
	{
		object_t *object =  info->scene.object;
		for(int i = 0; i < 3; i++)
		{
			info->point[i] = ray->origin[i] + ray->normal[i] * info->t;
		}
		CALL(object, get_normal, info, info->normal);
		CALL(object, shade, scene, info);
	}
	return retval;
}

int intersection_photon_scene(ray_t *ray, scene_t *scene, intersection_t *info)
{
	object_t   **objects = (object_t**) list_data(scene->objects);
	intersection_t temp;
	int num_objects= list_size(scene->objects);
	int retval = 0;
	info->t = INFINITY;
	for(int i = 0; i < num_objects; i++)
	{
		if(CALL(objects[i], intersection, ray, &temp) && temp.t < info->t && (temp.t < -EPSILON || temp.t > EPSILON))
		{
			*info = temp;
			info->scene.object = objects[i];
			retval             = 1;	
		}
	}
	return retval;
}

int intersection_ray_scene_bf(ray_t *ray, scene_t *scene, int depth, intersection_t *info)
{
	object_t **objects= (object_t**)  list_data(scene->objects);
	intersection_t temp;
	int num_objects = list_size(scene->objects);
	int retval = 0;
	info->t = INFINITY;

	for(int i = 0; i < num_objects; i++)
	{
		if(CALL(objects[i], intersection, ray, &temp) && temp.t < info->t && (temp.t < -EPSILON || temp.t > EPSILON))
		{
			*info = temp;
			info->scene.object= objects[i];
			retval = 1;	
		}
	}
	return retval;
}

/*
*	Returns if there is any object in the scene that intersects before t
*/
int intersection_ray_scene_before(ray_t *ray, scene_t *scene, double t)
{
	object_t **objects= (object_t**) list_data(scene->objects);
	intersection_t temp;
	int num_objects= list_size(scene->objects);
	for(int i = 0; i < num_objects; i++)
	{
		if(CALL(objects[i], intersection, ray, &temp) && (temp.t + EPSILON) < t && (temp.t < -EPSILON || temp.t > EPSILON))
			return 1;
	}
	return 0;
}
