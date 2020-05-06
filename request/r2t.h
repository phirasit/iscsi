#ifndef __ISCSI_REQUEST_R2T_H__
#define __ISCSI_REQUEST_R2T_H__

#include "iscsi_byte.h"
#include "iscsi_buffer.h"
#include "iscsi_connection.h"

void iscsi_request_r2t_send(
  struct iSCSIConnection* connection,
  int initiator_task_tag,
  int target_transfer_tag,
  int r2t_sn,
  int offset,
  int desired_data_transfer_length,
  struct iSCSIBuffer* response
);

#endif // __ISCSI_REQUEST_R2T_H__
