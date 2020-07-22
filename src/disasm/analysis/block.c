#include "block.h"
#include "disasm/disasm.h"
#include "disasm/instruction.h"
#include "disasm/disasm_utility.h"
#include "malloc.h"
#include "memory.h"
#include "reference.h"
#include "utility.h"

static BlockList *check_block(void *start_address, BlockList *block_list,
                              void *min_addr, void *max_addr);
static void free_block(Block *block);
static Block *find_block_for_address(void *addr, BlockList *block_list);
static size_t count_blocks(const BlockList *blocks);
static void *copy_block(Block *block, void *dest);
static ReferenceList *find_calls_in_block(const Block *block);
static void *get_next_entrypoint(ReferenceList **pending_calls,
                                 ReferenceList **checked_calls);
static ReferenceList *collect_calls_from_blocks(BlockList *block_list);
static ReferenceList *collect_new_calls(ReferenceList *pending_calls,
                                        ReferenceList *checked_calls,
                                        BlockList *block_list, void *min_addr,
                                        void *max_addr);
static ReferenceList *collect_references_from_block(const Block *block,
                                                    void *min_addr,
                                                    void *max_addr,
                                                    ReferenceList *references);
static int32_t calculate_new_offset(void *reference, void * reference_origin,const Block * block,  const BlockList * blocks);
static void *get_min_address(const BlockList *blocks);
static void *get_max_address(const BlockList *blocks);
static size_t get_max_fixup_size(const Block *block);

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
    // PRINT_DEBUG("Analysing block: start = 0x%p", start_address);
    Disassembler disasm;
    setup_disasm(start_address, &disasm);

    block_list = push_block(start_address, block_list);
    Block *block = top_block(block_list);

    bool found_end = false;
    while (next_instruction(&disasm)) {
        const Instruction *instr = get_current_instruction(&disasm);
        if (is_return(instr)) {
            found_end = true;
            break;
        } else if (is_jump(instr)) {
            if (is_conditional_jump(instr)) {
				if (opcode_in_range(instr->opcode[0], 0x70, 0x7F)) {
					block->end_cond_jmp_type = instr->opcode[0] - 0x70;
				} else if(instr->opcode[0] == 0x0F && opcode_in_range(instr->opcode[1], 0x80, 0x8F)) {
					block->end_cond_jmp_type = instr->opcode[1] - 0x80;
				}
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
                    new_block->end_type = target_block->end_type;
                    new_block->dest[0] = target_block->dest[0];
                    new_block->dest[1] = target_block->dest[1];

                    target_block->end = target;
                    target_block->end_type = BLOCK_END_JMP;
                    target_block->dest[0] = target;
                }
            }
            found_end = true;
            break;
        } else { // only write block end, if it's not the instruction which
                 // terminates the block (ret, jmp, jcc) -> will be written new
                 // on copy
            block->end = instr->end;
        }
    }
    if (!found_end) {
        PRINT_DEBUG("Block ended with invalid instruction! @ %p", block->end);
        free_block_list(block_list);
        return NULL;
    }
    const Instruction *end_instr = get_current_instruction(&disasm);
    if (is_return(end_instr)) {
        block->end_type = BLOCK_END_RET;
    } else if (is_jump_direct_offset(end_instr)) {
        void *target = get_jump_target(end_instr);
        if (is_conditional_jump(end_instr)) {
            block->end_type = BLOCK_END_JCC;
            block->dest[0] = target;
            block->dest[1] = end_instr->end;
        } else if (is_unconditional_jump(end_instr)) {
            block->end_type = BLOCK_END_JMP;
            block->dest[0] = target;
        } else {
            PRINT_DEBUG("Jump is neither conditional nor unconditional?!");
            free_block_list(block_list);
            return NULL;
        }
    } else {
        PRINT_DEBUG("0x%X", end_instr->opcode[0]);
        PRINT_DEBUG("Unsupported block end: neither ret nor jmp with "
                    "direct offset");
        free_block_list(block_list);
        return NULL;
    }
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

