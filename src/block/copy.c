#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "block.h"
#include "copy.h"
#include "discovery.h"
#include "memory.h"
#include "prepare.h"
#include "utility.h"

static void *copy_blocks(BlockList *blocks, void *src_entrypoint, void *dest,
                         size_t dest_size);
static void *copy_block(Block *block, Block *next_block, void *dest);
static int32_t get_block_delta(const Block *block);
static int32_t calculate_new_offset(void *reference, void *reference_origin,
                                    const Block *block, BlockList *blocks);
static size_t estimate_target_code_size(const BlockList *block_list);
static int32_t estimate_fixup_overhead(const Block *block,
                                       const Block *next_block);
static bool check_blocklist_sanity(const BlockList *blocks);

void *discover_and_copy(void **entrypoints, void *src_min, void *src_max,
                        void *target_addr, size_t target_size) {
    PRINT_DEBUG("Discover blocks");
    BlockList *blocks = discover_blocks(entrypoints, src_min, src_max);
    if (blocks == NULL) {
        return NULL;
    }

    size_t estimation_size = estimate_target_code_size(blocks);
    if (estimation_size > target_size) {
        free_block_list(blocks);
        return NULL;
    }

    PRINT_DEBUG("Prepare blocks");
    order_blocks(blocks);
    merge_order_blocks(blocks);
    blocks = fix_conditional_block_endings(blocks);
    if (blocks == NULL) {
        return NULL;
    }
    print_blocks(blocks);
    if (check_blocklist_sanity(blocks)) {
        free_block_list(blocks);
        return NULL;
    }
    PRINT_DEBUG("Copy blocks");
    void *dest_entrypoint =
        copy_blocks(blocks, entrypoints[0], target_addr, target_size);

    free_block_list(blocks);
    return dest_entrypoint;
}

void *copy_blocks(BlockList *blocks, void *src_entrypoint, void *dest,
                  size_t dest_size) {
    void *dest_entrypoint = NULL;
    void *dest_end = BYTE_OFFSET(dest, dest_size);

    void *ptr = dest;
    PRINT_DEBUG("Copying blocks");
    for (const BlockList *ble = blocks; ble != NULL; ble = ble->next) {
        if (ptr >= dest_end) {
            return NULL;
        }
        ptr = copy_block(ble->block,
                         ble->next != NULL ? ble->next->block : NULL, ptr);
        if (ble->block->start == src_entrypoint) {
            dest_entrypoint = ble->block->new_start;
        }
    }
    return dest_entrypoint;
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

    int32_t max_fixup_size =
        estimate_fixup_overhead(block, next_block) + (size - fixed_size);
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

static int32_t get_block_delta(const Block *block) {
    if (block->new_start != NULL) {
        return BYTE_DIFF(block->new_start, block->start);
    }
    return 0;
}

static size_t estimate_target_code_size(const BlockList *block_list) {
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
