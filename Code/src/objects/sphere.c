#include "sphere.h"
#include <stdlib.h>
#include "intersection.h"
#include "vector.h"
#include <math.h>
#include "utils.h"
#include <string.h>

static int intersection(sphere_t *object, ray_t *ray, intersection_t *info)
{
	double origin[3];
	info->type = intersection_type_sphere;

	double a = vector_dot(ray->normal, ray->normal);

	//Move the sphere to the center of the world.
	vector_sub(ray->origin, object->origin, origin);

	double b = 2 * vector_dot(origin, ray->normal);
	double c = vector_dot(origin, origin) - (object->r * object->r);

	double disc = b * b - 4 * a * c;
	if(disc < 0.0) return 0;

	double sq = sqrt(disc);
	double t0 = (- b - sq) / 2;
	double t1 = (- b + sq) / 2;

	if(t0 > t1)
	{
		double temp = t1;
		t1 = t0;
		t0 = temp;
	}

	if(t1 < 0) return false;

	if(t0 < 0)
	{
		info->t = t1;
	}
	else
	{
		info->t = t0;
	}

	vector_mult(info->t, ray->normal, info->point);
	vector_add(ray->origin, info->point, info->point);
	return true;

}

static void normal(sphere_t *object, intersection_t *info, double *normal)
{
	vector_sub(info->point, object->origin, normal);
	vector_normal(normal, normal);
}

static void tex(sphere_t *object, intersection_t *info, double *tex)
{
	tex[0] = 0.0;
	tex[1] = 1.0;
}

static void delete(sphere_t *object)
{
	if(object->material.texture)
	{
		texture_delete(object->material.texture);
	}
	free(object);
}

static double next_dist(double extinction)
{
	return -log(randf(0.0, 1.0)) / extinction;
}

static int pmedia_intersection(sphere_t *s, ray_t *r, intersection_t *info)
{
	//Are we inside the participating media?
	if(vector_distance(r->origin, s->origin) < s->r)
	{
		info->t = next_dist(s->material.av_ext);
		return 1;
	}
	else
	{
		return intersection(s, r, info);
	}
}

static void pmedia_direct(object_t *object, double x[3], scene_t *scene, intersection_t *info)
{
	int num_lights   = list_size(scene->lights);
	light_t **lights = list_data(scene->lights);
	for(int i = 0; i < num_lights; i++)
	{
		double l = vector_distance(x, lights[i]->origin);
		//TODO:Cae where we exit the medium
		double atten = exp(-l * object->material.av_ext);
		info->scene.colour[0] += object->material.v_ext * atten * lights[i]->power[0]/ (4 * PI * l * l);
		info->scene.colour[1] += object->material.v_ext * atten * lights[i]->power[1]/ (4 * PI * l * l);
		info->scene.colour[2] += object->material.v_ext * atten * lights[i]->power[2]/ (4 * PI * l * l);
	}
}

static void pmedia_shade(object_t *object, scene_t *scene, intersection_t *info)
{
	//Step through the participating media.
	ray_t new_ray;
	new_ray.depth = info->incident.depth - 1;
	memcpy(new_ray.normal, info->incident.normal, sizeof(double) * 3);
	maths_calculate_intersection(&info->incident, info->t, new_ray.origin, 1);
	intersection_t temp;
	//Calculate the radiance from the light sources.
	pmedia_direct(object, new_ray.origin, scene, info);
	//Calculate the radiance along the path.
	if(intersection_ray_scene(&new_ray, scene, &temp))
	{
		double attenuation = exp(- info->t * object->material.av_ext);
		info->scene.colour[0] += attenuation * temp.scene.colour[0];
		info->scene.colour[1] += attenuation * temp.scene.colour[1];
		info->scene.colour[2] += attenuation * temp.scene.colour[2];
	}
}

sphere_t *sphere_init(const char *str)
{
	char mat_buf[1024];
	sphere_t *out = malloc(sizeof(sphere_t));
	object_init((object_t*)out);
	if(sscanf(str, "sphere %lf %lf %lf %lf %s",
			&out->r, out->origin, out->origin + 1, out->origin + 2, mat_buf) != 5)
	{
		ERROR("Invalid sphere init string.\n");
	}
	out->intersection = (intersection_func)intersection;
	out->get_normal   = (normal_func)normal;
	out->get_tex      = (tex_func)tex;
	out->delete       = (delete_func)delete;
	material_init(&out->material, mat_buf);
	if(out->material.pmedia)
	{
		out->shade        = (shade_func) pmedia_shade;
		out->intersection = (intersection_func) pmedia_intersection;
	}
	return out;
}
