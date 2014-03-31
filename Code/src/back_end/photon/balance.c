#include "photon.h"
#include "utils.h"
#include "median.h"
#include "queue.h"
#include <stdlib.h>
#include <pthread.h>

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

typedef struct balance_data
{
	bool done;
	photon_t **p;
	photon_t *out;
	int       size;
	int       index;
	int       max_index;
	bounds_t box;
}balance_data_t;

pthread_mutex_t g_count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  g_count_cond  = PTHREAD_COND_INITIALIZER;
int g_count;
int g_to_find;

void inc_count()
{
	pthread_mutex_lock(&g_count_mutex);
	g_count++;
	//Tell the main thread to wake up.
	if(g_count == g_to_find)
	{
		pthread_cond_broadcast(&g_count_cond);
	}
	pthread_mutex_unlock(&g_count_mutex);
}

static void *balance_thread(void *_data)
{
	queue_t *q = (queue_t*)_data;
	balance_data_t data;
	balance_data_t out;
	queue_read(q, &data);
	while(!data.done)
	{
		if(data.size > 1)
		{
			int axis       = get_split_axis(data.box.min,data.box.max);
			int med_pos    = get_median_position(data.p, data.size);
			photon_t **med = median(data.p, data.size, med_pos, axis, data.index);
			//Calculate the index for the median element.
			int j = med - data.p;
			data.out[data.index] = **med;
			int left_index       = 2 * data.index + 1;
			int right_index      = 2 * data.index + 2;
			data.out[data.index].axis      = axis;
			data.out[data.index].has_left  = left_index  <= data.max_index;
			data.out[data.index].has_right = right_index <= data.max_index;
			bounds_t box;
			if(data.out[data.index].has_left)
			{
				out.p             = data.p;
				out.out           = data.out;
				out.size          = j;
				out.index         = left_index;
				out.max_index     = data.max_index;
				out.box           = data.box;
				out.box.max[axis] = data.out[data.index].origin[axis];

				queue_write(q, &out);

				//balance(p, out, j, left_index, max_index, box);
			}
			if(data.out[data.index].has_right)
			{
				out.p             = data.p + j + 1;
				out.out           = data.out;
				out.size          = data.size - (j + 1);
				out.index         = right_index;
				out.max_index     = data.max_index;
				out.box           = data.box;
				out.box.min[axis] = data.out[data.index].origin[axis];

				queue_write(q, &out);
				//balance(p + j + 1, out, size - (j + 1), right_index, max_index, box);
			}
		}
		else if(data.size)
		{
			Assert(*data.p != NULL, "Null Pointer");
			data.out[data.index] = **data.p;
			data.out[data.index].has_left  = false;
			data.out[data.index].has_right = false;
			data.out[data.index].axis      = 0;
		}
		inc_count();
		queue_read(q, &data);
	}
	return NULL;
}

static void balance(photon_t **p, photon_t *out, int size, int index, int max_index, bounds_t b)
{
#if SINGLE_THREAD
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
#else
	if(size > 1)
	{
		pthread_t threads[g_config.num_threads];
		balance_data_t data;
		data.p         = p;
		data.out       = out;
		data.size      = size;
		data.index     = index;
		data.max_index = max_index;
		data.box       = b;
		data.done      = false;
		queue_t *data_queue = queue_init(size, sizeof(balance_data_t));

		g_count   = 0;
		g_to_find = size;

		//Start all the threads.
		for(int i = 0; i < g_config.num_threads; i++)
		{
			pthread_create(&threads[i], NULL, balance_thread, data_queue);
		}

		//Send the initial data.
		queue_write(data_queue, &data);
		//Wait for the all the data to be processed.
		while(g_count != g_to_find)
		{
			pthread_mutex_lock(&g_count_mutex);
			pthread_cond_wait(&g_count_cond, &g_count_mutex);
			pthread_mutex_unlock(&g_count_mutex);
		}
		//Tell the threads that they are done.
		data.done = true;
		for(int i = 0; i < g_config.num_threads; i++)
		{
			queue_write(data_queue, &data);
		}
		//Wait for all the threads to terminate.
		for(int i = 0; i < g_config.num_threads; i++)
		{
			pthread_join(threads[i], NULL);
		}
		queue_delete(data_queue);
	}
	else if(size)
	{
			Assert(*p != NULL, "Null Pointer");
			out[index] = **p;
			out[index].has_left  = false;
			out[index].has_right = false;
			out[index].axis      = 0;
	}
#endif
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
