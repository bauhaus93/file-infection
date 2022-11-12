#include "utility.h"

void test_begin(void) {}

static int another_function(int x, int y) {
  if (x == y) {
    return x * x + y * y;
  } else {
    return x;
  }
}

static int function_with_switch(int x) {
  switch (x) {
  case 0:
    return 2 * x;
  case 1:
    x += 3;
    break;
  default:
    x = 100;
    break;
  }
  return x;
}

static int function_with_loop(int x) {
  int a = 0;
  while (x-- > 0) {
    a += x;
  }
  return a;
}

int test_entry(int value) {
  int x = value;
  if (value > 0) {
    value *= 2;
  } else if (value < 0) {
    value = value * value;
  } else {
    value = 42;
  }
  int y = x + function_with_switch(x) - function_with_loop(value);
  return value + another_function(x, value + y);
}

void test_end(void) {}
