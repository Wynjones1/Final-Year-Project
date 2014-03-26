#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "utils.h"
#define VECTOR_C
#include "vector.h"

#define VECTOR_SIZE 3

INLINE void vector_print(double *vector)
{
	vector_fprint(stdout, vector);
}

INLINE void vector_fprint(FILE *fp, double *vector)
{
	FOUTPUT(fp,"[");	
	int i;
	for(i = 0;i < VECTOR_SIZE - 1;i++)
	{
		if(vector[i] < 0)
				FOUTPUT(fp,"%f,",vector[i]);
		else
				FOUTPUT(fp," %f,",vector[i]);
	}
	if(vector[VECTOR_SIZE - 1] < 0)
		FOUTPUT(fp,"%f]\n",vector[VECTOR_SIZE - 1]);
	else
		FOUTPUT(fp," %f]\n",vector[VECTOR_SIZE - 1]);
}

INLINE void vector_add(double *v1,double *v2,double *vout)
{
	for(int i = 0;i < VECTOR_SIZE;i++)
	{
		vout[i] = v1[i] + v2[i];
	}
}

INLINE void vector_sub(double *v1,double *v2,double *vout)
{
	for(int i = 0;i < VECTOR_SIZE;i++)
	{
		vout[i] = v1[i] - v2[i];
	}
}

INLINE void vector_mult(double c,double *v1,double *vout)
{
	for(int i = 0;i < VECTOR_SIZE;i++)
	{
		vout[i] = v1[i] * c;
	}
}

INLINE void vector_div(double c,double *v1,double *vout)
{
	for(int i = 0;i < VECTOR_SIZE;i++)
	{
		vout[i] = v1[i] / c;
	}
}

INLINE void vector_cross(double *v1,double *v2,double *vout)
{
	vout[0] = v1[1] * v2[2] - v1[2] * v2[1];
	vout[1] = v1[2] * v2[0] - v1[0] * v2[2];
	vout[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

INLINE double vector_length(double *v1)
{
	double temp = 0.0;
	for(int i = 0;i < VECTOR_SIZE;i++)
	{
		temp += v1[i] * v1[i];
	}
	return sqrt(temp);
}

INLINE void vector_normal(double *v1,double *vout)
{
	double temp = vector_length(v1);
	vector_div(temp,v1,vout);
}

INLINE double vector_dot(double *v1,double *v2)
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

INLINE double vector_squared_dist(double *v1, double *v2)
{
	double out = 0.0;
	for(int i = 0; i < VECTOR_SIZE; i++)
	{
		double d = v1[i] - v2[i];
		out += d * d;
	}
	return out;
}

INLINE void vector_neg(double *v1)
{
	for(int i = 0; i < VECTOR_SIZE; i++)
	{
		v1[i] = -v1[i];
	}
}

INLINE void vector_copy(double *v1, double *v2)
{
	memcpy(v2, v1, sizeof(double) * VECTOR_SIZE);
}

INLINE void vector_addc(double *v1, double c, double *v2)
{
	for(int i = 0; i < VECTOR_SIZE; i++)
	{
		v2[i] = v1[i] + c;
	}
}

INLINE void vector_mult_const(double *v1, double c)
{
	for(int i = 0; i < 3; i++)
	{
		v1[i] *= c;
	}
}

INLINE void vector_add_const(double *v1, double c)
{
	for(int i = 0; i < 3; i++)
	{
		v1[i] += c;
	}
}

INLINE double vector_distance(double v0[3], double v1[3]) {
	double temp[] = {v0[0] - v1[0],
					 v0[1] - v1[1],
					 v0[2] - v1[2]};
	return vector_length(temp);
}
