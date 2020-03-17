#include "size.h"
#include "utility.h"

// assuming the default operand size to be 32bit
uint8_t get_operand_size(uint8_t *prefix, uint8_t prefix_count) {
    for (uint8_t i = 0; i < prefix_count; i++) {
        if (prefix[i] == 0x66) {
            return 16;
        }
    }
    return 32;
}

// assuming the default addressing mode to be 32bit
uint8_t get_addressing_mode(uint8_t *prefix, uint8_t prefix_count) {
    for (uint8_t i = 0; i < prefix_count; i++) {
        if (prefix[i] == 0x67) {
            return 16;
        }
    }
    return 32;
}

uint8_t get_opcode_size(uint8_t *opcode) {
    // 2 byte: 0x0F
    // 3 byte: 0x0F38, 0x0F3A
    if (*opcode != 0x0F) {
        return 1;
    } else if (*(opcode + 1) == 0x38 || *(opcode + 1) == 0x3A) {
        return 3;
    } else {
        return 2;
    }
}

uint8_t get_size_by_operand_type(OperandType type, uint8_t operand_size) {
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
