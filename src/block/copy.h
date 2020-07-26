#ifndef BLOCK_COPY_H
#define BLOCK_COPY_H

#include <stddef.h>
#include <stdbool.h>

#include "block.h"

void *copy_blocks(BlockList *blocks, void *src_entrypoint, void *dest,
                  size_t dest_size);
bool copy_blocks_test(BlockList * blocks, void * dest, size_t dest_size);
size_t estimate_target_code_size(const BlockList *block_list);

#endif // BLOCK_COPY_H
