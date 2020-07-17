#include <stdint.h>

#include "disasm_utility.h"
#include "instruction_op2.h"
#include "size.h"
#include "utility.h"

static bool has_modrm(uint8_t opcode);
static bool has_opcode_extension(uint8_t opcode);
static bool is_valid_opcode(uint8_t opcode);
static uint8_t get_opcode_extension_immediate_size(const Instruction *instr);
static bool has_immediate_Ib(uint8_t opcode);
static bool has_immediate_Jz(uint8_t opcode);
static uint8_t get_immediate_size(const Instruction *instr);

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

    instr->immediate_size = get_immediate_size(instr);
    if (instr->immediate_size > 0) {
        if (instr->displacement != NULL) {
            instr->immediate =
                BYTE_OFFSET(instr->displacement, instr->displacement_size);
        } else if (instr->sib != NULL) {
            instr->immediate = BYTE_INCREMENT(instr->sib);
        } else if (instr->modrm != NULL) {
            instr->immediate = BYTE_INCREMENT(instr->modrm);
        } else {
            instr->immediate = BYTE_OFFSET(instr->opcode, 2);
        }
    }

    if (has_opcode_extension(instr->opcode[1])) {
        uint8_t additional_immediate =
            get_opcode_extension_immediate_size(instr);
        if (additional_immediate > 0 && instr->immediate == NULL) {
            if (instr->displacement != NULL) {
                instr->immediate =
                    BYTE_OFFSET(instr->displacement, instr->displacement_size);
            } else if (instr->sib != NULL) {
                instr->immediate = BYTE_INCREMENT(instr->sib);
            } else if (instr->modrm != NULL) {
                instr->immediate = BYTE_INCREMENT(instr->modrm);
            } else {
                instr->immediate = BYTE_OFFSET(instr->opcode, 2);
            }
            instr->immediate_size += additional_immediate;
        }
    }

    return true;
}

static bool is_valid_opcode(uint8_t opcode) {
    return !(opcode_in_range(opcode, 0x1C, 0x1E) ||
             opcode_in_range(opcode, 0x24, 0x27) ||
             opcode_in_range(opcode, 0x38, 0x3F) || opcode == 0x07 ||
             opcode == 0x0A || opcode == 0x0C || opcode == 0x0E ||
             opcode == 0x36 || opcode == 0x0F || opcode == 0x19 ||
             opcode == 0xA6 || opcode == 0xA7);
}

static bool has_modrm(uint8_t opcode) {
    return opcode_in_range(opcode, 0x00, 0x03) ||
           opcode_in_range(opcode, 0x1A, 0x1B) ||
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
           opcode == 0x1F || opcode == 0xB8 || opcode == 0xFF;
}

static bool has_opcode_extension(uint8_t opcode) {
    return opcode_in_range(opcode, 0x71, 0x73) || opcode == 0x00 ||
           opcode == 0x01 || opcode == 0xBA || opcode == 0xC7 ||
           opcode == 0xB9 || opcode == 0xAE || opcode == 0x18;
}

static bool has_immediate_Ib(uint8_t opcode) {
    return opcode_in_range(opcode, 0xC4, 0xC6) || opcode == 0x70 ||
           opcode == 0xA4 || opcode == 0xAC || opcode == 0xBA || opcode == 0xC2;
}

static bool has_immediate_Jz(uint8_t opcode) {
    return opcode_in_range(opcode, 0x80, 0x8F);
}

static uint8_t get_immediate_size(const Instruction *instr) {
    uint8_t opcode = instr->opcode[1];
    if (has_immediate_Ib(opcode)) {
        return get_size_by_operand_type(OPERAND_TYPE_B, instr->operand_size);
    } else if (has_immediate_Jz(opcode)) {
        return get_size_by_operand_type(OPERAND_TYPE_Z, instr->operand_size);
    } else {
        return 0;
    }
}

static uint8_t get_opcode_extension_immediate_size(const Instruction *instr) {
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
