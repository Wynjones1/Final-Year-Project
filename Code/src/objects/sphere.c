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


static int pmedia_intersection(ray_t *r, sphere_t *s, intersection_t *info)
{
	double temp[3];
	info->t = 0.01;
	return 1;
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
		out->shade        = object_calculate_pmedia_colour;
//		out->intersection = (intersection_func) pmedia_intersection;
	}
	return out;
}
