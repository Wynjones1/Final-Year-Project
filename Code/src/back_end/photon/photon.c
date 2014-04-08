#include "photon.h"
#include "list.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "ray.h"
#include "intersection.h"
#include "utils.h"
#include "median.h"
#include <time.h>
#include "vector.h"
#include "maths.h"
#include "sampler.h"
#include "queue.h"
#include "trace_photon.h"
#include <pthread.h>


int photon_map_size(photon_map_t *map)
{
	return map->num;
}

void scale_photons(list_t *photons, int *emitted)
{
	photon_t *p = list_data(photons);
	int end = list_size(photons);
	for(int i = 0; i < end; i++)
	{
		p[i].power[0] /= emitted[p[i].light];
		p[i].power[1] /= emitted[p[i].light];
		p[i].power[2] /= emitted[p[i].light];
	}
}

struct pmap_data
{
	list_t *global;
	list_t *caustic;
	list_t *volume;
};


struct thread_input_data
{
	int              num_photons;
	bool             caustics;
	scene_t         *scene;
	int             *emitted_counts;
	pthread_mutex_t *count_mtx;
};


queue_t *output_queue;

static int shoot_photon(scene_t *scene, int light, double power[3], bool caustics, queue_t *output)
{
	ray_t ray;
	ray.depth = 0;
	light_t **l = list_data(scene->lights);
	light_generate_ray(l[light], &ray);
	return trace_photon(scene, &ray, light, power, false, false, caustics, output);

}
void *gen_photon_thread(void *_data)
{
	struct thread_input_data *data = _data;
	int      num_lights = list_size(data->scene->lights);
	light_t **lights    = list_data(data->scene->lights);
	double power_tot    = 0.0;

	int emitted_counts[num_lights];
	memset(emitted_counts, 0x00, sizeof(int) * num_lights);

	for(int i = 0; i < num_lights; i++)
	{
		power_tot += lights[i]->av_power;
	}

	for(int li = 0; li < num_lights; li++)
	{
		light_t *l = lights[li];
		double power[3];
		double prop = l->av_power / power_tot;
		vector_copy(l->power, power);
		int needed = data->num_photons * prop;
		int found  = 0;
		int shot   = 0;
		while(found < needed && !(found == 0 && shot == 1024))
		{
			found   += shoot_photon(data->scene, li, power, data->caustics, output_queue);
			emitted_counts[li]++;
			shot++;
		}
	}

	//Update the per light emittance count
	pthread_mutex_lock(data->count_mtx);
	for(int i = 0; i < num_lights; i++)
	{
		data->emitted_counts[i] += emitted_counts[i];
	}
	pthread_mutex_unlock(data->count_mtx);

	//Tell the main thread that we are done.
	struct thread_output_data output_data = {.last = true};
	queue_write(output_queue, &output_data);
	return NULL;
}

static void gen_photon_map(scene_t *scene, int num_to_gen, bool caustic, struct pmap_data *out, int *emitted_counts)
{
	pthread_t       threads[g_config.num_threads];
	pthread_mutex_t emit_count_mtx = PTHREAD_MUTEX_INITIALIZER;

	struct thread_input_data input;

	input.num_photons    = max(1, num_to_gen / g_config.num_threads);
	input.scene          = scene;
	input.caustics       = caustic;
	input.count_mtx      = &emit_count_mtx;
	input.emitted_counts = emitted_counts;
	output_queue         = queue_init(10000, sizeof(struct thread_output_data));

	for(int i = 0; i < g_config.num_threads; i++)
	{
		pthread_create(threads + i, NULL, gen_photon_thread, &input);
	}

	int finished_count = 0;
	struct thread_output_data output;
	int count = 0;
	while(finished_count != g_config.num_threads)
	{
		queue_read(output_queue, &output);
		if(count % 100 == 0)
		{
			progress_bar(count, g_config.photons, 40);
		}
		count++;
		if(output.last)
		{
			finished_count++;
		}
		else
		{
			if(output.volume)
			{
				list_push(out->volume, &output.photon);
			}
			else if(caustic)
			{
				list_push(out->caustic, &output.photon);
			}
			else
			{
				if(output.diffuse)
					list_push(out->global, &output.photon);
			}
		}
	}

	for(int i = 0; i < g_config.num_threads; i++)
	{
		pthread_join(threads[i], NULL);
	}
	queue_delete(output_queue);
}

struct pmap_data gen_photons(scene_t *scene, int num_photons)
{

	struct pmap_data out;
	out.global  = list(photon_t);
	out.caustic = list(photon_t);
	out.volume  = list(photon_t);

	int num_lights = list_size(scene->lights);
	int emitted_count[num_lights];

	VERBOSE("Generating Global Photon Map.\n");
	memset(emitted_count, 0x00, sizeof(int) * num_lights);
	gen_photon_map(scene, g_config.photons, false, &out, emitted_count);
	scale_photons(out.global , emitted_count);
	scale_photons(out.volume, emitted_count);

#if CAUSTICS
	VERBOSE("Generating Caustic Photon Map.\n");
	memset(emitted_count, 0x00, sizeof(int) * num_lights);
	gen_photon_map(scene, g_config.photons, true,  &out, emitted_count);
	scale_photons(out.caustic, emitted_count);
#endif


	return out;
}

void photon_map_build(scene_t *scene, int num_photons)
{
	//Create a list of photons for the tree.
	struct pmap_data photons = gen_photons(scene, num_photons);
	VERBOSE("Balancing Global Photon Map\n");
	scene->global            = photon_map_balance(photons.global);
	VERBOSE("Balancing Caustic Photon Map\n");
	scene->caustic           = photon_map_balance(photons.caustic);
	VERBOSE("Balancing Volume Photon Map\n");
	scene->volume            = photon_map_balance(photons.volume);
}

void photon_delete(photon_t *photon)
{
	free(photon);
}

void photon_map_delete(photon_map_t *map)
{
	free(map->photons);
	free(map);
}

/*
* Debug functions.
*/
void photon_map_dump(photon_map_t *map, const char *filename, FILE *fp)
{
	if(fp == NULL)
	{
		fp = OPEN(filename, "w");
		FOUTPUT(fp, "%d\n", map->num);

	}
	for(int i = 0; i < map->num; i++)
	{
		double *o = map->photons[i].origin;
		double *n = map->photons[i].power;
		FOUTPUT(fp, "%f %f %f %f %f %f\n", o[0], o[1], o[2], n[0], n[1], n[2]);
	}
}

static void verify(photon_t *pi, int index)
{
	photon_t *p = pi + index;
	if(p->has_left)
	{
		photon_t *c = pi + 2 * index + 1;
		verify(pi, 2 * index + 1);
		if(c->origin[p->axis] > p->origin[p->axis])
		{
			ERROR("Could not verify left child: %d\n", index);
		}
	}
	if(p->has_right)
	{
		photon_t *c = pi + 2 * index + 2;
		verify(pi, 2 * index + 2);
		if(c->origin[p->axis] < p->origin[p->axis])
		{
			ERROR("Could not verify right child: %d\n", index);
		}
	}
}

void photon_map_verify(photon_map_t *map)
{
	verify(map->photons, 0);
}
