#ifndef __ISCSI_REQUEST_CMD_H__
#define __ISCSI_REQUEST_CMD_H__

#include "iscsi_buffer.h"
#include "iscsi_connection.h"
#include "iscsi_type.h"

int iscsi_request_cmd_send_response(struct iSCSIConnection* connection, byte* request, struct iSCSIBuffer* response);
int iscsi_request_cmd_process(byte* request, struct iSCSIConnection* connection, struct iSCSIBuffer* response);

#endif // __ISCSI_REQUEST_CMD_H__