bool copy_blocks(BlockList *blocks, void *dest, size_t dest_size) {
    void *dest_end = BYTE_OFFSET(dest, dest_size);

    void *ptr = dest;
    PRINT_DEBUG("Copying blocks");
    for (const BlockList *ble = blocks; ble != NULL; ble = ble->next) {
        if (ptr >= dest_end) {
            return false;
        }
        ptr = copy_block(ble->block, ptr);
    }

    PRINT_DEBUG("Fixing up block endings");
    for (const BlockList *ble = blocks; ble != NULL; ble = ble->next) {
        const Block *block = ble->block;

        void *fixup_address = BYTE_OFFSET(block->end, get_block_delta(block));
        if (block->end_type == BLOCK_END_RET) {
            *(uint8_t *)fixup_address = 0xC3;
        } else {
			int32_t new_jmp_taken_offset = calculate_new_offset(block->dest[0], block->end, block, blocks);

            if (block->end_type == BLOCK_END_JMP) {
                if (new_jmp_taken_offset >= INT8_MIN && new_jmp_taken_offset <= INT8_MAX) {
                    *(uint8_t *)fixup_address = 0xEB;
                    *(int8_t *)BYTE_INCREMENT(fixup_address) = new_jmp_taken_offset;

                } else {
                    *(uint8_t *)fixup_address = 0xE9;
                    *(int32_t *)BYTE_INCREMENT(fixup_address) = new_jmp_taken_offset;
                }
            } else if (block->end_type == BLOCK_END_JCC) {
				size_t off = 0;
				if (new_jmp_taken_offset >= INT8_MIN && new_jmp_taken_offset <= INT8_MAX) {
                    *(uint8_t *)fixup_address = 0x70 + block->end_cond_jmp_type;
                    *(int8_t *)BYTE_INCREMENT(fixup_address) = new_jmp_taken_offset;
					off = 2;
				} else {
                    *(uint8_t *)fixup_address = 0x0F;
                    *(uint8_t *)BYTE_INCREMENT(fixup_address) = 0x80 + block->end_cond_jmp_type;
                    *(int32_t *)BYTE_OFFSET(fixup_address, 2) = new_jmp_taken_offset;
					off = 2 + 4;
				}
				fixup_address = BYTE_OFFSET(fixup_address, off);
				int32_t new_jmp_not_taken_offset = calculate_new_offset(block->dest[1], BYTE_OFFSET(block->end, off), block, blocks);
				if (new_jmp_not_taken_offset >= INT8_MIN && new_jmp_not_taken_offset <= INT8_MAX) {
                    *(uint8_t *)fixup_address = 0xEB;
                    *(int8_t *)BYTE_INCREMENT(fixup_address) = new_jmp_not_taken_offset;

                } else {
                    *(uint8_t *)fixup_address = 0xE9;
                    *(int32_t *)BYTE_INCREMENT(fixup_address) = new_jmp_not_taken_offset;
                }

            } else {
                PRINT_DEBUG("Invalid block end type");
                free_block_list(blocks);
                return NULL;
            }
        }
    }

    return true;
}

static int32_t calculate_new_offset(void *reference, void * reference_origin,const Block * block,  const BlockList * blocks) {
	int32_t old_offset = BYTE_DIFF(reference, reference_origin);
            const Block *target_block =
                find_block_for_address(reference, blocks);
            if (target_block == NULL) {
                return 0;
            }
            int32_t block_delta =
                get_block_delta(target_block) - get_block_delta(block);
	return old_offset + block_delta;
}

static void *copy_block(Block *block, void *dest) {
    size_t size = get_block_size(block);
    // PRINT_DEBUG("Copying block %p -> %p, size = 0x%X", block->start, dest,
    //            size);
    memcpy_local(block->start, dest, size);

    size_t additional_size = get_max_fixup_size(block);
    memset_local(BYTE_OFFSET(dest, size), 0x90, additional_size);
    size += additional_size;

    block->new_start = dest;

    return BYTE_OFFSET(dest, size);
}

int32_t get_block_delta(const Block *block) {
    if (block->new_start != NULL) {
        return BYTE_DIFF(block->new_start, block->start);
    }
    return 0;
}

static size_t get_max_fixup_size(const Block *block) {
    size_t fixup_size = 1;                  // ret needs only 1 byte
    if (block->end_type == BLOCK_END_JMP) { // 1 byte opcode, max 4 byte offset
        fixup_size = 1 + 4;
    } else if (block->end_type ==
               BLOCK_END_JCC) { // 1-2 byte opcode, max 4 byte offset, plus  1 +
                                // 4 for uncond jmp for jmp-is-not-taken branch
        fixup_size = 2 + 4 + 1 + 4;
    }
    return fixup_size;
}

static void *get_min_address(const BlockList *blocks) {
    void *min = blocks->block->start;
    while (blocks != NULL) {
        if (blocks->block->start < min) {
            min = blocks->block->start;
        }
        blocks = blocks->next;
    }
    return min;
}

static void *get_max_address(const BlockList *blocks) {
    void *max = blocks->block->end;
    while (blocks != NULL) {
        if (blocks->block->end > max) {
            max = blocks->block->end;
        }
        blocks = blocks->next;
    }
    return max;
}

ReferenceList *collect_references(const BlockList *blocks, void *min_addr,
                                  void *max_addr) {
    ReferenceList *references = NULL;
    for (const BlockList *ble = blocks; ble != NULL; ble = ble->next) {
        references = collect_references_from_block(ble->block, min_addr,
                                                   max_addr, references);
    }
    return references;
}

size_t get_code_size(const BlockList *block_list) {
    size_t sum = 0;
    for (const BlockList *ble = block_list; ble != NULL; ble = ble->next) {
        sum += get_block_size(ble->block);
    }
    return sum;
}

size_t get_target_code_size_max(const BlockList *block_list) {
    size_t sum = 0;
    for (const BlockList *ble = block_list; ble != NULL; ble = ble->next) {
        sum += get_block_size(ble->block) + get_max_fixup_size(ble->block);
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
        block->new_start = NULL;
        block->dest[0] = NULL;
        block->dest[1] = NULL;
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

static ReferenceList *collect_references_from_block(const Block *block,
                                                    void *min_addr,
                                                    void *max_addr,
                                                    ReferenceList *references) {
    Disassembler disasm;
    setup_disasm(block->start, &disasm);
    while (next_instruction(&disasm)) {
        const Instruction *instr = get_current_instruction(&disasm);
        if (instr->start >= block->end) {
            break;
        }
        if (is_call(instr)) {
            void *target = get_call_target(instr);
            if (target != NULL && target >= min_addr && target < max_addr) {
                references = push_reference(target, instr->start, references);
            }
        } else if (is_jump(instr)) {
            void *target = get_jump_target(instr);
            if (target != NULL && target >= min_addr && target < max_addr) {
                references = push_reference(target, instr->start, references);
            }
        }
    }
    return references;
}
