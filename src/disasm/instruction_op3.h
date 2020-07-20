#ifndef INSTRUCTION_OP3_H
#define INSTRUCTION_OP3_H

#include <stdbool.h>
#include <stdint.h>

#include "instruction.h"

bool is_valid_opcode_3byte(const Instruction *instr);
bool has_modrm_3byte(const Instruction *instr);
bool has_opcode_extension_3byte(const Instruction *instr);
uint8_t get_immediate_size_3byte(const Instruction *instr);
uint8_t get_opcode_extension_immediate_size_3byte(const Instruction *instr);

#endif // INSTRUCTION_OP3_H
