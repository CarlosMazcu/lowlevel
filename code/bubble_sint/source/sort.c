
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>

#include <stdio.h>
#include <stdlib.h>


#if 1
extern void SortArray (signed int *array, int len);
#else
static void SortArray (signed int *array, int len)
{
  while (len > 1) {

    int ordered = 1;
    int i;
    for (i = 0; i < (len - 1); i++) {

      if (array[i] > array[i+1]) {
        // Swap elements in the array
        int t = array[i];
        array[i] = array[i+1];
        array[i+1] = t; 
        ordered = 0;
      }
    }

    len--;
    if (ordered)
      len = 0; // nothing more to sort, exit
  }
}
#endif


#define RANDOM_ARRAY_LEN  (15)

static signed int random_array [RANDOM_ARRAY_LEN];

int main(void) 
{
  // the vblank interrupt must be enabled for VBlankIntrWait() to work
  // since the default dispatcher handles the bios flags no vblank handler
  // is required
  irqInit();
  irqEnable(IRQ_VBLANK);

  int i;
  for(i = 0; i < RANDOM_ARRAY_LEN; i++)
    random_array[i] = rand() << 1;

  consoleDemoInit();

  iprintf("Sorted list\n");
  iprintf("-----------\n");

  SortArray (random_array, RANDOM_ARRAY_LEN);

  for (i = 0; i < RANDOM_ARRAY_LEN; i++)
    iprintf("%d\n", random_array[i]);

  while (1) {
    VBlankIntrWait();
  }

  return 0;
}


