#include <stdint.h>

#include "disasm_utility.h"
#include "instruction_op2.h"
#include "size.h"
#include "utility.h"

static bool has_modrm(uint8_t opcode);
static bool is_valid_opcode(uint8_t opcode);

bool handle_2byte_instruction(Instruction *instr) {
    if (instr == NULL || !is_valid_opcode(instr->opcode[1])) {
        return false;
    }
    if (has_modrm(instr->opcode[1])) {
        instr->modrm = (uint8_t *)BYTE_OFFSET(instr->opcode, 2);

        if (instr->addressing_mode == 32 && has_sib_byte(*instr->modrm)) {
            instr->sib = (uint8_t *)BYTE_INCREMENT(instr->modrm);
            instr->displacement_size =
                get_sib_displacement_size(*instr->modrm, *instr->sib);
            if (instr->displacement_size > 0) {
                instr->displacement = (void *)BYTE_INCREMENT(instr->sib);
            }
        }
        if (instr->displacement == NULL) {
            instr->displacement_size = get_modrm_displacement_size(
                *instr->modrm, instr->addressing_mode);
            if (instr->displacement_size > 0) {
                instr->displacement = (void *)BYTE_INCREMENT(
                    instr->sib != NULL ? instr->sib : instr->modrm);
            }
        }
    }

    return true;
}

static bool is_valid_opcode(uint8_t opcode) {
    return true; // TODO: add check
}

static bool has_modrm(uint8_t opcode) {
    return opcode_in_range(opcode, 0x00, 0x03) ||
           opcode_in_range(opcode, 0x10, 0x28) ||
           opcode_in_range(opcode, 0x29, 0x2F) ||
           opcode_in_range(opcode, 0x40, 0x7F) ||
           opcode_in_range(opcode, 0x90, 0xF7) ||
           opcode_in_range(opcode, 0x98, 0x9F) ||
           opcode_in_range(opcode, 0xAB, 0xAF) ||
           opcode_in_range(opcode, 0xB8, 0xBF) ||
           opcode_in_range(opcode, 0xD8, 0xFE) ||
           opcode_in_range(opcode, 0xDF, 0xEF);
}
