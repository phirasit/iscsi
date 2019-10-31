#ifndef __ISCSI_REQUEST_LOGIN_H__
#define __ISCSI_REQUEST_LOGIN_H__

#include "iscsi_connection.h"
#include "iscsi_pdu.h"
#include "iscsi_server.h"
#include "iscsi_session.h"
#include "iscsi_type.h"

enum LOGIN_STATUS {
  SUCCESS = 0x0000,
  TARGET_MOVED_TEMPORARILY = 0x0101,
  TARGET_MOVED_PERMANENTLY = 0x0102,
  INITIATOR_ERROR = 0x0200,
  AUTHENTICATION_ERROR = 0x0201,
  AUTHORIZATION_ERROR = 0x0202,
  NOT_FOUND = 0x0203,
  TARGET_MOVED = 0x0204,
  UNSUPPORTED_VERSION = 0x0205,
  TOO_MANY_CONNECTION = 0x0206,
  MISSING_PARAMETER = 0x0207,
  CANT_INCLUDE_SESSION = 0x0208,
  SESSION_NOT_SUPPORTED = 0x0209,
  SESSION_NOT_EXIST = 0x020A,
  INVALID_DURING_LOGIN = 0x020B,
  TARGET_ERROR = 0x0300,
  SERVICE_UNAVAILABLE = 0x0301,
  OUT_OF_RESOURCE = 0x0302
};

enum LOGIN_STATE {
  SECURITY_NEGOTIATION = 0,
  LOGIN_OPERATIONAL = 1,
  FULL_FEATURE = 3
};

int iscsi_request_login_reject(byte* request, struct iSCSIConnection* connection, struct iSCSIBuffer* response);
int iscsi_request_login_process(byte* request, struct iSCSIConnection* connection, struct iSCSIBuffer* response);

#endif // __ISCSI_REQUEST_LOGIN_H__
