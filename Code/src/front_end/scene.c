#include "intersection.h"
#include "vector.h"
#include "texture.h"
#include "list.h"
#include "light.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include "utils.h"
#include "photon.h"
#include "sphere.h"

#define BUF_SIZE (1024)

struct pixel_update_data
{
	pixel_update_func func;
	void             *data;
};

void scene_print(scene_t *scene)
{
	int i;
	LIST_LOOP(scene->objects, i)
	{
		object_t *object;
		list_get(scene->objects, i, &object);
		CALL(object, print);
	}
}

scene_t *scene_read(const char *filename)
{
	scene_t *out       = malloc(sizeof(scene_t));	
	out->objects       = list(object_t *);
	out->lights        = list(light_t*);
	out->camera        = camera_default();
	out->pixel_outputs = list(struct pixel_update_data);

	FILE *fp = OPEN(filename, "r");
	char buf[BUF_SIZE];

	while(fgets(buf, BUF_SIZE, fp))
	{
		if(sscanf(buf, "mesh %s", buf))
		{
			mesh_t   *temp = mesh_read(buf);
			list_push(out->objects, &temp);
		}
		else if(BUFCMP(buf, "light") == 0)
		{
			light_t *temp = light_new(buf);
			list_push(out->lights, &temp);
		}
		else if(BUFCMP(buf, "camera") == 0)
		{
			camera_t *camera = out->camera;
			sscanf(buf, "camera %lf %lf %lf %lf %lf %lf %lf %lf %lf", 
					camera->origin,  camera->origin  + 1, camera->origin  + 2,
					camera->forward, camera->forward + 1, camera->forward + 2,
					camera->up,      camera->up      + 1, camera->up      + 2);
		}
		else if(BUFCMP(buf, "sphere") == 0)
		{
			sphere_t *sphere = sphere_init(buf);
			list_push(out->objects, &sphere);
		}
	}

	fclose(fp);
	VERBOSE("Reading scene: %s\n", filename);
	VERBOSE("Object count:  %d\n", list_size(out->objects));
	VERBOSE("Light count:   %d\n", list_size(out->lights));
	return out;
}

void scene_delete(scene_t *scene)
{
	camera_delete(scene->camera);
	for(int i = 0; i < list_size(scene->objects); i++)
	{
		object_t *temp;
		list_get(scene->objects, i, &temp);
		CALL(temp, delete);
	}
	for(int i = 0; i < list_size(scene->lights); i++)
	{
		light_t *temp;
		list_get(scene->lights, i, &temp);
		light_delete(temp);
	}
	list_free(scene->objects);
	list_free(scene->lights);
	list_free(scene->pixel_outputs);
#if PMAP
	photon_map_delete(scene->global);
	photon_map_delete(scene->caustic);
#endif
	free(scene);
}

void     scene_add_object(scene_t *scene, object_t *obj)
{
	list_push(scene->objects, &obj);
}

void scene_register_pixel_update(scene_t *scene, pixel_update_func func, void *data)
{
	struct pixel_update_data update = {.func = func, .data = data};
	list_push(scene->pixel_outputs, &update);
}

void scene_update_pixel(scene_t *scene, int x, int y, uint32_t data)
{
	struct pixel_update_data *to_update = list_data(scene->pixel_outputs);
	int num = list_size(scene->pixel_outputs);
	for(int i = 0; i < num; i++)
	{
		to_update[i].func(to_update[i].data, x, y, data);
	}
}
