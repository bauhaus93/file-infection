#ifndef OEP_H
#define OEP_H

#include <stddef.h>
#include <stdint.h>

#include "windows_wrapper.h"

#ifdef _WIN64
#define OEP_DEFAULT (0xDEADBEEFABCD1337)
#else
#define OEP_DEFAULT (0xDEADBEEF)
#endif

size_t get_original_entry_point(void);
uint8_t write_original_entry_point(uint32_t target_oep,
                                   void *target_image_base);

#endif
