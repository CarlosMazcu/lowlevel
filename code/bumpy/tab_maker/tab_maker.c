
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static inline int sat (int s, int min, int max)
{
  if (s < min) s = min;
  if (s > max) s = max;
  return s;
}

int main (int a, char** b)
{
    int x, y;
    static int cnt = 0;
    for (y=0; y<512;y++) {
        for (x=0; x<512; x++) {
          int xd = 256 - x;
          int yd = 256 - y;

          float d = sqrtf((float)(xd * xd + yd * yd));

          int di = (d * 1.3f);
          di = 255 - sat(di, 0, 255);
          //fprintf (stderr, "%d %d %d\n", dx, dy, offs);

          if (cnt == 0)
              fprintf (stdout, "const unsigned char light[%d * %d] = {\n    ", 512, 512);

          fprintf (stdout, "%d,", di);

          if ((cnt % 8) == 7)
              fprintf (stdout, "\n    ");
          cnt++;
        } 
    } 

    fprintf (stdout, "};\n\n");

    return 1;
}
