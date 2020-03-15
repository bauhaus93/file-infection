#ifndef CODE_SIZE_H
#define CODE_SIZE_H

#include "code_begin.h"
#include "code_end.h"
#include "memory.h"

#define CODE_SIZE (BYTE_DIFF(code_end, code_begin))

#endif // CODE_SIZE_H
