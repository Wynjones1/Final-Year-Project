#include "scene.h"
#include "render.h"
#include "utils.h"
#include "sampler.h"
#include "config.h"
#include "gui.h"
#include <stdlib.h>
#include "projection.h"

void sub_image(void *_data, int x, int y, uint32_t data)
{
	static bmp_t *bmp;
	if(!bmp)
	{
		bmp = bmp_init("sub_image.bmp", 20, 20);
	}
	int subx = 45;
	int suby = 10;
	int w = 20;
	int h = 20;
	if(x - subx < w && x - subx > 0)
	{
		if(y - suby < h && y - suby > 0)
		{
			bmp_write(bmp, x - subx, y - suby, data);
		}
	}
}

int main(int argc, char **argv)
{
#if 1
	VERBOSE("Starting Raytracer.\n");

	config_read("./data/settings.cfg", argc, argv);
	VERBOSE("Reading Scene.\n");
	scene_t  *scene  = scene_read(g_config.scene_filename);

#if GUI
	gui_t *gui;
	gui = gui_new(800, 800, g_config.width, g_config.height);
	if(!g_config.trace_pixel)
	{
		gui_start(gui);
		scene_register_pixel_update(scene, (pixel_update_func) gui_write, gui);
	}
#endif

	bmp_t *bmp = bmp_init(g_config.output_filename, g_config.width, g_config.height);
	scene_register_pixel_update(scene, (pixel_update_func) bmp_write, bmp);
	scene_register_pixel_update(scene, sub_image, NULL);

	render(scene);

	//Cleanup.
//	scene_delete(scene);
	bmp_delete(bmp);
#if GUI
	if(!g_config.trace_pixel)
	{
		gui_wait(gui);
	}
#endif
#else
	double x[3];
	double y[3];
	double z[3] = {10, 3, 4};
	vector_normal(z, z);
	maths_basis(z, x, y);


	FILE *fp = OPEN("photon_map0.pmap", "w");
	g_config.samples = 50;
	fprintf(fp, "%d\n", g_config.samples * g_config.samples);
	double out[3];
	for(int i = 0; i < g_config.samples; i++)
	{
		for(int j = 0; j < g_config.samples; j++)
		{
			//sample_sphere_strat(i, j,g_config.samples, out);
			sample_hemi_jitter(z, out, i, j, g_config.samples);
			//sample_sphere(out);
			fprintf(fp, "%lf %lf %lf 1 1 1\n", out[0], out[1], out[2]);
		}
	}
	fclose(fp);
#endif
	return 0;
}
