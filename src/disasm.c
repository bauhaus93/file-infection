#include "disasm.h"
#include "malloc.h"
#include "utility.h"

#define OPCODE_RANGE(hl, ll, hh, lh)                                           \
    (opcode_high >= hl && opcode_high <= hh && opcode_low >= ll &&             \
     opcode_low <= lh)

static bool is_prefix(uint8_t value);
static void fill_prefix_count(Instruction *instr);
static void fill_opcode_size(Instruction *instr);
static void fill_has_modrm(Instruction *instr);
static uint8_t get_instruction_size(const Instruction *instr);

Instruction *parse_instruction(void *addr) {
    Instruction *instr = MALLOC(sizeof(Instruction));
    if (instr != NULL) {
        memzero(instr, sizeof(Instruction));
        instr->addr = addr;
        fill_prefix_count(instr);
        fill_opcode_size(instr);
        // TODO: check if valid opcode!
        fill_has_modrm(instr);
    }
    return instr;
}

Instruction *next_instruction(const Instruction *instr) {
    void *next_addr = BYTE_OFFSET(instr->addr, get_instruction_size(instr));
    return parse_instruction(next_addr);
}

// TODO: add further values
static uint8_t get_instruction_size(const Instruction *instr) {
    return instr->prefix_count + instr->opcode_size + instr->has_modrm ? 1 : 0;
}

static void fill_prefix_count(Instruction *instr) {
    uint8_t *ptr = (uint8_t *)instr->addr;
    while (is_prefix(*ptr)) {
        instr->prefix_count++;
        ptr++;
    }
}

static void fill_opcode_size(Instruction *instr) {
    uint8_t *ptr = (uint8_t *)BYTE_OFFSET(instr->addr, instr->prefix_count);

    if (*ptr != 0x0F) {
        instr->opcode_size = 1;
    } else if (*(ptr + 1) == 0x38 || *(ptr + 1) == 0x3A) {
        instr->opcode_size = 3;
    } else {
        instr->opcode_size = 2;
    }
}

// NOT 0, C, D, E, G, M, N, P, Q, R, S, U, V, W
static void fill_has_modrm(Instruction *instr) {
    // considers certain invalid opcodes to also have a modrm
    if (instr->opcode_size == 1) {
        uint8_t opcode =
            *(uint8_t *)BYTE_OFFSET(instr->addr, instr->prefix_count);
        uint8_t opcode_low = opcode & 0xF; // column
        uint8_t opcode_high = opcode >> 4; // row
        instr->has_modrm = OPCODE_RANGE(0x0, 0x0, 0x3, 0x3) ||
                           OPCODE_RANGE(0x0, 0x6, 0x1, 0x7) ||
                           OPCODE_RANGE(0x0, 0x8, 0x3, 0xB) ||
                           OPCODE_RANGE(0x0, 0xE, 0x1, 0xE) ||
                           OPCODE_RANGE(0x6, 0x2, 0x6, 0x3) ||
                           OPCODE_RANGE(0x6, 0xB, 0x6, 0xF) ||
                           OPCODE_RANGE(0x8, 0x0, 0x8, 0xF) ||
                           OPCODE_RANGE(0xC, 0x0, 0xD, 0x1) ||
                           OPCODE_RANGE(0xC, 0x4, 0xC, 0x7) ||
                           OPCODE_RANGE(0xD, 0x2, 0xD, 0x3) ||
                           OPCODE_RANGE(0xF, 0x6, 0xF, 0x7) ||
                           OPCODE_RANGE(0xF, 0xE, 0xF, 0xF) ||
                           (opcode_high == 0x1 && opcode_low == 0xF) ||
                           (opcode_high == 0x6 && opcode_low == 0x9);
    } else if (instr->opcode_size == 2) {
        uint8_t opcode =
            *(uint8_t *)BYTE_OFFSET(instr->addr, instr->prefix_count + 1);
        uint8_t opcode_low = opcode & 0xF; // column
        uint8_t opcode_high = opcode >> 4; // row
        instr->has_modrm = OPCODE_RANGE(0x0, 0x0, 0x0, 0x3) ||
                           OPCODE_RANGE(0x1, 0x0, 0x2, 0x8) ||
                           OPCODE_RANGE(0x2, 0x9, 0x2, 0xF) ||
                           OPCODE_RANGE(0x4, 0x0, 0x7, 0xF) ||
                           OPCODE_RANGE(0x9, 0x0, 0xF, 0x7) ||
                           OPCODE_RANGE(0x9, 0x8, 0x9, 0xF) ||
                           OPCODE_RANGE(0xA, 0xB, 0xA, 0xF) ||
                           OPCODE_RANGE(0xB, 0x8, 0xB, 0xF) ||
                           OPCODE_RANGE(0xD, 0x8, 0xF, 0xE) ||
                           OPCODE_RANGE(0xD, 0xF, 0xE, 0xF);
    } else { // opcode_size == 3
        instr->has_modrm = true;
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
