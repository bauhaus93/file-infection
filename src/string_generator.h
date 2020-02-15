#ifndef STRING_GENERATOR_H
#define STRING_GENERATOR_H

#include <stdint.h>

#define STRING_KERNEL32_PATH (0)      // c:\windows\system32\kernel32.dll
#define STRING_EXE_SEARCH_PATTERN (1) // \\10.0.2.4\qemu\target\*.exe
#define STRING_SECTION_NAME (2)       // .hello

size_t get_string_length(uint16_t id);

size_t get_string(uint16_t id, char *dest, size_t destSize);

#endif // STRING_GENERATOR_H