#ifndef __ISCSI_SERVER_H__
#define __ISCSI_SERVER_H__

#include "iscsi_type.h"

enum PDU_STATUS {
  SOCKET_TERMINATE = -1,
  BUFFER_FULL = -2,
  PDU_IGNORE = -3,
  PDU_ERROR = -4
};

struct iSCSIConnection;

int iscsi_server_process(
  byte* request, 
  struct iSCSIConnection* connection,
  byte* response,
  int length
);

#endif // __ISCSI_SERVER_H__

