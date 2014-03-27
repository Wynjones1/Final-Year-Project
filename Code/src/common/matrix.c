#include "matrix.h"
#include <string.h>
#include <math.h>
#include "utils.h"
#include <stdlib.h>

matrix_t matrix_translation(vector_t t)
{
#if MATRIX_ROW_MAJOR
	matrix_t out = {.data = {1, 0, 0, t[0],
							 0, 1, 0, t[1],
							 0, 0, 1, t[2],
							 0, 0, 0, 1}};
#else
	matrix_t out = {.data = {1,    0,    0,    0,
							 0,    1,    0,    0,
							 0,    0,    1,    0,
							 t[0], t[1], t[2], 1}};
#endif
	return out;
}

matrix_t matrix_rotation_x(double theta)
{
	double ct = cos(theta);
	double st = sin(theta);
#if MATRIX_ROW_MAJOR
	matrix_t out = {.data = {1, 0,  0,  0,
							 0, ct,-st, 0,
							 0, st, ct, 0,
							 0, 0,  0,  1}};
#else
	matrix_t out = {.data = {1, 0,  0,  0,
							 0, ct, st, 0,
							 0,-st, ct, 0,
							 0, 0,  0,  1}};
#endif
	return out;
}

matrix_t matrix_rotation_y(double theta)
{
	double ct = cos(theta);
	double st = sin(theta);
#if MATRIX_ROW_MAJOR
	matrix_t out = {.data = {ct, 0, st, 0,
							 0 , 1, 0 , 0,
							-st, 0, ct, 0,
							 0 , 0, 0,  1}};
#else
	matrix_t out = {.data = {ct, 0, -st, 0,
							 0 , 1, 0 , 0,
							 st, 0, ct, 0,
							 0 , 0, 0,  1}};
#endif
	return out;
}

matrix_t matrix_rotation_z(double theta)
{
	double ct = cos(theta);
	double st = sin(theta);
#if MATRIX_ROW_MAJOR
	matrix_t out = {.data = {ct,-st, 0, 0,
							 st, ct, 0, 0,
							 0 , 0,  1, 0,
							 0 , 0,  0, 1}};
#else
	matrix_t out = {.data = {ct, st, 0, 0,
							-st, ct, 0, 0,
							 0 , 0,  1, 0,
							 0 , 0,  0, 1}};
#endif
	return out;
}

void matrix_translate(matrix_t *in, vector_t v0)
{
	matrix_t translate = matrix_translation(v0);
	matrix_mult(&translate, in, in);
}

void matrix_rotate_x(matrix_t *in, double theta)
{
	matrix_t r = matrix_rotation_x(theta);
	matrix_mult(&r, in, in);
}

void matrix_rotate_y(matrix_t *in, double theta)
{
	matrix_t r = matrix_rotation_y(theta);
	matrix_mult(&r, in, in);
}

void matrix_rotate_z(matrix_t *in, double theta)
{
	matrix_t r = matrix_rotation_z(theta);
	matrix_mult(&r, in, in);
}

void matrix_mult(matrix_t *a, matrix_t *b, matrix_t *out)
{
	matrix_t temp;
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			double d = 0.0;
			d += MATRIX_ELEM(*a, i, 0) * MATRIX_ELEM(*b, 0, j);
			d += MATRIX_ELEM(*a, i, 1) * MATRIX_ELEM(*b, 1, j);
			d += MATRIX_ELEM(*a, i, 2) * MATRIX_ELEM(*b, 2, j);
			d += MATRIX_ELEM(*a, i, 3) * MATRIX_ELEM(*b, 3, j);
			MATRIX_ELEM(temp, i, j) = d;
		}
	}
	*out = temp;
}

void matrix_mult_vec(matrix_t *a, vector_t v0, vector_t v1)
{
	double t[] = {v0[0], v0[1], v0[2], 1};
	for(int i = 0; i < 3;i++)
	{
		v1[i] = 0.0;
		v1[i] += MATRIX_ELEM(*a, i, 0) * t[0];
		v1[i] += MATRIX_ELEM(*a, i, 1) * t[1];
		v1[i] += MATRIX_ELEM(*a, i, 2) * t[2];
		v1[i] += MATRIX_ELEM(*a, i, 3) * t[3];
	}
}

void matrix_mult_norm(matrix_t *a, vector_t v0, vector_t v1)
{
	double t[] = {v0[0], v0[1], v0[2], 0};
	for(int i = 0; i < 3;i++)
	{
		v1[i] = 0.0;
		v1[i] += MATRIX_ELEM(*a, i, 0) * t[0];
		v1[i] += MATRIX_ELEM(*a, i, 1) * t[1];
		v1[i] += MATRIX_ELEM(*a, i, 2) * t[2];
		v1[i] += MATRIX_ELEM(*a, i, 3) * t[3];
	}
}

