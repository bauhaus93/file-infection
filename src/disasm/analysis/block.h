#ifndef DISASM_ANALYSIS_BLOCK_H
#define DISASM_ANALYSIS_BLOCK_H

#include "call.h"
#include "disasm/instruction.h"

typedef struct {
    void *start;
    void *end;
    CallList *calls;
    Instruction *last_instruction;
} Block;

typedef struct BlockList_ {
    Block *block;
    struct BlockList_ *next;
} BlockList;

BlockList *analyze_block(void *start_address, BlockList *block_list);
BlockList *push_block(void *block_start, BlockList *block_list);
void free_block_list(BlockList *block_list);
CallList *collect_calls_from_blocks(BlockList *block_list);

#endif // DISASM_ANALYSIS_BLOCK_H
