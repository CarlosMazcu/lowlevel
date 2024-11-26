
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <SDL/SDL.h>

/*
 Este ejemplo muestra como mover y proyectar puntos 3D
 en diferentes formatos numericos.
 Para vuestros practicas debeis fijaros primero en la
 version de coma flotante
*/

// Definicion de un cubo en 3 formatos numericos diferentes
// Entero, coma fija 8 bits y flotante

#define LCI (100)       // En enteros
#define LCX (100<<8)    // En fixed point, 8 bits
#define LCF (100.0f)    // En flotantes

// Enteros

typedef struct
{
  int x, y, z;
} PointI;

static const PointI cubei [8] = 
{
  {-LCI,-LCI, LCI},
  {-LCI, LCI, LCI},
  { LCI,-LCI, LCI},
  { LCI, LCI, LCI},

  {-LCI,-LCI, -LCI},
  {-LCI, LCI, -LCI},
  { LCI,-LCI, -LCI},
  { LCI, LCI, -LCI},
};

// Coma fija FX8, 8 bits de radix

typedef struct
{
  int x, y, z;
} Point_fx;

static const PointI cubefx [8] = 
{
  {-LCX,-LCX, LCX},
  {-LCX, LCX, LCX},
  { LCX,-LCX, LCX},
  { LCX, LCX, LCX},

  {-LCX,-LCX, -LCX},
  {-LCX, LCX, -LCX},
  { LCX,-LCX, -LCX},
  { LCX, LCX, -LCX},
};

// Flotantes

typedef struct
{
  float x, y, z;
} PointF;

static const PointF cubef [8] = 
{
    {-LCF,-LCF, LCF},
    {-LCF, LCF, LCF},
    { LCF,-LCF, LCF},
    { LCF, LCF, LCF},

    {-LCF,-LCF, -LCF},
    {-LCF, LCF, -LCF},
    { LCF,-LCF, -LCF},
    { LCF, LCF, -LCF},
};

// Estas funciones pintan un cubo a partir de su lista de vertices en la pantalla
// Para ello reciben un offset de translacion

static void PaintCubeInFloat ( unsigned int* pixels, float w, float h, int pitch, float trans_x, float trans_z, float proy)
{   
  int i;
  for ( i=0; i<8; i++) { 
    float xp, yp;
    float x = cubef [i].x;
    float y = cubef [i].y;
    float z = cubef [i].z;

    x += trans_x;
    z += trans_z;
    xp = (x * proy) / z;
    yp = (y * proy) / z;
    xp += w * 0.5f;
    yp += h * 0.5f;

    if (( xp >= 0.0f) && (xp < w) && (yp >= 0.0f) && (yp < h))
        pixels [ ((int)xp) + (((int)yp) * pitch)] = 0xff0000;
  }
}

static void PaintCubeInInteger ( unsigned int* pixels, int w, int h, int pitch, int trans_x, int trans_z, int proy)
{   
  int i;
  for ( i=0; i<8; i++) { 
    int xp, yp;
    int x = cubei [i].x;
    int y = cubei [i].y;
    int z = cubei [i].z;

    x += trans_x;
    z += trans_z;
    xp = (x * proy) / z;
    yp = (y * proy) / z;
    xp += w >> 1;
    yp += h >> 1;

    if (( xp >= 0) && (xp < w) && (yp >= 0) && (yp < h))
      pixels [ xp + (yp * pitch)] = 0xff00;
  }
}

static void PaintCubeInFixed ( unsigned int* pixels, int w, int h, int pitch, int trans_x8, int trans_z8, int proy)
{   
  int i;
  for ( i=0; i<8; i++) { 
    int xp, yp;
    int x = cubefx [i].x;
    int y = cubefx [i].y;
    int z = cubefx [i].z;

    x += trans_x8;
    z += trans_z8;
    // Division con coma fija
    // X o Z (fixed8) * Proj (simple int) = result fixed 8
    // Num. fixed 8 / Num. fixed 8 = fixed 0, o simple int
    xp = (x * proy) / z;
    yp = (y * proy) / z;
    // Centrar en pantalla
    xp += w >> 1;
    yp += h >> 1;

    if (( xp >= 0) && (xp < w) && (yp >= 0) && (yp < h))
      pixels [ xp + (yp * pitch)] = 0xffffff;
  }
}

int main ( int argc, char **argv)
{
  SDL_Surface  *g_SDLSrf;

  //int mouse_x = 0, mouse_y = 0;
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

  g_SDLSrf = SDL_GetVideoSurface();

  // Horizontal field of view
  float hfov = 60.0f * ((3.1416f * 2.0f) / 360.0f);  // De grados a radianes
  // Proyeccion usando la tangente
  float half_scr_w = (float)(g_SDLSrf->w >> 1);
  float projection = (1.0f / tanf ( hfov * 0.5f)) * half_scr_w;

  float ang = 0.0f;
  int end = 0;
  while ( !end) { 

    SDL_Event event;

    // Lock screen to get access to the memory array
    SDL_LockSurface( g_SDLSrf);

    // Borrar pantalla
    SDL_FillRect(g_SDLSrf, NULL, SDL_MapRGB(g_SDLSrf->format, 0, 0, 0));

    // Screen buffer data
    unsigned int* screen_pixels = (unsigned int *) g_SDLSrf->pixels;
    int pitch = g_SDLSrf->pitch >> 2;

    // Girar la posicion del cubo y pintar en pantalla
    // Idem en los 3 formatos numericos
    float x, z, offs_z = 1200.0f;
    x = 0.0f   + 300.0f * cos( ang + ((0.0f * 3.1416f * 2.0f) / 3.0f));
    z = offs_z + 300.0f * sin( ang + ((0.0f * 3.1416f * 2.0f) / 3.0f));
    PaintCubeInFloat ( screen_pixels, (float)g_SDLSrf->w, (float)g_SDLSrf->h, pitch, 
                       x, z, 
                       projection);

    x = 0.0f   + 300.0f * cos( ang + ((1.0f * 3.1416f * 2.0f) / 3.0f));
    z = offs_z + 300.0f * sin( ang + ((1.0f * 3.1416f * 2.0f) / 3.0f));
    PaintCubeInInteger ( screen_pixels, g_SDLSrf->w, g_SDLSrf->h, pitch, 
                       x, z, 
                       (int)projection);

    x = 0.0f   + 300.0f * cos( ang + ((2.0f * 3.1416f * 2.0f) / 3.0f));
    z = offs_z + 300.0f * sin( ang + ((2.0f * 3.1416f * 2.0f) / 3.0f));
    PaintCubeInFixed ( screen_pixels, g_SDLSrf->w, g_SDLSrf->h, pitch, 
                       (int)(x * 256.0f), (int)(z * 256.0f), 
                       (int)projection);

    ang += 0.02f;

    SDL_Delay ( 16);    // Forma simplona (no recomendada) de evitar framerates de vertigo

    SDL_UnlockSurface( g_SDLSrf);
    SDL_Flip( g_SDLSrf);

    // Check input events
    while ( SDL_PollEvent(&event) ) {
      switch (event.type) {
        case SDL_MOUSEMOTION:
          //mouse_x = event.motion.x;
          //mouse_y = event.motion.y;
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


