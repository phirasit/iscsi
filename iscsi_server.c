#include "iscsi_connection.h"
#include "iscsi_pdu.h"
#include "iscsi_server.h"

#include "request/login.h"
#include "request/logout.h"
#include "request/nop.h"
#include "request/reject.h"
#include "request/text.h"

static int valid_command_numbering(struct iSCSIConnection* connection, byte* buffer) {
  struct iSCSISession* session = connection->session_reference;
  if (session == NULL) return 1;
  if (iscsi_pdu_has_cmdSN(buffer)) return 1; // NO cmdSN

  int cmdSN = iscsi_pdu_cmdSN(buffer);
  
  if (session->command_numbering_start) {
    if (cmdSN != session->ExpCmdSN) {
      return 0;
    }
  } else {
    session->ExpCmdSN = cmdSN;
    session->command_numbering_start = 1;
  }


  int opcode = iscsi_pdu_opcode(buffer);
  if (opcode == LOGOUT || opcode == TEXT || opcode == SCSI_CMD || opcode == REJECT) {
    if (!iscsi_pdu_immediate(buffer)) {
      ++session->ExpCmdSN;
    }
  }

  return 1;
}

// TODO add other responses here
int iscsi_server_process(
  byte* request,
  struct iSCSIConnection* connection,
  byte* response,
  int length
) {

  if (!valid_command_numbering(connection, request)) {
    // ignore this PDU
    return PDU_IGNORE;
  }

  // not login
  if (connection->session_reference == NULL || !connection->session_reference->is_full_feature_phase) {
    if (iscsi_pdu_opcode(request) == LOGIN) {
      return iscsi_request_login_process(request, connection, response, length);
    }
    if (connection->session_reference == NULL) {
      return SOCKET_TERMINATE;
    }
    return iscsi_request_login_reject(request, connection, response, length);
  }

  // has login
  switch (iscsi_pdu_opcode(request)) {
    case LOGOUT:
      return iscsi_request_logout_process(request, connection, response, length);
    case TEXT:
      return iscsi_request_text_process(request, connection, response, length);
  }

  if (connection->session_reference->is_discovery) {
    return iscsi_request_reject(request, PROTOCOL_ERROR, response, length);
  }

  switch (iscsi_pdu_opcode(request)) {
    case NOP_OUT:
      return iscsi_request_nop_out_process(request, response, length);
    case SCSI_DATA_OUT:
    case SCSI_CMD:
      return iscsi_request_data_out_process(request, connection, response, length);
      return iscsi_request_cmd_process(request, connection, response, length);
    case LOGIN:
      return iscsi_request_reject(request, PROTOCOL_ERROR, response, length);
    default:
      return iscsi_request_reject(request, CMD_NOT_SUPPORTED, response, length);
  }
}

