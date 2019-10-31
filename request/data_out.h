#ifndef __ISCSI_REQUEST_DATA_OUT_H__
#define __ISCSI_REQUEST_DATA_OUT_H__

#include "iscsi_buffer.h"
#include "iscsi_connection.h"
#include "iscsi_type.h"

int iscsi_request_data_out_process(byte* request, struct iSCSIConnection* connection, struct iSCSIBuffer* response);

#endif // __ISCSI_REQUEST_DATA_OUT_H__
