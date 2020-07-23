#include "block.h"
#include "disasm/disasm.h"
#include "disasm/disasm_utility.h"
#include "disasm/instruction.h"
#include "malloc.h"
#include "memory.h"
#include "reference.h"
#include "utility.h"

static BlockList *check_block(void *start_address, BlockList *block_list,
                              void *min_addr, void *max_addr);
static void free_block(Block *block);
static Block *find_block_for_address(void *addr, BlockList *block_list);
static void *copy_block(Block *block, Block *next_block, void *dest);
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
static int32_t calculate_new_offset(void *reference, void *reference_origin,
                                    const Block *block, BlockList *blocks);
static void *get_min_address(const BlockList *blocks);
static void *get_max_address(const BlockList *blocks);
static bool check_blocklist_sanity(const BlockList *blocks);
static void print_block(const Block *block);
static BlockList *fix_conditional_block_endings(BlockList *blocks);
static BlockList *append_block(void *block_start, BlockList *block_list);
static BlockList *insert_block(void *block_start, BlockList *block_list);
static int32_t estimate_fixup_overhead(const Block *block,
                                       const Block *next_block);

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

    order_blocks(block_list);
    merge_order_blocks(block_list);
    block_list = fix_conditional_block_endings(block_list);
    print_blocks(block_list);
    if (check_blocklist_sanity(block_list)) {
        PRINT_DEBUG("Blocklist is sane");
    }

    return block_list;
}

static bool check_blocklist_sanity(const BlockList *blocks) {
    for (const BlockList *ble = blocks; ble != NULL; ble = ble->next) {
        const Block *block = ble->block;
        const Block *next_block = ble->next != NULL ? ble->next->block : NULL;
        if (block->dest_alternative != NULL && block->dest != NULL &&
            (next_block == NULL || block->dest != next_block->start)) {
            PRINT_DEBUG("Blocklist not sane: block->dest_alt != NULL, but "
                        "block->dest != next_block->start");
            print_block(block);
            print_block(next_block);
            return false;
        }
    }
    return true;
}

void print_blocks(const BlockList *blocks) {
    int i = 0;
    for (const BlockList *ble = blocks; ble != NULL; ble = ble->next) {
        i++;
        print_block(ble->block);
    }
    PRINT_DEBUG("%d blocks (%d effective)", i, count_effective_blocks(blocks));
}

static void print_block(const Block *block) {
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

void order_blocks(BlockList *blocks) {
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

void merge_order_blocks(BlockList *blocks) {
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

BlockList *fix_conditional_block_endings(BlockList *blocks) {
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

size_t count_effective_blocks(const BlockList *blocks) {
    size_t c = 0;
    for (const BlockList *ble = blocks; ble != NULL; ble = ble->next) {
        if (ble->next == NULL || ble->block->dest != ble->next->block->start) {
            c++;
        }
    }
    return c;
}

BlockList *shuffle_blocks(BlockList *blocks) { return blocks; }

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
        block->end = instr->end;
        block->last_instruction = instr->start;
        if (is_return(instr)) {
            block->dest = NULL;
            block->dest_alternative = NULL;
            found_end = true;
            break;
        } else if (is_jump(instr)) {
            void *target = get_jump_target(instr);
            if (is_conditional_jump(instr)) {
                block->dest = block->end;
                block->dest_alternative = target;
                block_list =
                    check_block(instr->end, block_list, min_addr, max_addr);
            } else if (is_unconditional_jump(instr)) {
                block->dest = target;
                block->dest_alternative = NULL;
            }
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
                    new_block->dest = target_block->dest;
                    new_block->dest_alternative =
                        target_block->dest_alternative;
                    new_block->last_instruction =
                        target_block->last_instruction;

                    target_block->end = target;
                    target_block->dest = target;
                    target_block->last_instruction = NULL;
                }
            }
            found_end = true;
            break;
        }
    }
    if (!found_end) {
        PRINT_DEBUG("Block ended with invalid instruction! @ %p", block->end);
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
        ptr = copy_block(ble->block,
                         ble->next != NULL ? ble->next->block : NULL, ptr);
    }

    return true;
}

