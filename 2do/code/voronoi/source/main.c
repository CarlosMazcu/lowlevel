
#include <gba_base.h>
#include <gba_video.h>
#include <gba_systemcalls.h>
#include <gba_interrupt.h>

#include "tpoint.h"

#define RGB16(r,g,b)  ((r)+((g)<<5)+((b)<<10)) 



static const TPoint _points_array [] =
{
  {30,11},
  {66,125},
  {100,37},
  {222,60},
  {201,87},
  {240,158},
};

static const unsigned short _points_colors [] =
{
  RGB16(31,0,0),
  RGB16(0,31,0),
  RGB16(0,0,31),
  RGB16(0,31,31),
  RGB16(31,0,31),
  RGB16(31,31,31),
};



// Fill the screen with closest points to each pixel
#if 1
void Voronoi (unsigned short* screen, const TPoint* points, int npoints, 
               const unsigned short* palette);
#else
void Voronoi (unsigned short* screen, const TPoint* points, int npoints, 
               const unsigned short* palette)
{
  int x,y;
  for(y = 0; y<SCREEN_HEIGHT; y++) {
    for(x = 0; x<SCREEN_WIDTH; x++) {
      int c = Closest (points, 
              npoints, 
              x, y);
      *screen = palette [c];
      screen++;
    }
  }
}
#endif


int main()
{
	// Set up the interrupt handlers
	irqInit();
	// Enable Vblank Interrupt to allow VblankIntrWait
	irqEnable(IRQ_VBLANK);
 
	// Allow Interrupts
	REG_IME = 1;

  // GBA's VRAM is located at address 0x6000000. 
  // Screen memory in MODE 3 is located at the same place
	volatile unsigned short* screen = (unsigned short*)0x6000000;
  // GBA's graphics chip is controled by registers located at 0x4000000 
	volatile unsigned int* video_regs = (unsigned int*) 0x4000000; // mode3, bg2 on (16 bits RGB)
  // Configure the screen at mode 3 using the display mode register
	video_regs[0] = 0x403; // mode3, bg2 on (16 bits RGB)

	while(1)  {

    VBlankIntrWait();

    Voronoi ((unsigned short*) screen, _points_array, 
              sizeof(_points_array) / sizeof(TPoint), // Quantity of points in the array
              _points_colors);

  }
}


