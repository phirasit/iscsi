#include "iscsi_connection.h"
#include "iscsi_session.h"
#include "iscsi_server.h"
#include "iscsi_pdu.h"

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

  create_new_iscsi_buffer(&connection->receive_buffer);
  create_new_iscsi_buffer(&connection->response_buffer);
}

int incoming_request(struct iSCSIConnection* connection, byte* buffer, int length) {
  int error = iscsi_buffer_receive(&connection->receive_buffer, buffer, length);
  if (error) return error;

  int pdu_length = iscsi_pdu_length(connection->receive_buffer.data);
  if (pdu_length > length) return BUFFER_FULL;

  int response_pdu_status;
  do {
    response_pdu_status = iscsi_server_process(
      connection->receive_buffer.data, 
      connection,
      connection->response_buffer.data,
      iscsi_buffer_free_space(&connection->response_buffer)
    );
  } while (response_pdu_status != BUFFER_FULL);

  iscsi_buffer_flush(&connection->receive_buffer, response_pdu_status);

  return response_pdu_status;
}
