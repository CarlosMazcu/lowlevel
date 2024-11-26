
#include <stdio.h>
#include <stdlib.h>

typedef enum
{
  eItem0,
  eItem1
} test1;

typedef enum
{
  eItemA = 300,
  eItemB
} test2;



int main (int argc, char** argv)
{
  unsigned char ref = 0xff;

  printf ("Sizeof of pointers: %d\n", sizeof(void*));
  printf ("Sizeof of int: %d\n", sizeof(int));
  printf ("Sizeof of long: %d\n", sizeof(long));
  printf ("Sizeof of short: %d\n", sizeof(short));
  printf ("Sizeof of char: %d\n", sizeof(char));
  printf ("Sizeof of bool: %d\n", sizeof(bool));
  printf ("Sizeof of little enum: %d\n", sizeof(test1));
  printf ("Sizeof of large enum: %d\n\n", sizeof(test2));
  printf ("Sign of char: %d\n", *(char*)&ref);

  return 1;
}
