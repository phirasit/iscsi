#include "iscsi_session.h"

#include "iscsi_buffer.h"
#include "iscsi_connection.h"
#include "iscsi_target.h"
#include "iscsi_transfer_entry.h"
#include "iscsi_logger.h"

#include <string.h>

extern struct iSCSITarget ISCSI_DEFAULT_TARGET;

void iscsi_session_create(struct iSCSISession* session) {
  session->num_connections = 0;
  session->max_connections = MAX_SUPPORTED_CONNS;

  session->TSIH = ISCSI_SESSION_DEFAULT_TSIH;

  session->ExpCmdSN = 0;
  session->error_recovery_level = 0;

  session->transfer_tag = 0;
  session->target = &ISCSI_DEFAULT_TARGET;
  iscsi_target_create_default(session->target);
  // memset(session->connections, 0, sizeof(struct Connection*) * MAX_SUPPORTED_CONNS);
}

void iscsi_session_add_connection(struct iSCSISession* session, struct iSCSIConnection* connection) {
  connection->session_reference = session;
  // TODO add reference back to session
}

int iscsi_session_next_transfer_tag(struct iSCSISession* session) {
  return ++session->transfer_tag;
}

struct iSCSITransferEntry* iscsi_session_get_transfer_entry(struct iSCSISession* session, int transfer_tag) {
  if (iscsi_transfer_entry_transfer_tag(&session->transfer_entry) == transfer_tag) {
    return &session->transfer_entry; 
  } else  {
    return NULL;
  }
}

void iscsi_session_execute_command(struct iSCSISession* session, byte* cdb, struct iSCSIBuffer* response) {
  /*
  // TODO add delay command to fasten the process
  if (iscsi_session_is_preceeding_command_pending(session, iscsi_command_cmd_sn(command))) {
    // session.DelayedCommands.Add(command);
    iscsi_command_add_delayed_command(session->delayed_command, command);
  } else {
    // commandsToExecute.Add(command);
    iscsi_command_execute_command(command);
  }
  */
  iscsi_target_execute_scsi_command(session->target, cdb);
}

void iscsi_session_execute_transfer_entry(struct iSCSISession* session, struct iSCSITransferEntry* entry, struct iSCSIBuffer* response) {
  // TODO this
  struct iSCSITarget* target = session->target;
  memcpy(iscsi_target_buffer(target), iscsi_transfer_entry_data(entry), iscsi_transfer_entry_data_length(entry));
  iscsi_session_execute_command(session, iscsi_transfer_entry_cdb(entry), response);
}

/*
// DEPRICATE
void iscsi_session_add_connection(struct iSCSISession* session, struct iSCSIConnection* connection) {
  if (iscsi_session_full(session)) {
    return;
  }

  struct iSCSIConnection** con = session->connections;
  while (*con != NULL) {
    ++con;
  }
  *con = connection;

  ++session->num_connections;
  connection->session_reference = session;
}
*/
