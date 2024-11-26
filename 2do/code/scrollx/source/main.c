
#include <gba_base.h>
#include <gba_video.h>
#include <gba_systemcalls.h>
#include <gba_interrupt.h>

#define SCREEN_W (240)
#define SCREEN_H (160)

#define PIC_W (284)
#define PIC_H (160)

extern const unsigned short pic[PIC_W * PIC_H];

// This function copies a rectangle of pixels from a large texture to a smaller one
// Size of the rectangle is the same as the screen
#if 1
extern void RectCopy (volatile unsigned short* dst, const volatile unsigned short* src, int src_width);
#else
static void RectCopy (volatile unsigned short* dst, const volatile unsigned short* src, int src_width);
{
  int x, y;
  for (y = 0; y < SCREEN_H; y++) {
    for (x = 0; x < SCREEN_W; x++) {
      *dst = *src;
      src++;
      dst++;
    }
    // After copying a line, we move the pointer to the next one
    // We add the source texture width, and substract the numer of pixels already copied (SCREEN_W)
    src += src_width - SCREEN_W;
  }
}
#endif


int main()
{
  int g = 0;  

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

  while(1) {
    VBlankIntrWait();
    // Copy screen
    int offsx = g % (PIC_W - SCREEN_W);
    RectCopy (screen, pic + offsx, PIC_W);

    g++;
  }

  return 1;
}


