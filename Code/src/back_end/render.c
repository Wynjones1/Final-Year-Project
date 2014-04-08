#include "render.h"
#include "scene.h"
#include "intersection.h"
#include "maths.h"
#include "config.h"
#include "utils.h"
#include "queue.h"
#include "photon.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

struct thread_input_data
{
	int  x;
	int  y;
	bool last;
};

struct thread_output_data
{
	int      x;
	int      y;
	uint32_t rgb;
};

static queue_t *g_input_queue;
static queue_t *g_output_queue;
static scene_t        *scene;

static void process_initial_ray(scene_t *scene, ray_t *ray, double *colour_out)
{
	intersection_t info;
	ray->depth   = 0;
	if(intersection_ray_scene(ray, scene, &info))
	{
		colour_out[0] += info.scene.colour[0];
		colour_out[1] += info.scene.colour[1];
		colour_out[2] += info.scene.colour[2];
	}
	else
	{
		vector_copy(scene->sky, colour_out);
	}
}

void *renderer(void *data)
{
	struct thread_input_data  input;
	ray_t ray;

	queue_read(g_input_queue, &input);
	while(!input.last)
	{
		double colour[3] = {0, 0, 0};
		int n = g_config.samples;
		for(int i = 0; i < n; i++)
		{
			for(int j = 0; j < n; j++)
			{
				camera_create_initial_ray(scene->camera, input.x, input.y, i, j, &ray);
				process_initial_ray(scene, &ray, colour);
			}
		}
		vector_div(n * n, colour, colour);
		uint32_t pixdata = convert_double_rgb_to_24bit(colour);

		struct thread_output_data out = {.x = input.x, .y = input.y, .rgb = pixdata};
		queue_write(g_output_queue, &out);
		queue_read(g_input_queue, &input);
	}
	return NULL;
}

void start_threads(pthread_t *threads)
{
	for(int i = 0; i < g_config.num_threads; i++)
	{
		pthread_create(threads + i, NULL, renderer, NULL);
	}
}

void *feed_pixels(void *_data)
{
	struct thread_input_data data;

	for(int i = 0; i < g_config.height; i++)
	{
		for(int j = 0; j < g_config.width; j++)
		{
			data.x    = j;
			data.y    = i;
			data.last = false;
			queue_write(g_input_queue, &data);
		}
	}

	//Tell the threads that we are done.
	for(int i = 0; i < g_config.num_threads; i++)
	{
		data.last = true;
		queue_write(g_input_queue, &data);
	}

	return NULL;
}

pthread_t start_feed_thread(void)
{
	pthread_t thread;
	pthread_create(&thread, NULL, feed_pixels, NULL);
	return thread;
}

void trace_pixel(void)
{
	double colour_out[] = {0,0,0};
	ray_t ray;
	camera_create_initial_ray(scene->camera, g_config.trace_pixel_x, g_config.trace_pixel_y, 0, 0, &ray);
	process_initial_ray(scene, &ray, colour_out);
	vector_print(colour_out);
}

void render(scene_t *scene_in)
{
	int width  = g_config.width;
	int height = g_config.height;
	scene  = scene_in;
	int start_time   = time(NULL);

#if PMAP
	photon_map_build(scene, g_config.photons);
#endif

	OUTPUT("Rendering.\n");

	if(g_config.trace_pixel)
	{
		trace_pixel();
	}
	else
	{
		pthread_t *threads = calloc(sizeof(pthread_t) * g_config.num_threads, 1);
		g_input_queue      = queue_init(1000, sizeof(struct thread_input_data));
		g_output_queue     = queue_init(1000, sizeof(struct thread_output_data));

		start_threads(threads);
		pthread_t feed_thread = start_feed_thread();
		struct thread_output_data output;
		for(int i = 0; i < height; i++)
		{
			progress_bar(i, height, 40);
			for(int j = 0; j < width; j++)
			{
				queue_read(g_output_queue, &output);
				scene_update_pixel(scene, output.x, output.y, output.rgb);
			}
		}

		progress_bar(1, 1, 40);
		OUTPUT("Render took %d seconds.\n", time(0) - start_time);

		//Wait for the threads to finish.
		pthread_join(feed_thread, NULL);
		for(int i = 0; i < g_config.num_threads; i++)
		{
			pthread_join(threads[i], NULL);
		}

		free(threads);
		queue_delete(g_input_queue);
		queue_delete(g_output_queue);
	}
}

