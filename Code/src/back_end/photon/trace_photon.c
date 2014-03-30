#include "trace_photon.h"
#include "sphere.h"
#include "sampler.h"
#include "utils.h"
#include <string.h>

static int trace_reflected(scene_t *scene, intersection_t *info, ray_t *ray, object_t *o, int light,
						   double power[3], bool diffuse, bool specular_only, queue_t *output)
{
	double normal[3];
	CALL(o, get_normal, info, normal);
	ray_t reflected_ray;
	reflected_ray.depth = ray->depth - 1;
	maths_calculate_intersection(ray, info->t, reflected_ray.origin, -1);
	maths_calculate_reflected_ray(ray->normal, normal, reflected_ray.normal);
	//Scale the power of the photon.
	double temp[3];
	material_t *mat = &o->material;
	temp[0] = power[0] * (mat->reflectivity[0] / mat->av_refl);
	temp[1] = power[1] * (mat->reflectivity[1] / mat->av_refl);
	temp[2] = power[2] * (mat->reflectivity[2] / mat->av_refl);
	return trace_photon(scene, &reflected_ray, light, temp, true, diffuse, specular_only, output);
}

static int trace_refracted(scene_t *scene, intersection_t *info, ray_t *ray, object_t *o, int light,
						   double power[3], bool diffuse, bool specular_only, queue_t *output)
{
	double normal[3];
	CALL(o, get_normal, info, normal);
	ray_t refracted_ray;
	refracted_ray.depth = ray->depth - 1;
	maths_calculate_intersection(ray, info->t, refracted_ray.origin, 1);
	if(maths_calculate_refracted_ray(ray->normal, normal, 1.0, g_config.time, refracted_ray.normal))
	{
		//Scale the power of the photon.
		double temp[3];
		material_t *mat = &o->material;
		temp[0] = power[0] * (mat->refractivity[0] / mat->av_refr);
		temp[1] = power[1] * (mat->refractivity[1] / mat->av_refr);
		temp[2] = power[2] * (mat->refractivity[2] / mat->av_refr);
		return trace_photon(scene, &refracted_ray, light, temp, true, diffuse, specular_only, output);
	}
	else
	{
		return trace_reflected(scene, info, ray, o, light, power, diffuse, specular_only, output);
	}
}

static void store_photon(intersection_t *info, ray_t *ray, int light, double power[3], bool specular, bool diffuse, queue_t *output)
{
	struct thread_output_data output_data;
	maths_calculate_intersection(ray, info->t, output_data.photon.origin, 0);

	vector_copy(ray->normal, output_data.photon.incident);
	vector_copy(power      , output_data.photon.power);

	output_data.specular      = specular;
	output_data.diffuse       = diffuse;
	output_data.last          = false;
	output_data.photon.light  = light;
	output_data.volume        = false;
	queue_write(output, &output_data);
}

static int trace_diffuse(scene_t *scene, intersection_t *info, ray_t *ray, object_t *o, int light,
						   double power[3], bool specular, bool diffuse, bool specular_only, queue_t *output)
{
	int ret = 0;
	double col[3];
	double normal[3];
	object_calculate_texture_colour(o, info, col);
	double pref = (col[0] + col[1] + col[2]) / 3.0;

	//Store the photon.
	if(!specular_only || specular)
	{
		store_photon(info, ray, light, power, specular, diffuse, output);
		ret += 1;
	}

	CALL(o, get_normal, info, normal);
	ray_t diffuse_ray;
	maths_calculate_intersection(ray, info->t, diffuse_ray.origin, -1);
	sample_hemi_cosine(normal, diffuse_ray.normal);
	diffuse_ray.depth = ray->depth - 1;
	double temp_col[3];
	memcpy(temp_col, power, sizeof(double) * 3);
	temp_col[0] *= col[0] / pref;
	temp_col[1] *= col[1] / pref;
	temp_col[2] *= col[2] / pref;
	ret += trace_photon(scene, &diffuse_ray, light, temp_col, specular, true, specular_only, output);

	return ret;
}

