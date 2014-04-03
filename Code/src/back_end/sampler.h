#ifndef SAMPLER_H
#define SAMPLER_H

typedef struct sampler sampler_h;

sampler_h *sampler_uniform(double a, double b);

void sample_hemi_cosine(double normal[3], double out[3]);
void sample_hemi_cosine_jitter(double normal[3], double out[3], int x, int y, int num_samples);
void sample_hemi(double normal[3], double out[3]);
void sample_hemi_jitter(double normal[3], double x[3], double y[3], int xsample, int ysample, double out[3]);
void sample_sphere(double out[3]);

#endif
