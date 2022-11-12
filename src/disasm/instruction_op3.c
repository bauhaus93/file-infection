#include <stdbool.h>
#include <stdint.h>

#include "disasm_utility.h"
#include "instruction.h"
#include "instruction_op3.h"
#include "size.h"

static bool is_valid_opcode_0F3A(const Instruction *instr);
static bool is_valid_opcode_0F38(const Instruction *instr);

bool is_valid_opcode_3byte(const Instruction *instr) {
  if (instr->opcode[1] == 0x38) {
    return is_valid_opcode_0F38(instr);
  } else if (instr->opcode[1] == 0x3A) {
    return is_valid_opcode_0F3A(instr);
  } else {
    return false;
  }
}

bool has_modrm_3byte(const Instruction *instr) { return true; }

bool has_opcode_extension_3byte(const Instruction *instr) { return false; }

uint8_t get_immediate_size_3byte(const Instruction *instr) {
  if (instr->opcode[1] == 0x3A) {
    return get_size_by_operand_type(OPERAND_TYPE_B, instr->operand_size);
  } else {
    return 0;
  }
}

uint8_t get_opcode_extension_immediate_size_3byte(const Instruction *instr) {
  return 0;
}

static bool is_valid_opcode_0F38(const Instruction *instr) {
  uint8_t opcode = instr->opcode[2];
  if (has_prefix(instr, 0x66)) {
    return opcode_in_range(opcode, 0x00, 0x0B) ||
           opcode_in_range(opcode, 0x1C, 0x1E) ||
           opcode_in_range(opcode, 0x20, 0x2B) ||
           opcode_in_range(opcode, 0x30, 0x35) ||
           opcode_in_range(opcode, 0x37, 0x3F) ||
           opcode_in_range(opcode, 0x80, 0x82) ||
           opcode_in_range(opcode, 0xDB, 0xDF) || opcode == 0x10 ||
           opcode == 0x14 || opcode == 0x15 || opcode == 0x17 ||
           opcode == 0x40 || opcode == 0x41 || opcode == 0xF0 ||
           opcode == 0xF1 || opcode == 0xF6;
  } else if (has_prefix(instr, 0xF3)) {
    return opcode == 0xF6;
  } else if (has_prefix(instr, 0xF2)) {
    return opcode == 0xF0 || opcode == 0xF1;
  } else {
    return opcode_in_range(opcode, 0x00, 0x0B) ||
           opcode_in_range(opcode, 0x1C, 0x1E) ||
           opcode_in_range(opcode, 0xC8, 0xCD) || opcode == 0xF0 ||
           opcode == 0xF1 || opcode == 0xF6 || opcode == 0xF9;
  }
}

static bool is_valid_opcode_0F3A(const Instruction *instr) {
  uint8_t opcode = instr->opcode[2];
  if (has_prefix(instr, 0x66)) {
    return opcode_in_range(opcode, 0x08, 0x0F) ||
           opcode_in_range(opcode, 0x14, 0x17) ||
           opcode_in_range(opcode, 0x20, 0x22) ||
           opcode_in_range(opcode, 0x40, 0x42) ||
           opcode_in_range(opcode, 0x60, 0x63) || opcode == 0x44;
  } else {
    return opcode == 0x0F || opcode == 0xCC;
  }
}
