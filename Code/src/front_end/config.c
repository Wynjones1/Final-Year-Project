#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "maths.h"
#include "utils.h"

config_t g_config;

void read_command_line_arguments(int argc, char **argv, config_t *out)
{
	int i;
	for(i = 1; i < argc - 1; i++)
	{
		char *arg = argv[i];
		if(strcmp(arg, "--width") == 0 || strcmp(arg, "-w") == 0)
		{
			sscanf(argv[i + 1], "%d", &out->width);
			i++;
			continue;
		}
		else if(strcmp(arg, "--height") == 0|| strcmp(arg, "-h") == 0)
		{
			sscanf(argv[i + 1], "%d", &out->height);
			i++;
			continue;
		}
		else if(strcmp(arg, "--photons") == 0|| strcmp(arg, "-p") == 0)
		{
			sscanf(argv[i + 1], "%d", &out->photons);
			i++;
			continue;
		}
		else if(strcmp(arg, "--output") == 0|| strcmp(arg, "-o") == 0)
		{
			sscanf(argv[i + 1], "%s", out->output_filename);
			i++;
			continue;
		}
		else if(strcmp(arg, "--input") == 0|| strcmp(arg, "-i") == 0)
		{
			sscanf(argv[i + 1], "%s", out->scene_filename);
			i++;
			continue;
		}
		else if(strcmp(arg, "--depth") == 0|| strcmp(arg, "-d") == 0)
		{
			sscanf(argv[i + 1], "%d", &out->ray_depth);
			i++;
			continue;
		}
		else if(strcmp(arg, "--threads") == 0|| strcmp(arg, "-t") == 0)
		{
			sscanf(argv[i + 1], "%d", &out->num_threads);
			i++;
			continue;
		}
		else if(strcmp(arg, "--shadows") == 0|| strcmp(arg, "-s") == 0)
		{
			sscanf(argv[i + 1], "%d", &out->shadows);
			i++;
			continue;
		}
		else if(strcmp(arg, "--time") == 0)
		{
			sscanf(argv[i + 1], "%lf", &out->time);
			i++;
			continue;
		}
		else if(strcmp(arg, "--trace_pixel") == 0)
		{
			sscanf(argv[i + 1], "%d,%d", &out->trace_pixel_x, &out->trace_pixel_y);
			out->trace_pixel = 1;
			i++;
			continue;
		}
		else if(strcmp(arg, "--radiance_est") == 0 || strcmp(arg, "-r") == 0)
		{
			sscanf(argv[i + 1], "%d,%lf", &out->radiance_est_num, &out->radiance_est_radius);
			i++;
			continue;
		}
		else if(strcmp(arg, "--samples") == 0)
		{
			sscanf(argv[i + 1], "%d", &out->samples);
			i++;
			continue;
		}
		ERROR("Invalid Option: %s\n", arg);
	}
}

void read_config_file(const char *filename, config_t *out)
{
	char buf[1024];
	errno = 0;
	FILE *fp = OPEN(filename, "r");
	int count = 0;
	while(fgets(buf, 1024, fp))
	{
		count++;
		if(sscanf(buf, "width %d",   &out->width))           continue;
		if(sscanf(buf, "height %d",  &out->height))          continue;
		if(sscanf(buf, "photons %d", &out->photons))         continue;
		if(sscanf(buf, "out %s",     out->output_filename))  continue;
		if(sscanf(buf, "in  %s",     out->scene_filename))   continue;
		if(sscanf(buf, "depth %d",   &out->ray_depth))       continue;
		if(sscanf(buf, "threads %d", &out->num_threads))     continue;
		if(sscanf(buf, "shadows %d", &out->num_threads))     continue;
		if(sscanf(buf, "time %lf",   &out->time))            continue;
		if(strncmp(buf, "#", 1) == 0)                        continue;
		ERROR("%s:%d Invalid option: %s\n", filename, count, buf);
	}
	fclose(fp);
}

void config_read(const char *filename, int argc, char **argv)
{
	g_config.ray_depth           = CONFIG_DEFAULT_RAY_DEPTH;
	g_config.photon_depth        = CONFIG_DEFAULT_PHOTON_DEPTH;
	g_config.width               = CONFIG_DEFAULT_WIDTH;
	g_config.height              = CONFIG_DEFAULT_HEIGHT;
	g_config.photons             = CONFIG_DEFAULT_PHOTON_COUNT;
	g_config.num_threads         = CONFIG_DEFAULT_THREAD_COUNT;
	g_config.shadows             = CONFIG_DEFAULT_SHADOWS;
	g_config.time                = CONFIG_DEFAULT_TIME;
	g_config.trace_pixel         = CONFIG_DEFAULT_TRACE_PIXEL;
	g_config.trace_pixel_x       = CONFIG_DEFAULT_TRACE_PIXEL_X;
	g_config.trace_pixel_y       = CONFIG_DEFAULT_TRACE_PIXEL_Y;
	g_config.radiance_est_num    = CONFIG_DEFAULT_RADIANCE_NUM;
	g_config.radiance_est_radius = CONFIG_DEFAULT_RADIANCE_RAD;
	g_config.samples             = CONFIG_DEFAULT_SAMPLES;
	sprintf(g_config.output_filename, "%s", CONFIG_DEFAULT_OUTPUT_BMP);

	read_config_file(filename, &g_config);
	read_command_line_arguments(argc, argv, &g_config);
	g_config.num_threads = max(1, g_config.num_threads);

	VERBOSE("------------------------\n");
	VERBOSE("width      %d\n", g_config.width);
	VERBOSE("height     %d\n", g_config.height);
	VERBOSE("photons    %d\n", g_config.photons);
	VERBOSE("out        %s\n", g_config.output_filename);
	VERBOSE("in         %s\n", g_config.scene_filename);
	VERBOSE("depth      %d\n", g_config.ray_depth);
	VERBOSE("threads    %d\n", g_config.num_threads);
	VERBOSE("rad count  %d\n", g_config.radiance_est_num);
	VERBOSE("rad radius %f\n", g_config.radiance_est_radius);
	VERBOSE("------------------------\n");
}
