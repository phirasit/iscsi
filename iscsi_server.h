#ifndef __ISCSI_SERVER_H__
#define __ISCSI_SERVER_H__

#include "iscsi_buffer.h"
#include "iscsi_connection.h"
#include "iscsi_type.h"

enum PDU_STATUS {
  SOCKET_TERMINATE = -1,
  BUFFER_FULL = -2,
  PDU_INCOMPLETE = -3,
  PDU_IGNORE = -4,
  PDU_ERROR = -5
};

int iscsi_server_process(
  byte* request, 
  struct iSCSIConnection* connection,
  struct iSCSIBuffer* buffer
);

#endif // __ISCSI_SERVER_H__

