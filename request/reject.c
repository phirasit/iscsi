#include "request/reject.h"

#include "iscsi_pdu.h"

static inline void iscsi_pdu_request_reject_set_reason(byte* buffer, enum REJECT_REASON reason) {
  buffer[2] = (byte) reason;
}

int iscsi_request_reject(struct iSCSIConnection* connection, enum REJECT_REASON reason, byte* data, int length, struct iSCSIBuffer* response) {
  int total_length = BASIC_HEADER_SEGMENT_LENGTH + length;
  int padded_length = iscsi_pdu_padded_length(total_length);

  byte* buffer = iscsi_buffer_acquire_lock_for_length(response, padded_length);

  // TODO create new header
  iscsi_pdu_set_opcode(buffer, REJECT);
  iscsi_pdu_set_final(buffer, 1);
  iscsi_pdu_set_response_header(buffer, connection);
  iscsi_pdu_request_reject_set_reason(buffer, reason);
  iscsi_pdu_set_initiator_task_tag(buffer, DEFAULT_INITIAL_TASK_TAG);
  iscsi_pdu_set_data_segment(buffer, data, length);
  iscsi_pdu_pad0(buffer, total_length);

  iscsi_buffer_release_lock(response, padded_length);
  return 0;
}
