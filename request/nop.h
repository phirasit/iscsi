#ifndef __ISCSI_REQUEST_NOP_H__
#define __ISCSI_REQUEST_NOP_H__

#include "iscsi_pdu.h"
#include "iscsi_server.h"
#include "iscsi_type.h"

/*
static int generate_nop_out_pdu(
  byte* response, int length,
  int initial_task_tag, int target_transfer_tag,
  byte* data, int data_length
) {
  return 0;
}

#define TARGET_TRANSFER_TAG 0xFFFFFFFF

int generate_keep_alive_pdu(byte* response, int length) {
  static int initial_task_tag = 0xFFFFFFFF;
  return generate_nop_out_pdu(
    response, length, initial_task_tag, TARGET_TRANSFER_TAG, NULL, 0
  );
}
*/

int iscsi_request_nop_out_process(byte* request, byte* response, int length) {
  if (length < iscsi_pdu_length(request)) return BUFFER_FULL;
  iscsi_pdu_generate_from_buffer(response, request);
  iscsi_pdu_set_immediate(response, 1);
  iscsi_pdu_set_opcode(response, NOP_IN);
  iscsi_pdu_set_final(response, 0);
  memcpy(
    response + BASIC_HEADER_SEGMENT_LENGTH,
    request + BASIC_HEADER_SEGMENT_LENGTH,
    iscsi_pdu_data_segment_length(request)
  );
  return iscsi_pdu_length(response);

  /*
  return iscsi_pdu_generate(
    response, length,
    0, NOP_IN, 1, 0,
    iscsi_pdu_ahs_length(request), 
    iscsi_pdu_data_segment_length(request),
    request + 8,
    request + 48
  );
  */
}

#endif // __ISCSI_REQUEST_NOP_H__
