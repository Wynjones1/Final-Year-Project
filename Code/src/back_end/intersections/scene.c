#include "scene.h"
#include "intersection.h"
#include <string.h>

int intersection_ray_scene(ray_t *ray, scene_t *scene, intersection_t *info)
{
	object_t **objects= (object_t**) list_data(scene->objects);
	intersection_t temp;
	int num_objects = list_size(scene->objects);
	int retval = 0;
	info->t = INFINITY;

	for(int i = 0; i < num_objects; i++)
	{
		if(CALL(objects[i], intersection, ray, &temp) && temp.t < info->t)
		{
			*info = temp;
			info->scene.object = objects[i];
			retval             = 1;	
		}
	}

	memset(info->scene.colour, 0x00, sizeof(double) * 3);
	if(retval) //Calculate the output colour.
	{
		object_t *object =  info->scene.object;
		material_t *mat  = &object->material;
		if(mat->pmedia)
		{
			object_calculate_pmedia_colour(object, scene, ray, info, info->scene.colour);
		}
		else
		{
			double eps = randf(0.0, 1.0);
			if(eps < mat->av_diff)
			{
				object_calculate_diffuse_colour(object, scene, ray, info, info->scene.colour);
			}
			else if(eps < mat->av_diff + mat->av_refl)
			{
				object_calculate_reflected_colour(object, scene, ray, info, info->scene.colour);
			}
			else if(eps < mat->av_diff + mat->av_refl + mat->av_refr)
			{
				object_calculate_refracted_colour(object, scene, ray, info, info->scene.colour);
			}
		}
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
		if(CALL(objects[i], intersection, ray, &temp) && temp.t < info->t)
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
		if(CALL(objects[i], intersection, ray, &temp) && temp.t < info->t)
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
		if(CALL(objects[i], intersection, ray, &temp) && temp.t < t)
			return 1;
	}
	return 0;
}
