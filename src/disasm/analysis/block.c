#include "block.h"
#include "call.h"
#include "disasm/disasm.h"
#include "disasm/instruction.h"
#include "malloc.h"
#include "utility.h"

static void free_block(Block *block);
static Block *find_block_for_address(void *addr, BlockList *block_list);
static CallList *find_calls_in_block(const Block *block);

BlockList *analyze_block(void *start_address, BlockList *block_list, void * min_addr, void * max_addr) {
    PRINT_DEBUG("Analysing block: start = 0x%p", start_address);
    Disassembler disasm;
    setup_disasm(start_address, &disasm);

    block_list = push_block(start_address, block_list);
    Block *block = top_block(block_list);

    while (next_instruction(&disasm)) {
        const Instruction *instr = get_current_instruction(&disasm);
        block->end = instr->end;
        if (is_return(instr)) {
            break;
        } else if (is_jump(instr)) {
            if (is_conditional_jump(instr)) {
                block_list = analyze_block(instr->end, block_list, min_addr, max_addr);
            }
            void *target = get_jump_target(instr);
            if (target != NULL && target >= min_addr && target < max_addr) {
                Block *target_block =
                    find_block_for_address(target, block_list);
                if (target_block == NULL) {
                    block_list = analyze_block(target, block_list, min_addr, max_addr);
                } else if (target_block->start !=
                           target) { // split up existing block
                    block_list = push_block(target, block_list);
                    Block *new_block = top_block(block_list);
                    new_block->end = target_block->end;
                    target_block->end = target;
                }
            }
            break;
        }
    }
    block->last_instruction = get_current_instruction(&disasm);
    return block_list;
}

size_t get_block_size(const Block * block) {
	if (block != NULL) {
		return BYTE_DIFF(block->end, block->start);
	}
	return 0;
}

static Block *create_block(void *start) {
    Block *block = (Block *)MALLOC(sizeof(Block));
    if (block != NULL) {
        block->start = start;
        block->end = start;
        block->last_instruction = NULL;
    }
    return block;
}

static void free_block(Block *block) {
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

static Block *find_block_for_address(void *addr, BlockList *block_list) {
    for (BlockList *ptr = block_list; ptr != NULL; ptr = ptr->next) {
        if (addr >= ptr->block->start && addr < ptr->block->end) {
            return ptr->block;
        }
    }
    return NULL;
}

static CallList *find_calls_in_block(const Block *block) {
    Disassembler disasm;
    setup_disasm(block->start, &disasm);

    CallList *calls = NULL;

    while (next_instruction(&disasm)) {
        const Instruction *instr = get_current_instruction(&disasm);
		if (instr->end > block->end) {
			break;
		}
        if (is_call(instr)) {
            void *target = get_call_target(instr);
            if (target != NULL) {
                calls = push_call(target, calls);
            }
        }
    }
    return calls;
}

CallList *collect_calls_from_blocks(BlockList *block_list) {
    CallList *calls = NULL;
    for (BlockList *ptr = block_list; ptr != NULL; ptr = ptr->next) {
        CallList *block_calls = find_calls_in_block(ptr->block);
        for (CallList *call = block_calls; call != NULL; call = call->next) {
            if (!call_in_list(call->address, calls)) {
                calls = push_call(call->address, calls);
            }
        }
    }
    return calls;
}
