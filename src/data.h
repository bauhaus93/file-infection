#ifndef DATA_H
#define DATA_H

#include <stdint.h>

#include "functions.h"

typedef struct {
    void *imageBase;
    void *kernel32Base;
    void *codeBegin;
    void *codeEnd;
    uint32_t codeSize;
    int32_t deltaOffset;
    uint32_t entryOffset;
    functions_t functions;
} data_t;

int init_data(data_t *data, void *codeBegin, void *codeEnd, void *entryPoint);

#endif
