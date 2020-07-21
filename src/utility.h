#ifndef UTILITY_H
#define UTILITY_H

#include <stddef.h>
#include <stdint.h>

// #define ENABLE_PRINT
// #undef ENABLE_PRINT

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

#define BYTE_INCREMENT(addr) BYTE_OFFSET(addr, 1)
#define BYTE_DECREMENT(addr) BYTE_OFFSET(addr, -1)

#define BYTE_DIFF(hi, lo) ((int32_t)(((uint8_t *)(hi)) - ((uint8_t *)(lo))))

uint8_t same_case_insensitive(char a, char b);

uint8_t write_value32(uint32_t value, void *memory);
void *find_value32(uint32_t value, void *start, void *end);
uint8_t find_replace_value32(uint32_t search_value, uint32_t new_value,
                             void *symbol_start_addr, void *symbol_end_addr,
                             void *code_begin, void *target_code_begin);

#endif // UTILITY_H
