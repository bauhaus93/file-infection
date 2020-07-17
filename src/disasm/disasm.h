#ifndef DISASM_H
#define DISASM_H

#include <stdbool.h>
#include <stdint.h>

#include "instruction.h"

#define INSTRUCTION_HISTORY_SIZE (2)

#if INSTRUCTION_HISTORY_SIZE < 2
#error INSTRUCTION_HISTORY_SIZE must be at least 2, but was less than 2.
#endif

typedef struct {
    Instruction instr[INSTRUCTION_HISTORY_SIZE];
    unsigned int index;
} Disassembler;

Disassembler *init_disasm(void *start_address);
void destroy_disasm(Disassembler *disasm);
bool next_instruction(Disassembler *disasm);
uint8_t get_current_instruction_size(Disassembler *disasm);
void print_current_instruction(Disassembler *disasm);

#endif // DISASM_H
