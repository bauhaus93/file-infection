#ifndef UTILITY_H
#define UTILITY_H

#include <stdint.h>

#define DISABLE_PRINT

#ifndef DISABLE_PRINT
#define PRINT_DEBUG(...) fprintf(stderr, __VA_ARGS__)
#else
#define PRINT_DEBUG(...) ((void)0)
#endif

#define IS_32_BIT (sizeof(void*) == 4)
#define IS_64_BIT (sizeof(void*) == 8)

int32_t get_delta_offset(void);

void memzero(void* start, uint32_t size);
void memcp(void* src, void* dest, uint32_t size);

#endif
