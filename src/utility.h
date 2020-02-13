#ifndef UTILITY_H
#define UTILITY_H

#include <stdint.h>
#include <windows.h>

#include "function_list.h"

#define ENABLE_PRINT

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

#define IS_32_BIT (sizeof(void *) == 4)
#define IS_64_BIT (sizeof(void *) == 8)

int32_t get_delta_offset(void);

void memzero(void *start, uint32_t size);
void memcp(void *src, void *dest, uint32_t size);
void *memalloc(size_t size, function_list_t *function_list);
void memfree(void *mem, function_list_t *function_list);
uint8_t same_case_insensitive(char a, char b);

#endif // UTILITY_H
