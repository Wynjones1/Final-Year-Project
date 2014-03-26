#ifndef GUI_H
#define GUI_H
#include <stdint.h>

#if GUI
	#include <pthread.h>

	typedef struct gui gui_t;

	gui_t *gui_new(int window_w, int window_h, int bmp_w, int bmp_h);
	void   gui_write(gui_t *gui, int x, int y, uint32_t data);
	pthread_t gui_start(gui_t *g);
	void gui_wait(gui_t *gui);
	void gui_refresh(gui_t *g);
#endif

#endif
