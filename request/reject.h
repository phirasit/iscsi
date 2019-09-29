#ifndef __ISCSI_REQUEST_REJECT_H__
#define __ISCSI_REQUEST_REJECT_H__

#include "iscsi_pdu.h"
#include "iscsi_server.h"
#include "iscsi_type.h"

enum REJECT_REASON {
  RESERVED = 0x01,
  DATA_DIGEST_ERROR = 0x02,
  SNACK_REJECT = 0x03,
  PROTOCOL_ERROR = 0x04,
  CMD_NOT_SUPPORTED = 0x05,
  IMM_CMD_REJECTED = 0x06,
  TASK_IN_PROGRESS = 0x07,
  INVALID_DATA_ACK = 0x08,
  INVALID_PDU_FIELD = 0x09,
  LONG_OP_REJECT = 0x0A,
  DEPRECATED = 0x0B,
  WAITING_FOR_LOGOUT = 0x0C
};

static inline void iscsi_pdu_request_reject_set_reason(byte* buffer, enum REJECT_REASON reason) {
  buffer[2] = (byte) reason;
}

int iscsi_request_reject(byte* request, enum REJECT_REASON reason, byte* response, int length) {
  if (length < BASIC_HEADER_SEGMENT_LENGTH) return BUFFER_FULL;
  iscsi_pdu_generate_from_buffer(response, request);
  iscsi_pdu_set_opcode(response, REJECT);
  iscsi_pdu_set_final(response, 1);
  iscsi_pdu_request_reject_set_reason(response, reason);
  return BASIC_HEADER_SEGMENT_LENGTH;
}

#endif // __ISCSI_REQUEST_REJECT_H__
