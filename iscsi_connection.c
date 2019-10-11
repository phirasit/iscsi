#include "iscsi_buffer.h"
#include "iscsi_connection.h"
#include "iscsi_session.h"
#include "iscsi_server.h"
#include "iscsi_pdu.h"

#include "logger.h"

// Connection 
void iscsi_connection_create(
  struct iSCSIConnection* connection,
  int socket_fd, 
  struct iSCSISession* session
) {
  connection->session_reference = session;
  connection->socket_fd = socket_fd;
  connection->so_far_in_order = 1;
  connection->connection_id = 0;
  // connection->state = STATE_IDLE;
  connection->current_timeout = 0;
  connection->expected_stat_sn = 0;
  connection->perform_connection_cleanup = 0;

  iscsi_buffer_new(&connection->receive_buffer);
  iscsi_buffer_new(&connection->response_buffer);
}

int incoming_request(struct iSCSIConnection* connection, byte* buffer, int length) {
  struct iSCSIBuffer* receiver = &connection->receive_buffer;

  int error = iscsi_buffer_receive(receiver, buffer, length);
  if (error) return error;

  if (!iscsi_pdu_valid(iscsi_buffer_data(receiver), iscsi_buffer_length(receiver))) {
    return PDU_INCOMPLETE;
  }

  int response_pdu_status = iscsi_server_process(
    connection->receive_buffer.data, 
    connection,
    &connection->response_buffer
  );

  return response_pdu_status;
}

void clear_buffer(struct iSCSIConnection* connection, int length) {
  iscsi_buffer_flush(&connection->receive_buffer, length);
}
