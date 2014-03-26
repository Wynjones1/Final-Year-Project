#ifndef PHOTON_H
#define PHOTON_H
#include "scene.h"
#include <stdio.h>

typedef struct photon     photon_t;
typedef struct photon_map photon_map_t;

struct photon
{
	unsigned int has_left  : 1;
	unsigned int has_right : 1;
	unsigned int axis      : 2;
	unsigned int light     : 26; 
	double       origin[3];
	double       incident[3];
	double       power[3];
};

struct photon_map
{
	unsigned int num;
	photon_t    *photons;
};

typedef struct
{
	photon_t *p;
	double    dist;
}photon_dist_key_t;

void          photon_map_build(scene_t *scene, int num_photons);
void          photon_map_delete(photon_map_t *map);
void          photon_map_dump(photon_map_t *map, const char *filename, FILE *fp);
void          photon_map_verify(photon_map_t *map);
int           photon_map_size(photon_map_t *map);

int           photon_map_nearest(photon_map_t *map, double *v);
int           photon_map_nearest_bf(photon_map_t *map, double *v);

int           photon_map_nearest_n(photon_map_t *map, double *v, int n, double radius,photon_dist_key_t *out);
int           photon_map_nearest_n_bf(photon_map_t *map, double *v, int n, double radius,photon_dist_key_t *out);

void          photon_map_estimate_radiance(photon_map_t *map, double x[3], double n[3], double radiance[3]);
void          photon_map_estimate_radiance_volume(photon_map_t *map, double x[3], double n[3], double radiance[3]);

photon_map_t *photon_map_balance(list_t *photons);
#endif
