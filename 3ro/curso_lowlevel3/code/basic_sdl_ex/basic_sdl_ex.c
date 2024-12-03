
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <SDL/SDL.h>


int main ( int argc, char **argv)
{
    SDL_Surface  *surface;

    int mouse_x = 0;
    int mouse_y = 0;
    int req_w = 640;
    int req_h = 480;

    // Init SDL and screen
    if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0 ) {
        fprintf(stderr, "Can't Initialise SDL: %s\n", SDL_GetError());
        exit(1);
    }

    if (0 == SDL_SetVideoMode( req_w, req_h, 32,  SDL_HWSURFACE | SDL_DOUBLEBUF)) {
        printf("Couldn't set %dx%dx32 video mode: %s\n", req_w, req_h, SDL_GetError());
        return 0;
    }

    surface = SDL_GetVideoSurface();

    int end = 0;
    while ( !end) { 
        int x,y;
        SDL_Event event;

        // Lock screen to get access to the memory array
        SDL_LockSurface( surface);
        unsigned int* screen_pixels = (unsigned int *) surface->pixels;

        screen_pixels [ mouse_x + mouse_y * (surface->pitch >> 2)] = rand();

        /*for ( y=0; y < g_SDLSrf->h; y++)
        {
            for ( x=0; x < g_SDLSrf->w; x++)
            {
            }
        }*/
               
        SDL_UnlockSurface( surface);
        SDL_Flip( surface);

        // Check input events
        while ( SDL_PollEvent(&event)) {
            switch (event.type)  {
                case SDL_MOUSEMOTION:
                    mouse_x = event.motion.x;
                    mouse_y = event.motion.y;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    //printf("Mouse button %d pressed at (%d,%d)\n",
                    //       event.button.button, event.button.x, event.button.y);
                    break;
                case SDL_QUIT:
                    end = 1;
                    break;
            }
        }
    }

    return 0;
}