static void *copy_block(Block *block, Block *next_block, void *dest) {
    // PRINT_DEBUG("Copying block %p -> %p, size = 0x%X", block->start,
    // dest,
    //            size);
	size_t size = get_block_size(block);
    size_t fixed_size = size;
    block->new_start = dest;
    if (block->last_instruction != NULL) {
        fixed_size -= BYTE_DIFF(block->end, block->last_instruction);
    }
    memcpy_local(
        block->start, dest,
        fixed_size); // don't copy existing block ending instruction (RET, JMP,
                     // JCC), will write the appropriate by hand later, when
                     // delta offsets of all blocks are available

    int32_t max_fixup_size = estimate_fixup_overhead(block, next_block) + (size - fixed_size);
    if (max_fixup_size > 0) {
        memset_local(BYTE_OFFSET(dest, fixed_size), 0x90, max_fixup_size);
    }
    fixed_size += max_fixup_size;

    return BYTE_OFFSET(dest, fixed_size);
}

static int32_t calculate_new_offset(void *reference, void *reference_origin,
                                    const Block *block, BlockList *blocks) {
    int32_t old_offset = BYTE_DIFF(reference, reference_origin);
    const Block *target_block = find_block_for_address(reference, blocks);
    if (target_block == NULL) {
        return 0;
    }
    int32_t block_delta =
        get_block_delta(target_block) - get_block_delta(block);
    return old_offset + block_delta;
}

int32_t get_block_delta(const Block *block) {
    if (block->new_start != NULL) {
        return BYTE_DIFF(block->new_start, block->start);
    }
    return 0;
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

size_t estimate_target_code_size(const BlockList *block_list) {
    size_t sum = 0;
    for (const BlockList *ble = block_list; ble != NULL; ble = ble->next) {
        sum += get_block_size(ble->block) +
               estimate_fixup_overhead(
                   ble->block, ble->next != NULL ? ble->next->block : NULL);
    }
    return sum;
}

static int32_t estimate_fixup_overhead(const Block *block,
                                       const Block *next_block) {
    const int32_t JMP_MAX = 2 + 4; // 2 byte opcode + rel32 offset
    const int32_t JCC_MAX = 1 + 4; // 1 byte opcode + rel32 offset

    if (block != NULL) {
        int32_t existing_size =
            BYTE_DIFF(block->end, block->last_instruction != NULL
                                      ? block->last_instruction
                                      : block->end);
        if (block->dest == NULL) {
            if (block->dest_alternative == NULL) {
                return 0; // ret instruction
            } else if (block->dest_alternative != NULL) {
                return JCC_MAX -
                       existing_size; // JCC with yet-to-determine next instr
                                      // pos, may up/downgrade JCC
            }
        } else if (next_block != NULL && block->dest == next_block->start) {
            if (block->dest_alternative != NULL) {
                return JCC_MAX -
                       existing_size; // Normal JCC, may up/downgrade it
            } else {
                return -existing_size; // May have a JMP or not, if a JMP can
                                       // trash it
            }
        } else if (next_block == NULL || block->dest != next_block->start) {
            return JMP_MAX -
                   existing_size; // May have a JMP or not, if not, add JMP, if
                                  // is JMP may up/dowgrade it
        } else {
            PRINT_DEBUG("Unreachable! dest = %p, dest_alt = %p, last_instr = "
                        "%p, next_start = %p",
                        block->dest, block->dest_alternative,
                        block->last_instruction,
                        next_block != NULL ? next_block->start : NULL);
        }
    }
    return 0;
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
        memzero_local(block, sizeof(Block));
        block->start = start;
        block->end = start;
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

static BlockList *append_block(void *block_start, BlockList *block_list) {
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

static BlockList *insert_block(void *block_start, BlockList *block_list) {
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
