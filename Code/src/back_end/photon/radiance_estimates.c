#include "photon.h"
#include "utils.h"
#define CONE_FILTER_K 1.1
#define CONE_FILTER_DIVISOR (1.0 - (2.0 / 3.0) / CONE_FILTER_K)

void photon_map_estimate_radiance(photon_map_t *map, double x[3], double normal[3], double radiance[3])
{
	photon_dist_key_t   photons[g_config.radiance_est_num];
	int n = photon_map_nearest_n(map, x, g_config.radiance_est_num, g_config.radiance_est_radius, photons);
	if(n > 8)
	{
		for(int i = 0; i < n; i++)
		{
			radiance[0] += photons[i].p->power[0];
			radiance[1] += photons[i].p->power[1];
			radiance[2] += photons[i].p->power[2];
		}

		double dist = photons[n - 1].dist;
		double inv_pi_dist = 1 / (PI * dist);
		radiance[0] *= inv_pi_dist;
		radiance[1] *= inv_pi_dist;
		radiance[2] *= inv_pi_dist;
	}
}

void photon_map_estimate_radiance_filter(photon_map_t *map, double x[3], double normal[3], double radiance[3])
{
	photon_dist_key_t   photons[g_config.radiance_est_num];
	int n = photon_map_nearest_n(map, x, g_config.radiance_est_num, g_config.radiance_est_radius, photons);
	if(n > 8)
	{
		double dist = photons[n - 1].dist;
		for(int i = 0; i < n; i++)
		{
			double weight = 1.0 - (sqrt(photons[i].dist) / (sqrt(dist) * CONE_FILTER_K));
			radiance[0] += weight * photons[i].p->power[0];
			radiance[1] += weight * photons[i].p->power[1];
			radiance[2] += weight * photons[i].p->power[2];
		}

		double inv_pi_dist = 1 / (CONE_FILTER_DIVISOR * PI * dist);
		radiance[0] *= inv_pi_dist;
		radiance[1] *= inv_pi_dist;
		radiance[2] *= inv_pi_dist;
	}
}

void photon_map_estimate_radiance_volume(photon_map_t *map, double x[3], double normal[3], double radiance[3])
{
	photon_dist_key_t   photons[g_config.radiance_est_num];
	int n = photon_map_nearest_n(map, x, g_config.radiance_est_num, g_config.radiance_est_radius, photons);
	if(n > 8)
	{
		for(int i = 0; i < n; i++)
		{
			radiance[0] += photons[i].p->power[0];
			radiance[1] += photons[i].p->power[1];
			radiance[2] += photons[i].p->power[2];
		}

		double dist = photons[n - 1].dist;
		double inv_pi_dist = (4.0  / 3.0) * (PI * dist * sqrt(dist));
		//phase function TODO:Add generic phase function.
		radiance[0] *= inv_pi_dist;
		radiance[1] *= inv_pi_dist;
		radiance[2] *= inv_pi_dist;
	}
}
