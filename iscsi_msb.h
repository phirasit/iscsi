#ifndef __ISCSI_MSB_H__
#define __ISCSI_MSB_H__

#include "iscsi_type.h"

/*
byte iscsi_msb8(byte c) {
  c = ((c & 0xAA) >> 1) | ((c & 0x55) << 1);
  c = ((c & 0xCC) >> 2) | ((c & 0x33) << 2);
  c = ((c & 0xF0) >> 4) | ((c & 0x0F) << 4);
  return c;
}

short iscsi_msb16(short s) {
  s = ((s & 0xAAAA) >> 1) | ((s & 0x5555) << 1);
  s = ((s & 0xCCCC) >> 2) | ((s & 0x3333) << 2);
  s = ((s & 0xF0F0) >> 4) | ((s & 0x0F0F) << 4);
  s = ((s & 0xFF00) >> 8) | ((s & 0x00FF) << 8);
  return s;
}

int iscsi_msb32(int i) {
  i = ((i & 0xAAAAAAAA) >>  1) | ((i & 0x55555555) <<  1);
  i = ((i & 0xCCCCCCCC) >>  2) | ((i & 0x33333333) <<  2);
  i = ((i & 0xF0F0F0F0) >>  4) | ((i & 0x0F0F0F0F) <<  4);
  i = ((i & 0xFF00FF00) >>  8) | ((i & 0x00FF00FF) <<  8);
  i = ((i & 0xFFFF0000) >> 16) | ((i & 0x0000FFFF) << 16);
  return i;
}
*/

#endif // __ISCSI_MSB_H__
