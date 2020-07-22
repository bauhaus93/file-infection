#include "block.h"
#include "disasm/disasm.h"
#include "disasm/instruction.h"
#include "malloc.h"
#include "reference.h"
#include "utility.h"

static BlockList *check_block(void *start_address, BlockList *block_list,
                              void *min_addr, void *max_addr);
static void free_block(Block *block);
static Block *find_block_for_address(void *addr, BlockList *block_list);
static ReferenceList *find_calls_in_block(const Block *block);
static void *get_next_entrypoint(ReferenceList **pending_calls,
                                 ReferenceList **checked_calls);
static ReferenceList *collect_calls_from_blocks(BlockList *block_list);
static ReferenceList *collect_new_calls(ReferenceList *pending_calls,
                                        ReferenceList *checked_calls,
                                        BlockList *block_list, void *min_addr,
                                        void *max_addr);

BlockList *collect_blocks(void **entry_points, void *min_addr, void *max_addr) {
    BlockList *block_list = NULL;

    ReferenceList *pending_calls = NULL;
    for (int i = 0; entry_points[i] != NULL; i++) {
        pending_calls = push_reference(entry_points[i], NULL, pending_calls);
    }
    ReferenceList *checked_calls = NULL;

    while (pending_calls != NULL) {
        void *entrypoint = get_next_entrypoint(&pending_calls, &checked_calls);

        block_list = check_block(entrypoint, block_list, min_addr, max_addr);
        if (block_list == NULL) {
            break;
        }
        if (pending_calls == NULL) {
            pending_calls = collect_new_calls(pending_calls, checked_calls,
                                              block_list, min_addr, max_addr);
        }
    }
    free_reference_list(pending_calls);
    free_reference_list(checked_calls);
    return block_list;
}

static BlockList *check_block(void *start_address, BlockList *block_list,
                              void *min_addr, void *max_addr) {
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
                block_list =
                    check_block(instr->end, block_list, min_addr, max_addr);
            }
            void *target = get_jump_target(instr);
            if (target != NULL && target >= min_addr && target < max_addr) {
                Block *target_block =
                    find_block_for_address(target, block_list);
                if (target_block == NULL) {
                    block_list =
                        check_block(target, block_list, min_addr, max_addr);
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

static void *get_next_entrypoint(ReferenceList **pending_calls,
                                 ReferenceList **checked_calls) {
    if (*pending_calls != NULL) {
        void *entrypoint = top_reference_dest(*pending_calls);
        *pending_calls = pop_reference(*pending_calls);
        *checked_calls = push_reference(entrypoint, NULL, *checked_calls);
        return entrypoint;
    }
    return NULL;
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

static ReferenceList *find_calls_in_block(const Block *block) {
    Disassembler disasm;
    setup_disasm(block->start, &disasm);

    ReferenceList *references = NULL;

    while (next_instruction(&disasm)) {
        const Instruction *instr = get_current_instruction(&disasm);
        if (instr->end > block->end) {
            break;
        }
        if (is_call(instr)) {
            void *target = get_call_target(instr);
            if (target != NULL) {
                references = push_reference(target, NULL, references);
            }
        }
    }
    return references;
}

static ReferenceList *collect_new_calls(ReferenceList *pending_calls,
                                        ReferenceList *checked_calls,
                                        BlockList *block_list, void *min_addr,
                                        void *max_addr) {
    ReferenceList *new_calls = collect_calls_from_blocks(block_list);
    while (new_calls != NULL) {
        void *new_target = top_reference_dest(new_calls);
        if (new_target >= min_addr && new_target <= max_addr) {
            if (!reference_in_list(new_target, pending_calls) &&
                !reference_in_list(new_target, checked_calls)) {
                pending_calls = push_reference(new_target, NULL, pending_calls);
            }
        }
        new_calls = pop_reference(new_calls);
    }
    return pending_calls;
}

static ReferenceList *collect_calls_from_blocks(BlockList *block_list) {
    ReferenceList *calls = NULL;
    for (BlockList *ptr = block_list; ptr != NULL; ptr = ptr->next) {
        ReferenceList *block_calls = find_calls_in_block(ptr->block);
        for (ReferenceList *call = block_calls; call != NULL;
             call = call->next) {
            if (!reference_in_list(call->dest, calls)) {
                calls = push_reference(call->dest, NULL, calls);
            }
        }
    }
    return calls;
}
