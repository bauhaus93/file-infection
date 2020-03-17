#include "disasm_utility.h"
#include "utility.h"

bool opcode_in_range(uint8_t opcode, uint8_t low, uint8_t high) {
    uint8_t row = opcode >> 4;
    uint8_t col = opcode & 0xF;
    return (row >= low >> 4) && row <= high >> 4 && col >= (low & 0xF) &&
           col <= (high & 0xF);
}

uint8_t get_modrm_mod(uint8_t mod_rm) { return mod_rm >> 6; }

uint8_t get_modrm_rm(uint8_t mod_rm) { return mod_rm & 0x3; }

uint8_t get_modrm_reg(uint8_t mod_rm) { return (mod_rm >> 3) & 0x3; }

// assumes addressing mode 32bit
bool has_sib_byte(uint8_t mod_rm) {
    uint8_t mod = get_modrm_mod(mod_rm);
    uint8_t rm = get_modrm_rm(mod_rm);
    return mod <= 0x2 && rm == 0x4;
}
