#include "disasm.h"
#include "malloc.h"
#include "memory.h"
#include "utility.h"

typedef enum {
    OPERAND_TYPE_B,
    OPERAND_TYPE_W,
    OPERAND_TYPE_D,
    OPERAND_TYPE_Q,
    OPERAND_TYPE_DQ,
    OPERAND_TYPE_QQ,
    OPERAND_TYPE_V,
    OPERAND_TYPE_Z,
    OPERAND_TYPE_P
} OperandType;

static bool parse_instruction(void *addr, Instruction *instr);
uint8_t get_instruction_size(const Instruction *instr);
static bool is_prefix(uint8_t value);
static uint8_t get_prefix_count(void *instruction_begin);
static uint8_t get_opcode_size(void *instruction_begin, uint8_t prefix_count);
static bool is_valid_opcode(void *instruction_begin, uint8_t prefix_count,
                            uint8_t opcode_size);
static uint8_t get_modrm_mod(uint8_t mod_rm);
static uint8_t get_modrm_rm(uint8_t mod_rm);
// static uint8_t get_modrm_reg(uint8_t mod_rm);
static uint8_t get_addressing_mode(void *instruction_begin,
                                   uint8_t prefix_count);
static uint8_t get_operand_size(void *instruction_begin, uint8_t prefix_count);
static uint8_t get_displacement_size(uint8_t mod_rm, uint8_t addressing_mode);
static uint8_t get_immediate_size(void *instruction_begin, uint8_t prefix_count,
                                  uint8_t opcode_size, uint8_t operand_size);
static uint8_t get_immediate_size_opcode_extension(void *instruction_begin,
                                                   uint8_t prefix_count,
                                                   uint8_t opcode_size,
                                                   uint8_t operand_size,
                                                   uint8_t modrm);
static bool has_modrm(void *instruction_begin, uint8_t prefix_count,
                      uint8_t opcode_size);
static bool has_sib_byte(uint8_t mod_rm);
static bool opcode_in_range(uint8_t opcode, uint8_t low, uint8_t high);
static uint8_t get_size_by_operand_type(OperandType type, uint8_t operand_size);
static bool has_opcode_extension_1byte(uint8_t opcode);

Disassembler *init_disasm(void *start_address) {
    Disassembler *disasm = (Disassembler *) MALLOC(sizeof(Disassembler));
    if (disasm != NULL) {
        memzero(disasm, sizeof(Disassembler));
        disasm->instr[0].addr = start_address;
    }
    return disasm;
}

void destroy_disasm(Disassembler *disasm) {
    if (disasm != NULL) {
        FREE(disasm);
    }
}

bool next_instruction(Disassembler *disasm) {
    if (disasm != NULL) {
        void *next_addr =
            BYTE_OFFSET(disasm->instr[disasm->index].addr,
                        get_instruction_size(&disasm->instr[disasm->index]));
        disasm->index = (disasm->index + 1) % INSTRUCTION_HISTORY_SIZE;
        return parse_instruction(next_addr, &disasm->instr[disasm->index]);
    }
    return false;
}

uint8_t get_current_instruction_size(Disassembler *disasm) {
    if (disasm != NULL) {
        return get_instruction_size(&disasm->instr[disasm->index]);
    }
    return 0;
}

static bool parse_instruction(void *addr, Instruction *instr) {
    if (addr != NULL && instr != NULL) {
        memzero(instr, sizeof(Instruction));
        instr->addr = addr;
        instr->prefix_count = get_prefix_count(addr);
        instr->addressing_mode = get_addressing_mode(addr, instr->prefix_count);
        instr->operand_size = get_operand_size(addr, instr->prefix_count);
        instr->opcode_size = get_opcode_size(addr, instr->prefix_count);
        instr->valid =
            is_valid_opcode(addr, instr->prefix_count, instr->opcode_size);
        if (instr->valid) {
            instr->has_modrm =
                has_modrm(addr, instr->prefix_count, instr->opcode_size);
            if (instr->has_modrm) {
                uint8_t modrm = *(uint8_t *)BYTE_OFFSET(
                    addr, instr->prefix_count + instr->opcode_size);
                if (instr->addressing_mode == 32) {
                    instr->has_sib = has_sib_byte(modrm);
                } else {
                    instr->has_sib = false;
                }
                instr->displacement_size =
                    get_displacement_size(modrm, instr->addressing_mode);

                if (has_opcode_extension_1byte(
                        *(uint8_t *)BYTE_OFFSET(addr, instr->prefix_count))) {
                    instr->immediate_size = get_immediate_size_opcode_extension(
                        addr, instr->prefix_count, instr->opcode_size,
                        instr->operand_size, modrm);
                }
            }
            if (instr->immediate_size == 0) {
                instr->immediate_size =
                    get_immediate_size(addr, instr->prefix_count,
                                       instr->opcode_size, instr->operand_size);
            }
        } else {
            return false;
        }
    }
    return true;
}

