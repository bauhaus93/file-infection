#ifndef FUNCTION_H
#define FUNCTION_H

#include <stdint.h>

void *get_function_pointer(void *base, uint32_t function_checksum);

#endif // FUNCTION_H
