
#include <gba_base.h>
#include <gba_video.h>
#include <gba_systemcalls.h>
#include <gba_interrupt.h>

#define SCREEN_W (240)
#define SCREEN_H (160)

#define PIC_W (240)
#define PIC_H (296)

extern const unsigned short pic[PIC_W * PIC_H];

#if 0

extern void LineCopy (unsigned short* dst, const unsigned short* src, int words); 

#else

static void LineCopy (volatile unsigned short* dst, const volatile unsigned short* src, int l)
{
  // Using "do - while" instead of for is dangerous. It will copy pixels even if
  // len is 0
  do {
    *dst++ = *src++;
    l--;
  } while (l > 0);

}

#endif


int main()
{
  int y,g = 0;  

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
    // Fill scren
    int offsy = g % (PIC_H - SCREEN_H);
    for(y = 0; y<SCREEN_H; y++) 
      LineCopy (&screen[y * SCREEN_W], &pic[(y + offsy) * SCREEN_W], SCREEN_W);

    g++;
  }

  return 1;
}


