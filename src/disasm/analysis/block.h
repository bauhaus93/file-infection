#ifndef DISASM_ANALYSIS_BLOCK_H
#define DISASM_ANALYSIS_BLOCK_H

#include <stddef.h>

#include "disasm/instruction.h"
#include "reference.h"

typedef enum { BLOCK_END_RET, BLOCK_END_JMP, BLOCK_END_JCC } BlockEndType;

typedef struct {
    void *start;
    void *end;
	void *new_start;

    BlockEndType end_type;
	uint8_t end_cond_jmp_type;
    void *dest[2];
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
size_t get_code_size(const BlockList *block_list);
size_t get_target_code_size_max(const BlockList *block_list);
bool copy_blocks(BlockList *blocks, void *dest, size_t dest_size);
ReferenceList *collect_references(const BlockList *blocks, void *min_addr,
                                  void *max_addr);
int32_t get_block_delta(const Block * block);

#endif // DISASM_ANALYSIS_BLOCK_H
