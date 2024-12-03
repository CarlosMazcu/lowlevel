

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
  if ((dump & 15) == 0)
    fprintf ( stdout, "%s: %f ms, %d cycles, %f cycles/loop\n", name, ms, (int)cycles, cyc_per_comp);
}


// // Limit framerate and return any remaining time to the OS 

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

// Fully UNoptimized graphics effect example
/*static int DoEffect(short* drawlist, int max_vertices, int w, int h, int frame, float projection) {
    short* ori = drawlist;  // Apuntador inicial del array de puntos
    int grid_size = 64;     // Tamaño de la cuadrícula (64x64)
    float spacing = 10.0f;  // Distancia entre puntos en la cuadrícula

    // Desplazamiento para centrar la cuadrícula en la pantalla
    int offset_x = (w - (grid_size * spacing)) / 2;
    int offset_y = (h - (grid_size * spacing)) / 2;

    // Iterar sobre la cuadrícula (64x64 puntos)
    for (int z = 0; z < grid_size; z++) {
        for (int x = 0; x < grid_size; x++) {
            // Coordenadas 2D del punto en la cuadrícula
            float px = offset_x + x * spacing; // Posición X en pantalla
            float py = offset_y + z * spacing; // Posición Y en pantalla

            // Asegurar que el punto está dentro de los límites de la pantalla
            if (px >= 0 && py >= 0 && px < w && py < h) {
                drawlist[0] = (short)px;     // Coordenada X
                drawlist[1] = (short)py;     // Coordenada Y
                drawlist[2] = 128;           // Color del punto (índice de paleta)
                drawlist += 3;               // Avanzar al siguiente punto
            }
        }
    }

    // Número total de vértices generados
    return (drawlist - ori) / 3;
}*/


static int DoEffect(short* drawlist, int max_vertices, int w, int h, int frame, float projection, SDL_Surface* heightmap, int offset_x, int offset_y) {
    short* ori = drawlist; 
    int grid_size = 64;
    int origin_pos = grid_size >> 1;    
    float spacing = 10.0f; 
    float z_offset = 1000.0f;  
    float height_offset = 500.0f; 
    int cx = w >> 1;        
    int cy = h >> 1;        
    
    

    for (int z = 0; z < grid_size; z++) {
        for (int x = 0; x < grid_size; x++) {

            // desplazamiento del sampple
            int sample_x = (offset_x + x); 
            int sample_y = (offset_y + z);
            // posiciones en pantalla 
            float fx = (x - origin_pos) * spacing; 
            float fz = (z - origin_pos) * spacing; 

            // leer el pixel // posible optimización -> precargar los pixeles fuera
            unsigned char* pixel = (unsigned char*)heightmap->pixels + (sample_y * heightmap->pitch) + sample_x;
            float fy = (*pixel) * 2.5f;

            //offset vista desde arriba
            fy -= height_offset;

            // perspectiva
            float px = cx + (fx * projection) / (z_offset + fz);
            float py = cy - (fy * projection) / (z_offset + fz);

            // comprobar que el punto está dentro de los límites de la pantalla
            if (px >= 0 && py >= 0 && px < w && py < h) {
                drawlist[0] = (short)px;     // Coordenada X proyectada
                drawlist[1] = (short)py;     // Coordenada Y proyectada
                drawlist[2] = (z + (x << 1)) & 0xff;           // Color del punto
                drawlist += 3;               // Avanzar al siguiente punto
            }
        }
    }

    // Número total de vértices generados
    return (drawlist - ori) / 3;
}


// ---------------------------------------------------------------------------

static void DisplayVertices (unsigned int* pixels, short* drawlist, int n_vertices, int stride, unsigned int* palette)
{
  int i;
  for(i = 0; i < n_vertices; i++) {
      int xp = drawlist[0];
      int yp = drawlist[1];
      pixels [xp + yp * stride] = palette[drawlist[2]];
      drawlist += 3;
  } 
}


