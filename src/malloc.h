#ifndef MALLOC_H
#define MALLOC_H

#ifndef USE_STANDARD_MALLOC
#include "function_kernel32.h"
#include "windows_wrapper.h"

#define MALLOC(size)                                                           \
    VIRTUAL_ALLOC(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE)
#define FREE(addr) VIRTUAL_FREE(addr, 0, MEM_RELEASE)

#else
#include <stdlib.h>

#define MALLOC(size) malloc(size)
#define FREE(addr) free(addr)

#endif // USE_STANDARD_MALLOC

#endif // MALLOC_H
