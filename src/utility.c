#include "utility.h"

uint8_t same_case_insensitive(char a, char b) {
    if (a >= 'A' && a <= 'Z') {
        a += 0x20;
    }
    if (b >= 'A' && b <= 'Z') {
        b += 0x20;
    }
    return a == b;
}

void *find_value32(uint32_t value, void *start, void *end) {
    uint32_t *ptr = (uint32_t *)start;
    while (ptr < (uint32_t *)end) {
        if (*ptr == value) {
            return (void *)ptr;
        } else {
            ptr = (uint32_t *)((uint8_t *)ptr + 1);
        }
    }
    return NULL;
}

uint8_t write_value32(uint32_t value, void *memory) {
    *(uint32_t *)memory = value;
    return 0;
}

uint8_t find_replace_value32(uint32_t search_value, uint32_t new_value,
                             void *symbol_start_addr, void *symbol_end_addr,
                             void *code_begin, void *target_code_begin) {
    if (symbol_start_addr < (void *)code_begin ||
        symbol_end_addr < (void *)code_begin) {
        PRINT_DEBUG("start/end addr smaller than code_begin: code_begin = "
                    "0x%p, start = 0x%p, send = 0x%p",
                    code_begin, symbol_start_addr, symbol_end_addr);
        return 1;
    }
    int32_t start_offset = BYTE_DIFF(symbol_start_addr, code_begin);
    int32_t end_offset = BYTE_DIFF(symbol_end_addr, code_begin);
    uint32_t *memory = (uint32_t *)find_value32(
        search_value, BYTE_OFFSET(target_code_begin, start_offset),
        BYTE_OFFSET(target_code_begin, end_offset));
    if (memory != NULL) {
        return write_value32(new_value, memory);
    } else {
        return 1;
    }
}
