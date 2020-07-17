#include <stdint.h>

#include "disasm_utility.h"
#include "instruction_op1.h"
#include "operand_type.h"
#include "size.h"
#include "utility.h"

static uint8_t get_immediate_size(const Instruction *instr);
static uint8_t get_opcode_extension_immediate_size(const Instruction *instr);
static bool is_valid_opcode(uint8_t opcode);
static bool has_opcode_extension(uint8_t opcode);
static bool has_modrm(uint8_t opcode);
static bool has_immediate_Jb(uint8_t opcode);
static bool has_immediate_Jz(uint8_t opcode);
static bool has_immediate_Ap(uint8_t opcode);
static bool has_immediate_Ib(uint8_t opcode);
static bool has_immediate_Iv(uint8_t opcode);
static bool has_immediate_Iw(uint8_t opcode);
static bool has_immediate_Iz(uint8_t opcode);
static bool has_immediate_Ob(uint8_t opcode);
static bool has_immediate_Ov(uint8_t opcode);

bool handle_1byte_instruction(Instruction *instr) {
    if (instr == NULL || !is_valid_opcode(instr->opcode[0])) {
        return false;
    }
    if (has_modrm(instr->opcode[0])) {
        instr->modrm = (uint8_t *)BYTE_INCREMENT(instr->opcode);

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
            instr->immediate = BYTE_INCREMENT(instr->opcode);
        }
    }
    if (has_opcode_extension(instr->opcode[0])) {
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
                instr->immediate = BYTE_INCREMENT(instr->opcode);
            }
        }
        instr->immediate_size += additional_immediate;
    }
    return true;
}

static bool
is_valid_opcode(uint8_t opcode) { // ignoring invalid opcode extensions
    return opcode != 0xD6 && (!opcode_in_range(opcode, 0xD8, 0xDF));
}

static bool has_immediate_Jb(uint8_t opcode) {
    return opcode_in_range(opcode, 0x70, 0x7F) ||
           opcode_in_range(opcode, 0xE0, 0xE3) || opcode == 0xEB;
}

static bool has_immediate_Jz(uint8_t opcode) {
    return opcode == 0xE8 || opcode == 0xE9;
}

static bool has_immediate_Ap(uint8_t opcode) {
    return opcode == 0xEA || opcode == 0x9A;
}

static bool has_immediate_Ib(uint8_t opcode) {
    return opcode_in_range(opcode, 0x04, 0x34) ||
           opcode_in_range(opcode, 0x0C, 0x3C) ||
           opcode_in_range(opcode, 0xB0, 0xB7) ||
           opcode_in_range(opcode, 0xE4, 0xE7) || opcode == 0x6A ||
           opcode == 0x6B || opcode == 0x80 || opcode == 0x82 ||
           opcode == 0x83 || opcode == 0xA8 || opcode == 0xC0 ||
           opcode == 0xC1 || opcode == 0xC8 || opcode == 0xCD ||
           opcode == 0xD4 || opcode == 0xD5;
}

static bool has_immediate_Iv(uint8_t opcode) {
    return opcode_in_range(opcode, 0xB8, 0xBF);
}

static bool has_immediate_Iw(uint8_t opcode) {
    return opcode == 0xC2 || opcode == 0xC8 || opcode == 0xCA;
}

static bool has_immediate_Iz(uint8_t opcode) {
    return opcode_in_range(opcode, 0x05, 0x35) ||
           opcode_in_range(opcode, 0x0D, 0x3D) || opcode == 0x68 ||
           opcode == 0x69 || opcode == 0x81 || opcode == 0xA9 || opcode == 0xC7;
}

static bool has_immediate_Ob(uint8_t opcode) {
    return opcode == 0xA0 || opcode == 0xA2;
}

static bool has_immediate_Ov(uint8_t opcode) {
    return opcode == 0xA1 || opcode == 0xA3;
}

static bool has_opcode_extension(uint8_t opcode) {
    return opcode_in_range(opcode, 0x80, 0x83) ||
           opcode_in_range(opcode, 0xD0, 0xD3) || opcode == 0x8F ||
           opcode == 0xC0 || opcode == 0xC1 || opcode == 0xF6 ||
           opcode == 0xF7 || opcode == 0xFE || opcode == 0xFF;
}

static uint8_t get_immediate_size(const Instruction *instr) {
    uint8_t opcode = instr->opcode[0];
    if (has_immediate_Ib(opcode) && has_immediate_Iw(opcode)) { // ENTER
        return get_size_by_operand_type(OPERAND_TYPE_B, instr->operand_size) +
               get_size_by_operand_type(OPERAND_TYPE_W, instr->operand_size);
    } else if (has_immediate_Jb(opcode) || has_immediate_Ib(opcode)) {
        return get_size_by_operand_type(OPERAND_TYPE_B, instr->operand_size);
    } else if (has_immediate_Iz(opcode) || has_immediate_Jz(opcode)) {
        return get_size_by_operand_type(OPERAND_TYPE_Z, instr->operand_size);
    } else if (has_immediate_Iw(opcode)) {
        return get_size_by_operand_type(OPERAND_TYPE_W, instr->operand_size);
    } else if (has_immediate_Iv(opcode)) {
        return get_size_by_operand_type(OPERAND_TYPE_V, instr->operand_size);
    } else if (has_immediate_Ap(opcode)) {
        return get_size_by_operand_type(OPERAND_TYPE_P, instr->operand_size);
    } else if (has_immediate_Ob(opcode) || has_immediate_Ov(opcode)) {
        return instr->addressing_mode / 8;
    } else {
        return 0;
    }
}
static uint8_t get_opcode_extension_immediate_size(const Instruction *instr) {
    if (instr->modrm == NULL) {
        return 0;
    }
    uint8_t opcode = instr->opcode[0];
    uint8_t modrm = *instr->modrm;
    uint8_t nnn = get_modrm_reg(modrm);
    if (opcode == 0xF6 && nnn == 0x00) { // Ib
        return get_size_by_operand_type(OPERAND_TYPE_B, instr->operand_size);
    } else if (opcode == 0xF7 && nnn == 0x00) {
        return get_size_by_operand_type(OPERAND_TYPE_Z, instr->operand_size);
    } else if (opcode == 0xFF &&
               (modrm == 0xC0 || modrm == 0xC8 || modrm == 0xD0)) { // Ev
        return get_size_by_operand_type(OPERAND_TYPE_V, instr->operand_size);
    } else {
        return 0;
    }
}

static bool has_modrm(uint8_t opcode) {
    return opcode_in_range(opcode, 0x00, 0x33) ||
           opcode_in_range(opcode, 0x08, 0x3B) ||
           opcode_in_range(opcode, 0x62, 0x63) ||
           opcode_in_range(opcode, 0x80, 0x8F) ||
           opcode_in_range(opcode, 0xC0, 0xD1) ||
           opcode_in_range(opcode, 0xC4, 0xC7) ||
           opcode_in_range(opcode, 0xD2, 0xD3) ||
           opcode_in_range(opcode, 0xF6, 0xF7) ||
           opcode_in_range(opcode, 0xFE, 0xFF) || opcode == 0x69 ||
           opcode == 0x6B;
}
