#include <stddef.h>
#include <stdio.h>

#include "instruction.h"
#include "utility.h"

uint8_t get_instruction_size(const Instruction *instr) {
    if (instr != NULL) {
        return (uint8_t)((uint8_t *)instr->end - (uint8_t *)instr->start);
    } else {
        return 0;
    }
}

bool has_prefix(const Instruction *instr, uint8_t prefix) {
    for (uint8_t i = 0; i < instr->prefix_count; i++) {
        if (instr->prefix[i] == prefix) {
            return true;
        }
    }
    return false;
}

void print_instruction(const Instruction *instr) {
    printf("Instruction | size: %2d | prefix count: %1d | opcode size: %1d | "
           "modrm: %3s | sib: %3s | displacement size: %1d | immediate size: "
           "%1d\n",
           get_instruction_size(instr), instr->prefix_count, instr->opcode_size,
           instr->modrm == NULL ? "no" : "yes",
           instr->sib == NULL ? "no" : "yes", instr->displacement_size,
           instr->immediate_size);
}
