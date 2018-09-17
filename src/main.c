#include <stdio.h>
#include <windows.h>

#include "code_begin.h"
#include "code_end.h"
#include "utility.h"
#include "virus.h"

int main(int argc, char **argv) {
  printf("code_begin = 0x%08X\n", (unsigned int)code_begin);
  printf("code_end = 0x%08X\n", (unsigned int)code_end);
  int retVal = run();
  printf("return value: %d\n", retVal);
  return retVal;
}
