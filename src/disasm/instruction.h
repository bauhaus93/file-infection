#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    void *start;
    void *end;

    uint8_t *prefix;
    uint8_t prefix_count;

    uint8_t *opcode;
    uint8_t opcode_size;

    uint8_t *modrm;
    uint8_t *sib;

    void *displacement;
    uint8_t displacement_size;

    void *immediate;
    uint8_t immediate_size;

    uint8_t addressing_mode;
    uint8_t operand_size;
} Instruction;

uint8_t get_instruction_size(const Instruction *instr);
bool has_prefix(const Instruction *instr, uint8_t prefix);
void print_instruction(const Instruction *instr);

#endif // INSTRUCTION_H
