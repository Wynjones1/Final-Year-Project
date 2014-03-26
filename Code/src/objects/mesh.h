#ifndef MESH_H
#define MESH_H
#include "object.h"
#include "matrix.h"

#define MESH_VERTEX(mesh,triangle,vertex)\
		(&mesh->vertices[mesh->triangles[triangle * 3 + vertex] * 3])
#define MESH_TEX(mesh, triangle, vertex)\
		(&mesh->tex[mesh->triangles[triangle * 3 + vertex] * 2])
#define MESH_NORMAL(mesh, triangle, vertex)\
		(&mesh->normals[mesh->triangles[triangle * 3 + vertex] * 3])

//Forward declarations.
typedef struct kdtree       kdtree_t;
typedef struct mesh         mesh_t;
typedef struct intersection intersection_t;
typedef struct texture      texture_t;
typedef struct scene        scene_t;
typedef struct ray          ray_t;

struct mesh
{
	OBJECT_DATA()
	int         triangle_count;
	int         vertex_count;
	int        *triangles;
	double     *vertices;
	double     *normals;
	double     *tex;
	kdtree_t   *tree;
	matrix_t    transform;
};

void mesh_print(mesh_t *mesh);
void mesh_get_triangle(mesh_t *mesh, int i, double **triangle);
void mesh_get_triangle_indices(mesh_t *mesh, int i, int *triangle);
void mesh_split_triangle(mesh_t *mesh, int tri, double split, int axis);
void mesh_delete(mesh_t *mesh);

mesh_t *mesh_read(const char *filename);
void mesh_get_bounds(mesh_t *mesh, double *min, double *max);
void mesh_calculate_surface_normal(mesh_t *mesh, int tri, double u, double v, double *out);
void mesh_calculate_texcoords(mesh_t *mesh,int triangle,double u,double v,double coords[2]);

void mesh_calculate_texture_colour(mesh_t *mesh, int triangle, double u, double v, double colour[3]);
void mesh_calculate_diffuse_colour(mesh_t *mesh, scene_t *scene, ray_t *ray, intersection_t *info, double *colour_out);
void mesh_calculate_reflective_colour(mesh_t *mesh, scene_t *scene, ray_t *ray, intersection_t *info, double *colour_out);
void mesh_calculate_refractive_colour(mesh_t *mesh, scene_t *scene, ray_t *ray, intersection_t *info, double *colour_out);
void mesh_calculate_ambient_colour(mesh_t  *mesh, scene_t *scene, ray_t *ray, intersection_t *info, double *colour_out);
#endif
