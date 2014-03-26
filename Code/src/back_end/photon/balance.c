#include "photon.h"
#include "utils.h"
#include "median.h"
#include <stdlib.h>

typedef struct
{
	double min[3];
	double max[3];
}bounds_t;

static void create_bounds(photon_t *p, int num, double *min, double *max)
{
	for(int axis = 0; axis < 3; axis++)
	{
		min[axis] = p[0].origin[axis];
		max[axis] = p[0].origin[axis];
	}
	for(int i = 1; i < num; i++)
	{
		for(int axis = 0; axis < 3; axis++)
		{
			if(p[i].origin[axis] < min[axis]) min[axis] = p[i].origin[axis];
			if(p[i].origin[axis] > max[axis]) max[axis] = p[i].origin[axis];
		}
	}
}

static int get_split_axis(double *min, double *max)
{
	double width = max[0] - min[0];
	int axis = 0;
	for(int i = 1; i < 3; i++)
	{
		if(max[i] - min[i] > width)
		{
			width = max[i] - min[i];
			axis = i;
		}
	}
	return axis;
}

/*
* Returns the median position that will left
* balance the tree.
*/
static int get_median_position(photon_t **p, int n)
{
	int m = 1;
	while(m <= n) m <<= 1;
	m >>= 1;
	int r = n - (m - 1);
	int left, right;
	if(r <= m / 2)
	{
		left  = (m - 2) / 2 + r;
		right = (m - 2) / 2;
	}
	else
	{
		left = (m - 2)/2 + m/2;
		right = (m - 2) / 2 + r - m/2;
	}
	Assert(left + right + 1 == n, "Partition doesn't make sense.");
	return left;
}

static void balance(photon_t **p, photon_t *out, int size, int index, int max_index, bounds_t b)
{
	if(size > 1)
	{
		int axis       = get_split_axis(b.min,b.max);
		int med_pos    = get_median_position(p, size);
		photon_t **med = median(p, size, med_pos, axis, index);
		//Calculate the index for the median element.
		int j = med - p;
		out[index] = **med;
		int left_index  = 2 * index + 1;
		int right_index = 2 * index + 2;
		out[index].axis = axis;
		out[index].has_left  = left_index  <= max_index;
		out[index].has_right = right_index <= max_index;
		bounds_t box;
		if(out[index].has_left)
		{
			box = b;
			box.max[axis] = out[index].origin[axis];
			balance(p, out, j, left_index, max_index, box);
			Assert(out[left_index].origin[axis] <= out[index].origin[axis], "Left not smaller");
		}
		if(out[index].has_right)
		{
			box = b;
			box.min[axis] = out[index].origin[axis];
			balance(p + j + 1, out, size - (j + 1), right_index, max_index, box);
			Assert(out[right_index].origin[axis] >= out[index].origin[axis], "Right not smaller");
		}
	}
	else
	{
		if(size)
		{
			Assert(*p != NULL, "Null Pointer");
			out[index] = **p;
			out[index].has_left  = false;
			out[index].has_right = false;
			out[index].axis      = 0;
		}
	}
}

photon_map_t *photon_map_balance(list_t *photons)
{
	int num         = list_size(photons);
	photon_t *p     = list_data(photons);
	photon_t **p0   = calloc(1, sizeof(photon_t*) * num);
	photon_t *out   = malloc(sizeof(photon_t) * num);
	for(int i = 0; i < num; i++)
		p0[i] = p + i;

	bounds_t bounds;
	create_bounds(p, num, bounds.min, bounds.max);
	balance(p0, out, num, 0, num - 1, bounds);
	free(p0);
	list_delete(photons, NULL);

	photon_map_t *m = malloc(sizeof(photon_map_t));
	m->num     = num;
	m->photons = out;
	VERBOSE("Photon Map: Stored %08d Photons.\n", m->num);
#if OUTPUT_PMAP
	static int pmap_count;
	char buf[1024];
	sprintf(buf, "photon_map%d.pmap", pmap_count++);
	photon_map_dump(m, buf, NULL);
#endif
	return m;
}
