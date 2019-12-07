#include "iscsi_byte.h"
#include "iscsi_connection.h"
#include "iscsi_session.h"
#include "iscsi_pdu.h"

#include "request/reject.h"
#include "request/logout.h"

static inline void iscsi_pdu_request_logout_set_response(byte* buffer, enum LOGOUT_RESPONSE_CODE code) {
  buffer[2] = code;
}

static inline void iscsi_pdu_request_logout_set_time2wait(byte* buffer, int time) {
  iscsi_byte_short2byte(buffer + 40, time);
}

static inline void iscsi_pdu_request_logout_set_time2retain(byte * buffer, int time) {
  iscsi_byte_short2byte(buffer + 42, time);
}

int iscsi_request_logout_process(byte* request, struct iSCSIConnection* connection, struct iSCSIBuffer* response) {
  struct iSCSISession* session = connection->session_reference;

  if (session == NULL) {
    return iscsi_request_reject(
      connection,
      PROTOCOL_ERROR,
      request,
      BASIC_HEADER_SEGMENT_LENGTH,
      response
    );
  }

  byte* buffer = iscsi_buffer_acquire_lock_for_length(response, BASIC_HEADER_SEGMENT_LENGTH);

  // iscsi_pdu_set_immediate(buffer, 1);
  iscsi_pdu_set_opcode(buffer, LOGOUT_RES);
  iscsi_pdu_set_final(buffer, 1);
  iscsi_pdu_set_response_header(buffer, connection);
  iscsi_pdu_request_logout_set_response(buffer, LOGOUT_SUCCESS);
  iscsi_pdu_request_logout_set_time2wait(buffer, 0);
  iscsi_pdu_request_logout_set_time2retain(buffer, 0);

  iscsi_buffer_release_lock(response, BASIC_HEADER_SEGMENT_LENGTH);

  return BASIC_HEADER_SEGMENT_LENGTH;
}
