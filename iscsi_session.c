#include <string.h>

#include "iscsi_session.h"
#include "iscsi_connection.h"

void iscsi_session_create(struct iSCSISession* session) {
  session->num_connections = 0;
  session->max_connections = MAX_SUPPORTED_CONNS;

  session->ExpCmdSN = 500;
  session->error_recovery_level = 0;
  session->target = NULL;

  // memset(session->connections, 0, sizeof(struct Connection*) * MAX_SUPPORTED_CONNS);
}

void iscsi_session_add_connection(struct iSCSISession* session, struct iSCSIConnection* connection) {
  if (iscsi_session_full(session)) {
    return;
  }

  /*
  struct iSCSIConnection** con = session->connections;
  while (*con != NULL) {
    ++con;
  }
  *con = connection;
  */

  ++session->num_connections;
  connection->session_reference = session;
}
