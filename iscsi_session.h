#ifndef __ISCSI_SESSION_H__
#define __ISCSI_SESSION_H__

#include "iscsi_define.h"
#include "iscsi_connection_parameter.h"
#include "iscsi_transfer_entry.h"
#include "iscsi_type.h"

// forward declaration
struct iSCSIBuffer;
struct iSCSIConnection;

struct iSCSISession {
  int num_connections;
  int max_connections;

  int TSIH;
  int ExpCmdSN;

  int command_numbering_start;

  int is_full_feature_phase;
  int is_discovery;
  int error_recovery_level;

  int transfer_tag;

  struct iSCSITarget* target;
  struct iSCSITransferEntry transfer_entry;
  struct iSCSIConnectionParameter parameter;
  // struct iSCSIConnection* connections[MAX_SUPPORTED_CONNS];
};

struct iSCSISession ISCSI_DEFAULT_SESSION;

// inline functions

static inline int iscsi_session_full(struct iSCSISession* session) {
  return session->num_connections >= session->max_connections;
}

static inline struct iSCSITransferEntry* iscsi_session_transfer_entry(struct iSCSISession* session) {
  return &session->transfer_entry;
}

static inline struct iSCSIConnectionParameter* iscsi_session_parameter(struct iSCSISession* session) {
  return &session->parameter;
}

// function prototypes

void iscsi_session_create(struct iSCSISession* session);

void iscsi_session_add_connection(struct iSCSISession* session, struct iSCSIConnection* connection);
int iscsi_session_next_transfer_tag(struct iSCSISession* session);

struct iSCSITransferEntry* iscsi_session_get_transfer_entry(struct iSCSISession* session, int target_transfer_tag);

void iscsi_session_execute_command(struct iSCSISession* session, byte* cdb, struct iSCSIBuffer* response);
void iscsi_session_execute_transfer_entry(struct iSCSISession* session, struct iSCSITransferEntry* entry, struct iSCSIBuffer* response);

#endif // __ISCSI_SESSION_H__