uint8_t get_instruction_size(const Instruction *instr) {
    if (instr == NULL) {
        return 0;
    } else {
        return instr->prefix_count + instr->opcode_size +
               (instr->has_modrm ? 1 : 0) + (instr->has_sib ? 1 : 0) +
               instr->displacement_size + instr->immediate_size;
    }
}

// TODO: check opcodes
static bool is_valid_opcode(void *addr, uint8_t prefix_count,
                            uint8_t opcode_size) {
    return true;
}

static uint8_t get_prefix_count(void *instruction_begin) {
    uint8_t *ptr = (uint8_t *)instruction_begin;
    uint8_t prefix_count = 0;
    while (is_prefix(*ptr)) {
        prefix_count++;
        ptr++;
    }
    return prefix_count;
}

static uint8_t get_opcode_size(void *instruction_begin, uint8_t prefix_count) {
    uint8_t *ptr = (uint8_t *)BYTE_OFFSET(instruction_begin, prefix_count);

    // 2 byte: 0x0F
    // 3 byte: 0x0F38, 0x0F3A
    if (*ptr != 0x0F) {
        return 1;
    } else if (*(ptr + 1) == 0x38 || *(ptr + 1) == 0x3A) {
        return 3;
    } else {
        return 2;
    }
}

// assuming the default addressing mode to be 32bit
static uint8_t get_addressing_mode(void *instruction_begin,
                                   uint8_t prefix_count) {
    uint8_t *prefix = (uint8_t *)instruction_begin;
    for (uint8_t i = 0; i < prefix_count; i++) {
        if (prefix[i] == 0x67) {
            return 16;
        }
    }
    return 32;
}

// assuming the default operand size to be 32bit
static uint8_t get_operand_size(void *instruction_begin, uint8_t prefix_count) {
    uint8_t *prefix = (uint8_t *)instruction_begin;
    for (uint8_t i = 0; i < prefix_count; i++) {
        if (prefix[i] == 0x66) {
            return 16;
        }
    }
    return 32;
}

