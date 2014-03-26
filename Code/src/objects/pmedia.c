#include "pmedia.h"
#include "object.h"
#include <stdlib.h>

typedef struct pmedia
{
	OBJECT_DATA()
	double scattering[3];
	double absorption[3];
	double exstinction[3];
}pmedia_t;
	
static int  intersection(object_t *o, ray_t *ray, intersection_t *info)
{
	return 1;
}

static void bounds(object_t *o, aabb_t *bounds)
{}

static void normal(object_t *o, intersection_t *info, double *normal)
{
}

static void tex(object_t *o, intersection_t *info, double *tex)
{
	
}

static void print(object_t *o)
{
	
}

static void delete(object_t *o)
{
	
}

pmedia_t *pmedia_init(const char *filename)
{
	return pmedia_new();
}

pmedia_t *pmedia_new(void)
{
	pmedia_t *out = malloc(sizeof(pmedia_t));
	out->intersection = intersection;
	out->get_normal   = normal;
	out->get_bounds   = bounds;
	out->get_tex      = tex;
	out->print        = print;
	out->delete       = delete;
	return out;
}
