#ifndef CONFIG_H
#define CONFIG_H
#include "bmp.h"

#define CONFIG_DEFAULT_WIDTH          128
#define CONFIG_DEFAULT_HEIGHT         128
#define CONFIG_DEFAULT_PHOTON_COUNT   10000
#define CONFIG_DEFAULT_RAY_DEPTH      10
#define CONFIG_DEFAULT_PHOTON_DEPTH   100
#define CONFIG_DEFAULT_THREAD_COUNT   4
#define CONFIG_DEFAULT_SHADOWS        1
#define CONFIG_DEFAULT_TIME           1.5
#define CONFIG_DEFAULT_TRACE_PIXEL    0
#define CONFIG_DEFAULT_TRACE_PIXEL_X  0
#define CONFIG_DEFAULT_TRACE_PIXEL_Y  0
#define CONFIG_DEFAULT_OUTPUT_BMP     "out.bmp"
#define CONFIG_DEFAULT_RADIANCE_NUM   50
#define CONFIG_DEFAULT_RADIANCE_RAD   0.5
#define CONFIG_DEFAULT_SAMPLES        1

typedef struct config
{
	int     width;
	int     height;
	int     photons;
	char    output_filename[1024];
	char    scene_filename[1024];
	int     ray_depth;
	int     photon_depth;
	int     num_threads;
	int     shadows;
	double  time;
	int     trace_pixel;
	int     trace_pixel_x;
	int     trace_pixel_y;
	int     radiance_est_num;
	int     samples;
	double  radiance_est_radius;
}config_t;

void config_read(const char *filename, int argc, char **argv);

extern config_t g_config;

#endif
