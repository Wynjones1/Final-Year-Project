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
double fresnel(double n1, double n2, double cost)
{
	double r0 = (n1 - n2) / (n1 + n2);
	r0 *= r0;

	double t = 1 - cost;
	return r0 + (1.0 - r0) * t * t * t * t * t;
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

double maths_calculate_refracted_ray(double incident[3], double normal[3], double ior, double out[3])
{
	double cosi = -vector_dot(incident, normal);
	double n1 = 1.0;
	double n2 = ior;
	double n[3] = {normal[0], normal[1], normal[2]};
	if(cosi < 0.0)
	{
		n1 = ior;
		n2 = 1.0;
		cosi = -cosi;
		n[0] = -normal[0];
		n[1] = -normal[1];
		n[2] = -normal[2];
	}

	double nn = n1 / n2;

	double sin2t = nn * nn * (1 - cosi * cosi);
	double cost = sqrt(1 - sin2t);
	if(sin2t >= 1.0) return 0;
	out[0] = nn * incident[0] + (nn * cosi - cost) * n[0];
	out[1] = nn * incident[1] + (nn * cosi - cost) * n[1];
	out[2] = nn * incident[2] + (nn * cosi - cost) * n[2];
	if(n1 <= n2)
	{
#if 1
		return 1.0 - fresnel(n1, n2, cosi);
#else
		return 1.0 - maths_calculate_reflectance(normal, incident, n1, n2);
#endif
	}
	else
	{
		return 1.0 - fresnel(n1, n2, cost);
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

void maths_spherical_to_vector(double theta, double phi, double out[3])
{
	double cp = cosf(phi);
	double ct = cosf(theta);
	double sp = sinf(phi);
	double st = sinf(theta);

	out[0] = st * cp;
	out[1] = st * sp;
	out[2] = ct;
}

void maths_vector_to_spherical(double in[3], double *theta, double *phi)
{
	*phi   = atan2(in[1], in[0]);
	*theta = acos(in[2]);
}

void maths_basis(double z[3], double x[3], double y[3])
{
	if(z[0] > 0.1 || z[0] < -0.1)
	{
		y[0] = z[1];
		y[1] = -z[0];
		y[2] = 0;
	}
	else
	{
		y[0] = 0;
		y[1] = -z[2];
		y[2] = z[1];
	}
	vector_normal(y, y);

	vector_cross(y, z, x);
}
