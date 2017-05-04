#include <stdio.h>
#include <windows.h>
#include <stdint.h>

#include "virus.h"
#include "checksum.h"

int main(int argc, char **argv) {

  printf("checksum: %s -> 0x%X\n", "abcdef", checksum("abcdef"));

  int retVal = run();
  printf("return value: %d\n", retVal);
  return retVal;
}
