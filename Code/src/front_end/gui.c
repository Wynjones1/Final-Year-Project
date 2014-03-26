#if GUI

#include "gui.h"
#include <stdlib.h>
#include <SDL.h>
#include <GL/gl.h>
#include <pthread.h>
#include "queue.h"
#include <stdbool.h>

typedef struct
{
	int x, y, data;
}write_data_t;

struct gui
{
	int           w;
	int           h;
	int window_w, window_h;
	GLuint        tex;
	SDL_Window    *window;
	SDL_GLContext context;
	queue_t *write_queue;
	pthread_t thread;

	int    write;
	int    write_x;
	int    write_y;
	int    write_data;
	bool   running;
};

gui_t *gui_new(int window_w, int window_h, int bmp_w, int bmp_h)
{
	gui_t *out = malloc(sizeof(gui_t));
	out->w     = bmp_w;
	out->h     = bmp_h;
	out->window_w = window_w;
	out->window_h = window_h;
	out->write = 0;
	out->write_queue = queue_init(10000, sizeof(write_data_t));
	out->window = NULL;
	out->running = true;

	return out;
}

void gui_stop(gui_t *gui)
{
	SDL_GL_DeleteContext(gui->context);
	SDL_DestroyWindow(gui->window);
	gui->running = false;
}

void gui_delete(gui_t *gui)
{
	free(gui);
}


void gui_write(gui_t *gui, int x, int y, uint32_t data)
{
	if(gui->running)
	{
		write_data_t wdata;
		wdata.x = x;
		wdata.y = y;
		wdata.data = data;
		queue_write(gui->write_queue, &wdata);
	}
}

void gui_refresh(gui_t *g)
{
	write_data_t data;
	while(queue_read_nb(g->write_queue, &data))
		glTexSubImage2D(GL_TEXTURE_2D, 0, data.x, data.y, 1, 1, GL_BGR, GL_UNSIGNED_BYTE, &data.data);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex2f(-1, 1);
	glTexCoord2f(1, 1); glVertex2f(1, 1);
	glTexCoord2f(1, 0); glVertex2f(1, -1);
	glTexCoord2f(0, 0); glVertex2f(-1, -1);
	glEnd();
	SDL_GL_SwapWindow(g->window);
}

static void *gui_thread(void *d)
{
	gui_t *g = d;
	SDL_Event ev;
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		fprintf(stderr, "Error initiing sdl.\n");
		abort();
	}

	g->window = SDL_CreateWindow("Raytracer", 100, 100, g->window_w, g->window_h, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if(g->window == NULL)
	{
		fprintf(stderr, "Error creating sdl window.\n");
		abort();
	}
	g->context = SDL_GL_CreateContext(g->window);
	glEnable(GL_TEXTURE_2D);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	char *tex_data = calloc(4, g->w * g->h);
	glGenTextures(1, &g->tex);
	glBindTexture(GL_TEXTURE_2D, g->tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g->w, g->h, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);
	free(tex_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	while(1)
	{
		while(SDL_PollEvent(&ev))
		{
			if((ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) || ev.type == SDL_QUIT)
			{
				gui_stop(g);
				//TODO:Figure out how to end gracefully.
				exit(0);
				return NULL;
			}
		}
		gui_refresh(g);
	}
}

pthread_t gui_start(gui_t *g)
{
	pthread_create(&g->thread, NULL, gui_thread, g);
	return g->thread;
}

void gui_wait(gui_t *gui)
{
	pthread_join(gui->thread, NULL);
}

#endif
