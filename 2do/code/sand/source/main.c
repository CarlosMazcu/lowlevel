
#include <string.h>

#include <gba_base.h>
#include <gba_video.h>
#include <gba_systemcalls.h>
#include <gba_interrupt.h>

#define NUM_PARTICLES (200)

#define SCREEN_W (240)
#define SCREEN_H (160)

#define PIC_W (SCREEN_W)
#define PIC_H (SCREEN_H)

#define BLACK (0)
#define WHITE (0x7FFF)

extern const unsigned short pic[PIC_W * PIC_H];


typedef struct {
  unsigned char  x, y;
  unsigned char  alive;
  char           PADDING; // Just to be sure sizeof(Particle) is a factor of 4
} Particle;

#define NO_SLOT (33)
#if 1
void UpdateParticles (Particle* particles, int nparticles, unsigned short* screen);
#else
void UpdateParticles (Particle* particles, int nparticles, unsigned short* screen)
{
  do {
    if (particles->alive) {
      // Mirar si hay sitio debajo
      int x = particles->x;
      int y = particles->y;
      unsigned short* current = &screen [x + SCREEN_W * y];
      unsigned short* down = current + SCREEN_W;
      int new_x = NO_SLOT;
      if (down[0] == BLACK)
        new_x = 0;      // There is room just down the current position
      else if (down[-1] == BLACK)
          new_x = -1;      // There is room letf-down 
        else if (down[1] == BLACK)
            new_x = 1;      // There is room right-down 

      if (new_x != NO_SLOT) {
        *current = BLACK; // Clean the old position
        down[new_x] = WHITE; // Ocuppy the new position, color white
        particles->x = x + new_x;
        particles->y = y + 1;
      }
      else
        particles->alive = 0;

    }

    particles++;
    nparticles--;

  } while (nparticles != 0);  // Do while there is particles left
}
#endif

typedef struct
{
  unsigned short x, y;
} Source;

#define NUM_SOURCES (8)

static const Source g_Sources [NUM_SOURCES] =
{
  {53,0},
  {55,0},
  {124,0},
  {125,0},
  {182,0},
  {183,0},
  {227,39},
  {36,93},
};
 
#if 1
void SeedParticles (const Source* sources, Particle* particles, int nparticles, unsigned short* screen);
#else
void SeedParticles (const Source* sources, Particle* particles, int nparticles, unsigned short* screen)
{
  int nsources = NUM_SOURCES;
  do {
    if (!particles->alive) {
      // Check if the source is clear 
      int x = sources->x;
      int y = sources->y;
      unsigned short pixel = screen[x + y * SCREEN_W];
      if (pixel == BLACK)
      {
        particles->x = x;
        particles->y = y;
        particles->alive = 1;
      }
      sources++;
      nsources--;
    }

    particles++;
    nparticles--;

  } while ((nparticles != 0) && (nsources != 0));  // Do while there is particles or sources left
}
#endif



static Particle g_Particles [NUM_PARTICLES];


int main()
{
  int i, g = 0;  

  // Set up the interrupt handlers
  irqInit();
  // Enable Vblank Interrupt to allow VblankIntrWait
  irqEnable(IRQ_VBLANK);

  // Allow Interrupts
  REG_IME = 1;

  // GBA's VRAM is located at address 0x6000000. 
  // Screen memory in MODE 3 is located at the same place
  unsigned short* screen = (unsigned short*)0x6000000;
  // GBA's graphics chip is controled by registers located at 0x4000000 
  unsigned int* video_regs = (unsigned int*) 0x4000000; // mode3, bg2 on (16 bits RGB)
  // Configure the screen at mode 3 using the display mode register
  video_regs[0] = 0x403; // mode3, bg2 on (16 bits RGB)

  // Copy background pic to screen
  memcpy(screen, pic, SCREEN_W * SCREEN_H * sizeof(short));

  for (i=0; i<NUM_PARTICLES; i++)
    g_Particles [i].alive = 0;

  while(1) {
    VBlankIntrWait();

    SeedParticles (g_Sources, g_Particles, NUM_PARTICLES, screen);

    UpdateParticles (g_Particles, NUM_PARTICLES, screen);
    
    g++;
  }

  return 0;
}


