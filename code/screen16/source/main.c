
#include <gba_base.h>
#include <gba_video.h>
#include <gba_systemcalls.h>
#include <gba_interrupt.h>

#define RGB16(r,g,b)  ((r)+((g)<<5)+((b)<<10)) 

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
  volatile unsigned short* tmp_screen = screen;
  // GBA's graphics chip is controled by registers located at 0x4000000 
  volatile unsigned int* video_regs = (unsigned int*) 0x4000000; // mode3, bg2 on (16 bits RGB)
  // Configure the screen at mode 3 using the display mode register
  video_regs[0] = 0x403; // mode3, bg2 on (16 bits RGB)

		VBlankIntrWait();

    // Fill scren
 
    for(int g = 0; g<240*160; g++){
         *tmp_screen++ = 0x7FFF;
    }
    
    /*for(int y = 20; y < 150; y++){
        corner[30 + y * 240] = 0x0000;
        corner[210+ y * 240] = 0x0000;
    }*/
    unsigned short* corner0 = screen + 4830; // &screen[30 + 20 * 240]
    unsigned short* corner2 = screen + 4830;
    for(int y = 20; y < 150; y++){
        corner0[31200] = 0x0000;
        corner0  = corner0 - 240;

        corner2[31380] = 0x0000;
        corner2 = corner2 - 240;
    }

    unsigned short* corner1; //&screen[30 + 20 * 240]
    unsigned short* start = screen + 4830;
    unsigned short* end = start + 180; 

   for(corner1 = start; corner1 <= end; corner1++){
        corner1[31200] = 0x0000; 
        *corner1 = 0x0000;
    }

   while(1);

 }

