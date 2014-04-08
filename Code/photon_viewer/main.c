#include "opengl.h"
#include "reader.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

SDL_Event event;

GLuint CreateBox(float *vertices, int n)
{
	GLuint points = glGenLists(1);
	glNewList(points, GL_COMPILE);
	glBegin( GL_POINTS);
		int i;
		for(i = 0; i < n; i++)
		{
			glVertex3fv(vertices+ i * 6);
			glColor3fv(vertices+ i * 6 + 3);
		}
	glEnd( );
	glEndList();
	return points;
}

void Render(SDL_Renderer *displayRenderer, GLuint list)
{
	static float angle;
	angle += 0.7;
	glLoadIdentity();
	glTranslatef( -0.0f, -2.0f, -15.0f );
	glRotatef(angle, 0.0, 1.0, 0.0);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glCallList(list);

	SDL_RenderPresent(displayRenderer);
}

void ProcessEvents(void)
{
		SDL_Event ev;
		while(SDL_PollEvent(&ev))
		{
			if((ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) || ev.type == SDL_QUIT)
			{
				exit(0);
			}
		}
}

void MainLoop(SDL_Renderer *r, int n, float *points)
{
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	GLuint list = CreateBox(points, n);
	while(1)
	{
		ProcessEvents();
		Render(r, list);
		SDL_Delay(30);
	}
}

int main(int argc, char *argv[])
{
	int num = 0;
	char *filename = "../photon_map0.pmap";
	float *points = reader_points(filename, &num);
	SDL_Renderer *render = initGL();
	MainLoop(render, num, points);
    return 0;
}
