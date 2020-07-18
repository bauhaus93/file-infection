#ifndef INSTRUCTION_OP2_H
#define INSTRUCTION_OP2_H

#include <stdbool.h>
#include <stdint.h>

#include "instruction.h"

bool is_valid_opcode_2byte(const Instruction *instr);
bool has_modrm_2byte(const Instruction *instr);
bool has_opcode_extension_2byte(const Instruction *instr);
uint8_t get_immediate_size_2byte(const Instruction *instr);
uint8_t get_opcode_extension_immediate_size_2byte(const Instruction *instr);

#endif // INSTRUCTION_OP2_H
