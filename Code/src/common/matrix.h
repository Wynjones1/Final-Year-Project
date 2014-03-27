#ifndef MATRIX_H
#define MATRIX_H
#include "vector.h"
#include "maths.h"
#define MATRIX_ROW_MAJOR 1
#define DEG(x) (x * PI / 180)

typedef struct matrix
{
	double data[16];
}matrix_t;

#if MATRIX_ROW_MAJOR
#define MATRIX_ELEM(m, r, c) (m).data[r * 4 + c]
#else
#define MATRIX_ELEM(m, r, c) (m).data[c * 4 + r]
#endif

matrix_t matrix_translation(vector_t t);
matrix_t matrix_rotation_x(double theta);
matrix_t matrix_rotation_y(double theta);
matrix_t matrix_rotation_z(double theta);
matrix_t matrix_new (double a0, double a1, double a2, double a3,
					 double b0, double b1, double b2, double b3,
					 double c0, double c1, double c2, double c3,
					 double d0, double d1, double d2, double d3);

matrix_t matrix_new_identity(void);
void     matrix_identity(matrix_t *m);

void     matrix_init(matrix_t *m,
					 double a0, double a1, double a2, double a3,
					 double b0, double b1, double b2, double b3,
					 double c0, double c1, double c2, double c3,
					 double d0, double d1, double d2, double d3);

void matrix_translate(matrix_t *m, vector_t v0);
void matrix_rotate_x(matrix_t  *m, double theta);
void matrix_rotate_y(matrix_t  *m, double theta);
void matrix_rotate_z(matrix_t  *m, double theta);

void matrix_mult(matrix_t *a, matrix_t *b, matrix_t *out);
void matrix_mult_vec(matrix_t *a, vector_t v0, vector_t v1);
void matrix_mult_norm(matrix_t *a, vector_t v0, vector_t v1);

matrix_t matrix_inv_new(matrix_t *m);
void     matrix_inv(matrix_t *m, matrix_t *inv);

void     matrix_transpose(matrix_t *m0, matrix_t *m1);

void     matrix_normal_transf(matrix_t *m0, matrix_t *m1);

matrix_t matrix_random_i();
void matrix_print(matrix_t *m);

#endif
