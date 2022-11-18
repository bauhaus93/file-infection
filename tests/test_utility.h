#ifndef TEST_UTILITY_H
#define TEST_UTILITY_H

#include <stdlib.h>

void *read_binary(const char *path, size_t *size);
void *copy_code(void *entry_point, size_t *target_size);

#endif // TEST_UTILITY_H
