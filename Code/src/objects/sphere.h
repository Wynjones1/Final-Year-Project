#ifndef SPHERE_H
#define SPHERE_H
#include "object.h"

typedef struct sphere
{
	OBJECT_DATA()
	double r;
	double origin[3];
}sphere_t;

sphere_t *sphere_init(const char *str);
#endif
