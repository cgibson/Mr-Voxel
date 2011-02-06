#ifndef _MALLOCX_H_
#define _MALLOCX_H_
#include <stdlib.h>

void *mallocx(int size)
{
  void *ret = malloc(size);
  if(ret == NULL)
  {
    printf("Error: allocation of memory failed.\n");
    exit(-2);
  }
  return ret;
}
#endif
