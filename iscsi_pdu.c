#include <string.h>

#include "iscsi_server.h"
#include "iscsi_pdu.h"

/*
// generate pdu

int iscsi_pdu_generate(
    byte* response, int length, 
    int immediate, enum OPCODE opcode, int final_bit, int opcode_specific,
    int total_ahs_length, int data_segment_length,
    byte* lun_opcode_specific,
    byte* data
) {
  // impossible as the packet size is larger than the buffer size
  int pdu_size = iscsi_pdu_calc_size(total_ahs_length, data_segment_length);
  if (pdu_size > length) {
    return BUFFER_FULL;
  }

  response[0] = (immediate << 7) | (byte) opcode;
  response[1] = (final_bit << 7) | ((opcode_specific >> 16) & 0x7F);
  response[2] = (opcode >> 8) & 0xFF;
  response[3] = (opcode     ) & 0xFF;

  response[4] = total_ahs_length;
  response[5] = (data_segment_length >> 16) & 0xFF;
  response[6] = (data_segment_length >>  8) & 0xFF;
  response[7] = (data_segment_length      ) & 0xFF;

  memcpy(response + 8, lun_opcode_specific, 48);
  memcpy(response + 48, data, data_segment_length);

  return pdu_size;
}
*/

