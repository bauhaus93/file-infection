#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

#define BYTE_OFFSET(start, byte_offset)                                        \
  ((void *)(((uint8_t *)(start)) + (byte_offset)))

#define BYTE_INCREMENT(addr) BYTE_OFFSET(addr, 1)
#define BYTE_DECREMENT(addr) BYTE_OFFSET(addr, -1)

#define BYTE_DIFF(hi, lo) ((int32_t)(((uint8_t *)(hi)) - ((uint8_t *)(lo))))

void memzero_local(void *start, uint32_t size);
void memcpy_local(void *src, void *dest, uint32_t size);
void memset_local(void *start, uint8_t value, uint32_t size);

#endif // MEMORY_H
