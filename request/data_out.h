#ifndef __ISCSI_REQUEST_DATA_OUT_H__
#define __ISCSI_REQUEST_DATA_OUT_H__

#include "iscsi_buffer.h"

#include "iscsi_connection.h"
#include "iscsi_type.h"

int iscsi_request_data_out_process(byte* request, struct iSCSIConnection* connection, struct iSCSIBuffer* response) {
  iscsi_target_data_out_get_r2t_pdu(request, connection, response);
  return 0;
}

#endif // __ISCSI_REQUEST_DATA_OUT_H__
