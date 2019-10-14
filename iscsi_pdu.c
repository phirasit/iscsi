#include "iscsi_pdu.h"

int iscsi_pdu_valid(byte* pdu, int length) {
  if (length < BASIC_HEADER_SEGMENT_LENGTH) return 0;
  return iscsi_pdu_length(pdu) <= length;
}

void iscsi_pdu_pad0(byte* pdu, int length) {
  int pad_size = (iscsi_pdu_padded_length(length) - length) & 0x03;
  memset(pdu + length, 0, pad_size);
}
