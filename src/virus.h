#ifndef VIRUS_H
#define VIRUS_H

#include <stdint.h>
#include <stdio.h>
#include <windows.h>

#include "checksum.h"
#include "checksum_list.h"
#include "utility.h"
#include "infect.h"
#include "data.h"
#include "code_begin.h"
#include "code_end.h"

int run(void);

#endif