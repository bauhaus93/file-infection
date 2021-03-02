#ifndef BLOCK_COPY_H
#define BLOCK_COPY_H

#include <stdbool.h>
#include <stddef.h>

#include "block.h"

bool copy_blocks(BlockList *blocks, void *dest, size_t dest_size);
size_t estimate_target_code_size(const BlockList *block_list);

#endif // BLOCK_COPY_H
