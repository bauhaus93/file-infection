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

void *memalloc(size_t size, function_list_t *function_list) {
    HANDLE h_heap = function_list->get_process_heap();
    if (h_heap == 0) {
        return NULL;
    }
    return (void *)function_list->heap_alloc(h_heap, HEAP_ZERO_MEMORY, size);
}

void memfree(void *mem, function_list_t *function_list) {
    if (mem != NULL) {
        HANDLE h_heap = function_list->get_process_heap();
        if (h_heap != 0) {
            function_list->heap_free(h_heap, 0, mem);
        }
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
