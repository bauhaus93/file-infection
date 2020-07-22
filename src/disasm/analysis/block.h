#ifndef DISASM_ANALYSIS_BLOCK_H
#define DISASM_ANALYSIS_BLOCK_H

#include <stddef.h>

#include "reference.h"
#include "disasm/instruction.h"

typedef struct {
    void *start;
    void *end;
    Instruction *last_instruction;
} Block;

typedef struct BlockList_ {
    Block *block;
    struct BlockList_ *next;
} BlockList;

BlockList *collect_blocks(void **entry_points, void *min_addr, void *max_addr);
BlockList *push_block(void *block_start, BlockList *block_list);
Block *top_block(BlockList *block_list);
void free_block_list(BlockList *block_list);
size_t get_block_size(const Block *block);

#endif // DISASM_ANALYSIS_BLOCK_H
