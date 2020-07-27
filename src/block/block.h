#ifndef BLOCK_H
#define BLOCK_H

#include <stddef.h>

typedef struct {
    void *start;
    void *end;
    void *new_start;

    void *last_instruction;

    void *dest;
    void *dest_alternative;
} Block;

typedef struct BlockList_ {
    Block *block;
    struct BlockList_ *next;
} BlockList;

Block *create_block(void *start);
void free_block(Block *block);

BlockList *push_block(void *block_start, BlockList *block_list);
BlockList *append_block(void *block_start, BlockList *block_list);
BlockList *insert_block(void *block_start, BlockList *block_list);
Block *top_block(BlockList *block_list);

void free_block_list(BlockList *block_list);

void print_block(const Block *block);
void print_blocks(const BlockList *blocks);

size_t get_block_size(const Block *block);
size_t get_code_size(const BlockList *block_list);

void *get_target_address(void *address, const BlockList *blocks);
Block *find_block_for_address(void *addr, BlockList *block_list);

void *get_min_address(const BlockList *blocks);
void *get_max_address(const BlockList *blocks);
size_t count_effective_blocks(const BlockList *blocks);

#endif // BLOCK_H
