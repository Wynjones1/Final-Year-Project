#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "median.h"

static const int bsize = 5;
int g_index;

static void swap(photon_t **a, photon_t **b)
{
	photon_t *t = *a;
	*a = *b;
	*b = t;
}

static int g_axis_sort;
static int cmp(const void *a, const void *b)
{
	int axis = g_axis_sort;
	double f0 = (*(photon_t**)a)->origin[axis];
	double f1 = (*(photon_t**)b)->origin[axis];
	if(f0 < f1)  return -1;
	if(f0 == f1) return 0;
	return 1;
}

static void sort(photon_t **a, int size, int axis)
{
	assert(size <= 5);
	g_axis_sort = axis;
	qsort(a, size, sizeof(photon_t*), cmp);
}

static int partition(photon_t **a, int size, photon_t *pivot, int axis)
{
	photon_t **l = a;
	photon_t **m = a + size;
	int num_same = 0;
	int i = 0;
	int p = 0;
	while(l + num_same < m)
	{
		if(a[i]->origin[axis] < pivot->origin[axis])
		{
			swap(a + i, l++);
			p++;
		}
		else if(a[i]->origin[axis] > pivot->origin[axis])
		{
			swap(a + i, --m);
			i--;
		}
		else
		{
			num_same++;
		}
		i++;
	}
	return p + num_same / 2;
}


static photon_t **median_select(photon_t **a, int k, int len, int axis)
{
	if(len <= 5)
	{
		sort(a, len, axis);
		return a + k;
	}
	int n   = len / bsize;
	int l   = len % bsize;
	for(int i = 0; i < n; i++)
	{
		sort(a + i * 5, 5, axis);
		swap(a + i, a + i * 5 + 2);
	}
	//Sort out the leftover bits at the end.
	if(l)
	{
		sort(a + n * 5, l, axis);
		swap(a + n, a + n * 5 + l / 2);
		n++;
	}
	photon_t **med = median_select(a, n / 2, n, axis);
	int j   = partition(a, len, *med, axis);
	if(k < j)  return median_select(a, k, j, axis);
	if(k == j) return a + j;
	j++;
	return median_select(a + j, k - j, len - j, axis);
}

photon_t **median(photon_t **p, int n, int med_pos, int axis, int index)
{
	g_index = index;
	return median_select(p, med_pos, n, axis);
}
