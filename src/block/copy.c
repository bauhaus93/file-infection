#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "block.h"
#include "copy.h"
#include "disasm/disasm.h"
#include "disasm/instruction.h"
#include "discovery.h"
#include "memory.h"
#include "prepare.h"
#include "utility.h"

static void *copy_block(Block *block, Block *next_block, void *dest);
static int32_t get_block_delta(const Block *block);
static int32_t calculate_new_offset(void *reference, void *reference_origin,
                                    const Block *block, BlockList *blocks);
static int32_t estimate_fixup_overhead(const Block *block,
                                       const Block *next_block);

static bool fix_block_endings(BlockList *blocks);
static void write_ret(void *addr);
static void write_endbr32(void *addr);
static bool write_jcc(void *addr, int32_t offset, const Instruction *old_instr);
static void write_jmp(void *addr, int32_t offset);

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

    if (!fix_block_endings(blocks)) {
        return false;
    }

    return true;
}

bool copy_blocks_test(BlockList *blocks, void *dest, size_t dest_size) {
    void *dest_end = BYTE_OFFSET(dest, dest_size);

    void *ptr = dest;
    PRINT_DEBUG("Copying blocks");
    for (const BlockList *ble = blocks; ble != NULL; ble = ble->next) {
        if (ptr >= dest_end) {
            return false;
        }
        size_t bs = get_block_size(ble->block);
        memcpy_local(ble->block->start, ptr, bs);
        ptr = BYTE_OFFSET(ptr, bs);
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

    int32_t max_fixup_size =
        estimate_fixup_overhead(block, next_block) + (size - fixed_size);
    if (max_fixup_size > 0) {
        memset_local(BYTE_OFFSET(dest, fixed_size), 0x90, max_fixup_size);
    }
    fixed_size += max_fixup_size;

    return BYTE_OFFSET(dest, fixed_size);
}

static bool fix_block_endings(BlockList *blocks) {
    for (BlockList *ble = blocks; ble != NULL; ble = ble->next) {
        Block *block = ble->block;
        if (block->last_instruction != NULL) {
            Instruction instr;
            if (parse_instruction(block->last_instruction, &instr)) {
                void *target_instruction = BYTE_OFFSET(
                    block->new_start,
                    BYTE_DIFF(block->last_instruction, block->start));
                if (is_return(&instr)) {
                    write_ret(target_instruction);
                } else if (is_endbr(&instr)) {
                    write_endbr32(target_instruction);
                } else if (is_jump_direct_offset(&instr)) {
                    if (is_conditional_jump(&instr)) {
                        int32_t new_offset = calculate_new_offset(
                            block->dest_alternative, instr.end, block, blocks);
                        if (!write_jcc(target_instruction, new_offset,
                                       &instr)) {
                            PRINT_DEBUG("Unsupported jcc given");
                            return false;
                        }
                    } else {
                        void *next_start =
                            ble->next != NULL ? ble->next->block->start : NULL;
                        if (block->dest != next_start) {
                            int32_t new_offset = calculate_new_offset(
                                block->dest, instr.end, block, blocks);
                            write_jmp(target_instruction, new_offset);
                        }
                    }
                } else {
                    PRINT_DEBUG("Can't fix non-rel8/32 jmps");
                    return false;
                }
            } else {
                return false;
            }
        }
    }
    return true;
}

static void write_ret(void *addr) { *(uint8_t *)addr = 0xC3; }

static void write_endbr32(void *addr) {
    uint8_t *x = (uint8_t *)addr;
    x[0] = 0xF3;
    x[1] = 0x0F;
    x[2] = 0x1E;
    x[3] = 0xFB;
}

static bool write_jcc(void *addr, int32_t offset,
                      const Instruction *old_instr) {
    uint8_t *opcode = old_instr->opcode;
    uint8_t jcc_type = 0;
    if (opcode[0] >= 0x70 && opcode[0] <= 0x7F) {
        jcc_type = opcode[0] - 0x70;
    } else if (opcode[0] == 0x0F && opcode[1] >= 0x80 && opcode[1] <= 0x8F) {
        jcc_type = opcode[1] - 0x80;
    } else {
        return false;
    }

    if (offset >= INT8_MIN && offset <= INT8_MAX) {
        *(uint8_t *)addr = 0x70 + jcc_type;
        *(int8_t *)BYTE_INCREMENT(addr) = (int8_t)offset;
    } else if (offset >= INT16_MIN && offset <= INT16_MAX) {
        *(uint8_t *)addr = 0x66;
        *(uint8_t *)BYTE_INCREMENT(addr) = 0x0F;
        *(uint8_t *)BYTE_OFFSET(addr, 2) = 0x80 + jcc_type;
        *(int16_t *)BYTE_OFFSET(addr, 3) = (int16_t)offset;
    } else {
        *(uint8_t *)addr = 0x0F;
        *(uint8_t *)BYTE_INCREMENT(addr) = 0x80 + jcc_type;
        *(int32_t *)BYTE_OFFSET(addr, 2) = offset;
    }
    return true;
}

static void write_jmp(void *addr, int32_t offset) {
    if (offset >= INT8_MIN && offset <= INT8_MAX) {
        *(uint8_t *)addr = 0xEB;
        *(int8_t *)BYTE_INCREMENT(addr) = (int8_t)offset;
    } else if (offset >= INT16_MIN && offset <= INT16_MAX) {
        *(uint8_t *)addr = 0x66;
        *(uint8_t *)BYTE_INCREMENT(addr) = 0xE9;
        *(int16_t *)BYTE_OFFSET(addr, 2) = (int16_t)offset;
    } else {
        *(uint8_t *)addr = 0xE9;
        *(int32_t *)BYTE_INCREMENT(addr) = (int32_t)offset;
    }
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

// only used for testing
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
                   existing_size; // May have a JMP or not, if not, add JMP,
                                  // if is JMP may up/dowgrade it
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
