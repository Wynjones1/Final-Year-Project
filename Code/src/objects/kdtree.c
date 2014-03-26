#include "kdtree.h"
#include "vector.h"
#include "intersection.h"
#include "utils.h"
#include "maths.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#define KDTREE_DEBUG    0

static const int    TREE_MAX_DEPTH = 20;
static const int    TREE_MIN_SIZE  = 3;
static const double TREE_MIN_WIDTH = 0.001;

static void build(kdtree_node_t *tree, box_t box, mesh_t *mesh);
static kdtree_node_t *new_node();

kdtree_t *kdtree_build(mesh_t *mesh)
{
	kdtree_t *out = malloc(sizeof(kdtree_t));
	box_t box;

	//Setup the bounds to send down the tree.
	mesh_get_bounds(mesh, out->min, out->max);
	memcpy(box.min, out->min, sizeof(double) * 3);
	memcpy(box.max, out->max, sizeof(double) * 3);
	//Adjust the max bounds so tris that lie on a plane are included.
	for(int i = 0; i < 3; i++)
	{
		box.max[i] += EPSILON;
		box.min[i] -= EPSILON;
	}

	out->first.indices = list(int);
	out->first.depth   = 0;
	//Add all of the triangles to the top level.
	for(int i = 0;i < mesh->triangle_count; i++)
	{
		list_push(out->first.indices, &i);
	}
#if DEBUG
	out->first.id   = 0;
	out->first.leaf = true;
#endif
	build(&out->first, box, mesh);
	return out;
}

static kdtree_node_t *new_node()
{
	kdtree_node_t *out = malloc(2 * sizeof(kdtree_node_t));	
	return out;
}

/*
*	Calculates if the triangle crosses the bounds.
*/
static int triangle_crosses(mesh_t *mesh, int triangle, int axis, double min, double max)
{
	double *vertices[3];
	mesh_get_triangle(mesh, triangle, vertices);
	double tmin = INFINITY;
	double tmax = -INFINITY;
	for(int vertex = 0; vertex < 3; vertex++)
	{
		double comp = vertices[vertex][axis];
		if(comp < tmin) tmin = comp;
		if(comp > tmax) tmax = comp;
	}
	return (min <= tmax) && (tmin < max);
}

static double calculate_split(double *min, double *max, int axis,
								list_t *indices, mesh_t *mesh)
{
	double axis_min = min[axis];
	double axis_max = max[axis];
	double mid = (axis_max + axis_min) * 0.5;
	return mid;
}

/*
*	Build the next level of the tree.
*/
static void build(kdtree_node_t *tree, box_t box, mesh_t *mesh)
{
#if DEBUG
	memcpy(tree->min, box.min, sizeof(tree->min));
	memcpy(tree->max, box.max, sizeof(tree->max));
#endif
	if(tree->depth < TREE_MAX_DEPTH && list_size(tree->indices) >= TREE_MIN_SIZE)
	{
		int axis = tree->depth % 3;
		if(box.max[axis] - box.min[axis] < TREE_MIN_WIDTH) return;

		tree->axis  = axis;
		tree->split = calculate_split(box.min, box.max, tree->axis, 
										tree->indices, mesh);
		tree->children             = new_node();
		tree->children[0].depth    = tree->depth + 1;
		tree->children[1].depth    = tree->depth + 1;
#if DEBUG
		tree->children[0].id     = tree->id;
		tree->children[1].id     = tree->id | (1 << (tree->depth + 1));
		tree->children[0].parent = tree;
		tree->children[1].parent = tree;
		tree->children[0].leaf   = true;
		tree->children[1].leaf   = true;
#endif
		tree->children[0].indices  = list(int);
		tree->children[1].indices  = list(int);
		tree->children[0].children = NULL;
		tree->children[1].children = NULL;

		//set up the bounds for the children.
		box_t left  = box;
		box_t right = box;
		left.max[tree->axis]  = tree->split;
		right.min[tree->axis] = tree->split;

		int i;
		LIST_LOOP(tree->indices, i)
		{
			int triangle;
			list_get(tree->indices, i, &triangle);
			bool left_test, right_test;
			left_test  = triangle_crosses(mesh, triangle, tree->axis, left.min[tree->axis], left.max[tree->axis]);
			right_test = triangle_crosses(mesh, triangle, tree->axis, right.min[tree->axis], right.max[tree->axis]);
			if(left_test)
			{
				assert(tree->children != NULL);
				list_push(tree->children[0].indices, &triangle);
			}
			if(right_test)
			{
				assert(tree->children + 1 != NULL);
				list_push(tree->children[1].indices, &triangle);
			}
			assert(left_test || right_test);
			assert(tree->children != NULL && tree->children + 1 != NULL);
			if(list_size(tree->children[0].indices) == list_size(tree->indices) || list_size(tree->children[1].indices) == list_size(tree->indices))
			{
				list_free(tree->children[0].indices);
				list_free(tree->children[1].indices);
				free(tree->children);
				tree->children = NULL;
			}
		}
		if(tree->children)
		{
#if DEBUG
			tree->leaf = false;
#endif
			list_free(tree->indices);
			tree->indices = NULL;

			build(tree->children    , left,  mesh);
			build(tree->children + 1, right, mesh);
		}
	}
}


void kdtree_node_delete(kdtree_node_t *node)
{
#if 0
	if(!node->leaf){}
#else
	if(node->children)
	{
		kdtree_node_delete(node->children);
		kdtree_node_delete(node->children + 1);
		free(node->children);
	}
	else
	{
		list_free(node->indices);
	}
#endif
}
void kdtree_delete(kdtree_t *tree)
{
	kdtree_node_delete(&tree->first);
	free(tree);
}

#if DEBUG
kdtree_node_t *get_node(kdtree_node_t *node, uint64_t id, int depth)
{
	uint64_t mask = (1 << (depth + 1)) - 1;
	uint64_t id_masked = id & mask;
	if(node->id == id_masked  && node->depth == depth)
	{
		return node;
	}
	if(node->depth != depth)
	{
		if( (id >> (node->depth + 1)) & 0x1)
		{
			return get_node(node->children + 1, id, depth);
		}
		else
		{
			return get_node(node->children, id, depth);
		}
	}
	return NULL;
}

kdtree_node_t *kdtree_get_node(kdtree_t *tree, uint64_t id, int depth)
{
	return get_node(&tree->first, id, depth);
}

bool kdtree_contains_index(kdtree_node_t *tree, int index)
{
	int size = list_size(tree->indices);
	for(int i = 0; i < size; i++)
	{
		int temp;
		list_get(tree->indices, i, &temp);
		if(index == temp) return true;
	}
	return false;
}

#endif

