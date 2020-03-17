#include <stddef.h>

#include "prefix.h"

static uint8_t get_prefix_count(void *instruction_begin);
static bool is_prefix(uint8_t value);

void handle_prefixes(Instruction *instr) {
    instr->prefix_count = get_prefix_count(instr->start);
    if (instr->prefix_count > 0) {
        instr->prefix = (uint8_t *)instr->start;
    } else {
        instr->prefix = NULL;
    }
}

static uint8_t get_prefix_count(void *instruction_begin) {
    uint8_t *ptr = (uint8_t *)instruction_begin;
    uint8_t prefix_count = 0;
    while (is_prefix(ptr[prefix_count])) {
        prefix_count++;
    }
    return prefix_count;
}

#pragma GCC push_options
#pragma GCC optimize("O0")
static bool is_prefix(uint8_t value) {
    switch (value) {
    case 0xF0:
    case 0xF2:
    case 0xF3:
    case 0x2E:
    case 0x36:
    case 0x3E:
    case 0x26:
    case 0x64:
    case 0x65:
    case 0x66:
    case 0x67:
        return true;
    default:
        return false;
    }
}
#pragma GCC pop_options
