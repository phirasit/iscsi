#include "iscsi_pdu.h"

int iscsi_pdu_valid(byte* pdu, int length) {
  if (length < BASIC_HEADER_SEGMENT_LENGTH) return 0;
  int total_length = iscsi_pdu_length(pdu);
  if (total_length < length) return total_length;
  return length;
}