matrix_t matrix_random_i()
{
	matrix_t out = {.data =
					{rand()%100, rand()%100, rand()%100, rand()%100,
					 rand()%100, rand()%100, rand()%100, rand()%100,
					 rand()%100, rand()%100, rand()%100, rand()%100,
					 rand()%100, rand()%100, rand()%100, rand()%100}};
	return out;
}

void matrix_print(matrix_t *m)
{
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			printf("%12.4f, ", MATRIX_ELEM(*m, i, j));
		}
		printf("\r\n");
	}
}

matrix_t matrix_new (double a0, double a1, double a2, double a3,
					 double b0, double b1, double b2, double b3,
					 double c0, double c1, double c2, double c3,
					 double d0, double d1, double d2, double d3)
{
#if MATRIX_ROW_MAJOR
	matrix_t out = {.data ={
					a0, a1, a2, a3,
					b0, b1, b2, b3,
					c0, c1, c2, c3,
					d0, d1, d2, d3}};
#else
	matrix_t out = {.data ={
					a0, b0, c0, d0,
					a1, b1, c1, d1,
					a2, b2, c2, d2,
					a3, b3, c3, d3}};
#endif
	return out;
}

void matrix_init(matrix_t *m,
				     double a0, double a1, double a2, double a3,
					 double b0, double b1, double b2, double b3,
					 double c0, double c1, double c2, double c3,
					 double d0, double d1, double d2, double d3)
{
	*m = matrix_new(a0,  a1,  a2,  a3,
					b0,  b1,  b2,  b3,
					c0,  c1,  c2,  c3,
					d0,  d1,  d2,  d3);
}

