#ifndef __ISCSI_REQUEST_REJECT_H__
#define __ISCSI_REQUEST_REJECT_H__

#include "iscsi_buffer.h"
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

int iscsi_request_reject(byte* request, enum REJECT_REASON reason, byte* data, int length, struct iSCSIBuffer* response);

#endif // __ISCSI_REQUEST_REJECT_H__
