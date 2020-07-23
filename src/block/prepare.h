#ifndef BLOCK_PREPARE_H
#define BLOCK_PREPARE_H

#include "block.h"

void order_blocks(BlockList *blocks);
void merge_order_blocks(BlockList *blocks);
BlockList *fix_conditional_block_endings(BlockList *blocks);

#endif // BLOCK_PREPARE_H
