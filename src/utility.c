#include <windows.h>

#include "code_begin.h"
#include "code_end.h"
#include "utility.h"

int32_t get_delta_offset(void) {
    int32_t offset;
    if (IS_32_BIT) {
        asm("call .base\n"
            ".base:\n"
            "pop %%edx\n"
            "sub $.base, %%edx\n"
            "mov %%edx, %0"
            : "=r"(offset));
    } else {
        asm("call .base\n"
            ".base:\n"
            "pop %%rdx\n"
            "sub $.base, %%rdx\n"
            "mov %%edx, %0"
            : "=r"(offset));
    }
    return offset;
}

void memzero(void *start, uint32_t size) {
    for (uint8_t *ptr = start; ptr < (uint8_t *)start + size; ptr++) {
        *ptr = 0;
    }
}

void memcp(void *src, void *dest, uint32_t size) {
    for (uint32_t i = 0; i < size; i++) {
        ((uint8_t *)dest)[i] = ((uint8_t *)src)[i];
    }
}

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

// global data must already be initialized and pointer to it set
uint8_t write_value32(uint32_t value, void *memory) {
    DWORD old_protect;
    if (!VIRTUAL_PROTECT((LPVOID)memory, (SIZE_T)sizeof(uint32_t),
                         PAGE_EXECUTE_READWRITE, &old_protect)) {
        PRINT_DEBUG("could not change acces rights");
        return 1;
    }
    *(uint32_t *)memory = value;
    VIRTUAL_PROTECT((LPVOID)memory,
                    (SIZE_T)sizeof(uint32_t), old_protect,
                    &old_protect);
    return 0;
}

// global data must already be initialized and pointer to it set
uint8_t find_replace_value32(uint32_t search_value, uint32_t new_value,
                             void *symbol_start_addr, void *symbol_end_addr,
                             void *target_code_begin) {
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
