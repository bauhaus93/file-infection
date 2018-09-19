#include "oep.h"

void* get_original_entry_point(void) {
    return (void*) -1;
}

//TODO: handle 64 bit case
uint8_t write_original_entry_point(uint32_t srcOep, uint32_t targetOep, void* targetCodeBegin) {
    uint32_t startOffset = (uint8_t*)get_original_entry_point - (uint8_t*)code_begin;
    uint32_t stopOffset = (uint8_t*)write_original_entry_point - (uint8_t*)code_begin;
    uint32_t* ptr = (uint32_t*)((uint8_t*)targetCodeBegin + startOffset);
    uint32_t* stop = (uint32_t*)((uint8_t*)targetCodeBegin + stopOffset);
    while (ptr < stop) {
        if (*ptr == srcOep) {
            *ptr = targetOep;
            return 0;
        } else {
            ptr = (uint32_t*)(((uint8_t*)ptr) + 1);
        }
    }
    return 1;
}