#ifndef __ISCSI_CONNECTION_PARAMETER_H__
#define __ISCSI_CONNECTION_PARAMETER_H__

#include "iscsi_define.h"
#include "iscsi_type.h"

struct iSCSIConnectionParameter {
  byte buffer[ISCSI_CONNECTION_PARAMETER_BUFFER_SIZE];
  int length;
};

void iscsi_connection_parameter_create(struct iSCSIConnectionParameter* parameter, byte* data, int length);
void iscsi_connection_parameter_update(struct iSCSIConnectionParameter* parameter, byte* key, byte* value);
byte* iscsi_connection_parameter_get(struct iSCSIConnectionParameter* parameter, byte* key);

byte* iscsi_connection_parameter_operational_data(struct iSCSIConnectionParameter* parameter);
int iscsi_connection_parameter_operational_length(struct iSCSIConnectionParameter* parameter);

byte* iscsi_connection_parameter_data(struct iSCSIConnectionParameter* parameter);
int iscsi_connection_parameter_length(struct iSCSIConnectionParameter* parameter);

int iscsi_connection_parameter_vendor_specific(struct iSCSIConnectionParameter* parameter);

// helper functions

byte* iscsi_parameter_get(byte* buffer, int length, byte* key);

#endif // __ISCSI_CONNECTION_PARAMETER_H__
