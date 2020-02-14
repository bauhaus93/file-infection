#ifndef UTILITY_H
#define UTILITY_H

#include <stdint.h>
#include <windows.h>

#include "code_begin.h"
#include "code_end.h"

//#define ENABLE_PRINT
#undef ENABLE_PRINT

#ifdef ENABLE_PRINT
#include <stdio.h>
#include <stdlib.h>
#define PRINT_DEBUG(...)                                                       \
    {                                                                          \
        fprintf(stderr, "[%s] ", __func__);                                    \
        fprintf(stderr, __VA_ARGS__);                                          \
        fprintf(stderr, "\n");                                                 \
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

#define BYTE_DIFF(hi, lo) ((int32_t)(((uint8_t *)(hi)) - ((uint8_t *)(lo))))

#define CODE_SIZE BYTE_DIFF(code_end, code_begin)

int32_t get_delta_offset(void);
void memzero(void *start, uint32_t size);
void memcp(void *src, void *dest, uint32_t size);
uint8_t same_case_insensitive(char a, char b);
void *find_value_32(uint32_t value, void *start, void *end);

#endif // UTILITY_H
