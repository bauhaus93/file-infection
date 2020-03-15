#include "memory.h"

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
