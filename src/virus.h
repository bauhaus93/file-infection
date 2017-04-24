#include <stdint.h>
#include <stdio.h>

#include "ass.h"

typedef struct {
   uint8_t* imageBase;
   uint8_t* kernel32Base;
}Data;

void start_code(void);
int run(void);
