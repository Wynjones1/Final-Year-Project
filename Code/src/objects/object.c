#include "object.h"
#include "ray.h"
#include "intersection.h"
#include "vector.h"
#include "utils.h"
#include "light.h"
#include "photon.h"
#include <math.h>
#include "sampler.h"
#include "maths.h"
#include <string.h>

void object_calculate_reflected_colour( object_t *object, scene_t *scene, ray_t *ray,
										intersection_t *info, double colour_out[3])
{
	if(ray->depth)
	{
		double surface_normal[3];
		intersection_t temp;
		ray_t new_ray;
		new_ray.depth = ray->depth - 1;
		maths_calculate_intersection(ray, info->t, new_ray.origin, -1);
		CALL(object, get_normal, info, surface_normal);
		maths_calculate_reflected_ray(ray->normal, surface_normal, new_ray.normal);

		if(intersection_ray_scene(&new_ray, scene, &temp))
		{
			memcpy(colour_out, temp.scene.colour, sizeof(double) * 3);
		}
		else
		{
			memset(colour_out, 0x00, sizeof(double) * 3);
		}
	}
}


void object_calculate_refracted_colour( object_t *object, scene_t *scene, ray_t *ray,
										intersection_t *info, double colour_out[3])
{
	double refr_col[3] = {0.0, 0.0, 0.0};
	double refl_col[3] = {0.0, 0.0, 0.0};
	double normal[3];
	ray_t  refracted_ray;
	double r = 1.0;//reflectance.
	double *incident = ray->normal;
	if(ray->depth)
	{
		refracted_ray.depth = ray->depth - 1;
		CALL(object, get_normal, info, normal);
		if(maths_calculate_refracted_ray(incident, normal, 1.0, g_config.time, refracted_ray.normal))
		{
			r = 0.0;
			intersection_t refraction_info;
			for(int i = 0; i < 3; i++)refracted_ray.origin[i] = ray->origin[i] + ray->normal[i] * (info->t + EPSILON);
			if(intersection_ray_scene(&refracted_ray, scene, &refraction_info))
			{
				vector_copy(refraction_info.scene.colour, refr_col);
			}
		}
		else
		{
			object_calculate_reflected_colour(object, scene, ray, info, refl_col);
		}

		for(int i = 0; i < 3; i++)
		{
			colour_out[i] = (1.0 - r) * refr_col[i] + r * refl_col[i];
		}
	}
}

void object_calculate_texture_colour( object_t *object, intersection_t *info, double colour_out[3])
{
	if(object->material.texture)
	{
		double tex[2];
		CALL(object, get_tex, info, tex);
		texture_sample(object->material.texture, tex[0], tex[1], colour_out);
	}
	else
	{
		memcpy(colour_out, object->material.diffuse, sizeof(double) * 3);
	}
}

void calculate_diffuse_direct( object_t *object, scene_t *scene, ray_t *ray,
									  intersection_t *info, double colour_out[3])
{
	double x[3];
	double n[3];

	CALL(object, get_normal, info, n);

	light_t **lights = list_data(scene->lights);
	int num_lights   = list_size(scene->lights);

	maths_calculate_intersection(ray, info->t, x, -1);

	double radiance[3];
	memset(radiance, 0x00, sizeof(double) * 3);
	for(int i = 0; i < num_lights; i++)
	{
		light_calculate_radiance(lights[i], scene, x, n, radiance);
		vector_add(colour_out, radiance, colour_out);
	}
}

