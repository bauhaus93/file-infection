#include "disasm.h"
#include "instruction_op1.h"
#include "instruction_op2.h"
#include "malloc.h"
#include "memory.h"
#include "prefix.h"
#include "size.h"
#include "utility.h"

static bool parse_instruction(void *addr, Instruction *instr);
static uint8_t calculate_instruction_size(const Instruction *instr);

Disassembler *init_disasm(void *start_address) {
    Disassembler *disasm = (Disassembler *)MALLOC(sizeof(Disassembler));
    if (disasm != NULL) {
        memzero(disasm, sizeof(Disassembler));
        disasm->instr[0].start = start_address;
        disasm->instr[0].end = start_address;
    }
    return disasm;
}

void destroy_disasm(Disassembler *disasm) {
    if (disasm != NULL) {
        FREE(disasm);
    }
}

bool next_instruction(Disassembler *disasm) {
    if (disasm != NULL) {
        void *next_addr = disasm->instr[disasm->index].end;
        disasm->index = (disasm->index + 1) % INSTRUCTION_HISTORY_SIZE;
        return parse_instruction(next_addr, &disasm->instr[disasm->index]);
    }
    return false;
}

uint8_t get_current_instruction_size(Disassembler *disasm) {
    if (disasm != NULL) {
        return get_instruction_size(&disasm->instr[disasm->index]);
    }
    return 0;
}

static bool parse_instruction(void *addr, Instruction *instr) {
    if (addr != NULL && instr != NULL) {
        memzero(instr, sizeof(Instruction));
        instr->start = addr;
        instr->end = instr->start;
        handle_prefixes(instr);

        instr->addressing_mode =
            get_addressing_mode(instr->prefix, instr->prefix_count);
        instr->operand_size =
            get_operand_size(instr->prefix, instr->prefix_count);

        instr->opcode = BYTE_OFFSET(instr->start, instr->prefix_count);
        instr->opcode_size = get_opcode_size(instr->opcode);

        if (instr->opcode_size == 1) {
            if (!handle_1byte_instruction(instr)) {
                return false;
            }
        } else if (instr->opcode_size == 2) {
            if (!handle_2byte_instruction(instr)) {
                return false;
            }
        } else if (instr->opcode_size == 3) {
            return false; // TODO: handle
        } else {
            PRINT_DEBUG("Invalid instruction size: expected 1-3, got %d",
                        instr->opcode_size);
            return false;
        }
        instr->end = (void *)BYTE_OFFSET(instr->start,
                                         calculate_instruction_size(instr));
        return true;
    } else {
        return false;
    }
}

static uint8_t calculate_instruction_size(const Instruction *instr) {
    if (instr == NULL) {
        return 0;
    } else {
        return instr->prefix_count + instr->opcode_size +
               (instr->modrm == NULL ? 0 : 1) + (instr->sib == NULL ? 0 : 1) +
               instr->displacement_size + instr->immediate_size;
    }
}
