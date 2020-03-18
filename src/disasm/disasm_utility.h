#ifndef DISASM_UTILITY_H
#define DISASM_UTILITY_H

#include <stdbool.h>
#include <stdint.h>

bool opcode_in_range(uint8_t opcode, uint8_t low, uint8_t high);
uint8_t get_modrm_mod(uint8_t mod_rm);
uint8_t get_modrm_rm(uint8_t mod_rm);
uint8_t get_modrm_reg(uint8_t mod_rm);
bool has_sib_byte(uint8_t mod_rm);
uint8_t get_sib_scale(uint8_t sib);
uint8_t get_sib_index(uint8_t sib);
uint8_t get_sib_base(uint8_t sib);

#endif // DISASM_UTILITY_H
