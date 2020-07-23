#include <stddef.h>
#include <stdint.h>

#include "block.h"
#include "malloc.h"
#include "memory.h"
#include "reference.h"
#include "utility.h"

void *get_min_address(const BlockList *blocks) {
    void *min = blocks->block->start;
    while (blocks != NULL) {
        if (blocks->block->start < min) {
            min = blocks->block->start;
        }
        blocks = blocks->next;
    }
    return min;
}

void *get_max_address(const BlockList *blocks) {
    void *max = blocks->block->end;
    while (blocks != NULL) {
        if (blocks->block->end > max) {
            max = blocks->block->end;
        }
        blocks = blocks->next;
    }
    return max;
}

size_t get_code_size(const BlockList *block_list) {
    size_t sum = 0;
    for (const BlockList *ble = block_list; ble != NULL; ble = ble->next) {
        sum += get_block_size(ble->block);
    }
    return sum;
}

size_t get_block_size(const Block *block) {
    if (block != NULL) {
        return BYTE_DIFF(block->end, block->start);
    }
    return 0;
}

Block *create_block(void *start) {
    Block *block = (Block *)MALLOC(sizeof(Block));
    if (block != NULL) {
        memzero_local(block, sizeof(Block));
        block->start = start;
        block->end = start;
    }
    return block;
}

void free_block(Block *block) {
    if (block != NULL) {
        FREE(block);
    }
}

BlockList *push_block(void *block_start, BlockList *block_list) {
    BlockList *new_list = (BlockList *)MALLOC(sizeof(BlockList));
    if (new_list != NULL) {
        new_list->block = create_block(block_start);
        if (new_list->block != NULL) {
            new_list->next = block_list;
        } else {
            free_block_list(block_list);
            free_block_list(new_list);
            return NULL;
        }
    } else {
        free_block_list(block_list);
    }
    return new_list;
}

BlockList *append_block(void *block_start, BlockList *block_list) {
    BlockList *new_list = (BlockList *)MALLOC(sizeof(BlockList));
    if (new_list != NULL) {
        new_list->block = create_block(block_start);
        if (new_list->block != NULL) {
            if (block_list == NULL) {
                return new_list;
            } else {
                for (BlockList *ptr = block_list; ptr != NULL;
                     ptr = ptr->next) {
                    if (ptr->next == NULL) {
                        ptr->next = new_list;
                        return block_list;
                    }
                }
            }
        } else {
            free_block_list(block_list);
            free_block_list(new_list);
            return NULL;
        }
    } else {
        free_block_list(block_list);
        return NULL;
    }
    return block_list;
}

BlockList *insert_block(void *block_start, BlockList *block_list) {
    BlockList *new_list = (BlockList *)MALLOC(sizeof(BlockList));
    if (new_list != NULL) {
        new_list->block = create_block(block_start);
        if (new_list->block != NULL) {
            BlockList *old_next = block_list->next;
            block_list->next = new_list;
            new_list->next = old_next;
        } else {
            free_block_list(block_list);
            return NULL;
        }
    } else {
        free_block_list(block_list);
        return NULL;
    }
    return block_list;
}

void free_block_list(BlockList *block_list) {
    if (block_list != NULL) {
        BlockList *next = block_list->next;
        free_block(block_list->block);
        FREE(block_list);
        block_list = next;
    }
}

Block *top_block(BlockList *block_list) {
    if (block_list != NULL) {
        return block_list->block;
    }
    return NULL;
}

Block *find_block_for_address(void *addr, BlockList *block_list) {
    for (BlockList *ptr = block_list; ptr != NULL; ptr = ptr->next) {
        if (addr >= ptr->block->start && addr < ptr->block->end) {
            return ptr->block;
        }
    }
    return NULL;
}

void print_blocks(const BlockList *blocks) {
    int i = 0;
    for (const BlockList *ble = blocks; ble != NULL; ble = ble->next) {
        i++;
        print_block(ble->block);
    }
    PRINT_DEBUG("%d blocks (%d effective)", i, count_effective_blocks(blocks));
}

void print_block(const Block *block) {
    if (block == NULL) {
        PRINT_DEBUG("NULL");
    } else if (block->dest != NULL) {
        if (block->dest_alternative != NULL) {
            PRINT_DEBUG("start = %p | end = %p | dest = %p | dest_alt = %p",
                        block->start, block->end, block->dest,
                        block->dest_alternative);
        } else {
            PRINT_DEBUG("start = %p | end = %p | dest = %p", block->start,
                        block->end, block->dest);
        }
    } else {
        if (block->dest_alternative != NULL) {
            PRINT_DEBUG(
                "start = %p | end = %p | dest = FIXUP    | dest_alt = %p",
                block->start, block->end, block->dest_alternative);
        } else {
            PRINT_DEBUG("start = %p | end = %p", block->start, block->end);
        }
    }
}

size_t count_effective_blocks(const BlockList *blocks) {
    size_t c = 0;
    for (const BlockList *ble = blocks; ble != NULL; ble = ble->next) {
        if (ble->next == NULL || ble->block->dest != ble->next->block->start) {
            c++;
        }
    }
    return c;
}
