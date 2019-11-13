#include "iscsi_utility.h"

int divide_round_up(int a, int b) {
  return (a + b - 1) / b;
}

int min(int a, int b) {
  return a < b ? a : b;
}

int max(int a, int b) {
  return a > b ? a : b;
}

int string_to_int(byte* s) {
  int val = 0;
  while (*s != '\0') {
    val = 10 * val + (*s++) - '0';
  }
  return val;
}

byte* int_to_string(int s) {
  static byte buffer[20];
  int i, len;
  
  if (s == 0) {
    buffer[0] = '0';
    buffer[1] = 0;
    return buffer;
  }
  
  len = 0;
  for (i = s; i > 0; i /= 10) {
    ++len;
  }
  buffer[len] = 0;
  for (i = s; i > 0; i /= 10) {
    buffer[--len] = i % 10 + '0';
  }
  
  return buffer;
}


