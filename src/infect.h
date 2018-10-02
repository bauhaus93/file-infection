#ifndef INFECT_H
#define INFECT_H

#include <windows.h>
#include <stdint.h>

#include "data.h"
#include "pe.h"
#include "utility.h"
#include "oep.h"

#define INFECTION_MARKER (0xDEADBEEF)

int infect(const char* filename, data_t* data);

#endif
