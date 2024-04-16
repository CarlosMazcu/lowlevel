
// Simple bump mapping

//#include <gba_base.h>
//#include <gba_video.h>
//#include <gba_systemcalls.h>
#include "gba_interrupt.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define GBA_SCREEN_WIDTH  (240)
#define GBA_SCREEN_HEIGHT (160)

extern const signed short gBump [GBA_SCREEN_WIDTH * GBA_SCREEN_HEIGHT];
extern const unsigned char gLight [512 * 512];
extern const unsigned short gPal [256];

static unsigned int gFastAsm [128];

// GBA's graphics chip is controled by registers located at 0x4000000 
static volatile unsigned int* gVideoRegs = (volatile unsigned int*) 0x4000000; // video registers address
// GBA's VRAM is located at address 0x6000000. 
// Screen memory in MODE 4 is located at the same place
static volatile unsigned char* gScreen0 = (volatile unsigned char*)0x06000000;
static volatile unsigned char* gScreen1 = (volatile unsigned char*)0x0600a000;

// ---------------------------------------------------------------------------

volatile static int gDisplayCount = 0;
volatile static int gScreenBuffers = 0;

unsigned char* GetBackBuffer()
{
  if (gScreenBuffers & 1)
    return (unsigned char*)gScreen0;
  else 
    return (unsigned char*)gScreen1;
}

void Flip ()
{
  gScreenBuffers++;
} 

void VBlankIntr (void)
{
  gDisplayCount++;

  if ((gScreenBuffers & 1) != 0) 
    gVideoRegs[0] |= 0x10;
  else
    gVideoRegs[0] &= ~0x10;
}

// ---------------------------------------------------------------------------

static void BumpMap (unsigned char* dst,
                     const signed short* bump_src, 
                     const unsigned char* light_src)
{
  unsigned short* dst16 = (unsigned short*) dst; 
  int x, y;
  for (y = 0; y < GBA_SCREEN_HEIGHT; y++) {
    for (x = 0; x < GBA_SCREEN_WIDTH; x+=2) {
      int offs = *bump_src++;
      int l0 = light_src[offs];
      light_src++; 
      offs = *bump_src++;
      int l1 = light_src[offs];
      light_src++;
      *dst16++ = l0 | (l1 << 8);
    }
    light_src+= 512 - GBA_SCREEN_WIDTH;
  }
}


int main()
{
  int i;

  // Palette memory 
  volatile unsigned short* palette = (unsigned short*)0x5000000;
  // Configure the screen at mode4, bg2 on (8 bits LUT palette based) 
  gVideoRegs[0] = 0x404; // mode4, bg2 on

  // Copy the palette to fast ram
  for (i=0; i<256; i++)
    palette[i] = gPal[i];

  // Copy inner loop to fast memory
  memcpy(gFastAsm, BumpMap, 256); 
  void (*fun_ptr)(unsigned char* dst,const signed short* bump_src, const unsigned char* light_src) = (void*)gFastAsm;

#if 0

  // SIMPLE VERSION WITHOUT INTERRUPTS

  float a = 0.0f;
  while(1) { 

    // Swap frame buffers
    volatile unsigned char* t = gScreen0;
    gScreen0 = gScreen1;
    gScreen1 = t;

    int u = (int)(256.0f - 120.0f + sin(a * 3.0f) * 100.0f);
    int v = (int)(256.0f - 80.0f + cos(a * 1.0f) * 100.0f);
    a += 0.05f;
    unsigned char* l = (unsigned char*)&gLight [u + v * 512];
    fun_ptr ((unsigned char*)gScreen0, gBump, l);

    gVideoRegs[0] ^= 0x10;
  }

#else

  // VERSION USING CORRECT DOUBLE BUFFER SYNCHRONISM (vblank interrupt)
  // Set up the interrupt handlers
  irqInit();
  // Enable Vblank Interrupt to allow VblankIntrWait
  irqEnable(IRQ_VBLANK);
  // Setup an interrupt synched to the display's verticval blank
  irqSet(IRQ_VBLANK, VBlankIntr);

  float a = 0.0f;
  while(1) { 



    int u = (int)(256.0f - 120.0f + sin(a * 3.0f) * 100.0f);
    int v = (int)(256.0f - 80.0f + cos(a * 1.0f) * 100.0f);
    a += 0.1f;
  
    unsigned char* l = (unsigned char*)&gLight [u + v * 512];
    fun_ptr (GetBackBuffer(), gBump, l);


    int now = gDisplayCount;

    Flip ();

    // If we were faster the 60 FPS, wait so we don't draw on the display
    while (now == gDisplayCount);
  }
#endif
}


