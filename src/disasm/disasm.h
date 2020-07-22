#ifndef DISASM_H
#define DISASM_H

#include <stdbool.h>
#include <stdint.h>

#include "instruction.h"

#define INSTRUCTION_HISTORY_SIZE (2)

typedef struct {
    Instruction instr[INSTRUCTION_HISTORY_SIZE];
    unsigned int index;
    void *next_addr;
} Disassembler;

Disassembler *init_disasm(void *start_address);
void setup_disasm(void *start_address, Disassembler *disasm);
void destroy_disasm(Disassembler *disasm);
void set_next_address(void *next_address, Disassembler *disasm);
bool next_instruction(Disassembler *disasm);
bool parse_instruction(void *addr, Instruction *instr);
Instruction *get_current_instruction(Disassembler *disasm);
uint8_t get_current_instruction_size(Disassembler *disasm);
void print_current_instruction(const Disassembler *disasm);

#endif // DISASM_H
