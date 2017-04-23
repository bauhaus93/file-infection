#include <stdio.h>
#include <stint.h>

typedef int(*fpTest)(int, int);

void test_rel(void) {
  int size = (uint32_t)test_rel - (uint32_t)test_function;
  int offset = (uint32_t) other_function - (uint32_t)test_function;

  printf("function size: %d\noffset: %d", size, offset);

  fpTest ptr = malloc(size);
  memcpy(ptr, test_function, size);
  //memset(ptr + offset, 0, size - offset); should crash if enabled

  printf("result = %d\n", ptr(5, 15));

  free(ptr);
}

int test_function(int a, int b) {
  int c = 666;
  return c + other_function(a, b);
}

int other_function(int a, int b) {
  return a + b;
}
