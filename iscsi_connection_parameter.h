#ifndef __ISCSI_CONNECTION_PARAMETER_H__
#define __ISCSI_CONNECTION_PARAMETER_H__

#include "iscsi_define.h"
#include "iscsi_type.h"

struct iSCSIConnectionParameter {
  byte buffer[ISCSI_CONNECTION_PARAMETER_BUFFER_SIZE];
  int length;

  // list of connection information
  int max_receive_data_segment_length;
  int max_connections;
  int initial_r2t;
  int immediate_data;
  int max_burst_length;
  int first_burst_length;
  int data_pdu_in_order;
  int data_sequence_in_order;
  int default_time_2_wait;
  int default_time_2_retain;
  int max_outstanding_r2t;
};

// inline functions
static inline int iscsi_connection_parameter_max_receive_data_segment_length(struct iSCSIConnectionParameter* parameter) {
  return parameter->max_receive_data_segment_length;
}

static inline int iscsi_connection_parameter_max_connections(struct iSCSIConnectionParameter* parameter) {
  return parameter->max_connections;
}

static inline int iscsi_connection_parameter_initial_r2t(struct iSCSIConnectionParameter* parameter) {
  return parameter->initial_r2t;
}

static inline int iscsi_connection_parameter_immeidate_data(struct iSCSIConnectionParameter* parameter) {
  return parameter->immediate_data;
}

static inline int iscsi_connection_parameter_max_burst_length(struct iSCSIConnectionParameter* parameter) {
  return parameter->max_burst_length;
}

static inline int iscsi_connection_parameter_first_burst_length(struct iSCSIConnectionParameter* parameter) {
  return parameter->first_burst_length;
}

static inline int iscsi_connection_parameter_data_pdu_in_order(struct iSCSIConnectionParameter* parameter) {
  return parameter->data_pdu_in_order;
}

static inline int iscsi_connection_parameter_data_sequence_in_order(struct iSCSIConnectionParameter* parameter) {
  return parameter->data_sequence_in_order;
}

static inline int iscsi_connection_parameter_default_time_2_wait(struct iSCSIConnectionParameter* parameter) {
  return parameter->default_time_2_wait;
}

static inline int iscsi_connection_parameter_default_time_2_retain(struct iSCSIConnectionParameter* parameter) {
  return parameter->default_time_2_retain;
}

static inline int iscsi_connection_parameter_max_outstanding_r2t(struct iSCSIConnectionParameter* parameter) {
  return parameter->max_outstanding_r2t;
}

// prototype functions

void iscsi_connection_parameter_generate_operational_data(struct iSCSIConnectionParameter* parameter);

void iscsi_connection_parameter_create(struct iSCSIConnectionParameter* parameter, byte* data, int length);
void iscsi_connection_parameter_update(struct iSCSIConnectionParameter* parameter, byte* key, byte* value);
byte* iscsi_connection_parameter_get(struct iSCSIConnectionParameter* parameter, byte* key);

byte* iscsi_connection_parameter_data(struct iSCSIConnectionParameter* parameter);
int iscsi_connection_parameter_length(struct iSCSIConnectionParameter* parameter);

int iscsi_connection_parameter_vendor_specific(struct iSCSIConnectionParameter* parameter);

// helper functions

byte* iscsi_parameter_get(byte* buffer, int length, byte* key);

#endif // __ISCSI_CONNECTION_PARAMETER_H__
