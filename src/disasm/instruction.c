#include <stddef.h>

#include "instruction.h"
#include "utility.h"

uint8_t get_instruction_size(const Instruction *instr) {
    if (instr != NULL) {
        return (uint8_t)((uint8_t *)instr->end - (uint8_t *)instr->start);
    } else {
        return 0;
    }
}

void print_instruction(const Instruction *instr) {
    printf("Instruction\n\tSize = %d\n\tPrefix count = %d\n\tOpcode size = "
           "%d\n\tmodrm = %s\n\tsib = %s\n\tDisplacement size = "
           "%d\n\tImmediate size = %d\n",
           get_instruction_size(instr), instr->prefix_count, instr->opcode_size,
           instr->modrm == NULL ? "no" : "yes",
           instr->sib == NULL ? "no" : "yes", instr->displacement_size,
           instr->immediate_size);
}
