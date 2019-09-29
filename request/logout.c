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

int iscsi_request_logout_process(byte* request, struct iSCSIConnection* connection, byte* response, int length) {
  if (length > BASIC_HEADER_SEGMENT_LENGTH) return BUFFER_FULL;
  iscsi_pdu_generate_from_buffer(response, request);
  iscsi_pdu_set_opcode(response, LOGOUT_RES);
  iscsi_pdu_set_final(response, 1);
  iscsi_pdu_request_logout_set_response(response, LOGOUT_SUCCESS);
  iscsi_pdu_request_logout_set_time2wait(response, 0);
  iscsi_pdu_request_logout_set_time2retain(response, 0);
  return BASIC_HEADER_SEGMENT_LENGTH;
}
