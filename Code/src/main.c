#include "scene.h"
#include "render.h"
#include "utils.h"
#include "sampler.h"
#include "config.h"
#include "gui.h"
#include <stdlib.h>
#include "projection.h"


int main(int argc, char **argv)
{
#if 1
	VERBOSE("Starting Raytracer.\n");

	config_read("./data/settings.cfg", argc, argv);
	VERBOSE("Reading Scene.\n");
	scene_t  *scene  = scene_read(g_config.scene_filename);

#if GUI
	gui_t *gui = gui_new(800, 800, g_config.width, g_config.height);
	gui_start(gui);
	scene_register_pixel_update(scene, (pixel_update_func) gui_write, gui);
#endif

	bmp_t *bmp = bmp_init(g_config.output_filename, g_config.width, g_config.height);
	scene_register_pixel_update(scene, (pixel_update_func) bmp_write, bmp);

	render(scene);

	//Cleanup.
	scene_delete(scene);
	bmp_delete(bmp);
#if GUI
	gui_wait(gui);
#endif
#else
	double x[3];
	double y[3];
	double z[3] = {0, 1, 0};
	vector_normal(z, z);
	maths_basis(z, x, y);


	FILE *fp = OPEN("photon_map0.pmap", "w");
	g_config.samples = 100;
	fprintf(fp, "%d\n", g_config.samples * g_config.samples);
	double out[3];
	for(int i = 0; i < g_config.samples; i++)
	{
		for(int j = 0; j < g_config.samples; j++)
		{
			sample_hemi_cosine(z, out);
			fprintf(fp, "%lf %lf %lf 1 1 1\n", out[0], out[1], out[2]);
		}
	}
	fclose(fp);
#endif
	return 0;
}
