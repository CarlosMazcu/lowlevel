

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include <SDL/SDL.h>

#include "chrono.h"

// ---------------------------------------------------------------------------
// Textura

struct GimpScrFormat
{
  int  w;
  int  h;
  int  bytes_per_pixel; // 3:RGB, 4:RGBA
  unsigned char pixel_data[4];
};
  
extern struct GimpScrFormat gimp_image;

// ---------------------------------------------------------------------------
// Dado un numero, cual es su potencia de 2?
// (no acepta numeros diferentes a potencias de 2)

static inline int GetPow2 (int v)
{
  int pow = 0;
  while ((( v & (1<<pow)) == 0) && (pow < 32))
    pow++;
  
  assert(((1 << pow) == v) && "El parametro v no es potencia de 2");
  return pow;
}

// ---------------------------------------------------------------------------
// Funciones de ayuda para tiempos

static int cpu_mhz = 0;

static void ChronoShow ( char* name, int computations)
{
  float ms = ChronoWatchReset();
  float cycles = ms * (1000000.0f/1000.0f) * (float)cpu_mhz;
  float cyc_per_comp = cycles / (float)computations;
  fprintf ( stdout, "%s: %f ms, %d cycles, %f cycles/iteration\n", name, ms, (int)cycles, cyc_per_comp);
}

// Limitar el framerate. El tiempo sobrante se devuelve al OS con "sleep"
static void LimitFramerate (int fps) 
{  
  static unsigned int frame_time = 0;
  unsigned int t = GetMsTime();
  unsigned int elapsed = t - frame_time;
  // El tiempo sobrante se "regala" al sistema operativo con la funcion POSIX sleep
  const unsigned int limit = 1000 / fps;  // 1000 miliseconds / framerate minimo deseado 
  if ( elapsed < limit)
    usleep (( limit - elapsed) * 1000); // arg in microseconds
  frame_time = GetMsTime();
}

// ---------------------------------------------------------------------------
// Funcion principal (desde el punto de vista del coste de CPU)

static void Zoom (unsigned int* scr, int w, int h,  int stride_pixels,
      unsigned int* tex)
{
}

// ---------------------------------------------------------------------------

int main ( int argc, char** argv)
{
  int end = 0;
  int mouse_x = 0; 
  //int mouse_y = 0;
  SDL_Surface  *g_SDLSrf;
  int req_w = 640;
  int req_h = 480; 

  if ( argc < 2) { fprintf ( stderr, "I need the cpu speed in Mhz!\n"); exit(0);}
  cpu_mhz = atoi( argv[1]);
  assert(cpu_mhz > 0);
  fprintf ( stdout, "Cycle times for a %d Mhz cpu\n", cpu_mhz);

  // Init SDL and screen
  if (SDL_Init( SDL_INIT_VIDEO) < 0)  {
    fprintf(stderr, "Can't Initialise SDL: %s\n", SDL_GetError());
    exit(1);
  }
  if (0 == SDL_SetVideoMode( req_w, req_h, 32,  SDL_HWSURFACE | SDL_DOUBLEBUF)) {
    printf("Couldn't set %dx%dx32 video mode: %s\n", req_w, req_h, SDL_GetError());
    return 0;
  }

  g_SDLSrf = SDL_GetVideoSurface();

  while ( !end) { 
    SDL_Event event;

    // Lock screen to get access to the memory array
    SDL_LockSurface( g_SDLSrf);

    ChronoWatchReset();

    Zoom ((unsigned int *) g_SDLSrf->pixels,
       g_SDLSrf->w , g_SDLSrf->h, g_SDLSrf->pitch >> 2,  // Leer doc. SDL sobre pitch!
       (unsigned int*) &gimp_image.pixel_data[0]);


    // Liberar y volcado a la pantalla real  
    SDL_UnlockSurface( g_SDLSrf);
    SDL_Flip( g_SDLSrf);
  
    // Limitar el framerate
    // (evitar que el refresco de pantalla sea mas rapido que 60 frames/segundo) 
    LimitFramerate (60);

    // Recoger eventos de la ventana
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_MOUSEMOTION:
          mouse_x = event.motion.x;
          //mouse_y = event.motion.y;
          break;
        case SDL_MOUSEBUTTONDOWN:
          //printf("Mouse button %d pressed at (%d,%d)\n",
          //   event.button.button, event.button.x, event.button.y);
          break;
        case SDL_QUIT:
          end = 1;
          break;
      }
    }
  }

  return 1;
}


