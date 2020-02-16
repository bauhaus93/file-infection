#ifndef MALLOC_H
#define MALLOC_H

#include "function.h"

#define MALLOC(size) (VIRTUAL_ALLOC(NULL, (SIZE_T)(size), MEM_COMMIT | MEM_RESERVE, 0))
#define FREE(addr) (VIRTUAL_FREE((LPVOID)addr, 0, MEM_RELEASE))

#endif // MALLOC_H
