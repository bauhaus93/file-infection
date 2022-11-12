#ifndef UTILITY_H
#define UTILITY_H

#include <stddef.h>
#include <stdint.h>

#include "poison.h"

#ifdef ENABLE_PRINT
#include <stdio.h>
#include <stdlib.h>
#define PRINT_DEBUG(...)                                                       \
  {                                                                            \
    fprintf(stderr, "[%-20s] ", __func__);                                     \
    fprintf(stderr, __VA_ARGS__);                                              \
    fprintf(stderr, "\n");                                                     \
  }
#else
#define PRINT_DEBUG(...) ((void)0)
#endif

#define BREAKPOINT                                                             \
  { asm("int3"); }

#define IS_32_BIT (sizeof(void *) == 4)
#define IS_64_BIT (sizeof(void *) == 8)

#define BYTE_OFFSET(start, byte_offset)                                        \
  ((void *)(((uint8_t *)(start)) + (byte_offset)))

#define BYTE_INCREMENT(addr) BYTE_OFFSET(addr, 1)
#define BYTE_DECREMENT(addr) BYTE_OFFSET(addr, -1)

#define BYTE_DIFF(hi, lo) ((int32_t)(((uint8_t *)(hi)) - ((uint8_t *)(lo))))

uint8_t same_case_insensitive(char a, char b);

#endif // UTILITY_H