matrix_t matrix_inv_new(matrix_t *m)
{
	matrix_t out;
	matrix_inv(m, &out);
	return out;
}
void matrix_inv(matrix_t *m, matrix_t *inv)
{
	double s0 = MATRIX_ELEM(*m, 0, 0) * MATRIX_ELEM(*m, 1, 1) - MATRIX_ELEM(*m, 1, 0) * MATRIX_ELEM(*m, 0, 1);
	double s1 = MATRIX_ELEM(*m, 0, 0) * MATRIX_ELEM(*m, 1, 2) - MATRIX_ELEM(*m, 1, 0) * MATRIX_ELEM(*m, 0, 2);
	double s2 = MATRIX_ELEM(*m, 0, 0) * MATRIX_ELEM(*m, 1, 3) - MATRIX_ELEM(*m, 1, 0) * MATRIX_ELEM(*m, 0, 3);
	double s3 = MATRIX_ELEM(*m, 0, 1) * MATRIX_ELEM(*m, 1, 2) - MATRIX_ELEM(*m, 1, 1) * MATRIX_ELEM(*m, 0, 2);
	double s4 = MATRIX_ELEM(*m, 0, 1) * MATRIX_ELEM(*m, 1, 3) - MATRIX_ELEM(*m, 1, 1) * MATRIX_ELEM(*m, 0, 3);
	double s5 = MATRIX_ELEM(*m, 0, 2) * MATRIX_ELEM(*m, 1, 3) - MATRIX_ELEM(*m, 1, 2) * MATRIX_ELEM(*m, 0, 3);

	double c5 = MATRIX_ELEM(*m, 2, 2) * MATRIX_ELEM(*m, 3, 3) - MATRIX_ELEM(*m, 3, 2) * MATRIX_ELEM(*m, 2, 3);
	double c4 = MATRIX_ELEM(*m, 2, 1) * MATRIX_ELEM(*m, 3, 3) - MATRIX_ELEM(*m, 3, 1) * MATRIX_ELEM(*m, 2, 3);
	double c3 = MATRIX_ELEM(*m, 2, 1) * MATRIX_ELEM(*m, 3, 2) - MATRIX_ELEM(*m, 3, 1) * MATRIX_ELEM(*m, 2, 2);
	double c2 = MATRIX_ELEM(*m, 2, 0) * MATRIX_ELEM(*m, 3, 3) - MATRIX_ELEM(*m, 3, 0) * MATRIX_ELEM(*m, 2, 3);
	double c1 = MATRIX_ELEM(*m, 2, 0) * MATRIX_ELEM(*m, 3, 2) - MATRIX_ELEM(*m, 3, 0) * MATRIX_ELEM(*m, 2, 2);
	double c0 = MATRIX_ELEM(*m, 2, 0) * MATRIX_ELEM(*m, 3, 1) - MATRIX_ELEM(*m, 3, 0) * MATRIX_ELEM(*m, 2, 1);

	double det = s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0;
	if(det == 0.0) ERROR("Singular Matrix.");
	double inv_det = 1 / det;

	MATRIX_ELEM(*inv, 0, 0) = ( MATRIX_ELEM(*m, 1, 1) * c5 - MATRIX_ELEM(*m, 1, 2) * c4 + MATRIX_ELEM(*m, 1, 3) * c3) * inv_det;
	MATRIX_ELEM(*inv, 0, 1) = (-MATRIX_ELEM(*m, 0, 1) * c5 + MATRIX_ELEM(*m, 0, 2) * c4 - MATRIX_ELEM(*m, 0, 3) * c3) * inv_det;
	MATRIX_ELEM(*inv, 0, 2) = ( MATRIX_ELEM(*m, 3, 1) * s5 - MATRIX_ELEM(*m, 3, 2) * s4 + MATRIX_ELEM(*m, 3, 3) * s3) * inv_det;
	MATRIX_ELEM(*inv, 0, 3) = (-MATRIX_ELEM(*m, 2, 1) * s5 + MATRIX_ELEM(*m, 2, 2) * s4 - MATRIX_ELEM(*m, 2, 3) * s3) * inv_det;
                                                                                                           
	MATRIX_ELEM(*inv, 1, 0) = (-MATRIX_ELEM(*m, 1, 0) * c5 + MATRIX_ELEM(*m, 1, 2) * c2 - MATRIX_ELEM(*m, 1, 3) * c1) * inv_det;
	MATRIX_ELEM(*inv, 1, 1) = ( MATRIX_ELEM(*m, 0, 0) * c5 - MATRIX_ELEM(*m, 0, 2) * c2 + MATRIX_ELEM(*m, 0, 3) * c1) * inv_det;
	MATRIX_ELEM(*inv, 1, 2) = (-MATRIX_ELEM(*m, 3, 0) * s5 + MATRIX_ELEM(*m, 3, 2) * s2 - MATRIX_ELEM(*m, 3, 3) * s1) * inv_det;
	MATRIX_ELEM(*inv, 1, 3) = ( MATRIX_ELEM(*m, 2, 0) * s5 - MATRIX_ELEM(*m, 2, 2) * s2 + MATRIX_ELEM(*m, 2, 3) * s1) * inv_det;
                                                                                                           
	MATRIX_ELEM(*inv, 2, 0) = ( MATRIX_ELEM(*m, 1, 0) * c4 - MATRIX_ELEM(*m, 1, 1) * c2 + MATRIX_ELEM(*m, 1, 3) * c0) * inv_det;
	MATRIX_ELEM(*inv, 2, 1) = (-MATRIX_ELEM(*m, 0, 0) * c4 + MATRIX_ELEM(*m, 0, 1) * c2 - MATRIX_ELEM(*m, 0, 3) * c0) * inv_det;
	MATRIX_ELEM(*inv, 2, 2) = ( MATRIX_ELEM(*m, 3, 0) * s4 - MATRIX_ELEM(*m, 3, 1) * s2 + MATRIX_ELEM(*m, 3, 3) * s0) * inv_det;
	MATRIX_ELEM(*inv, 2, 3) = (-MATRIX_ELEM(*m, 2, 0) * s4 + MATRIX_ELEM(*m, 2, 1) * s2 - MATRIX_ELEM(*m, 2, 3) * s0) * inv_det;
                                                                                                           
	MATRIX_ELEM(*inv, 3, 0) = (-MATRIX_ELEM(*m, 1, 0) * c3 + MATRIX_ELEM(*m, 1, 1) * c1 - MATRIX_ELEM(*m, 1, 2) * c0) * inv_det;
	MATRIX_ELEM(*inv, 3, 1) = ( MATRIX_ELEM(*m, 0, 0) * c3 - MATRIX_ELEM(*m, 0, 1) * c1 + MATRIX_ELEM(*m, 0, 2) * c0) * inv_det;
	MATRIX_ELEM(*inv, 3, 2) = (-MATRIX_ELEM(*m, 3, 0) * s3 + MATRIX_ELEM(*m, 3, 1) * s1 - MATRIX_ELEM(*m, 3, 2) * s0) * inv_det;
	MATRIX_ELEM(*inv, 3, 3) = ( MATRIX_ELEM(*m, 2, 0) * s3 - MATRIX_ELEM(*m, 2, 1) * s1 + MATRIX_ELEM(*m, 2, 2) * s0) * inv_det;

}

matrix_t matrix_new_identity(void)
{
	matrix_t out;
	matrix_identity(&out);
	return out;
}

void     matrix_identity(matrix_t *m)
{
	matrix_init(m, 1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1);
}

#define TRANS_ELEM(m0, m1, i, j)\
	temp                   = MATRIX_ELEM(*m0, i, j);\
	MATRIX_ELEM(*m1, i, j) = MATRIX_ELEM(*m0, j, i);\
	MATRIX_ELEM(*m1, j, i) = temp;

void matrix_transpose(matrix_t *m0, matrix_t *m1)
{
	double temp;
	TRANS_ELEM(m0, m1, 0, 1);
	TRANS_ELEM(m0, m1, 0, 2);
	TRANS_ELEM(m0, m1, 0, 3);
	TRANS_ELEM(m0, m1, 1, 2);
	TRANS_ELEM(m0, m1, 1, 3);
	TRANS_ELEM(m0, m1, 2, 3);
}

void matrix_normal_transf(matrix_t *m0, matrix_t *m1)
{
	matrix_t temp;
	matrix_inv(m0, &temp);
	matrix_transpose(&temp, m1);
}
