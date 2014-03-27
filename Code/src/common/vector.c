#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "utils.h"
#define VECTOR_C
#include "vector.h"

#define VECTOR_SIZE 3

INLINE void vector_print(vector_t vector)
{
	vector_fprint(stdout, vector);
}

INLINE void vector_fprint(FILE *fp, vector_t vector)
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

INLINE void vector_add(vector_t v1,vector_t v2,vector_t vout)
{
	for(int i = 0;i < VECTOR_SIZE;i++)
	{
		vout[i] = v1[i] + v2[i];
	}
}

INLINE void vector_sub(vector_t v1,vector_t v2,vector_t vout)
{
	for(int i = 0;i < VECTOR_SIZE;i++)
	{
		vout[i] = v1[i] - v2[i];
	}
}

INLINE void vector_mult(vector_float_t c,vector_t v1,vector_t vout)
{
	for(int i = 0;i < VECTOR_SIZE;i++)
	{
		vout[i] = v1[i] * c;
	}
}

INLINE void vector_div(vector_float_t c,vector_t v1,vector_t vout)
{
	for(int i = 0;i < VECTOR_SIZE;i++)
	{
		vout[i] = v1[i] / c;
	}
}

INLINE void vector_cross(vector_t v1,vector_t v2,vector_t vout)
{
	vout[0] = v1[1] * v2[2] - v1[2] * v2[1];
	vout[1] = v1[2] * v2[0] - v1[0] * v2[2];
	vout[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

INLINE vector_float_t vector_length(vector_t v1)
{
	vector_float_t temp = 0.0;
	for(int i = 0;i < VECTOR_SIZE;i++)
	{
		temp += v1[i] * v1[i];
	}
	return sqrt(temp);
}

INLINE void vector_normal(vector_t v1,vector_t vout)
{
	vector_float_t temp = vector_length(v1);
	vector_div(temp,v1,vout);
}

INLINE vector_float_t vector_dot(vector_t v1,vector_t v2)
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

INLINE vector_float_t vector_squared_dist(vector_t v1, vector_t v2)
{
	vector_float_t out = 0.0;
	for(int i = 0; i < VECTOR_SIZE; i++)
	{
		vector_float_t d = v1[i] - v2[i];
		out += d * d;
	}
	return out;
}

INLINE void vector_neg(vector_t v1)
{
	for(int i = 0; i < VECTOR_SIZE; i++)
	{
		v1[i] = -v1[i];
	}
}

INLINE void vector_copy(vector_t v1, vector_t v2)
{
	memcpy(v2, v1, sizeof(vector_float_t ) * VECTOR_SIZE);
}

INLINE void vector_addc(vector_t v1, vector_float_t c, vector_t v2)
{
	for(int i = 0; i < VECTOR_SIZE; i++)
	{
		v2[i] = v1[i] + c;
	}
}

INLINE void vector_mult_const(vector_t v1, vector_float_t c)
{
	for(int i = 0; i < 3; i++)
	{
		v1[i] *= c;
	}
}

INLINE void vector_add_const(vector_t v1, vector_float_t c)
{
	for(int i = 0; i < 3; i++)
	{
		v1[i] += c;
	}
}

INLINE vector_float_t vector_distance(vector_t v0, vector_t v1)
{
	vector_t temp = {v0[0] - v1[0],
					 v0[1] - v1[1],
					 v0[2] - v1[2]};
	return vector_length(temp);
}
