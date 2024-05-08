
#include "tpoint.h"

int Closest (const TPoint* points_array, int npoints, int x, int y)
{
  int i = 0;
  int closest = 0;    // 1st point is closest, by default
  int xd = points_array[0].x - x;
  int yd = points_array[0].y - y;
  int min_dist = xd * xd + yd * yd;
  for (i = 1; i < npoints; i++) {
    xd = points_array[i].x - x;
    yd = points_array[i].y - y;
    int dist = xd * xd + yd * yd;
    if (dist < min_dist) {
      min_dist = dist;
      closest = i;
    }
  }
  return closest;
}

