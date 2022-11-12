#include "memory.h"
#include "utility.h"

void memzero_local(void *start, uint32_t size) { memset_local(start, 0, size); }

void memcpy_local(void *src, void *dest, uint32_t size) {
  for (uint32_t i = 0; i < size; i++) {
    ((uint8_t *)dest)[i] = ((uint8_t *)src)[i];
  }
}

void memset_local(void *start, uint8_t value, uint32_t size) {
  for (uint8_t *ptr = start; ptr < (uint8_t *)start + size; ptr++) {
    *ptr = value;
  }
}
