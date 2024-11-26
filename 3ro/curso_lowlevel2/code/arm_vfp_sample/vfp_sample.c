

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include <SDL/SDL.h>

#include "chrono.h"

// ---------------------------------------------------------------------------

static int cpu_mhz = 0;
static int dump = 0;

static void ChronoShow ( char* name, int computations)
{
  float ms = ChronoWatchReset();
  float cycles = ms * (1000000.0f/1000.0f) * (float)cpu_mhz;
  float cyc_per_comp = cycles / (float)computations;
  if ((dump & 7) == 0)
    fprintf ( stdout, "%s: %f ms, %d cycles, %f cycles/loop\n", name, ms, (int)cycles, cyc_per_comp);
}



// Limitar el framerate
// (evitar que el refresco de pantalla sea mas rapido que 60 frames/segundo) 

static void FramerateLimit (int max_fps)
{   
  static unsigned int frame_time = 0;

  unsigned int t = GetMsTime();
  unsigned int elapsed = t - frame_time;
  // El tiempo sobrante se "regala" al sistema operativo con la funcion POSIX sleep
  const unsigned int limit = 1000 / max_fps;  // 1000 miliseconds / framerate minimo deseado 
  if ( elapsed < limit)
    usleep (( limit - elapsed) * 1000); // arg in microseconds
  frame_time = GetMsTime();
}

// ---------------------------------------------------------------------------

#if 0
extern void RotateVertices (float* vert_out, float* vert_in, int num_vertices, 
                             float* matrix3x3);

#else
static void RotateVertices (float* vert_out, float* vert_in, int num_vertices, 
                             float* matrix3x3)
{
  while(num_vertices > 0) {
    float vx = *vert_in++;
    float vy = *vert_in++;
    float vz = *vert_in++;

    float x = vx * matrix3x3[0] + vy * matrix3x3[1] + vz * matrix3x3[2];
    float y = vx * matrix3x3[3] + vy * matrix3x3[4] + vz * matrix3x3[5];
    float z = vx * matrix3x3[6] + vy * matrix3x3[7] + vz * matrix3x3[8];

    *vert_out++ = x;
    *vert_out++ = y;
    *vert_out++ = z;

    num_vertices--;
  }
}
#endif
// ---------------------------------------------------------------------------

typedef struct
{
  unsigned int* pixels;
  int stride;
  int w, h;
} t_screen;

#if 0
extern void DisplayVertices (float* vertices, int n_vertices, const t_screen* scr);
#else
static void DisplayVertices (float* vertices, int n_vertices, const t_screen* scr)
{
  int i;
  for(i = 0; i < n_vertices; i++) {
      int xp = (scr->w >> 1) + (int)vertices[i * 3 + 0];
      int yp = (scr->h >> 1) + (int)vertices[i * 3 + 1];
      scr->pixels [xp + yp * scr->stride] = i & 0xff;
  } 
}
#endif

static inline void Rot2D (float* x, float* y, float angle)
{
  float xc = *x, yc = *y;
  *x = xc * cosf (angle) - yc * sinf (angle);

  *y = xc * sinf (angle) + yc * cosf (angle);
}
static void RotMat (float* m, float axis_x, float axis_y)
{
  m[0] = 1.0f, m[1] = 0.0f, m[2] = 0.0f;
  m[3] = 0.0f, m[4] = 1.0f, m[5] = 0.0f;
  m[6] = 0.0f, m[7] = 0.0f, m[8] = 1.0f;

  Rot2D (&m[1], &m[2], axis_x);
  Rot2D (&m[4], &m[5], axis_x);
  Rot2D (&m[7], &m[8], axis_x);

  Rot2D (&m[0], &m[2], axis_y);
  Rot2D (&m[3], &m[5], axis_y);
  Rot2D (&m[6], &m[8], axis_y);
}

// ---------------------------------------------------------------------------

int main ( int argc, char** argv)
{
  int i = 0;
  int end = 0;
  int mouse_x = 0, mouse_y = 0;
  SDL_Surface  *g_SDLSrf;
  int req_w = 1024;
  int req_h = 768; 

  if ( argc < 2) { fprintf ( stderr, "I need the cpu speed in Mhz!\n"); exit(0);}
  cpu_mhz = atoi( argv[1]);
  assert(cpu_mhz > 0);
  fprintf ( stdout, "Cycle times for a %d Mhz cpu\n", cpu_mhz);

  // Init SDL and screen
  if ( SDL_Init( SDL_INIT_VIDEO) < 0 ) {
      fprintf(stderr, "Can't Initialise SDL: %s\n", SDL_GetError());
      exit(1);
  }
  if (0 == SDL_SetVideoMode( req_w, req_h, 32,  SDL_HWSURFACE | SDL_DOUBLEBUF)) {
      printf("Couldn't set %dx%dx32 video mode: %s\n", req_w, req_h, SDL_GetError());
      return 0;
  }

  //Prepare a test set of vertices, torus shaped.
  int n_vertices = 10000;
  float* torus = (float*) malloc (n_vertices * 3 * sizeof(float));
  float* vertices = (float*) malloc (n_vertices * 3 * sizeof(float));
  for(i = 0; i < n_vertices; i++) {
    const float r1 = 100.0f;
    const float r2 = 250.0f;
    float alpha = 2.0f * 3.1426f * (1.0f / 32768.0f) * (float)(rand() & 0x7fff);
    float beta  = 2.0f * 3.1426f * (1.0f / 32768.0f) * (float)(rand() & 0x7fff);
    torus[i * 3 + 0] = (r2 + r1 * cosf(beta)) * cosf(alpha);
    torus[i * 3 + 1] = (r2 + r1 * cosf(beta)) * sinf(alpha);
    torus[i * 3 + 2] = r1 * sinf(beta);
  } 

  g_SDLSrf = SDL_GetVideoSurface();

  while ( !end) { 

    SDL_Event event;

    float matrix3x3 [3 * 3] = {};

    RotMat (matrix3x3, 
            (1.0f/60.0f) * (float)mouse_x, 
            (1.0f/60.0f) * (float)mouse_y);

    ChronoWatchReset();

    RotateVertices (vertices, torus, n_vertices, matrix3x3);

    ChronoShow ( "Transform", n_vertices);

    // Visualizar el resultado
    // Lock screen to get access to the memory array
    SDL_LockSurface( g_SDLSrf);
    unsigned int* screen_pixels = (unsigned int *) g_SDLSrf->pixels;

    // Clean the screen
    SDL_FillRect(g_SDLSrf, NULL, SDL_MapRGB(g_SDLSrf->format, 0, 0, 0));
    ChronoShow ( "Clean", g_SDLSrf->w * g_SDLSrf->h);

    // Paint vertices
    t_screen scr = { screen_pixels, g_SDLSrf->pitch >> 2, g_SDLSrf->w, g_SDLSrf->h};
    DisplayVertices (vertices, n_vertices, &scr);
    ChronoShow ( "Preview", n_vertices);

    // Liberar y volcado a la pantalla real  
    ChronoWatchReset();
    SDL_UnlockSurface( g_SDLSrf);
    SDL_Flip( g_SDLSrf);
    ChronoShow ( "Screen dump", g_SDLSrf->w * g_SDLSrf->h);

    // Limitar el framerate y devolver el tiempo sobrante a la CPU
    FramerateLimit (60);

    dump++;

    // Recoger eventos de la ventana
    while ( SDL_PollEvent(&event) ) 
    {
      switch (event.type) 
      {
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

  return 1;
}


