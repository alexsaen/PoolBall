/*  
	Copyright (c) 2012, Alexey Saenko
	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/ 

#include "opengl.h"
#include "ResourceManager.h"
#include "Chapter.h"

#include <SDL/SDL.h>
#include <stdlib.h>
#include <time.h>
#include <sstream>

#undef main

#ifdef _DEBUG
	#define USE_FULL_SCREEN 0
#else
	#define USE_FULL_SCREEN 1
#endif

const char *windowTitle = "Pool Ball Demo";

int main(int argc, char* argv[]) {

	Main *game = new Main("assets.zip");
	srand ( (unsigned)time(NULL) );

    /* First, initialize SDL's video subsystem. */
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        fprintf( stderr, "Video initialization failed: %s\n", SDL_GetError( ) );
		exit(1);
    }

	SDL_WM_SetCaption(windowTitle, windowTitle);

	/* Let's get some video information. */
    const SDL_VideoInfo* info = SDL_GetVideoInfo( );

    if( !info ) {
        fprintf( stderr, "Video query failed: %s\n", SDL_GetError( ) );
		exit(1);
    }

//    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

#if USE_FULL_SCREEN
	int width = 0, height = 0;
    int flags = SDL_OPENGL | SDL_FULLSCREEN;
#else
	int width = 1024;
	int height = 576;
	int flags = SDL_OPENGL;
#endif

	SDL_Surface* mainSurface =  SDL_SetVideoMode( width, height, info->vfmt->BitsPerPixel, flags );
	// Set the video mode
    if( !mainSurface ) {
        fprintf( stderr, "Video mode set failed: %s\n", SDL_GetError( ) );
		exit(1);
    }
	width = mainSurface->w;
	height = mainSurface->h;

	if (glewInit() != GLEW_OK) {
		fprintf( stderr, "Error in glewInit\n" );
		exit(1);
	}

	static unsigned tick =  SDL_GetTicks();

	game->reshape(width, height, 0);

	int touchID = 0;

	while(!game->finished()) {

		SDL_Event event;
		while( SDL_PollEvent( &event ) ) {
			switch( event.type ) {
				case SDL_MOUSEBUTTONDOWN:
					switch(event.button.button) {
						case 1:
							game->touchBegan(0, event.button.x, event.button.y); 
							break;
						case 2:
							break;
						case 3:
							touchID = 1;
							game->touchBegan(touchID, event.button.x, event.button.y); 
							break;
						case 4:
						case 5:
							game->keyDown(event.button.button);
							break;
					}
					break;
				case SDL_MOUSEBUTTONUP:
					switch(event.button.button) {
						case 1:
							game->touchEnded(0); 
							break;
						case 3:
							game->touchEnded(touchID); 
							touchID = 0;
							break;
					}
				case SDL_MOUSEMOTION:
					game->touchMove(touchID, event.motion.x, event.motion.y); 
					break;

				case SDL_KEYDOWN:
					switch( event.key.keysym.sym ) {
						case SDLK_ESCAPE:
							return 0;
							break;
						case SDLK_SPACE:
							break;
						case SDLK_1:
						case SDLK_2:
						case SDLK_3:
						case SDLK_4:
						case SDLK_5:
							break;
						case SDLK_F1:
							game->suspend();
							game->release();
							break;
						case SDLK_F2:
							game->reshape(width, height, 0);
							game->resume();
							break;
						case SDLK_F3:
							game->suspend();
							game->release();
							delete game;
							game = new Main("assets.zip");
							game->reshape(width, height, 0);
							game->resume();
							break;
						case SDLK_F4:
						case SDLK_F5:
						case SDLK_F6:
						case SDLK_F7:
						case SDLK_F8:
						case SDLK_F9:
							break;
						case SDLK_F10:
							return 0;
							break;
						case SDLK_F11:
							break;
						default:
							game->keyDown(event.key.keysym.sym);
							break;
					}
					break;
				case SDL_QUIT:
					/* Handle quit requests (like Ctrl-c). */
					SDL_Quit();
					return 0;
			}

		}

		game->draw();
		SDL_GL_SwapBuffers();
		SDL_Delay(20);	
	}

	return 0;
}

