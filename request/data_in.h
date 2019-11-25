#ifndef __ISCSI_REQUEST_DATA_IN_H__
#define __ISCSI_REQUEST_DATA_IN_H__

#include "iscsi_buffer.h"
#include "iscsi_connection.h"
#include "iscsi_target.h"

void iscsi_request_data_in_send(struct iSCSIConnection* connection, struct iSCSIBuffer* buffer);

#endif // __ISCSI_REQUEST_DATA_IN_H__
