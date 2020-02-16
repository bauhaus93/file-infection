#ifndef DISASM_H
#define DISASM_H

#include <stdbool.h>
#include <stdint.h>

typedef struct _Instruction {
    void *addr;
    uint8_t prefix_count;
    uint8_t opcode_size;
    bool has_modrm;
} Instruction;

Instruction *parse_instruction(void *addr);
Instruction *next_instruction(const Instruction *instr);

#endif // DISASM_H