static int absorb_photon(scene_t *scene, intersection_t *info, ray_t *ray, object_t *o, int light,
						   double power[3], bool specular, bool diffuse, bool specular_only, queue_t *output)
{
	if(!specular_only || specular)
	{
		store_photon(info, ray, light, power, specular, diffuse, output);
		return 1;
	}
	return 0;
}

static void phase_function(double dir[3], double out[3])
{
	double temp[3];
	memcpy(temp, dir, sizeof(double) * 3);
	sample_sphere(out);
}

static void store_photon_volume(ray_t *ray, int light, double power[0], queue_t *output)
{
	struct thread_output_data output_data;
	vector_copy(ray->normal, output_data.photon.incident);
	vector_copy(ray->origin, output_data.photon.origin);
	vector_copy(power      , output_data.photon.power);

	output_data.specular      = true;
	output_data.diffuse       = true;
	output_data.volume        = true;
	output_data.last          = false;
	output_data.photon.light  = light;
	queue_write(output, &output_data);
}

//TODO: make general and move to object files.
int point_in_object(object_t *o, double x[3])
{
	return 0;
	sphere_t *s = (sphere_t *)o;
	double v[3];
	v[0] = x[0] - s->origin[0];
	v[1] = x[1] - s->origin[1];
	v[2] = x[2] - s->origin[2];
	return vector_length(v) < s->r;
}

double next_dist(double extinction)
{
	return -log(randf(0.0, 1.0)) / extinction;
}

/* Called when a photon interacts with a participating media */
static int trace_pmedia(scene_t *scene, intersection_t *info, ray_t *ray, object_t *o, int light,
						   double power[3], bool specular, bool diffuse, bool specular_only, queue_t *output)
{
	ray_t new;
	memcpy(new.normal, ray->normal, sizeof(double) * 3);
	new.depth = ray->depth - 1;
	maths_calculate_intersection(ray, info->t, new.origin, 0);

	//We assume the point is in the pmedia at this point.
	material_t *m = &o->material;
	double albedo = m->av_albedo;
	//Store the photon in the map.
	store_photon_volume(&new, light, power, output);

	//BREAKPOINT(fabs(new.origin[0]) > 0.5 || fabs(new.origin[1]) > 0.5 || fabs(new.origin[2] > 0.5));
	//Decide if we absorb or scatter.
	double eps = randf(0.0, 1.0);
	if(eps <= albedo)
	{
		//Create a new direction according the the phase function.
		double new_power[3];
		vector_copy(power, new_power);
		phase_function(new.normal, new.normal);
		//Continue the ray on its path.
		return 1 + trace_photon(scene, &new, light, power, specular, true, specular_only, output);
	}
	else
	{
		return 1;
	}
	return 0;
}

int trace_photon(scene_t *scene, ray_t *ray, int light, double power[3], bool specular,
				 bool diffuse, bool specular_only, queue_t *output)
{
	int ret = 0;
	if(ray->depth)
	{
		intersection_t info;
		if(intersection_photon_scene(ray,scene, &info))
		{
			object_t *o = info.scene.object;
			material_t *mat = &o->material;

			double col[3];
			object_calculate_texture_colour(o, &info, col);
			double pref = (col[0] + col[1] + col[2]) / 3.0;

			if(mat->pmedia)
			{
				ret += trace_pmedia(scene, &info, ray, o, light, power, specular, diffuse, specular_only, output);
			}
			else
			{
				double eps = randf(0.0, 1.0);
				if(eps < mat->av_refl)
				{
					ret += trace_reflected(scene, &info, ray, o, light, power, diffuse, specular_only, output);
				}
				else if(eps < (mat->av_refl + mat->av_refr))
				{
					ret += trace_refracted(scene, &info, ray, o, light, power, diffuse, specular_only, output);
				}
				else if(eps < (mat->av_refl + mat->av_refr + pref))
				{
					ret += trace_diffuse(scene, &info, ray, o, light, power, specular, diffuse, specular_only, output);
				}
				else
				{
					ret += absorb_photon(scene, &info, ray, o, light, power, specular, diffuse, specular_only, output);
				}
			}
		}
	}
	return ret;
}
