#ifndef INTERSECTION_H
#define INTERSECTION_H
#include "mesh.h"
#include "kdtree.h"
#include "scene.h"
#include "ray.h"

static const int intersection_max_depth = 0;
typedef struct intersection intersection_t;

enum intersection_type
{
	intersection_type_triangle,
	intersection_type_aabb,
	intersection_type_mesh,
	intersection_type_sphere,
	intersection_type_scene,
	intersection_type_count
};

struct intersection
{
	enum intersection_type type;
	double t;
	double point[3];//The point of the intersection.
	double normal[3];
	ray_t  incident;
	bool   pmedia;
	struct
	{
		struct
		{
			double u;
			double v;
		}triangle;

		struct
		{
			double tmax;
			double tmin;
		}aabb;

		struct
		{
			int triangle;
#if DEBUG
			int id;
			int depth;
#endif
		}mesh;

		struct
		{
//			int    object;
			object_t *object;
			double    colour[3];
		}scene;
	};
};

int intersection_ray_object(
				ray_t *ray,
				object_t *object,
				intersection_t *info);

int intersection_ray_tri(
				ray_t *ray,
				double **triangle,
				intersection_t *info);

int intersection_ray_aabb(
				ray_t *ray,
				double *box_min,
				double *box_max,
				intersection_t *info);

int intersection_ray_aabb_axis(
				ray_t *ray,
				double *min,
				double *max,
				int axis,
				intersection_t *info);

int intersection_ray_mesh(
				ray_t *ray,
				mesh_t *mesh,
				intersection_t *info);

int intersection_ray_mesh_bf(
				ray_t *ray,
				mesh_t *mesh,
				intersection_t *info);

int intersection_ray_scene(
				ray_t *ray,
				scene_t *scene, 
				intersection_t *info);

int intersection_photon_scene(
				ray_t *ray,
				scene_t *scene,
				intersection_t *info);

int intersection_ray_scene_before(
				ray_t *ray,
				scene_t *scene,
				double t);

int intersection_ray_scene_bf(
				ray_t *ray,
				scene_t *scene, 
				int depth,
				intersection_t *info);

#endif
