#include "delta.h"
#include "utility.h"

int32_t get_delta_offset(void) {
  int32_t offset = 0;
#ifdef ENV32
  __asm__ __volatile__("call .base\n"
                       ".base:\n"
                       "pop %%edx\n"
                       "sub $.base, %%edx\n"
                       "mov %%edx, %0"
                       : "=r"(offset));
#else
  /*__asm__ __volatile__("call .base\n"
                       ".base:\n"
                       "pop %%rdx\n"
                       "sub $.base, %%rdx\n"
                       "mov %%edx, %0"
                       : "=r"(offset));*/

#endif
  return offset;
}
