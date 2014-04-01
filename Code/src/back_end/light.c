#include "light.h"
#include <stdlib.h>
#include "intersection.h"
#include "utils.h"
#include "maths.h"
#include "vector.h"
#include "sampler.h"
#include <string.h>
static const int NUM_SAMPLES = 16;

#define JITTER 0

static double point_is_blocked(light_t *l, scene_t *scene, double point[3])
{
	NotImplemented();
	return 0.0;
}

static double area_is_blocked(light_t *l, scene_t *scene, double point[3])
{
	NotImplemented();
	return 0.0;
}

double light_is_blocked(light_t *l, scene_t *scene, double point[3])
{
	if(g_config.shadows)
	{
		switch(l->type)
		{
			case light_type_point:
				return point_is_blocked(l, scene, point);
			case light_type_area:
				return area_is_blocked(l, scene, point);
			default:
				ERROR("Invalid Light Type.\n");
		}
	}
	else
	{
		return 1.0;
	}
}

void light_delete(light_t *light)
{
	free(light);
}

void point_generate_ray(light_t *l, ray_t *out)
{
	vector_copy(l->origin, out->origin);
	sample_sphere(out->normal);
}

void area_generate_ray(light_t *l, ray_t *out)
{
	double x = randf(-l->width/2,l->width/2);
	double y = randf(-l->height/2,l->height/2);
	out->origin[0] = l->origin[0] + x;
	out->origin[1] = l->origin[1];
	out->origin[2] = l->origin[2] + y;

	sample_hemi_cosine(l->normal, out->normal);
}

void light_generate_ray(light_t *l, ray_t *out)
{
	switch(l->type)
	{
		case light_type_point:
			point_generate_ray(l, out);
			break;
		case light_type_area:
			point_generate_ray(l, out);
			break;
		case light_type_invalid:
			ERROR("Invalid Light Type");
			break;
	}
	memcpy(out->origin, l->origin, sizeof(double) * 3);
}

light_t *light_new(const char *input)
{
	light_t *out = malloc(sizeof(light_t));
	out->type    = light_type_invalid;
	out->power[0] = 1.0;
	out->power[1] = 1.0;
	out->power[2] = 1.0;

	if(sscanf(input, "light point %lf %lf %lf %lf %lf %lf",
						out->origin, out->origin + 1, out->origin + 2,
						out->power,  out->power  + 1, out->power  + 2) == 6)
	{
		out->type = light_type_point;
	}
	else if(sscanf(input, "light area %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
					out->origin, out->origin + 1, out->origin + 2,
					out->normal, out->normal + 1, out->normal + 2,
					out->power, out->power + 1, out->power + 2,
					&out->width,  &out->height))
	{
		out->type = light_type_area;
	}
	double *p = out->power;
	out->av_power = (p[0] + p[1] + p[2]) / 3.0;
	return out;
}

static void area_calculate_sample_radiance(light_t *l, scene_t *scene, double point[3], double normal[3],double x, double y, double radiance[3])
{
	double to_point[3];
	double sample_point[3] = {l->origin[0] + x, l->origin[1], l->origin[2] + y};
	vector_sub(point, sample_point, to_point);
	double t = vector_length(to_point);
	vector_normal(to_point, to_point);

	double dot = vector_dot(l->normal, to_point);
	double inv_t2 = 1.0 / (t * t);
	double inv_samples2 = 1.0 / ((double)NUM_SAMPLES * NUM_SAMPLES);
	double pdot = -vector_dot(to_point, normal);
	pdot = max(0, pdot);

	//Create shadow ray
	ray_t shadow_ray;
	memcpy(shadow_ray.origin, sample_point, sizeof(double) * 3);
	memcpy(shadow_ray.normal, to_point, sizeof(double) * 3);

	if(dot > 0.0 && !intersection_ray_scene_before(&shadow_ray, scene, t))
	{
		radiance[0] = l->power[0] * pdot * dot * inv_t2 * inv_samples2;
		radiance[1] = l->power[1] * pdot * dot * inv_t2 * inv_samples2;
		radiance[2] = l->power[2] * pdot * dot * inv_t2 * inv_samples2;
	}
	else
	{
		radiance[0] = 0.0;
		radiance[1] = 0.0;
		radiance[2] = 0.0;
	}
}

static void area_calculate_radiance(light_t *l, scene_t *scene, double point[3], double normal[3],double radiance[3])
{
	double sample_radiance[3];
	for(int i = 0; i < NUM_SAMPLES; i++)
	{
		for(int j = 0; j < NUM_SAMPLES; j++)
		{
			double jit_x = randf(0.0, 1.0) - 0.5;
			double jit_y = randf(0.0, 1.0) - 0.5;
			double x = (2.0 * (i + jit_x)) / NUM_SAMPLES - 1.0;
			double y = (2.0 * (j + jit_y)) / NUM_SAMPLES - 1.0;
			x *= l->width / 2.0;
			y *= l->height / 2.0;
			area_calculate_sample_radiance(l, scene, point, normal, x, y, sample_radiance);
			vector_add(radiance, sample_radiance, radiance);
		}
	}
}

static void point_calculate_radiance(light_t *l, scene_t *scene, double point[3], double normal[3], double radiance[3])
{
	double to_point[3];
	vector_sub(point, l->origin, to_point);
	double t = vector_length(to_point);
	vector_normal(to_point, to_point);

	double inv_t2 = 1.0 / (4 * PI * t * t);

	//Create shadow ray
	ray_t shadow_ray;
	memcpy(shadow_ray.origin, l->origin, sizeof(double) * 3);
	memcpy(shadow_ray.normal, to_point, sizeof(double) * 3);

	double dot = -vector_dot(normal, to_point);
	dot = max(0, min(1.0, dot));

	if(!g_config.shadows || !intersection_ray_scene_before(&shadow_ray, scene, t))
	{
		radiance[0] = l->power[0] * dot * inv_t2;
		radiance[1] = l->power[1] * dot * inv_t2;
		radiance[2] = l->power[2] * dot * inv_t2;
	}
	else
	{
		radiance[0] = 0.0;
		radiance[1] = 0.0;
		radiance[2] = 0.0;
	}
}

void light_calculate_radiance(light_t *l, scene_t *scene, double point[3], double normal[3],double radiance[3])
{
	switch(l->type)
	{
		case light_type_point:
			point_calculate_radiance(l, scene, point, normal,radiance);
			break;
		case light_type_area:
			area_calculate_radiance(l, scene, point, normal, radiance);
			break;
		case light_type_invalid:
			ERROR("Invalid light type");
			break;
	}
}
