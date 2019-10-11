#ifndef __ISCSI_REQUEST_CMD_H__
#define __ISCSI_REQUEST_CMD_H__

#include "target/iscsi_target.h"
#include "iscsi_buffer.h"
#include "iscsi_connection.h"
#include "iscsi_type.h"

int iscsi_request_cmd_process(byte* request, struct iSCSIConnection* connection, struct iSCSIBuffer* response) {
  iscsi_target_cmd_get_r2t_pdu(request, connection, response);
  return 0;
}

#endif // __ISCSI_REQUEST_CMD_H__
