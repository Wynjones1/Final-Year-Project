#include "ray.h"
#include "intersection.h"
#include "vector.h"
#include <string.h>

int traverse(kdtree_node_t *tree, mesh_t *mesh,ray_t *ray, box_t box, intersection_t *info_out);

int intersection_ray_mesh( ray_t *ray, mesh_t *mesh, intersection_t *info)
{
#if BRUTEFORCE
	intersection_t tri_info;
	info->t = INFINITY;
	info->type = intersection_type_mesh;
#if DEBUG
	info->mesh.id = 0;
#endif
	double *triangle[3];
	int retval = 0;
	for(int i = 0; i < mesh->triangle_count; i++)
	{
		//Set up the to test.
		triangle[0] = MESH_VERTEX(mesh, i, 0);
		triangle[1] = MESH_VERTEX(mesh, i, 1);
		triangle[2] = MESH_VERTEX(mesh, i, 2);
		if(intersection_ray_tri(ray, triangle, &tri_info))
		{
			if(tri_info.t < info->t)
			{
				info->triangle.u  = tri_info.triangle.u;
				info->triangle.v  = tri_info.triangle.v;
				info->t  = tri_info.t;
				info->mesh.triangle = i;
				memcpy(info->point, trie_info.point, sizeof(double) * 3);
			}
			retval = 1;
		}
	}
	return retval;
#else
	intersection_t info_temp;
	box_t box;
	memcpy(box.min, mesh->tree->min, sizeof(double) * 3);
	memcpy(box.max, mesh->tree->max, sizeof(double) * 3);
	info_temp.t = INFINITY;
	if(intersection_ray_aabb(ray, mesh->tree->min, mesh->tree->max, &info_temp))
	{
		if(traverse(&mesh->tree->first, mesh, ray, box, &info_temp))
		{
			*info = info_temp;
			return 1;
		}
	}
	return 0;
#endif
}


int intersection_ray_mesh_bf(
				ray_t *ray,
				mesh_t *mesh,
				intersection_t *info)
{
	intersection_t tri_info;
	info->t = INFINITY;
	info->type = intersection_type_mesh;
#if DEBUG
	info->mesh.id = 0;
#endif
	double *triangle[3];
	int retval = 0;
	for(int i = 0; i < mesh->triangle_count; i++)
	{
		//Set up the to test.
		triangle[0] = MESH_VERTEX(mesh, i, 0);
		triangle[1] = MESH_VERTEX(mesh, i, 1);
		triangle[2] = MESH_VERTEX(mesh, i, 2);
		if(intersection_ray_tri(ray, triangle, &tri_info))
		{
			if(tri_info.t < info->t)
			{
				info->triangle.u = tri_info.triangle.u;
				info->triangle.v = tri_info.triangle.v;
				info->t = tri_info.t;
				info->mesh.triangle = i;
			}
			retval = 1;
		}
	}
	return retval;
}

