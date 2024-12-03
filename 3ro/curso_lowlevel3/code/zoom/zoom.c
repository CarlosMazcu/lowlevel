

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
      unsigned int* tex, int tex_pow2_w, int tex_pow2_h,
      float u0, float v0, float step_u, float step_v)
{
  int x,y;
  // Pasamos los parametros del zoom a coma fija para optimizar
  // La parte fraccionaria que hemos elegido es de 12 bits; esto quiere decir 
  // que cada entero tendra 4096 (2^12)
  // partes. 
  int iu0 = (int)(u0 * 4096.f);
  int iv0 = (int)(v0 * 4096.f);
  int istep_u = (int)(step_u * 4096.f);
  int istep_v = (int)(step_v * 4096.f);
  // Mascaras de la textura, para accederlas ciclicamente
  int tw = 1 << tex_pow2_w; 
  int th = 1 << tex_pow2_h;
  int tmw = tw - 1;
  int tmh = th - 1;
  for ( y=0; y < h; y++) {
    int iu = iu0;
    // Puntero de la linea de pantalla donde amos a dibujar
    unsigned int* line = scr + (y * stride_pixels);
    // Puntero de la linea de textura: quitamos la parte fraccionaria a la 
    // coordenada V ( >> 12) y enmascaramos para el acceso ciclico ( & tmh)
    unsigned int* tex_line = tex + (tw * ((iv0 >> 12) & tmh));
    // INNER LOOP
    // Este bucle es el que se lleva casi todo el tiempo de CPU. Una forma de ayudar
    // a que el compilador optimice es sacar el loop a una funcion aparte (el porque
    // se vera en las clases de ensamblador) 
    for ( x=0; x < w; x++) {
      // Leer texel: quitamos la parte fraccionaria a la coordenada U ( >> 12) y
      // enmascaramos para el acceso ciclico ( & tmw)
      unsigned int texel = tex_line [(iu >> 12) & tmw];
      // La textura tiene orden ABGR, y la pantalla ARGB. Swapeamos componentes
      // Idealmente esto se puede sacar el inner loop, preparando la textura antes del run time
      texel = (texel & 0xff00ff00) |  // Alpha y Green se quedan como estan
        ((texel & 0x00ff0000) >> 16) |  // Blue para abajo
        ((texel & 0x000000ff) << 16);  // Red arriba
      line [x] = texel; 
      // Siguiente texel
      iu += istep_u;
    }
    // Siguiente linea de textura
    iv0 += istep_v;
  }
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

    float u0 = 0.0f, v0 = 0.0f;
    // Sacamos un factor de zoom a partir de la posicion del raton
    float step = ((float) mouse_x) / 300.0f;

    // Lock screen to get access to the memory array
    SDL_LockSurface( g_SDLSrf);

    ChronoWatchReset();

    // Opcional: Centrar la textura en pantalla
    // Si el zoom minimiza lo suficiente, las coordenadas resultantes seran negativas
    // No ocurre nada, ya que la mascara empleada en el zoom elimina todos los bits
    // fuera del rango de la textura, incluyendo los de signo
    u0 = ((float)(gimp_image.w >> 1)) - step * (float)(g_SDLSrf->w >> 1);
    v0 = ((float)(gimp_image.h >> 1)) - step * (float)(g_SDLSrf->h >> 1);

    Zoom ((unsigned int *) g_SDLSrf->pixels,
       g_SDLSrf->w , g_SDLSrf->h, g_SDLSrf->pitch >> 2,  // Leer doc. SDL sobre pitch!
       (unsigned int*) &gimp_image.pixel_data[0],
       GetPow2 (gimp_image.w),
       GetPow2 (gimp_image.h),
       u0, v0, step, step);

    ChronoShow ( "Zoom", g_SDLSrf->w * g_SDLSrf->h);
    
    // Liberar y volcado a la pantalla real  
    ChronoWatchReset();
    SDL_UnlockSurface( g_SDLSrf);
    SDL_Flip( g_SDLSrf);
    //ChronoShow ( "Screen dump", g_SDLSrf->w * g_SDLSrf->h);
    
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


