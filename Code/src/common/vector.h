#ifndef VECTOR_H
#define VECTOR_H
#include <stdio.h>

#define INLINE 

INLINE void    vector_print(double *vector);
INLINE void    vector_fprint(FILE *fp, double *vector);
INLINE void    vector_add(double * v1,double * v2,double * vout);
INLINE void    vector_sub(double * v1,double * v2,double * vout);
INLINE void    vector_mult(double *v1,double * v2,double * vout);
INLINE void    vector_div(double c,double * v1,double * vout);
INLINE void    vector_cross(double * v1,double * v2,double * vout);
INLINE double  vector_length(double * v1);
INLINE void    vector_normal(double * v1,double * vout);
INLINE double  vector_dot(double *v1,double *v2);
INLINE double  vector_squared_dist(double *v1, double *v2);
INLINE void    vector_neg(double *v1);
INLINE void    vector_copy(double *v1, double *v2);
INLINE void    vector_addc(double *v1, double c, double *v2);
INLINE void    vector_mult_const(double *v1, double c);
INLINE void    vector_add_const(double *v1, double c);
INLINE double  vector_distance(double v0[3], double v1[3]);

#endif
