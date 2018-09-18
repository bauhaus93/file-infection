#ifndef INFECT_H
#define INFECT_H

#include <windows.h>
#include <stdint.h>
#include <stdio.h>

#include "data.h"
#include "utility.h"

int infect(const char* filename, data_t* data);

#endif
