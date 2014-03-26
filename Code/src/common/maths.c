#include "maths.h"
#include "vector.h"
#include <string.h>
#include "utils.h"
#include <math.h>
#include <stdlib.h>
#include "ray.h"

void maths_calculate_reflected_ray(double incident[3], double normal[3], double out[3])
{
	double dot = -vector_dot(incident, normal);
	memcpy(out, normal, sizeof(double) * 3);
	vector_mult_const(out, dot * 2);
	vector_add(out, incident, out);
}

/* Returns 0 for TIR and 1 otherwise. */
int maths_calculate_refracted_ray(double incident[3], double normal[3], double n1, double n2, double out[3])
{
	double dot = vector_dot(incident, normal);
	double fac = 0.0;
	double n   = 0.0;
	//If the dot is positive we are inside the mesh so we need to swap n1 n2
	if(dot > 0.0)
	{
		n = n2 / n1;
		fac = 1.0 - n * n * (1.0 - dot * dot);
		if(fac <= 0.0) return 0;
		fac =  (n * -dot) + sqrt(fac);
	}
	else
	{
		n = n1 / n2;
		fac = 1.0 - n * n * (1.0 - dot * dot);
		if(fac <= 0.0) return 0;
		fac =  (n * -dot) - sqrt(fac);
	}
	for(int i = 0; i < 3; i++)
	{
		out[i] = n * incident[i] + fac * normal[i];
	}
	return 1;
}

double maths_calculate_reflectance(double normal[3], double incident[3], double n1, double n2)
{
	double n = n1 / n2;
	double cosi = -vector_dot(normal, incident);
	double sint2 = n * n * (1.0 - cosi * cosi);
	if(sint2 > 1.0) return 1.0;
	double cost = sqrt(1.0 - sint2);
	double rorth = (n1 * cosi - n2 * cost) / (n1 * cosi + n2 * cost);
	double rpar  = (n2 * cosi - n1 * cosi) / (n2 * cosi + n1 * cost);
	return max(0.0, min(1.0,(rorth * rorth + rpar * rpar) / 2.0));
}

void maths_calculate_intersection(ray_t *ray, double t, double out[3], int offset)
{
	assert(-1 <= offset && offset <= 1);
	for(int i = 0; i < 3; i++)
	{
		out[i] = ray->origin[i] + ray->normal[i] * (t + offset * BIG_EPSILON);
	}
}

double maths_calculate_normal(double A[3], double B[3], double out[3])
{
	double ret;
	vector_sub(B, A, out);
	ret = vector_length(out);
	vector_normal(out, out);
	return ret;
}

double maths_calculate_t(double origin, double normal, double pos)
{
	return (pos - origin) / normal;	
}

double samples[1024];
int start;

double randf(double min, double max)
{
#if 0
	if(start == 0)
	{
		for(int i = 0; i < 1024; i++)
		{
			samples[i] = rand();
		}
		start = 1;
	}
	static int count;
	double r = samples[count++ % 1024];
#else
	double r = rand();
#endif
	return (r * ( max - min) / RAND_MAX) + min;
}
