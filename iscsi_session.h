#ifndef __ISCSI_SESSION_H__
#define __ISCSI_SESSION_H__

#include "iscsi_define.h"

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

  struct iSCSITarget* target;
  // struct iSCSIConnection* connections[MAX_SUPPORTED_CONNS];
};

struct iSCSISession ISCSI_DEFAULT_SESSION;

static inline int iscsi_session_full(struct iSCSISession* session) {
  return session->num_connections >= session->max_connections;
}

void iscsi_session_create(struct iSCSISession* session);

void iscsi_session_add_connection(struct iSCSISession* session, struct iSCSIConnection* connection);

#endif // __ISCSI_SESSION_H__
