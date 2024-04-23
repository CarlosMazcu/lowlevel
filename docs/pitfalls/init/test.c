

int nasty (int* input, int len)
{
  int i, t;
  for (i=0; i<len; i++)
    t += input[i];

  return t;
}


