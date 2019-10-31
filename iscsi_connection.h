#ifndef __ISCSI_CONNECTION_H__
#define __ISCSI_CONNECTION_H__

#include "iscsi_buffer.h"
#include "iscsi_session.h"

struct iSCSISession;
struct iSCSITransferEntry;

struct iSCSIConnection {
  struct iSCSISession* session_reference;
  struct iSCSIBuffer receive_buffer;
  struct iSCSIBuffer response_buffer;

  struct iSCSITransferEntry* transfer_entry;

  int socket_fd;
  int so_far_in_order; // boolean
  int connection_id; 
  int state;
  int current_timeout;  // 32-bit

  int stat_sn; // 32-bit
  int expected_cmd_sn;
  int max_cmd_sn;
  // int missing_stat_sn_list[MaxMissingSPDU];
  int perform_connection_cleanup; // boolean
};

// static inline functions

static inline void iscsi_connection_advance_exp_cmd_sn(struct iSCSIConnection* connection) {
  connection->stat_sn++;
}

static inline int iscsi_connection_stat_sn(struct iSCSIConnection* connection) {
  return connection->stat_sn;
}

static inline int iscsi_connection_exp_cmd_sn(struct iSCSIConnection* connection) {
  // TODO this
  return connection->expected_cmd_sn;
}

static inline int iscsi_connection_max_cmd_sn(struct iSCSIConnection* connection) {
  // TODO this
  return connection->max_cmd_sn;
}

// function prototypes 

void iscsi_connection_create(
  struct iSCSIConnection* connection,
  int socket_fd,
  struct iSCSISession* session
);

struct iSCSITransferEntry* iscsi_connection_get_transfer_entry(
  struct iSCSIConnection* connection,
  int target_transfer_tag
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
