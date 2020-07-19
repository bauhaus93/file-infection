#include <stdbool.h>
#include <stdint.h>

#include "disasm.h"
#include "disasm_utility.h"
#include "instruction_op1.h"
#include "instruction_op2.h"
#include "instruction_op3.h"
#include "malloc.h"
#include "memory.h"
#include "prefix.h"
#include "size.h"
#include "utility.h"

static bool parse_instruction(void *addr, Instruction *instr);
static uint8_t calculate_instruction_size(const Instruction *instr);
static void handle_modrm(Instruction *instr);
static void handle_sib(Instruction *instr);
static void handle_modrm_displacement(Instruction *instr);
static void handle_immediate(Instruction *instr);
static void handle_opcode_extension(Instruction *instr);

static bool is_valid_opcode(const Instruction *instr);
static bool has_modrm(const Instruction *instr);
static bool has_opcode_extension(const Instruction *instr);
static uint8_t get_immediate_size(const Instruction *instr);
static uint8_t get_opcode_extension_immediate_size(const Instruction *instr);

Disassembler *init_disasm(void *start_address) {
    Disassembler *disasm = (Disassembler *)MALLOC(sizeof(Disassembler));
    setup_disasm(start_address, disasm);
    return disasm;
}

void setup_disasm(void *start_address, Disassembler *disasm) {
    if (disasm != NULL) {
        memzero(disasm, sizeof(Disassembler));
        set_next_address(start_address, disasm);
        disasm->next_addr = start_address;
    }
}

void destroy_disasm(Disassembler *disasm) {
    if (disasm != NULL) {
        FREE(disasm);
    }
}

void set_next_address(void *next_address, Disassembler *disasm) {
    if (disasm != NULL) {
        disasm->next_addr = next_address;
    }
}

bool next_instruction(Disassembler *disasm) {
    if (disasm != NULL) {
        disasm->index = (disasm->index + 1) % INSTRUCTION_HISTORY_SIZE;
        bool result =
            parse_instruction(disasm->next_addr, &disasm->instr[disasm->index]);
        disasm->next_addr = get_current_instruction(disasm)->end;
        return result;
    }
    return false;
}

uint8_t get_current_instruction_size(Disassembler *disasm) {
    if (disasm != NULL) {
        return get_instruction_size(get_current_instruction(disasm));
    }
    return 0;
}

Instruction *get_current_instruction(Disassembler *disasm) {
    if (disasm != NULL) {
        return &disasm->instr[disasm->index];
    }
    return NULL;
}

void print_current_instruction(const Disassembler *disasm) {
    if (disasm != NULL) {
        print_instruction(&disasm->instr[disasm->index]);
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

        if (instr->opcode_size == 0 || instr->opcode_size > 3) {
            PRINT_DEBUG("Invalid instruction size: expected 1-3, got %d",
                        instr->opcode_size);
            return false;
        } else if (!is_valid_opcode(instr)) {
            return false;
        } else {
            handle_modrm(instr);
            handle_immediate(instr);
            handle_opcode_extension(instr);
            instr->end = (void *)BYTE_OFFSET(instr->start,
                                             calculate_instruction_size(instr));

            return true;
        }
    } else {
        return false;
    }
}

static void handle_modrm(Instruction *instr) {
    if (has_modrm(instr)) {
        instr->modrm =
            (uint8_t *)BYTE_OFFSET(instr->opcode, instr->opcode_size);

        handle_sib(instr);
        handle_modrm_displacement(instr);
    }
}

static void handle_sib(Instruction *instr) {
    if (instr->addressing_mode == 32 && has_sib_byte(*instr->modrm)) {
        instr->sib = (uint8_t *)BYTE_INCREMENT(instr->modrm);
        instr->displacement_size =
            get_sib_displacement_size(*instr->modrm, *instr->sib);
        if (instr->displacement_size > 0) {
            instr->displacement = (void *)BYTE_INCREMENT(instr->sib);
        }
    }
}

static void handle_modrm_displacement(Instruction *instr) {
    if (instr->displacement == NULL) {
        instr->displacement_size =
            get_modrm_displacement_size(*instr->modrm, instr->addressing_mode);
        if (instr->displacement_size > 0) {
            instr->displacement = (void *)BYTE_INCREMENT(
                instr->sib != NULL ? instr->sib : instr->modrm);
        }
    }
}

static void handle_immediate(Instruction *instr) {
    instr->immediate_size = get_immediate_size(instr);
    if (instr->immediate_size > 0) {
        if (instr->displacement != NULL) {
            instr->immediate =
                BYTE_OFFSET(instr->displacement, instr->displacement_size);
        } else if (instr->sib != NULL) {
            instr->immediate = BYTE_INCREMENT(instr->sib);
        } else if (instr->modrm != NULL) {
            instr->immediate = BYTE_INCREMENT(instr->modrm);
        } else {
            instr->immediate = BYTE_OFFSET(instr->opcode, instr->opcode_size);
        }
    }
}

static void handle_opcode_extension(Instruction *instr) {
    if (has_opcode_extension(instr)) {
        uint8_t additional_immediate =
            get_opcode_extension_immediate_size(instr);
        if (additional_immediate > 0 && instr->immediate == NULL) {
            if (instr->displacement != NULL) {
                instr->immediate =
                    BYTE_OFFSET(instr->displacement, instr->displacement_size);
            } else if (instr->sib != NULL) {
                instr->immediate = BYTE_INCREMENT(instr->sib);
            } else if (instr->modrm != NULL) {
                instr->immediate = BYTE_INCREMENT(instr->modrm);
            } else {
                instr->immediate =
                    BYTE_OFFSET(instr->opcode, instr->opcode_size);
            }
            instr->immediate_size += additional_immediate;
        }
    }
}

static bool is_valid_opcode(const Instruction *instr) {
    if (instr->opcode_size == 1) {
        return is_valid_opcode_1byte(instr);
    } else if (instr->opcode_size == 2) {
        return is_valid_opcode_2byte(instr);
    } else if (instr->opcode_size == 3) {
        return is_valid_opcode_3byte(instr);
    } else {
        return false;
    }
}

static bool has_modrm(const Instruction *instr) {
    if (instr->opcode_size == 1) {
        return has_modrm_1byte(instr);
    } else if (instr->opcode_size == 2) {
        return has_modrm_2byte(instr);
    } else if (instr->opcode_size == 3) {
        return has_modrm_3byte(instr);
    } else {
        return false;
    }
}

static bool has_opcode_extension(const Instruction *instr) {
    if (instr->opcode_size == 1) {
        return has_opcode_extension_1byte(instr);
    } else if (instr->opcode_size == 2) {
        return has_opcode_extension_2byte(instr);
    } else if (instr->opcode_size == 3) {
        return has_opcode_extension_3byte(instr);
    } else {
        return 0;
    }
}

static uint8_t get_immediate_size(const Instruction *instr) {
    if (instr->opcode_size == 1) {
        return get_immediate_size_1byte(instr);
    } else if (instr->opcode_size == 2) {
        return get_immediate_size_2byte(instr);
    } else if (instr->opcode_size == 3) {
        return get_immediate_size_3byte(instr);
    } else {
        return 0;
    }
}

static uint8_t get_opcode_extension_immediate_size(const Instruction *instr) {
    if (instr->opcode_size == 1) {
        return get_opcode_extension_immediate_size_1byte(instr);
    } else if (instr->opcode_size == 2) {
        return get_opcode_extension_immediate_size_2byte(instr);
    } else if (instr->opcode_size == 3) {
        return get_opcode_extension_immediate_size_3byte(instr);
    } else {
        return 0;
    }
}
