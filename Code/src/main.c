#include "scene.h"
#include "render.h"
#include "utils.h"
#include "config.h"
#include "gui.h"
#include <stdlib.h>

int main(int argc, char **argv)
{
	VERBOSE("Starting Raytracer.\n");

	config_read("./data/settings.cfg", argc, argv);
	VERBOSE("Reading Scene.\n");
	scene_t  *scene  = scene_read(g_config.scene_filename);

#if GUI
	gui_t *gui = gui_new(400, 400, g_config.width, g_config.height);
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
	return 0;
}
