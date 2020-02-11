#include "code_begin.h"
#include "oep.h"
#include "utility.h"

// TODO: handle 64 bit case
size_t get_original_entry_point(void) { return OEP_DEFAULT; }

// TODO: handle 64 bit case
uint8_t write_original_entry_point(uint32_t oep, void *targetCodeBegin) {
    uint32_t srcOep = get_original_entry_point();
    int32_t startOffset =
        (uint8_t *)get_original_entry_point - (uint8_t *)code_begin;
    int32_t stopOffset =
        (uint8_t *)write_original_entry_point - (uint8_t *)code_begin;
    uint32_t *ptr = (uint32_t *)((uint8_t *)targetCodeBegin + startOffset);
    uint32_t *stop = (uint32_t *)((uint8_t *)targetCodeBegin + stopOffset);
    while (ptr < stop) {
        // PRINT_DEBUG("PTR = 0x%X, must match 0x%X\n", *ptr, srcOep);
        if (*ptr == srcOep) {
            *ptr = oep;
            return 0;
        } else {
            ptr = (uint32_t *)(((uint8_t *)ptr) + 1);
        }
    }
    return 1;
}
