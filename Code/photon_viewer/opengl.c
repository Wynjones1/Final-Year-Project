#include "opengl.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <GL/glu.h>


void Display_InitGL()
{
    /* Enable smooth shading */
    glShadeModel( GL_SMOOTH );

    /* Set the background black */
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    /* Depth buffer setup */
    glClearDepth( 1.0f );

    /* Enables Depth Testing */
    glEnable( GL_DEPTH_TEST );

    /* The Type Of Depth Test To Do */
    glDepthFunc( GL_LEQUAL );

    /* Really Nice Perspective Calculations */
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
}
/* function to reset our viewport after a window resize */
int Display_SetViewport( int width, int height )
{
    /* Height / width ration */
    GLfloat ratio;

    /* Protect against a divide by zero */
    if ( height == 0 ) {
        height = 1;
    }

    ratio = ( GLfloat )width / ( GLfloat )height;

    /* Setup our viewport. */
    glViewport( 0, 0, ( GLsizei )width, ( GLsizei )height );

    /* change to the projection matrix and set our viewing volume. */
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );

    /* Set our perspective */
    gluPerspective( 45.0f, ratio, 0.1f, 100.0f );

    /* Make sure we're chaning the model view and not the projection */
    glMatrixMode( GL_MODELVIEW );

    /* Reset The View */
    glLoadIdentity( );

    return 1;
}


SDL_Renderer *initGL()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* displayWindow;
    SDL_Renderer* displayRenderer;
    SDL_RendererInfo displayRendererInfo;
    SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_OPENGL, &displayWindow, &displayRenderer);
    SDL_GetRendererInfo(displayRenderer, &displayRendererInfo);
    /*TODO: Check that we have OpenGL */
    if ((displayRendererInfo.flags & SDL_RENDERER_ACCELERATED) == 0 || 
        (displayRendererInfo.flags & SDL_RENDERER_TARGETTEXTURE) == 0) {
        /*TODO: Handle this. We have no render surface and not accelerated. */
    }
    
    
    Display_InitGL();
    
    Display_SetViewport(800, 600);
	return displayRenderer;
}
