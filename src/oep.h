#ifndef OEP_H_INCLUDED
#define OEP_H_INCLUDED

#include <stdint.h>

#include "code_begin.h"

void* get_original_entry_point(void);
uint8_t write_original_entry_point(uint32_t srcOep, uint32_t targetOep, void* targetImageBase);

#endif