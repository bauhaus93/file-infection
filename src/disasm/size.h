#ifndef SIZE_H
#define SIZE_H

#include <stdint.h>

#include "operand_type.h"

uint8_t get_operand_size(uint8_t *prefix, uint8_t prefix_count);
uint8_t get_opcode_size(uint8_t *opcode);
uint8_t get_addressing_mode(uint8_t *prefix, uint8_t prefix_count);
uint8_t get_size_by_operand_type(OperandType type, uint8_t operand_size);
uint8_t get_modrm_displacement_size(uint8_t modrm, uint8_t addressing_mode);
uint8_t get_sib_displacement_size(uint8_t modrm, uint8_t sib);

#endif // SIZE_H
