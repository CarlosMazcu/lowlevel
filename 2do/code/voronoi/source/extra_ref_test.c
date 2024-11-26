
#include "tpoint.h"

static const TPoint check = {0,0};

int LinkTest ()
{
  return Closest (&check, 1, 0, 0);
}

