#include "photon.h"
#include <string.h>
#include "utils.h"

bool insert(photon_dist_key_t *out, photon_t *p, float dist, int found, int size)
{
	if(found >= size)
	{
		if(dist < out[size - 1].dist)
		{
			int j = size;
			for(int i = 0; i < size; i++)
			{
				if(dist < out[i].dist)
				{
					memmove(out + i + 1, out + i, sizeof(photon_dist_key_t) * (size - i - 1));
					j = i;
					break;
				}
			}
			if(j != size)
			{
				out[j].dist = dist;
				out[j].p    = p;
				return j == size-1;
			}
		}
	}
	else
	{
		int i;
		for(i = 0; i < found;i++)
		{
			if(dist < out[i].dist)
			{
				memmove(out + i + 1, out + i, sizeof(photon_dist_key_t) * (found - i));
				out[i].dist = dist;
				out[i].p    = p;
				break;
			}
		}
		if(i == found)//Insert it in first index free (i.e found)
		{
			out[i].dist = dist;
			out[i].p    = p;
		}
	}
	return false;
}

int photon_map_nearest_n_bf(photon_map_t *map, double *v, int n, double radius, photon_dist_key_t *out)
{
	int found = 0;
	photon_t *p = map->photons;
	double rad2 = radius * radius;
	for(int i = 0; i < map->num; i++)
	{
		double d = vector_squared_dist(p[i].origin, v);
		if(d <= rad2)
		{
			insert(out, p + i, d, found, n);
			found++;
		}
	}
	return min(found, n);
}

/*
* Finds the nearest n photons to v within the search radius, returns in out which is assumed to
* be a valid pointer.
*/
void nearest_n(photon_t *photons, int num, int index, int n, double *radius, double *v, int *found, photon_dist_key_t *out)
{
	if(index < num)//We are a valid child.
	{
		int went_left;
		int axis = photons[index].axis;
		if(v[axis] < photons[index].origin[axis])
		{
			went_left = 1;
			nearest_n(photons, num, 2 * index + 1, n,radius, v, found, out);
		}
		else
		{
			went_left = 0;
			nearest_n(photons, num, 2 * index + 2, n,radius, v, found, out);
		}
		//Check if the current point is closer, if so set it to the best
		double pd = vector_squared_dist(photons[index].origin, v);
		double ad = photons[index].origin[axis] - v[axis];
		ad = ad * ad;
		if(pd < *radius)//It is valid and we can insert it.
		{
			if(insert(out, photons + index, pd, *found, n))//It is the largest in the list.
				*radius = pd;
			(*found)++;
		}
		if(ad < *radius) //We need to check the other side.
		{
			int offset = went_left ? 2 : 1;
			nearest_n(photons, num, 2 * index + offset, n, radius, v, found, out);
		}
	}
}

int photon_map_nearest_n(photon_map_t *map, double *v, int n, double radius,photon_dist_key_t *out)
{
#if BRUTEFORCE
	return photon_map_nearest_n_bf(map, v, n, radius, out);
#else
	double r = radius * radius;
	int found = 0;
	nearest_n(map->photons, map->num, 0, n, &r, v, &found, out);
	return min(n, found);
#endif
}

int find_index(photon_t *photons, double *v, int index, list_t *l)
{
	int axis = photons[index].axis;
	int child;
	list_push(l, &index);
	if(photons[index].has_left && v[axis] < photons[index].origin[axis])
	{
		child = 2 * index + 1;
		return find_index(photons, v, child, l);
	}
	else if(photons[index].has_right && v[axis] > photons[index].origin[axis])
	{
		child = 2 * index + 2;
		return find_index(photons, v, child, l);
	}
	return index;
}

int nearest(photon_t *photons, int num, int index, int current_best,double *best_dist, double *v)
{
	int best = current_best;
	if(index < num)
	{
		int went_left;
		int axis = photons[index].axis;
		if(v[axis] < photons[index].origin[axis])
		{
			went_left = 1;
			best = nearest(photons, num, 2 * index + 1, current_best, best_dist, v); }
		else
		{
			went_left = 0;
			best = nearest(photons, num, 2 * index + 2, current_best, best_dist, v);
		}
		//Check if the current point is closer, if so set it to the best
		double pd = vector_squared_dist(photons[index].origin, v);
		double ad = photons[index].origin[axis] - v[axis];
		ad = ad * ad;
		if(pd < *best_dist)
		{
			best = index;
			*best_dist = pd;
		}
		if(ad < *best_dist) //We need to check the other side.
		{
			int offset = went_left ? 2 : 1;
			best = nearest(photons, num, 2 * index + offset, best, best_dist, v);
		}
	}
	return best;
}

int photon_map_nearest(photon_map_t *map, double *v)
{
#if BRUTEFORCE
	return photon_map_nearest_bf(map, v);
#else
	double d = INFINITY;
	return nearest(map->photons, map->num, 0, 0, &d, v);
#endif
}

int photon_map_nearest_bf(photon_map_t *map, double *v)
{
	Assert(map->num, "Empty Photon Map.");
	double square_dist = vector_squared_dist(v, map->photons[0].origin);
	int index = 0;
	for(int i = 1; i < map->num; i++)
	{
		double d = vector_squared_dist(v, map->photons[i].origin);
		if(d < square_dist)
		{
			square_dist = d;
			index = i;
		}
	}
	return index;
}
