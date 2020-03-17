#include <stdint.h>

#include "instruction_op2.h"
#include "disasm_utility.h"

static bool has_modrm(uint8_t opcode);

bool handle_2byte_instruction(Instruction *instr) { return false; }

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
