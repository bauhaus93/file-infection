#include <stdbool.h>

#include "block.h"
#include "prepare.h"

static void order_blocks(BlockList *blocks);
static void merge_order_blocks(BlockList *blocks);
static BlockList *fix_conditional_block_endings(BlockList *blocks);
static bool check_blocklist_sanity(const BlockList *blocks);

BlockList *prepare_blocks(BlockList *blocks) {
  order_blocks(blocks);
  merge_order_blocks(blocks);
  blocks = fix_conditional_block_endings(blocks);
  if (blocks == NULL) {
    return NULL;
  }
  if (!check_blocklist_sanity(blocks)) {
    free_block_list(blocks);
    return NULL;
  }
  return blocks;
}

static void order_blocks(BlockList *blocks) {
  for (BlockList *ble = blocks; ble != NULL; ble = ble->next) {
    BlockList *min = ble;
    for (BlockList *blef = ble->next; blef != NULL; blef = blef->next) {
      if (ble->block->dest == NULL && blef->block->dest != NULL) {
        min = blef;
      } else if (blef->block->dest != NULL &&
                 blef->block->dest < min->block->dest) {
        min = blef;
      }
    }
    Block *xchg = ble->block;
    ble->block = min->block;
    min->block = xchg;
  }
}

static void merge_order_blocks(BlockList *blocks) {
  for (BlockList *ble = blocks; ble != NULL; ble = ble->next) {
    if (ble->block->dest == NULL) {
      continue;
    }
    for (BlockList *blef = ble->next; blef != NULL; blef = blef->next) {
      if (ble->block->dest == blef->block->start) {
        Block *xchg = ble->next->block;
        ble->next->block = blef->block;
        blef->block = xchg;
        break;
      }
    }
  }
}

static BlockList *fix_conditional_block_endings(BlockList *blocks) {
  for (BlockList *ble = blocks; ble != NULL; ble = ble->next) {
    Block *block = ble->block;
    if (block->dest_alternative != NULL) {
      Block *next_block = ble->next != NULL ? ble->next->block : NULL;
      if (next_block == NULL || block->dest != next_block->start) {
        if (insert_block(block->end, ble) == NULL) {
          free_block_list(blocks);
          return NULL;
        }
        Block *fixup_block = ble->next->block;
        void *old_block_dest = block->dest;
        block->dest = NULL;
        fixup_block->dest = old_block_dest;
      }
    }
  }
  return blocks;
}

static bool check_blocklist_sanity(const BlockList *blocks) {
  for (const BlockList *ble = blocks; ble != NULL; ble = ble->next) {
    const Block *block = ble->block;
    const Block *next_block = ble->next != NULL ? ble->next->block : NULL;
    if (block->dest_alternative != NULL && block->dest != NULL &&
        (next_block == NULL || block->dest != next_block->start)) {
      return false;
    }
  }
  return true;
}
