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

#define INSTRUCTION_HISTORY_SIZE (2)

typedef struct {
    Instruction instr[INSTRUCTION_HISTORY_SIZE];
    unsigned int index;
} Disassembler;

Disassembler *init_disasm(void *start_address);
void destroy_disasm(Disassembler *disasm);
bool next_instruction(Disassembler *disasm);
uint8_t get_current_instruction_size(Disassembler *disasm);

#endif // DISASM_H
