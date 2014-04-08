#ifndef SCENE_H
#define SCENE_H

#include "list.h"
#include "light.h"
#include "mesh.h"
#include "camera.h"
#include "bmp.h"
#include "kdtree.h"
#include "config.h"

#define SKY_DEFAULT_SKY_R 0.1328
#define SKY_DEFAULT_SKY_G 0.1328
#define SKY_DEFAULT_SKY_B 1.0

typedef struct photon_map photon_map_t;

struct scene
{
	list_t       *objects;
	list_t       *lights;
	list_t       *pixel_outputs;
	camera_t     *camera;
	photon_map_t *global;
	photon_map_t *caustic;
	photon_map_t *volume;
	double        sky[3];
};

typedef void (*pixel_update_func)(void *data, int x, int y, uint32_t rgb);
typedef struct scene scene_t;

void     scene_print(scene_t *scene);
void     scene_add_object(scene_t *scene, object_t *obj);
scene_t *scene_read(const char *filename);
void     scene_delete(scene_t *scene);
void     scene_register_pixel_update(scene_t *scene, pixel_update_func func, void *data);
void     scene_update_pixel(scene_t *scene, int x, int y, uint32_t data);

#endif
