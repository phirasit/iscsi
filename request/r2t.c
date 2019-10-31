#include "request/r2t.h"

#include "iscsi_pdu.h"

static inline void iscsi_pdu_r2t_set_r2t_sn(byte* buffer, int r2t_sn) {
  iscsi_byte_int2byte(buffer + 36, r2t_sn);
}

static inline void iscsi_pdu_r2t_set_buffer_offset(byte* buffer, int offset) {
  iscsi_byte_int2byte(buffer + 40, offset);
}

static inline void iscsi_pdu_r2t_set_desired_data_transfer_length(byte* buffer, int desired_data_transfer_length) {
  iscsi_byte_int2byte(buffer + 44, desired_data_transfer_length);
}

void iscsi_request_r2t_send(
  int initiator_task_tag,
  int target_transfer_tag,
  int r2t_sn,
  int offset,
  int desired_data_transfer_length,
  struct iSCSIBuffer* response
) {
  byte* buffer = iscsi_buffer_acquire_lock_for_length(response, BASIC_HEADER_SEGMENT_LENGTH);
  iscsi_pdu_set_opcode(buffer, R2T);
  iscsi_pdu_set_final(buffer, 1);
  iscsi_pdu_set_ahs_length(buffer, 0);
  iscsi_pdu_set_data_segment_length(buffer, 0);
  iscsi_pdu_set_initiator_task_tag(buffer, initiator_task_tag);
  iscsi_pdu_set_target_transfer_tag(buffer, target_transfer_tag);

  iscsi_pdu_r2t_set_r2t_sn(buffer, r2t_sn);
  iscsi_pdu_r2t_set_buffer_offset(buffer, offset);
  iscsi_pdu_r2t_set_desired_data_transfer_length(buffer, desired_data_transfer_length);

  iscsi_buffer_release_lock(response, BASIC_HEADER_SEGMENT_LENGTH);
}
