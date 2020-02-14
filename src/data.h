#ifndef DATA_H
#define DATA_H

#include <stdint.h>

#include "function_list.h"

typedef struct {
    void *image_base;
    void *kernel32_base;
    int32_t delta_offset;
    function_list_t function_list;
} data_t;

data_t * get_data(void);
void free_data(void);

#endif
