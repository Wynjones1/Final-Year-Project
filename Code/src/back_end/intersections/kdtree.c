#include "intersection.h"

#define traverse_near()\
{\
	if(intersection_ray_aabb_axis(ray,near_box.min, near_box.max,tree->axis, &info) &&\
		traverse(near, mesh, ray, near_box, info_out))\
	{\
		return 1;\
	}\
}

#define traverse_far()\
{\
	if(intersection_ray_aabb_axis(ray, far_box.min, far_box.max, tree->axis, &info) &&\
	traverse(far, mesh, ray, far_box, info_out))\
	{\
		return 1;\
	}\
}

int contains_point(double *min, double *max, double *point)
{
	for(int i = 0; i < 3; i++)
	{
		if((min[i] - EPSILON) > point[i]) return 0;
		if((max[i] + EPSILON) < point[i]) return 0;
	}
	return 1;
}

//ktrav
int traverse(kdtree_node_t *tree, mesh_t *mesh,ray_t *ray, box_t box, intersection_t *info_out)
{
	intersection_t info;
#if DEBUG
	if(tree->leaf)
#else
	if(tree->indices)
#endif
	{
		int retval = 0;
		int i;
		info_out->t = INFINITY;
		LIST_LOOP(tree->indices, i)
		{
			int tri_index;
			list_get(tree->indices, i, &tri_index);
			double *tri[3];
			mesh_get_triangle(mesh, tri_index, tri);
			if(intersection_ray_tri(ray, tri, &info))
			{
				double temp[3];
				for(int i = 0; i < 3; i++)temp[i] = ray->origin[i] + ray->normal[i] * info.t;
				if( contains_point(box.min, box.max, temp) && info.t < info_out->t)
				{
					retval = 1;
					info.mesh.triangle = tri_index;
					*info_out = info;
				}
			}
		}
		return retval;
	}
	else if(intersection_ray_aabb(ray, box.min, box.max, &info))
	{
		kdtree_node_t *near = NULL, *far = NULL;
		int axis = tree->axis;
		double tsplit  = maths_calculate_t(ray->origin[axis], ray->normal[axis], tree->split);
		double tmin  = info.aabb.tmin;
		double tmax  = info.aabb.tmax;
		box_t near_box = box;
		box_t far_box  = box;

		//Calculate the near and far child node from our ray.
		if(ray->origin[tree->axis] < tree->split)
		{
			near = tree->children;	
			far  = tree->children + 1;
			near_box.max[tree->axis] = tree->split;
			far_box.min[tree->axis]  = tree->split;
		}
		else
		{
			near  = tree->children + 1;
			far   = tree->children;	
			near_box.min[tree->axis] = tree->split;
			far_box.max[tree->axis]  = tree->split;
		}

#if 0
		traverse_near();
		traverse_far();
		return 0;
#endif

		//Calculate which path (near or far) to take based on the origin, split position and normal.
		if(tsplit == 0)
		{
			if(ray->normal[tree->axis] < 0)
				traverse_far()
			else
				traverse_near()
		}
		else if(tsplit >= tmax)
			traverse_near()
		else if(tsplit < tmin)
		{
			if(tsplit > 0)
				traverse_far()
			else
				traverse_near()
		}
		else if(tmin <= tsplit && tsplit < tmax)
		{
			if(tsplit > 0)
			{
				traverse_near()
				traverse_far()
			}
			else
				traverse_near()
		}
		else if(ray->normal[tree->axis] == 0.0)
		{
			traverse_near();
		}
	}
	return 0;
}
