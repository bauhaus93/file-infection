#include <stdio.h>

#include "../utility.h"
#include "disasm_utility.h"
#include "size.h"

// assuming the default operand size to be 32bit
uint8_t get_operand_size(uint8_t *prefix, uint8_t prefix_count) {
  for (uint8_t i = 0; i < prefix_count; i++) {
    if (prefix[i] == 0x66) {
      return 16;
    }
  }
  return 32;
}

// assuming the default addressing mode to be 32bit
uint8_t get_addressing_mode(uint8_t *prefix, uint8_t prefix_count) {
  for (uint8_t i = 0; i < prefix_count; i++) {
    if (prefix[i] == 0x67) {
      return 16;
    }
  }
  return 32;
}

uint8_t get_opcode_size(uint8_t *opcode) {
  // 2 byte: 0x0F
  // 3 byte: 0x0F38, 0x0F3A
  if (*opcode != 0x0F) {
    return 1;
  } else if (*(opcode + 1) == 0x38 || *(opcode + 1) == 0x3A) {
    return 3;
  } else {
    return 2;
  }
}

uint8_t get_size_by_operand_type(OperandType type, uint8_t operand_size) {
  if (type == OPERAND_TYPE_B) {
    return 1;
  } else if (type == OPERAND_TYPE_W) {
    return 2;
  } else if (type == OPERAND_TYPE_D) {
    return 4;
  } else if (type == OPERAND_TYPE_Q) {
    return 8;
  } else if (type == OPERAND_TYPE_DQ) {
    return 16;
  } else if (type == OPERAND_TYPE_QQ) {
    return 32;
  } else if (type == OPERAND_TYPE_V) {
    return operand_size / 8;
  } else if (type == OPERAND_TYPE_Z) {
    if (operand_size == 16) {
      return 2;
    } else {
      return 4;
    }
  } else if (type == OPERAND_TYPE_P) {
    if (operand_size == 16) {
      return 2 + 2;
    } else {
      return 2 + 4;
    }
  }
  PRINT_DEBUG("Got unhandeled operand type: %d", (int)type);
  return 0;
}

uint8_t get_modrm_displacement_size(uint8_t modrm, uint8_t addressing_mode) {
  uint8_t mod = get_modrm_mod(modrm);
  if (addressing_mode == 16) {
    if (mod == 0x0 && get_modrm_rm(modrm) == 0x6) {
      return 2;
    } else if (mod == 0x1) {
      return 1;
    } else if (mod == 0x2) {
      return 2;
    }
  } else if (addressing_mode == 32) {
    if (mod == 0x0 && get_modrm_rm(modrm) == 0x5) {
      return 4;
    } else if (mod == 0x1) {
      return 1;
    } else if (mod == 0x2) {
      return 4;
    }
  } else {
    return 0;
  }
  return 0;
}

uint8_t get_sib_displacement_size(uint8_t modrm, uint8_t sib) {
  if (get_sib_base(sib) == 0x05) {
    uint8_t mod = get_modrm_mod(modrm);
    if (mod == 0x00 || mod == 0x02) {
      return 4;
    } else if (mod == 0x01) {
      return 1;
    }
  }
  return 0;
}
