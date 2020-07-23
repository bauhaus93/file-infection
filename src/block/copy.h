#ifndef BLOCK_COPY_H
#define BLOCK_COPY_H

#include <stddef.h>

void *discover_and_copy(void **entrypoints, void *src_min, void *src_max,
                        void *target_addr, size_t target_size);

#endif // BLOCK_COPY_H
