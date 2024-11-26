
#include <gba_base.h>
#include <gba_video.h>
#include <gba_systemcalls.h>
#include <gba_interrupt.h>

#include <stdlib.h>

#define SCREEN_W (240)
#define SCREEN_H (160)

#define BALL_SIDE (16)

#define RGB16(R,G,B)  ((R)+((G)<<5)+((B)<<10)) 

#if 1
void Paint (unsigned short* dst, unsigned char* sprite, int stride_pixels, unsigned int color);
#else
void Paint (unsigned short* dst, unsigned char* sprite, int stride_pixels, unsigned int color)
{
  int ball_r = color & 0x1f;
  int ball_g = (color >> 5) & 0x1f;
  int ball_b = (color >> 10) & 0x1f;
  int x, y;
  int side = BALL_SIDE;
  for (y=0; y<side; y++) {
    for (x=0; x<side; x++) {
      unsigned char t = *sprite++; // read solid/transparent
      if (t) {
        unsigned int back_color = *dst;
        // Unpack color
        int r = back_color & 0x1f;
        int g = (back_color >> 5) & 0x1f;
        int b = (back_color >> 10) & 0x1f;
        // Color average
        r = ((r << 1) + r + ball_r) >> 2;
        g = ((g << 1) + g + ball_g) >> 2;
        b = ((b << 1) + b + ball_b) >> 2;
        // Reassemble color
        *dst = r; //| (g << 5) | (b<<10);
        //*dst = 0xFFFF;
      }
      dst++;
    }
    dst += stride_pixels - side;
  } 
}
#endif

static void FillWithCircle (unsigned char* sprite, int side, int rad)
{
  int x, y;
  int square = rad * rad;
  for (y=0; y<side; y++) {
    for (x=0; x<side; x++) {
      int xoff = x - (side >> 1);
      int yoff = y - (side >> 1);
      int pos_square = xoff * xoff + yoff * yoff;
      *sprite++ = (pos_square < square) ? 1 : 0; // 1: solid, 0: transparent
    }
  }  
}


unsigned char Ball [BALL_SIDE * BALL_SIDE];

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
	volatile unsigned short* Screen = (volatile unsigned short*)0x6000000;
    // GBA's graphics chip is controled by registers located at 0x4000000 
	volatile unsigned int* video_regs = (volatile unsigned int*) 0x4000000; // mode3, bg2 on (16 bits RGB)
    // Configure the screen at mode 3 using the display mode register
	video_regs[0] = 0x403; // mode3, bg2 on (16 bits RGB)

  FillWithCircle (Ball, BALL_SIDE,  (BALL_SIDE / 2) - 1);

  int x = SCREEN_W >> 1;
  int y = SCREEN_H >> 1;
  int vx = 5;
  int vy = 3;
  int r = 0, g = 0, b = 0;

  while(1) {

    int lim_x = SCREEN_W - BALL_SIDE - 1;
    int lim_y = SCREEN_H - BALL_SIDE - 1;

    VBlankIntrWait();

    Paint ((unsigned short*) Screen + x + y * SCREEN_W, Ball, SCREEN_W, RGB16(r >> 5,g >> 5,b >> 5));

    x += vx;
    y += vy;
    if (x < 0)
      x = 0, vx = -vx;
    if (y < 0)
      y = 0, vy = -vy;
    if (x > lim_x)
      x = lim_x, vx = -vx;
    if (y > lim_y)
      y = lim_y, vy = -vy;

    if ((rand() & 0x7) == 0) {
      vx = (rand() & 0x3) - 2;
      vy = (rand() & 0x3) - 2;
    }
 
    r += 3;
    g += 1;
    b += 2;
    r &= 0x3ff;
    g &= 0x3ff;
    b &= 0x3ff;
  }
}


