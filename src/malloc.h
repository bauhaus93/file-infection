#ifndef MALLOC_H
#define MALLOC_H

#include <windows.h>

#include "function_kernel32.h"

#define MALLOC(size)                                                           \
    VIRTUAL_ALLOC(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE)
#define FREE(addr) VIRTUAL_FREE(addr, 0, MEM_RELEASE)

#endif // MALLOC_H
