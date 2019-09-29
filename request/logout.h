#ifndef __ISCSI_REQUEST_LOGOUT_H__
#define __ISCSI_REQUEST_LOGOUT_H__

#include "iscsi_type.h"

enum LOGOUT_RESPONSE_CODE {
  LOGOUT_SUCCESS = 0x00,
  LOGOUT_CID_NOT_FOUND = 0x01,
  LOGOUT_CONNECTION_RECOVERY_NOT_SUPPORT = 0x02,
  LOGOUT_CLEANUP_FAIL = 0x03
};

int iscsi_request_logout_process(byte* request, struct iSCSIConnection* connection, byte* response, int length);

#endif
