#ifndef MATHS_H
#define MATHS_H
#include <math.h>

#define EPSILON     1e-12
#define BIG_EPSILON 1e-1
#define max(a, b) ( (a) > (b) ? (a) : (b))
#define min(a, b) ( (a) < (b) ? (a) : (b))
#define PI   3.14159265358979323846
#define PI_2 1.57079632679489661923

typedef struct ray ray_t;

double maths_calculate_refracted_ray(double incident[3], double normal[3], double ior, double out[3]);
void   maths_calculate_reflected_ray(double incident[3], double normal[3], double out[3]);
double maths_calculate_reflectance(double normal[3], double incident[3], double n1, double n2);
void   maths_calculate_intersection(ray_t *ray, double t, double out[3], int offset);
double maths_calculate_normal(double A[3], double B[3], double out[3]);
double maths_calculate_t(double origin, double normal, double pos);

void maths_spherical_to_vector(double theta, double phi, double out[3]);
void maths_vector_to_spherical(double in[3], double *theta, double *phi);

void maths_basis(double normal[3], double out0[3], double out1[3]);

double randf(double min, double max);

#endif
