#ifndef OEP_H
#define OEP_H

#include <stdint.h>
#include <windows.h>

#include "code_begin.h"
#include "utility.h"

#ifdef _WIN64
#define OEP_DEFAULT (0xDEADBEEFABCD1337)
#else
#define OEP_DEFAULT (0xDEADBEEF)
#endif

size_t get_original_entry_point(void);
uint8_t write_original_entry_point(uint32_t targetOep, void *targetImageBase);

#endif
