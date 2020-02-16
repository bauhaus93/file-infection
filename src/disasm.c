#include <stdbool.h>
#include <stdint.h>

#include "utility.h"

#define OPCODE_RANGE(hl, ll, hh, lh)                                           \
    (opcode_high >= hl && opcode_high <= hh && opcode_low >= ll &&             \
     opcode_low <= lh)

static bool is_prefix(uint8_t value);
static int get_prefix_count(void *instruction_addr);
static int get_opcode_size(void *instruction_addr);
static bool has_modrm(void *instruction_addr);
static bool is_prefix(uint8_t value);

int get_instruction_size(void *instruction_addr) {
    int prefix_count = get_prefix_count(instruction_addr);
    int opcode_size = get_opcode_size(instruction_addr);
    if (has_modrm(instruction_addr)) {
    }

    return prefix_count + opcode_size;
}

static int get_prefix_count(void *instruction_addr) {
    int count = 0;
    while (is_prefix(*(uint8_t *)instruction_addr)) {
        count++;
        instruction_addr = BYTE_INCREMENT(instruction_addr);
    }
    return count;
}

static int get_opcode_size(void *instruction_addr) {
    uint8_t *ptr = (uint8_t *)instruction_addr;
    int prefix_count = get_prefix_count(instruction_addr);
    ptr += prefix_count;

    if (*ptr != 0x0F) {
        return 1;
    } else if (*(ptr + 1) == 0x38 || *(ptr + 1) == 0x3A) {
        return 3;
    } else {
        return 2;
    }
}

// NOT 0, C, D, E, G, M, N, P, Q, R, S, U, V, W
static bool has_modrm(void *instruction_addr) {
    // considers certain invalid opcodes to also have a modrm
    int prefix_count = get_prefix_count(instruction_addr);
    int opcode_size = get_opcode_size(instruction_addr);
    if (opcode_size == 1) {
        uint8_t opcode =
            *(uint8_t *)BYTE_OFFSET(instruction_addr, prefix_count);
        uint8_t opcode_low = opcode & 0xF; // column
        uint8_t opcode_high = opcode >> 4; // row
        return OPCODE_RANGE(0x0, 0x0, 0x3, 0x3) ||
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
    } else if (opcode_size == 2) {
        uint8_t opcode =
            *(uint8_t *)BYTE_OFFSET(instruction_addr, prefix_count + 1);
        uint8_t opcode_low = opcode & 0xF; // column
        uint8_t opcode_high = opcode >> 4; // row
        return OPCODE_RANGE(0x0, 0x0, 0x0, 0x3) ||
               OPCODE_RANGE(0x1, 0x0, 0x2, 0x8) ||
               OPCODE_RANGE(0x2, 0x9, 0x2, 0xF) ||
               OPCODE_RANGE(0x4, 0x0, 0x7, 0xF) ||
               OPCODE_RANGE(0x9, 0x0, 0xF, 0x7) ||
               OPCODE_RANGE(0x9, 0x8, 0x9, 0xF) ||
               OPCODE_RANGE(0xA, 0xB, 0xA, 0xF) ||
               OPCODE_RANGE(0xB, 0x8, 0xB, 0xF) ||
               OPCODE_RANGE(0xD, 0x8, 0xF, 0xE) ||
               OPCODE_RANGE(0xD, 0xF, 0xE, 0xF);
    } else {    // opcode_size == 3
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
