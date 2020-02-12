#ifndef DATA_H
#define DATA_H

#include <stdint.h>

#include "function_list.h"

typedef struct {
    void *image_base;
    void *kernel32_base;
    void *code_begin_addr;
    void *code_end_addr;
    uint32_t code_size;
    int32_t delta_offset;
    uint32_t entry_offset;
    function_list_t function_list;
} data_t;

int init_data(data_t *data, void *code_begin_addr, void *code_end_addr, void *entry_point);

#endif
