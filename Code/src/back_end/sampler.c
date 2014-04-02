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

void sample_hemi_cosine(double normal[3], double out[3])
{
	double xi1 = randf(0.0, 1.0);
	double xi2 = randf(0.0, 1.0);

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

/* Sample uniformly from the hemishere */
void sample_hemi_jitter(double normal[3], double x[3], double y[3], int xsample, int ysample, double out[3])
{
#if JITTER
	double sx = xsample + randf(0, 1);
	double sy = ysample + randf(0, 1);
#else
	double sx = xsample + 0.5;
	double sy = ysample + 0.5;
#endif
	//Map to 0, 1
	sx /= g_config.samples;
	sy /= g_config.samples;

	double phi = 2 * PI * sx;
	double theta = acos(1 - sy);
	double st = sinf(theta);
	double sp = sinf(phi);
	double ct = cosf(theta);
	double cp = cosf(phi);
	for(int i = 0; i < 3; i++)
	{
		out[i] = st * cp * x[i] + st * sp * y[i] + ct * normal[i];
	}
}
