
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>

#include <stdio.h>
#include <stdlib.h>

#if 0  // Choose ASM or C


int Biggest (int a, int b)
{
  int res = a;
  if (b > a)
    res = b;

  return res;
}

int Smallest (int a, int b)
{
  int res = a;
  if (b < a)
    res = b;

  return res;
}

int TotalOfArray (int *array, int len)
{
  int res = *array++;
  len--;
  while (len != 0) {
    int v = *array++;
    res += v; 
    len--;
  }

  return res;
}

int BiggestOfArray (int *array, int len)
{
  int res = *array++;
  len--;
  while (len != 0) {
    int v = *array++;
    if (v > res)
      res = v;
    len--;
  }

  return res;
}

int SmallestOfArray (int *array, int len)
{
  int res = *array++;
  len--;
  while (len != 0) {
    int v = *array++;
    if (v < res)
      res = v;
    len--;
  }

  return res;
}


#else

extern int Biggest (int a, int b);
extern int Smallest (int a, int b);
extern int TotalOfArray (int *array, int len);
extern int BiggestOfArray (int *array, int len);
extern int SmallestOfArray (int *array, int len);

#endif


#define RANDOM_ARRAY_LEN  (100)
static int random_array [RANDOM_ARRAY_LEN];

int main(void) 
{
  // the vblank interrupt must be enabled for VBlankIntrWait() to work
  // since the default dispatcher handles the bios flags no vblank handler
  // is required
  irqInit();
  irqEnable(IRQ_VBLANK);

  int i;
  // List of random numbers 0-3999
  for(i=0; i<RANDOM_ARRAY_LEN; i++)
    random_array[i] = rand() % 4000;

  consoleDemoInit();

  int a = 1000, b = 2000;
  iprintf("Num comparisons\n---------------\n");

  iprintf("Biggest of %d,%d = %d\n", b, a, Biggest (a,b));
  iprintf("Biggest of %d,%d = %d\n", a, b, Biggest (a,b));
  iprintf("Smallest of %d,%d = %d\n", b, a, Smallest (a,b));
  iprintf("Smallest of %d,%d = %d\n", a, b, Smallest (a,b));

  iprintf("Array comparisons\n---------------\n");

  iprintf("Total of list = %d\n", TotalOfArray (random_array,RANDOM_ARRAY_LEN));
  iprintf("Biggest of list = %d\n", BiggestOfArray (random_array,RANDOM_ARRAY_LEN));
  iprintf("Smallest of list = %d\n", SmallestOfArray (random_array,RANDOM_ARRAY_LEN));

  while (1) {
    VBlankIntrWait();
  }

}


