#ifndef __ISCSI_BYTE_H__
#define __ISCSI_BYTE_H__

#include "iscsi_type.h"

static inline int iscsi_byte_bit(byte b, int p) {
  return (b >> (7-p)) & 0x01;
}

static inline void iscsi_byte_bit2byte(byte* buffer, int p, int val) {
  p = 7 - p;
  *buffer = ((val & 0x01) << p) | (*buffer & ~(0x01 << p));
}

static inline int iscsi_byte_byte2int(byte* buffer) {
  return ((int) buffer[0] << 24)
    | ((int) buffer[1] << 16)
    | ((int) buffer[2] <<  8)
    | ((int) buffer[3]      );
}

static inline void iscsi_byte_int2byte(byte* buffer, int value) {
  buffer[0] = (value >> 24) & 0xFF;
  buffer[1] = (value >> 16) & 0xFF;
  buffer[2] = (value >>  8) & 0xFF;
  buffer[3] = (value      ) & 0xFF;
}

static inline void iscsi_byte_short2byte(byte* buffer, int value) {
  buffer[0] = (value >> 8) & 0xFF;
  buffer[1] = (value     ) & 0xFF;
}

#endif // __ISCSI_BYTE_H__
