#include "iscsi_connection.h"
#include "iscsi_server.h"
#include "iscsi_pdu.h"

#include "request/logout.h"

static inline void iscsi_pdu_request_logout_set_response(byte* buffer, enum LOGOUT_RESPONSE_CODE code) {
  buffer[2] = code;
}

static inline void iscsi_pdu_request_logout_set_time2wait(byte* buffer, int time) {
  buffer[40] = (time >> 8) & 0xFF;
  buffer[41] = (time     ) & 0xFF;
}

static inline void iscsi_pdu_request_logout_set_time2retain(byte * buffer, int time) {
  buffer[42] = (time >> 8) & 0xFF;
  buffer[43] = (time     ) & 0xFF;
}

int iscsi_request_logout_process(byte* request, struct iSCSIConnection* connection, struct iSCSIBuffer* response) {
  byte* ptr = iscsi_buffer_acquire_lock_for_length(response, BASIC_HEADER_SEGMENT_LENGTH);
  iscsi_pdu_generate_from_buffer(ptr, request);
  iscsi_pdu_set_opcode(ptr, LOGOUT_RES);
  iscsi_pdu_set_final(ptr, 1);
  iscsi_pdu_request_logout_set_response(ptr, LOGOUT_SUCCESS);
  iscsi_pdu_request_logout_set_time2wait(ptr, 0);
  iscsi_pdu_request_logout_set_time2retain(ptr, 0);
  iscsi_buffer_release_lock(response, BASIC_HEADER_SEGMENT_LENGTH);
  return BASIC_HEADER_SEGMENT_LENGTH;
}