// ---------------------------------------------------------------------------

static unsigned int palette [256];

int main ( int argc, char** argv)
{
  int end = 0;
  int mouse_x = 0, mouse_y = 0;
  SDL_Surface  *g_SDLSrf;
  int req_w = 1024;
  int req_h = 768; 

  /////////
  static int offset_x_ = 0; // Offset inicial en X
  static int offset_y_ = 0; // Offset inicial en Y
  static int speed_x = 1;  // Velocidad en X
  static int speed_y = 1;  // Velocidad en Y
  ////////

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

  // Dot palette
  int i;
  for (i=0; i<256; i++)
    palette [i] = i | (i << 8) | (i << 16);

  // Small footprint buffer for vertices draw  
  // we are not using an structure to avoid padding to 8 bytes (instead of current 6)
  int n_vertices = 10000;
  short* drawlist = (short*) malloc (n_vertices * 3 * sizeof(short));

  // Setup your effect initialization here
  // Horizontal field of view
  float hfov = 60.0f * ((3.1416f * 2.0f) / 360.0f);  // Degrees to radians
  float half_scr_w = (float)(req_w >> 1);
  float projection = (1.0f / tan ( hfov * 0.5f)) * half_scr_w;
/////////////
   SDL_Surface* heightmap = SDL_LoadBMP("img/heightmap2_24bits.bmp");
    if (!heightmap) {
        fprintf(stderr, "Error al cargar el heightmap: %s\n", SDL_GetError());
        return -1;
    }
    if (heightmap->format->BitsPerPixel != 8) {
        fprintf(stderr, "Heightmap no válido.\n");
        return 0;
    }
    
    
    // Verificar formato del heightmap
    if (heightmap->format->BitsPerPixel != 8) {
        fprintf(stderr, "El heightmap debe ser en escala de grises (8 bits).\n");
        SDL_FreeSurface(heightmap);
        return -1;
    }
///////////////
  // Main loop
  g_SDLSrf = SDL_GetVideoSurface();
  while ( !end) { 

    SDL_Event event;
////////

 
//    diagonal
 
    offset_x_ += speed_x;
    offset_y_ += speed_y;

    if (offset_x_ >= heightmap->w - 64 || offset_x_ <= 0) speed_x *= -1; 
    if (offset_y_ >= heightmap->h - 64 || offset_y_ <= 0) speed_y *= -1; 

    //espiral
////////
    // Your gfx effect goes here

    ChronoWatchReset();
    int n, n_draw=0;
    for (n=0; n<10; n++)
       n_draw = DoEffect (drawlist, n_vertices, g_SDLSrf->w, g_SDLSrf->h, dump, projection, heightmap, offset_x_, offset_y_);
    assert(n_draw <= n_vertices);
    ChronoShow ( "Landscape Loco Festival", n_draw * 10);

    // Draw vertices; don't modify this section
    // Lock screen to get access to the memory array
    SDL_LockSurface( g_SDLSrf);

    // Clean the screenss
    SDL_FillRect(g_SDLSrf, NULL, SDL_MapRGB(g_SDLSrf->format, 0, 0, 0));
    //ChronoShow ( "Clean", g_SDLSrf->w * g_SDLSrf->h);

    // Paint vertices

    DisplayVertices (g_SDLSrf->pixels, drawlist, n_draw, g_SDLSrf->pitch >> 2, palette);
    //ChronoShow ( "Preview", n_draw);

    //Unlock the draw surface, dump to physical screen
    ChronoWatchReset();
    SDL_UnlockSurface( g_SDLSrf);
    SDL_Flip( g_SDLSrf);
    //ChronoShow ( "Screen dump", g_SDLSrf->w * g_SDLSrf->h);

    // Limit framerate and return any remaining time to the OS
    // Comment this line for benchmarking
    //FramerateLimit (60);

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


