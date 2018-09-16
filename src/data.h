#ifndef DATA_H
#define DATA_H

#include "functions.h"

typedef struct {
   void* imageBase;
   void* kernel32Base;
   void* codeAddress;
   uint32_t codeSize;
   functions_t functions;
}data_t;

#endif
