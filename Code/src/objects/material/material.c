#include "material.h"
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "maths.h"

void material_init(material_t *material, const char *filename)
{
	FILE *fp = OPEN(filename, "r");
	char buf[1024];
	char tex_file[1024] = {'\0'};
	int count = -1;
	memset(material, 0x00, sizeof(material_t));

	double *c = material->diffuse;
	double *r = material->reflectivity;
	double *t = material->refractivity;

	double *pa = material->absorption;
	double *ps = material->scattering;
	while(fgets(buf, 1024, fp))
	{
		count++;
		if(sscanf(buf, "pmedia       %d", &material->pmedia) == 1)         continue;
		if(sscanf(buf, "diffuse      %lf %lf %lf", c, c + 1, c + 2) == 3)  continue;
		if(sscanf(buf, "reflectivity %lf %lf %lf", r, r + 1, r + 2) == 3)  continue;
		if(sscanf(buf, "refractivity %lf %lf %lf", t, t + 1, t + 2) == 3)  continue;
		if(sscanf(buf, "ior %lf", &material->ior))                         continue;
		if(sscanf(buf, "texture %s", tex_file))                            continue;
		if(sscanf(buf, "scatter    %lf %lf %lf", ps, ps + 1, ps + 2) == 3) continue;
		if(sscanf(buf, "absorption %lf %lf %lf", pa, pa + 1, pa + 2) == 3) continue;
		if(strncmp(buf, "#", 1) == 0)                                      continue;
		ERROR("Invalid Line in material file %s at line %d.\n", filename, count);
	}

	/*       Normal Material Properties          */
	material->texture = NULL;
	material->av_diff = (c[0] + c[1] + c[2]) / 3.0;
	material->av_refl = (r[0] + r[1] + r[2]) / 3.0;
	material->av_refr = (t[0] + t[1] + t[2]) / 3.0;

	if(tex_file[0] != '\0')
	{
		material->texture = texture_read(tex_file);
	}

	/*       Participating Media Properties      */
	for(int i = 0; i < 3; i++)
	{
		material->extinction[i] = pa[i] + ps[i];
		material->albedo[i] = ps[i] / material->extinction[i];
	}

	double *pe = material->extinction;
	double *pb = material->albedo;
	material->av_ext     = (pe[0] + pe[1] + pe[2]) / 3.0;
	material->av_scatter = (ps[0] + ps[1] + ps[2]) / 3.0;
	material->av_abs     = (pa[0] + pa[1] + pa[2]) / 3.0;
	material->av_albedo  = (pb[0] + pb[1] + pb[2]) / 3.0;

	fclose(fp);
}

material_t *material_read(const char *filename)
{
	material_t *out = calloc(1, sizeof(material_t));
	material_init(out, filename);
	return out;
}

void material_delete(material_t *material)
{
	texture_delete(material->texture);
	free(material);
}
