#ifndef INFECT_H
#define INFECT_H

#include <stdint.h>
#include <windows.h>

#include "data.h"

#define INFECTION_MARKER (0xDEADBEEF)

int infect(const char *filename, data_t *data);

#endif
