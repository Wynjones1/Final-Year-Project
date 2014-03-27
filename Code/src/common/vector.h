#ifndef VECTOR_H
#define VECTOR_H
#include <stdio.h>

#define INLINE 
typedef double          vector_float_t;
typedef vector_float_t  vector_t[3];
typedef vector_float_t *vector_ptr_t;

INLINE void            vector_print(vector_t vector);
INLINE void            vector_fprint(FILE *fp, vector_t vector);
INLINE void            vector_add(vector_t v1,vector_t v2,vector_t vout);
INLINE void            vector_sub(vector_t v1,vector_t v2,vector_t vout);
INLINE void            vector_mult(vector_float_t c,vector_t v1,vector_t vout);
INLINE void            vector_div(vector_float_t c,vector_t v1,vector_t vout);
INLINE void            vector_cross(vector_t v1,vector_t v2,vector_t vout);
INLINE vector_float_t  vector_length(vector_t v1);
INLINE void            vector_normal(vector_t v1,vector_t vout);
INLINE vector_float_t  vector_dot(vector_t v1,vector_t v2);
INLINE vector_float_t  vector_squared_dist(vector_t v1, vector_t v2);
INLINE void            vector_neg(vector_t v1);
INLINE void            vector_copy(vector_t v1, vector_t v2);
INLINE void            vector_addc(vector_t v1, vector_float_t c, vector_t v2);
INLINE void            vector_mult_const(vector_t v1, vector_float_t c);
INLINE void            vector_add_const(vector_t v1, vector_float_t c);
INLINE vector_float_t  vector_distance(vector_t v0, vector_t v1);

#endif
