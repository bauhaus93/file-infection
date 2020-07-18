#ifndef INSTRUCTION_OP1_H
#define INSTRUCTION_OP1_H

#include <stdbool.h>
#include <stdint.h>

#include "instruction.h"

bool is_valid_opcode_1byte(const Instruction *instr);
bool has_modrm_1byte(const Instruction *instr);
bool has_opcode_extension_1byte(const Instruction *instr);
uint8_t get_immediate_size_1byte(const Instruction *instr);
uint8_t get_opcode_extension_immediate_size_1byte(const Instruction *instr);

#endif // INSTRUCTION_OP1_H
