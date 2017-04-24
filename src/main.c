#include <stdio.h>
#include <windows.h>
#include <stdint.h>

#include "virus.h"

int main(int argc, char **argv) {
  int retVal = run();
  printf("return value: %d\n", retVal);
  return retVal;
}
