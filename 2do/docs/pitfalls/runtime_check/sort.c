

#include <stdio.h>
#include <stdlib.h>

static void SortArray (signed char *array, int len)
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


#define RANDOM_ARRAY_LEN  (17)

//static signed char random_array [RANDOM_ARRAY_LEN];

int main(void) 
{
  signed char* random_array = (signed char*)malloc ((RANDOM_ARRAY_LEN-10) * sizeof(char));

  int i;
  for(i = 0; i < RANDOM_ARRAY_LEN; i++)
    random_array[i] = rand();

  printf("Sorted list\n");
  printf("-----------\n");

  SortArray (random_array, RANDOM_ARRAY_LEN);

  for (i = 0; i < RANDOM_ARRAY_LEN; i++)
    printf("%d\n", random_array[i]);


  return 0;
}