void calculate_diffuse_indirect ( object_t *object, scene_t *scene, ray_t *ray,
									  intersection_t *info, double colour_out[3])
{
#if PMAP
	//Simulate the first diffuse bounce of the light to estimate the radiance at the point.
	double normal[3];
	double x[3];
	CALL(object, get_normal, info, normal);

	ray_t diffuse_ray;
	//TODO:Use the sample count;
	maths_calculate_intersection(ray, info->t, diffuse_ray.origin, -1);
#if FAST_DIFFUSE
	double inten[3] = {0, 0, 0};
	photon_map_estimate_radiance(scene->global, x, normal, inten);
	vector_add(colour_out, inten, colour_out);
#else
	double sample_col[3] = {0, 0, 0};
	int num_samples = 100;

	for(int i = 0; i < num_samples; i++)
	{
		double inten[] = {0, 0, 0};
		diffuse_ray.depth = ray->depth;
		//Sample the BRDF (TODO: Add general BRDF computation)
		sample_hemi(normal, diffuse_ray.normal);

		intersection_t temp;
		if(intersection_photon_scene(&diffuse_ray, scene, &temp))
		{
			maths_calculate_intersection(&diffuse_ray, temp.t, x, -1);
			object_t *new_obj = temp.scene.object;
			CALL(new_obj, get_normal, &temp, normal);
			photon_map_estimate_radiance(scene->global, x, normal, inten);

			sample_col[0] += inten[0];
			sample_col[1] += inten[1];
			sample_col[2] += inten[2];
		}
	}

	colour_out[0] += sample_col[0] / num_samples;
	colour_out[1] += sample_col[1] / num_samples;
	colour_out[2] += sample_col[2] / num_samples;
#endif

#endif
}


void calculate_diffuse_caustic( object_t *object, scene_t *scene, ray_t *ray,
									  intersection_t *info, double colour_out[3])
{
#if PMAP
	double inten[] = {0, 0, 0};
	double normal[3];
	double x[3];
	CALL(object, get_normal, info, normal);
	maths_calculate_intersection(ray, info->t, x, 0);

	//We are directly sampling the radiance due to caustics.
	photon_map_estimate_radiance(scene->caustic, x, normal, inten);

	colour_out[0] += inten[0];
	colour_out[1] += inten[1];
	colour_out[2] += inten[2];
#endif
}

static void pmedia_calculate_direct_illumination(object_t *o, scene_t *scene, ray_t *ray, double t, double out[3])
{
	double dx = 0.1;
	for(double t0 = 0.0; t0 < t; t0 += dx)
	{
	}
}

void object_calculate_pmedia_colour( object_t *o, scene_t *scene, ray_t *ray,
									  intersection_t *info, double colour_out[3])
{
	double x0[3];
	maths_calculate_intersection(ray, info->t, x0, 1);
	ray_t fog_ray;
	vector_copy(x0, fog_ray.origin);
	vector_copy(ray->normal, fog_ray.normal);
	intersection_t temp;
	if(intersection_photon_scene(&fog_ray, scene, &temp))
	{
		double x1[3];
		maths_calculate_intersection(&fog_ray, temp.t, x1, 1);
		double d = vector_distance(x0, x1);
		double red = exp(-o->material.av_ext * d);

		double direct_illumination[3] = {0.0, 0.0, 0.0};
		pmedia_calculate_direct_illumination(o, scene, &fog_ray, temp.t, direct_illumination);

		ray_t exit_ray;
		vector_copy(x1, exit_ray.origin);
		vector_copy(fog_ray.normal, exit_ray.normal);
		if(intersection_ray_scene(&exit_ray, scene, &temp))
		{
			colour_out[0] = red * temp.scene.colour[0] + direct_illumination[0];
			colour_out[1] = red * temp.scene.colour[1] + direct_illumination[1];
			colour_out[2] = red * temp.scene.colour[2] + direct_illumination[2];
		}
		else
		{
			intersection_ray_scene(&fog_ray, scene, &temp);
			vector_copy(temp.scene.colour, colour_out);
		}
	}
	else //Trace the ray normaly.
	{
		intersection_ray_scene(&fog_ray, scene, &temp);
		vector_copy(temp.scene.colour, colour_out);
	}
}

void object_calculate_diffuse_colour( object_t *object, scene_t *scene, ray_t *ray,
									  intersection_t *info, double colour_out[3])
{
	double tex[3];
	memset(colour_out, 0x00, sizeof(double) * 3);
	object_calculate_texture_colour(object, info, tex);

	calculate_diffuse_direct(object,   scene, ray, info, colour_out);
//	calculate_diffuse_indirect(object, scene, ray, info, colour_out);
//	calculate_diffuse_caustic(object,  scene, ray, info, colour_out);

	colour_out[0] *= tex[0];
	colour_out[1] *= tex[1];
	colour_out[2] *= tex[2];
}

