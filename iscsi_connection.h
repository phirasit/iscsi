#ifndef __ISCSI_CONNECTION_H__
#define __ISCSI_CONNECTION_H__

#include "iscsi_buffer.h"
#include "iscsi_connection_parameter.h"
#include "iscsi_session.h"

struct iSCSISession;

struct iSCSIConnection {
  struct iSCSISession* session_reference;
  struct iSCSIBuffer receive_buffer;
  struct iSCSIBuffer response_buffer;
  struct iSCSIConnectionParameter parameter;

  int socket_fd;
  int so_far_in_order; // boolean
  int connection_id; 
  int state;
  int current_timeout;  // 32-bit
  int expected_stat_sn; // 32-bit
  // int missing_stat_sn_list[MaxMissingSPDU];
  int perform_connection_cleanup; // boolean
};

// static inline functions

static inline struct iSCSIConnectionParameter* iscsi_connection_get_parameter(
  struct iSCSIConnection* connection
) {
  return &connection->parameter;
}

static inline int iscsi_connection_stat_sn(struct iSCSIConnection* connection) {
  return connection->expected_stat_sn;
}

static inline int iscsi_connection_exp_cmd_sn(struct iSCSIConnection* connection) {
  // TODO this
  return connection->expected_stat_sn;
}

static inline int iscsi_connection_max_cmd_sn(struct iSCSIConnection* connection) {
  // TODO this
  return connection->expected_stat_sn;
}

// function prototypes 

void iscsi_connection_create(
  struct iSCSIConnection* connection,
  int socket_fd,
  struct iSCSISession* session
);

int incoming_request(
  struct iSCSIConnection *connection,
  byte* buffer,
  int length
);

void clear_buffer(
  struct iSCSIConnection* connection,
  int length
);

#endif // __ISCSI_CONNECTION_H__
