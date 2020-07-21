#include "oep.h"
#include "code_begin.h"
#include "utility.h"

#pragma GCC push_options
#pragma GCC optimize("O0")
size_t __attribute__((noinline)) get_original_entry_point(void) {
    return OEP_DEFAULT;
}
#pragma GCC pop_options

uint8_t write_original_entry_point(uint32_t oep, void *targetCodeBegin) {
    uint32_t src_oep = get_original_entry_point();
    int32_t start_offset =
        (uint8_t *)get_original_entry_point - (uint8_t *)code_begin;
    int32_t stop_offset =
        (uint8_t *)write_original_entry_point - (uint8_t *)code_begin;
    uint32_t *ptr = (uint32_t *)((uint8_t *)targetCodeBegin + start_offset);
    uint32_t *stop = (uint32_t *)((uint8_t *)targetCodeBegin + stop_offset);
    while (ptr < stop) {
        // PRINT_DEBUG("PTR = 0x%X, must match 0x%X\n", *ptr, src_oep);
        if (*ptr == src_oep) {
            *ptr = oep;
            return 0;
        } else {
            ptr = (uint32_t *)(((uint8_t *)ptr) + 1);
        }
    }
    return 1;
}
