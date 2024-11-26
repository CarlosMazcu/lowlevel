
#include <gba_base.h>
#include <gba_video.h>
#include <gba_systemcalls.h>
#include <gba_interrupt.h>

#include <stdlib.h>
#include <stdio.h>

#define SNAKE_LEN (100)

typedef struct
{
  int x, y;
} Slab;

typedef struct
{
  Slab* slabs;
  unsigned short len;
  unsigned short head_slab;
  int   speed_x, speed_y;
} Snake;

static Slab snake_slabs [SNAKE_LEN];


static void InitSnake (Snake* snake, unsigned short* screen)
{
  int i;
  // Snake declaration
  snake->len = SNAKE_LEN;
  snake->head_slab = SNAKE_LEN - 1;
  snake->slabs = snake_slabs;
  snake->speed_x = 1;
  snake->speed_y = 0;

  for (i = 0; i < snake->len; i++) {
    snake->slabs[i].x = 10 + i;
    snake->slabs[i].y = 80;
    // Paint the slabs
    screen[snake->slabs[i].x + snake->slabs[i].y * 240] = 0xffff; 
  }
}

static void CleanScreen (unsigned short* screen)
{
  int x, y, i;
  for (i=0; i<(240*160); i++)
    screen[i] = 0; // Black

  for (i=0; i<(100); i++) {
    int x = rand() % 240;
    int y = rand() % 160;
    if (y == 80)
      y--; 
    screen[x + 240 * y] = 0x7e; // Red
  }

  // Set red borders
  for (y=0; y<160; y++) {
    screen [y * 240] = 0x1f;
    screen [239 + y * 240] = 0x1f;
  }  
  for (x=0; x<240; x++) {
    screen [x] = 0x1f;
    screen [159 * 240 + x] = 0x1f;
  }
}

#if 1
extern int UpdateSnake (int keypad, Snake* snake, unsigned short* screen);
#else
int UpdateSnake (int keypad, Snake* snake, unsigned short* screen)
{
  int crash = 0;
  Slab* slabs = snake->slabs;
  int head_slab = snake->head_slab;
  // Cut the tail
  int tail = snake->head_slab + 1;
  if (tail >= snake->len)
    tail = 0;

  screen[slabs[tail].x + slabs[tail].y * 240] = 0; // BLACK 

  // Advance the head
  int new_x = slabs[snake->head_slab].x;
  int new_y = slabs[snake->head_slab].y;

  head_slab++;
  if (head_slab >= snake->len)
    head_slab = 0;
  snake->head_slab = head_slab;

  int speed_x = snake->speed_x;
  int speed_y = snake->speed_y;  
  /*if (keypad != 0) {
    speed_x = speed_y = 0;
    if (keypad & 1)
      speed_x = 1;
    if (keypad & 2)
      speed_x = -1;
    if (keypad & 4)
      speed_y = -1;
    if (keypad & 8)
      speed_y = 1;
*/
   // snake->speed_x = speed_x;
   // snake->speed_y = speed_y;
  //}

  // Head new pos.
  new_x += speed_x;
  new_y += speed_y;

  unsigned short* head_pix = screen + (new_x + new_y * 240);
  if (*head_pix == 0) {
    *head_pix = 0xffff; // Paint head, white

    slabs[head_slab].x = new_x;
    slabs[head_slab].y = new_y;

  } 
  else
    crash = 1;
  
  keypad = crash;
  return keypad;
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
	unsigned short* screen = (unsigned short*)0x6000000;
  // GBA's graphics chip is controled by registers located at 0x4000000 
	unsigned int* video_regs = (unsigned int*) 0x4000000; // mode3, bg2 on (16 bits RGB)
  // Configure the screen at mode 3 using the display mode register
	video_regs[0] = 0x403; // mode3, bg2 on (16 bits RGB)

  unsigned short* reg_key = (unsigned short*)0x4000130;

  // Snake data
  Snake snake;

  CleanScreen (screen);

  // Init snake
  InitSnake (&snake, screen);

	while(1) {
    VBlankIntrWait();

    // Read the keypad from the hw register
    int keypad = (~(*reg_key >> 4)) & 0xf;

    if (UpdateSnake (keypad, &snake, screen)) {
      // The snake crashed
      CleanScreen (screen);
      InitSnake (&snake, screen);
    }

 

  }
}


