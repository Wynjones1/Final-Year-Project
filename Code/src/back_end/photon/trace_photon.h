#ifndef TRACE_PHOTON
#define TRACE_PHOTON
#include "scene.h"
#include "queue.h"
#include "ray.h"
#include "intersection.h"
#include "photon.h"

struct thread_output_data
{
	int      last     : 1;
	int      diffuse  : 1;
	int      specular : 1;
	photon_t photon;
};

int trace_photon(scene_t *scene, ray_t *ray, int light, double power[3], bool specular, bool diffuse, bool specular_only, queue_t *output);

#endif
