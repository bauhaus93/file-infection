#ifndef DISASM_H
#define DISASM_H

#include <stdbool.h>
#include <stdint.h>

typedef struct _Instruction {
    void *addr;
    uint8_t prefix_count;
    uint8_t opcode_size;
    uint8_t displacement_size;
    uint8_t immediate_size;
    uint8_t addressing_mode;
    uint8_t operand_size;
    bool valid;
    bool has_modrm;
    bool has_sib;
} Instruction;

Instruction *parse_instruction(void *addr);
Instruction *next_instruction(const Instruction *prev_instr);
uint8_t get_instruction_size(const Instruction *instr);

#endif // DISASM_H
