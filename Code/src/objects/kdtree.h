#ifndef KDTREE_H
#define KDTREE_H
#include "list.h"
#include <stdbool.h>
#if DEBUG
#include <stdint.h>
#include <inttypes.h>
#endif

struct kdtree_node
{
	char    leaf  : 1;
	char    depth : 7;
	double  split;
	char    axis;
	struct kdtree_node *children;
	list_t             *indices;
#if DEBUG
	uint64_t id;
	double min[3];
	double max[3];
	struct kdtree_node *parent;
#endif
};

typedef struct kdtree_node kdtree_node_t;
typedef struct mesh mesh_t;

struct kdtree
{
	double min[3];
	double max[3];
	struct kdtree_node first;
};

struct box
{
	double min[3];
	double max[3];
};

typedef struct box box_t;
typedef struct kdtree kdtree_t;

kdtree_t *kdtree_build(mesh_t *mesh);

#if DEBUG
kdtree_node_t *kdtree_get_node(kdtree_t *tree, uint64_t id, int depth);
kdtree_node_t *kdtree_get_parent(kdtree_node_t *tree);
kdtree_node_t *kdtree_get_child(kdtree_node_t *tree, bool right_not_left);
bool kdtree_contains_index(kdtree_node_t *tree, int index);
#endif
void kdtree_delete(kdtree_t *tree);

#endif
