#include <stdio.h>
#include <windows.h>

#include "ass.h"
#include "virus.h"

int main(int argc, char **argv) {
  get_image_base();
  get_kernel32_base();
  /*int retVal = run();
  printf("return value: %d\n", retVal);
  return retVal;*/
  return 0;
}
