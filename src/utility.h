#ifndef UTILITY_H
#define UTILITY_H

#include <stdint.h>
#include <windows.h>

#include "functions.h"

#define ENABLE_PRINT

#ifdef ENABLE_PRINT
#include <stdio.h>
#include <stdlib.h>
#define PRINT_DEBUG(...) fprintf(stderr, __VA_ARGS__)
#else
#define PRINT_DEBUG(...) ((void)0)
#endif

#define IS_32_BIT (sizeof(void *) == 4)
#define IS_64_BIT (sizeof(void *) == 8)
#define ASSERT_FUNCTION_IN_BOUNDARIES(func)                                    \
    {                                                                          \
        assert((void *)func >= (void *)code_begin &&                           \
               (void *)func < (void *)code_end);                               \
    }

int32_t get_delta_offset(void);

void memzero(void *start, uint32_t size);
void memcp(void *src, void *dest, uint32_t size);
void *memalloc(size_t size, functions_t *funcs);
void memfree(void *mem, functions_t *funcs);

#endif // UTILITY_H