static bool opcode_in_range(uint8_t opcode, uint8_t low, uint8_t high) {
    uint8_t row = opcode >> 4;
    uint8_t col = opcode & 0xF;
    return (row >= low >> 4) && row <= high >> 4 && col >= (low & 0xF) &&
           col <= (high & 0xF);
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
           opcode_in_range(opcode, 0x80, 0x83) ||
           opcode_in_range(opcode, 0xB0, 0xB7) || opcode == 0x6A ||
           opcode == 0x6B || opcode == 0xA8 || opcode == 0xC0 ||
           opcode == 0xC1 || opcode == 0xC8 || opcode == 0xCD ||
           opcode == 0xD4 || opcode == 0xD5 || opcode == 0xE4 || opcode == 0xE6;
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

static bool has_opcode_extension_1byte(uint8_t opcode) {
    return opcode_in_range(opcode, 0x80, 0x83) ||
           opcode_in_range(opcode, 0xD0, 0xD3) || opcode == 0x8F ||
           opcode == 0xC0 || opcode == 0xC1 || opcode == 0xF6 ||
           opcode == 0xF7 || opcode == 0xFE || opcode == 0xFF;
}

static uint8_t get_size_by_operand_type(OperandType type,
                                        uint8_t operand_size) {
    if (type == OPERAND_TYPE_B) {
        return 1;
    } else if (type == OPERAND_TYPE_W) {
        return 2;
    } else if (type == OPERAND_TYPE_D) {
        return 4;
    } else if (type == OPERAND_TYPE_Q) {
        return 8;
    } else if (type == OPERAND_TYPE_DQ) {
        return 16;
    } else if (type == OPERAND_TYPE_QQ) {
        return 32;
    } else if (type == OPERAND_TYPE_V) {
        return operand_size / 8;
    } else if (type == OPERAND_TYPE_Z) {
        if (operand_size == 16) {
            return 2;
        } else {
            return 4;
        }
    } else if (type == OPERAND_TYPE_P) {
        if (operand_size == 16) {
            return 2 + 2;
        } else {
            return 2 + 4;
        }
    }
    PRINT_DEBUG("Got unhandeled operand type: %d", (int)type);
    return 0;
}

static uint8_t get_immediate_size(void *instruction_begin, uint8_t prefix_count,
                                  uint8_t opcode_size, uint8_t operand_size) {
    uint8_t *opcode_ptr =
        (uint8_t *)BYTE_OFFSET(instruction_begin, prefix_count);
    if (opcode_size == 1) {
        uint8_t opcode = *opcode_ptr;
        if (has_immediate_Ib(opcode) && has_immediate_Iw(opcode)) { // ENTER
            return get_size_by_operand_type(OPERAND_TYPE_B, operand_size) +
                   get_size_by_operand_type(OPERAND_TYPE_W, operand_size);
        } else if (has_immediate_Jb(opcode) || has_immediate_Ib(opcode) ||
                   has_immediate_Ob(opcode)) {
            return get_size_by_operand_type(OPERAND_TYPE_B, operand_size);
        } else if (has_immediate_Iz(opcode) || has_immediate_Jz(opcode)) {
            return get_size_by_operand_type(OPERAND_TYPE_Z, operand_size);
        } else if (has_immediate_Iw(opcode)) {
            return get_size_by_operand_type(OPERAND_TYPE_W, operand_size);
        } else if (has_immediate_Iv(opcode) || has_immediate_Ov(opcode)) {
            return get_size_by_operand_type(OPERAND_TYPE_V, operand_size);
        } else if (has_immediate_Ap(opcode)) {
            return get_size_by_operand_type(OPERAND_TYPE_P, operand_size);
        } else {
            return 0;
        }
    } else if (opcode_size == 2) {
        // TODO: opcode_size 2
    } else if (opcode_size == 3) {
        // TODO: opcode_size 3
    } else {
        PRINT_DEBUG("invalid opcode size: expected 1/2/3, got %d", opcode_size);
        return 0;
    }
    return 0;
}
static uint8_t get_immediate_size_opcode_extension(void *instruction_begin,
                                                   uint8_t prefix_count,
                                                   uint8_t opcode_size,
                                                   uint8_t operand_size,
                                                   uint8_t modrm) {
    uint8_t *opcode_ptr =
        (uint8_t *)BYTE_OFFSET(instruction_begin, prefix_count);

    uint8_t opcode = *opcode_ptr;
    if (opcode == 0xF6 && modrm == 0xC0) { // Ib
        return get_size_by_operand_type(OPERAND_TYPE_B, operand_size);
    } else if (opcode == 0xF7 && modrm == 0xC0) { // Iz
        return get_size_by_operand_type(OPERAND_TYPE_Z, operand_size);
	} else if(opcode == 0xFF && (modrm == 0xC0 || modrm == 0xC8 || modrm == 0xD0)) {	// Ev
		return get_size_by_operand_type(OPERAND_TYPE_V, operand_size);	
    } else {
        return 0;
    }
}

static uint8_t get_displacement_size(uint8_t mod_rm, uint8_t addressing_mode) {

    uint8_t mod = get_modrm_mod(mod_rm);
    if (addressing_mode == 16) {
        if (mod == 0x0 && get_modrm_rm(mod_rm) == 0x6) {
            return 2;
        } else if (mod == 0x1) {
            return 1;
        } else if (mod == 0x2) {
            return 2;
        }
    } else if (addressing_mode == 32) {
        if (mod == 0x0 && get_modrm_rm(mod_rm) == 0x5) {
            return 4;
        } else if (mod == 0x1) {
            return 1;
        } else if (mod == 0x2) {
            return 4;
        }
    } else {
        PRINT_DEBUG("invalid addressing mode: must be 16 or 32, but found %d",
                    addressing_mode);
        return 0;
    }
    return 0;
}

// assumes addressing mode 32bit
static bool has_sib_byte(uint8_t mod_rm) {
    uint8_t mod = get_modrm_mod(mod_rm);
    uint8_t rm = get_modrm_rm(mod_rm);
    return mod <= 0x2 && rm == 0x4;
}

static uint8_t get_modrm_mod(uint8_t mod_rm) { return mod_rm >> 6; }

static uint8_t get_modrm_rm(uint8_t mod_rm) { return mod_rm & 0x3; }

// static uint8_t get_modrm_reg(uint8_t mod_rm) { return (mod_rm >> 3) & 0x3; }

// NOT 0, C, D, E, G, M, N, P, Q, R, S, U, V, W
static bool has_modrm(void *instruction_begin, uint8_t prefix_count,
                      uint8_t opcode_size) {
    // considers certain invalid opcodes to also have a modrm
    if (opcode_size == 1) {
        uint8_t opcode =
            *(uint8_t *)BYTE_OFFSET(instruction_begin, prefix_count);
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
    } else if (opcode_size == 2) {
        uint8_t opcode =
            *(uint8_t *)BYTE_OFFSET(instruction_begin, prefix_count + 1);
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
    } else { // opcode_size == 3
        return true;
    }
}

static bool is_prefix(uint8_t value) {
    switch (value) {
    case 0xF0:
    case 0xF2:
    case 0xF3:
    case 0x2E:
    case 0x36:
    case 0x3E:
    case 0x26:
    case 0x64:
    case 0x65:
    case 0x66:
    case 0x67:
        return true;
    default:
        return false;
    }
}
