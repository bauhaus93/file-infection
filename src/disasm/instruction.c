#include <stddef.h>

#include "instruction.h"
#include "utility.h"

uint8_t get_instruction_size(const Instruction *instr) {
    if (instr != NULL) {
        return (uint8_t) ((uint8_t*)instr->end - (uint8_t*)instr->start);
    } else {
        return 0;
    }
}
