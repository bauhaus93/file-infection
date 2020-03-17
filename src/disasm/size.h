#ifndef SIZE_H
#define SIZE_H

#include <stdint.h>

#include "operand_type.h"

uint8_t get_operand_size(uint8_t *prefix, uint8_t prefix_count);
uint8_t get_opcode_size(uint8_t *opcode);
uint8_t get_addressing_mode(uint8_t *prefix, uint8_t prefix_count);
uint8_t get_size_by_operand_type(OperandType type, uint8_t operand_size);

#endif // SIZE_H
