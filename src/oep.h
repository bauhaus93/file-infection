#ifndef OEP_H
#define OEP_H

#include <stdint.h>

#include "code_begin.h"
#include "utility.h"

#define OEP_DEFAULT (0xABCDABCD)

uint32_t get_original_entry_point(void);
uint8_t write_original_entry_point(uint32_t targetOep, void* targetImageBase);

#endif