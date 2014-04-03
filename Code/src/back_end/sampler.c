#include "sampler.h"
#include <stdlib.h>
#include "vector.h"
#include "config.h"
#include "maths.h"

enum sampler_type
{
	uniform,
};

struct sampler
{
	enum sampler_type type;
	union
	{
		struct
		{
			double a, b;
		}uniform;
	};
};

sampler_h *sampler_uniform(double a, double b)
{
	return NULL;
}

void sample_hemi(double normal[3], double out[3])
{
	sample_sphere(out);
	while(vector_dot(normal, out) <= 0.0)
	{
		sample_sphere(out);
	}
}


void sample_hemi_cosine_common(double normal[3], double out[3], double xi1, double xi2)
{
	double t = acos(sqrt(1.0 - xi1));
	double p = 2.0 * PI * xi2;

	float xs = sinf(t) * cosf(p);
	float ys = cosf(t);
	float zs = sinf(t) * sinf(p);

	double y[3], h[3];
	vector_copy(normal, y);
	vector_copy(normal, h);

	if(fabs(h[0]) <= fabs(h[1]) && fabs(h[0]) <= fabs(h[2]))
	{
		h[0] = 1.0;
	}
	else if(fabs(h[1]) <= fabs(h[0]) && fabs(h[1]) <= fabs(h[2]))
	{
		h[1] = 1.0;
	}
	else
	{
		h[2] = 1.0;
	}
	double x[3], z[3];
	vector_cross(h, y, x);
	vector_normal(x, x);
	vector_cross(x, y, z);
	vector_normal(z, z);

	out[0] = xs * x[0] + ys * y[0] + zs * z[0];
	out[1] = xs * x[1] + ys * y[1] + zs * z[1];
	out[2] = xs * x[2] + ys * y[2] + zs * z[2];
	vector_normal(out, out);
}

void sample_hemi_cosine(double normal[3], double out[3])
{
	double xi1 = randf(0.0, 1.0);
	double xi2 = randf(0.0, 1.0);
	sample_hemi_cosine_common(normal, out, xi1, xi2);
}

/*
* Found http://mathworld.wolfram.com/SpherePointPicking.html
*/
void sample_sphere(double out[3])
{
	double x1 = randf(-1, 1);
	double x2 = randf(-1, 1);
	while(x1 * x1 + x2 *x2 >= 1.0)
	{
		x1 = randf(-1, 1);
		x2 = randf(-1, 1);
	}
	double sum_sq = x1 * x1 + x2 * x2;
	double sq = sqrt(1 - sum_sq);
	out[0] = 2 * x1 * sq;
	out[1] = 2 * x2 * sq;
	out[2] = 1 - 2 * sum_sq;
}

#define JITTER 0
/* Sample uniformly from the hemishere */
void sample_hemi_jitter(double n[3], double _x[3], double _y[3], int xsample, int ysample, double out[3])
{

	double Xi1 = randf(0, 1);
	double Xi2 = randf(0, 1);

	double  theta = acos(sqrt(1.0-Xi1));
	double  phi = 2.0 * PI * Xi2;

	double xs = sinf(theta) * cosf(phi);
	double ys = cosf(theta);
	double zs = sinf(theta) * sinf(phi);

	double x[3];
	double y[3];
	double z[3];
	double h[3];
	vector_copy(n, y);
	vector_copy(n, h);
	if (fabs(h[0])<=fabs(h[1]) && fabs(h[0])<=fabs(h[2]))
	{
		h[0]= 1.0;
	}
	else if (fabs(h[1])<=fabs(h[0]) && fabs(h[1])<=fabs(h[2]))
	{
		h[1]= 1.0;
	}
	else
	{
		h[2]= 1.0;
	}

	vector_cross(h, y, x);
	vector_normal(x, x);
	vector_cross(x, y, z);
	vector_normal(z, z);

	for(int i = 0; i < 3; i++)
	{
		n[i] = xs * x[i] + ys * y[i] + zs * z[i];
	}
	vector_normal(n, n);
}

void sample_hemi_cosine_jitter(double normal[3], double out[3], int x, int y, int num_samples)
{
	double u0 = (x + randf(0, 1)) / num_samples;
	double u1 = (x + randf(0, 1)) / num_samples;
	sample_hemi_cosine_common(normal, out, u0, u1);
}
