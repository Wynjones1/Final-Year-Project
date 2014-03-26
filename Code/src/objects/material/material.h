#ifndef MATERIAL_H
#define MATERIAL_H
#include "texture.h"

typedef struct material
{
	int        pmedia;

	double     extinction[3];
	double     scattering[3];
	double     absorption[3];
	double     albedo[3];

	double     av_ext;
	double     av_scatter;
	double     av_abs;
	double     av_albedo;
	
	double     reflectivity[3];
	double     refractivity[3];
	double     diffuse[3];
	double     ior;
	texture_t *texture;

	double av_diff;
	double av_refl;
	double av_refr;
}material_t;

void material_init(material_t *material, const char *filename);
void material_delete(material_t *material);
material_t *material_read(const char *filename);
#endif
