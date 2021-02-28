#include <stdint.h>

#include "disasm_utility.h"
#include "instruction.h"
#include "instruction_op2.h"
#include "size.h"
#include "utility.h"

static bool has_immediate_Ib(uint8_t opcode);
static bool has_immediate_Jz(uint8_t opcode);

bool is_valid_opcode_2byte(const Instruction *instr) {
    uint8_t opcode = instr->opcode[1];
    return !(opcode_in_range(opcode, 0x1C, 0x1D) ||
             opcode_in_range(opcode, 0x24, 0x27) ||
             opcode_in_range(opcode, 0x38, 0x3F) || opcode == 0x07 ||
             opcode == 0x0A || opcode == 0x0C || opcode == 0x0E ||
             opcode == 0x36 || opcode == 0x0F || opcode == 0x19 ||
             opcode == 0xA6 || opcode == 0xA7);
}

bool has_modrm_2byte(const Instruction *instr) {
    uint8_t opcode = instr->opcode[1];
    return opcode_in_range(opcode, 0x00, 0x03) ||
           opcode_in_range(opcode, 0x1A, 0x1F) ||
           opcode_in_range(opcode, 0x10, 0x28) ||
           opcode_in_range(opcode, 0x29, 0x2F) ||
           opcode_in_range(opcode, 0x40, 0x6F) ||
           opcode_in_range(opcode, 0x70, 0x76) ||
           opcode_in_range(opcode, 0x78, 0x7F) ||
           opcode_in_range(opcode, 0x90, 0x97) ||
           opcode_in_range(opcode, 0xA3, 0xA7) ||
           opcode_in_range(opcode, 0xB0, 0xF7) ||
           opcode_in_range(opcode, 0x98, 0x9F) ||
           opcode_in_range(opcode, 0xAB, 0xAF) ||
           opcode_in_range(opcode, 0xBA, 0xBF) ||
           opcode_in_range(opcode, 0xD8, 0xFE) ||
           opcode_in_range(opcode, 0xDF, 0xEF) || opcode == 0x0D ||
           opcode == 0xB8 || opcode == 0xFF;
}

bool has_opcode_extension_2byte(const Instruction *instr) {
    uint8_t opcode = instr->opcode[1];
    return opcode_in_range(opcode, 0x71, 0x73) || opcode == 0x00 ||
           opcode == 0x01 || opcode == 0xBA || opcode == 0xC7 ||
           opcode == 0xB9 || opcode == 0xAE || opcode == 0x18;
}

uint8_t get_immediate_size_2byte(const Instruction *instr) {
    uint8_t opcode = instr->opcode[1];
    if (has_immediate_Ib(opcode)) {
        return get_size_by_operand_type(OPERAND_TYPE_B, instr->operand_size);
    } else if (has_immediate_Jz(opcode)) {
        return get_size_by_operand_type(OPERAND_TYPE_Z, instr->operand_size);
    } else {
        return 0;
    }
}

uint8_t get_opcode_extension_immediate_size_2byte(const Instruction *instr) {
    if (instr->modrm == NULL) {
        return 0;
    }
    uint8_t opcode = instr->opcode[1];
    uint8_t modrm = *instr->modrm;
    uint8_t nnn = get_modrm_reg(modrm);
    if ((opcode == 0x71 || opcode == 0x72) &&
        (nnn == 0x2 || nnn == 0x4 || nnn == 0x6)) {
        return get_size_by_operand_type(OPERAND_TYPE_B, instr->operand_size);
    } else if (opcode == 0x73 && (nnn == 0x2 || nnn == 0x3 || nnn >= 0x6)) {
        return get_size_by_operand_type(OPERAND_TYPE_B, instr->operand_size);
    } else {
        return 0;
    }
}

static bool has_immediate_Ib(uint8_t opcode) {
    return opcode_in_range(opcode, 0xC4, 0xC6) || opcode == 0x70 ||
           opcode == 0xA4 || opcode == 0xAC || opcode == 0xBA || opcode == 0xC2;
}

static bool has_immediate_Jz(uint8_t opcode) {
    return opcode_in_range(opcode, 0x80, 0x8F);
}
