#ifndef __ISCSI_TARGET_H__
#define __ISCSI_TARGET_H__

#include "iscsi_buffer.h"
#include "iscsi_type.h"
#include "iscsi_connection.h"

struct iSCSITarget {
  char* address;
};

struct iSCSITarget ISCSI_DEFAULT_TARGET;

void iscsi_target_cmd_get_r2t_pdu(byte* command, struct iSCSIConnection* connection, struct iSCSIBuffer* response);
void iscsi_target_data_out_get_r2t_pdu(byte* request, struct iSCSIConnection* connection, struct iSCSIBuffer* response);

#endif // __ISCSI_TARGET_H__
