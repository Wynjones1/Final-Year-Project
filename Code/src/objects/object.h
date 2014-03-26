#ifndef OBJECT_H
#define OBJECT_H
#include "material.h"

typedef struct aabb         aabb_t;
typedef struct object       object_t;
typedef struct intersection intersection_t;
typedef struct ray          ray_t;
typedef struct scene        scene_t;

/* Function pointer defines needed by all objects. */
typedef int  (*intersection_func)(object_t *o, ray_t *ray, intersection_t *info);
typedef void (*bounds_func)(object_t *o, aabb_t *bounds);
typedef void (*normal_func)(object_t *o, intersection_t *info, double *normal);
typedef void (*tex_func)(object_t *o, intersection_t *info, double *tex);
typedef void (*print_func)(object_t *o);
typedef void (*delete_func)(object_t *o);
typedef void (*shade_func)( object_t *object, scene_t *scene, intersection_t *info, double colour_out[3]);

#define CALL(object, func, ...) object->func(object, ##__VA_ARGS__)

#define OBJECT_DATA()\
	intersection_func intersection;\
	bounds_func       get_bounds;\
	normal_func       get_normal;\
	tex_func          get_tex;\
	shade_func        shade;\
	print_func        print;\
	delete_func       delete;\
	material_t        material;

struct object
{
	OBJECT_DATA()
};

void object_calculate_reflected_colour( object_t *o, scene_t *s, ray_t *ray,
										intersection_t *info, double colour_out[3]);

void object_calculate_refracted_colour( object_t *o, scene_t *s, ray_t *ray,
										intersection_t *info, double colour_out[3]);

void object_calculate_diffuse_colour( object_t *o, scene_t *s, ray_t *ray,
									  intersection_t *info, double colour_out[3]);

void object_calculate_pmedia_colour( object_t *o, scene_t *s, ray_t *ray,
									  intersection_t *info, double colour_out[3]);

void object_calculate_texture_colour( object_t *o, intersection_t *info, double colour_out[3]);

#endif
