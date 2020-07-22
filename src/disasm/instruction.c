#include <stddef.h>
#include <stdio.h>

#include "disasm_utility.h"
#include "instruction.h"
#include "utility.h"

uint8_t get_instruction_size(const Instruction *instr) {
    if (instr != NULL) {
        return (uint8_t)((uint8_t *)instr->end - (uint8_t *)instr->start);
    } else {
        return 0;
    }
}

bool has_prefix(const Instruction *instr, uint8_t prefix) {
    for (uint8_t i = 0; i < instr->prefix_count; i++) {
        if (instr->prefix[i] == prefix) {
            return true;
        }
    }
    return false;
}

bool is_call(const Instruction *instr) {
    uint8_t opcode = instr->opcode[0];
    return opcode == 0xE8 || opcode == 0x9A ||
           (opcode == 0xFF && (get_modrm_reg(*instr->modrm) == 2 ||
                               get_modrm_reg(*instr->modrm) == 3));
}

bool is_call_direct_offset(const Instruction *instr) {
    return instr->opcode[0] == 0xE8;
}

void *get_call_target(const Instruction *instr) {
    if (is_call_direct_offset(instr)) {
        if (instr->immediate_size == 2) {
            return BYTE_OFFSET(instr->end, *(int16_t *)instr->immediate);
        } else if (instr->immediate_size == 4) {
            return BYTE_OFFSET(instr->end, *(int32_t *)instr->immediate);
        }
    } else {
        // PRINT_DEBUG("WARN: NON-Direct offset");
    }
    return NULL;
}

bool is_return(const Instruction *instr) {
    uint8_t opcode = instr->opcode[0];
    return opcode == 0xC2 || opcode == 0xC3 || opcode == 0xCA ||
           opcode == 0xCB || opcode == 0xCF;
}

bool is_jump(const Instruction *instr) {
    return is_conditional_jump(instr) || is_unconditional_jump(instr);
}

bool is_conditional_jump(const Instruction *instr) {
    return opcode_in_range(instr->opcode[0], 0x70, 0x7F) ||
           opcode_in_range(instr->opcode[0], 0xE0, 0xE3) ||
           (instr->opcode[0] == 0x0F &&
            (opcode_in_range(instr->opcode[1], 0x80, 0x8F)));
}

bool is_unconditional_jump(const Instruction *instr) {
    uint8_t opcode = instr->opcode[0];
    return opcode == 0xEB || opcode == 0xE9 || opcode == 0xEA ||
           (opcode == 0xFF && (get_modrm_reg(*instr->modrm) == 0x4 ||
                               get_modrm_reg(*instr->modrm) == 0x5));
}

bool is_jump_direct_offset(const Instruction *instr) {
    uint8_t opcode = instr->opcode[0];
    return is_conditional_jump(instr) || opcode == 0xEB || opcode == 0xE9;
}

void *get_jump_target(const Instruction *instr) {
    if (is_jump_direct_offset(instr)) {
        if (instr->immediate_size == 1) {
            return BYTE_OFFSET(instr->end, *(int8_t *)instr->immediate);
        } else if (instr->immediate_size == 2) {
            return BYTE_OFFSET(instr->end, *(int16_t *)instr->immediate);
        } else if (instr->immediate_size == 4) {
            return BYTE_OFFSET(instr->end, *(int32_t *)instr->immediate);
        }
    } else {
        // PRINT_DEBUG("WARN: Non-direct jmp");
    }
    return NULL;
}

void write_updated_jump_instruction(const Instruction *old_instr,
                                    void *new_addr, int32_t new_offset) {
    if (is_jump_direct_offset(old_instr)) {
		PRINT_DEBUG("Write jmp @ %p", new_addr);
        uint8_t opcode0 = old_instr->opcode[0];
        if (new_offset <= INT8_MAX && new_offset >= INT8_MIN) {
            if (opcode_in_range(opcode0, 0x70, 0x7F) ||
                opcode0 == 0xEB) { // rel8 jmps, leave them as-is
                *(uint8_t *)new_addr = opcode0;
                *(int8_t *)BYTE_INCREMENT(new_addr) = (int8_t)new_offset;
                PRINT_DEBUG("Write rel8 jmp as-is");
            } else if (opcode0 ==
                       0xE9) { // unconditional rel32, downgrade to rel8
                *(uint8_t *)new_addr = 0xEB;
                *(int8_t *)BYTE_INCREMENT(new_addr) = (int8_t)new_offset;
                PRINT_DEBUG("Write downgraded unconditional jmp");
            } else if (opcode0 == 0x0F) { // is a 2 byte opcode jmp (rel32
                                          // conditional jmps)
                uint8_t opcode1 = old_instr->opcode[1];
                if (opcode_in_range(opcode1, 0x80, 0x8F)) {
                    *(uint8_t *)new_addr = opcode1 - 0x10;
                    *(int8_t *)BYTE_INCREMENT(new_addr) = (int8_t)new_offset;
                    PRINT_DEBUG("Write downgraded conditional jmp");
                }
            }
        } else {
            if (opcode_in_range(opcode0, 0x70,
                                0x7F)) { // conditionals, upgrade to rel32
                PRINT_DEBUG("Write upgraded conditional jmp");
                *(uint8_t *)new_addr = 0x0F;
                *(uint8_t *)BYTE_INCREMENT(new_addr) = opcode0 + 0x10;
                *(int32_t *)BYTE_OFFSET(new_addr, 2) = new_offset;
            } else if (opcode0 == 0xEB) { // unconditional, upgrade to rel32
                PRINT_DEBUG("Write upgraded unconditional jmp");
                *(uint8_t *)new_addr = 0xE9;
                *(int32_t *)BYTE_INCREMENT(new_addr) = (int32_t)new_offset;
            } else if (opcode0 == 0xE9) {
                PRINT_DEBUG("Write rel32 unconditional jmp as-is");
                *(uint8_t *)new_addr = 0xE9;
                *(int32_t *)BYTE_INCREMENT(new_addr) = (int32_t)new_offset;
            } else if (opcode0 == 0x0F) { // is a 2 byte opcode jmp (rel32
                                          // conditional jmps)
                uint8_t opcode1 = old_instr->opcode[1];
                if (opcode_in_range(opcode1, 0x80, 0x8F)) {
                    PRINT_DEBUG("Write rel32 conditional jmp as-is");
                    *(uint8_t *)new_addr = opcode0;
                    *(uint8_t *)BYTE_INCREMENT(new_addr) = opcode1;
                    *(int8_t *)BYTE_OFFSET(new_addr, 2) = (int32_t)new_offset;
                }
            }
        }
    }
}

void print_instruction(const Instruction *instr) {
    PRINT_DEBUG(
        "Instruction | size: %2d | prefix count: %1d | opcode size: %1d | "
        "modrm: %3s | sib: %3s | displacement size: %1d | immediate size: "
        "%1d",
        get_instruction_size(instr), instr->prefix_count, instr->opcode_size,
        instr->modrm == NULL ? "no" : "yes", instr->sib == NULL ? "no" : "yes",
        instr->displacement_size, instr->immediate_size);
}
