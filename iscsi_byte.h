#ifndef __ISCSI_BYTE_H__
#define __ISCSI_BYTE_H__

#include "iscsi_type.h"

static inline int iscsi_byte_bit(byte b, int p) {
  return (b >> p) & 0x01;
}

static inline int iscsi_byte_byte2int(byte* buffer) {
  return ((int) buffer[0] << 24)
    | ((int) buffer[1] << 16)
    | ((int) buffer[2] <<  8)
    | ((int) buffer[3]      );
}

#endif // __ISCSI_BYTE_H__
