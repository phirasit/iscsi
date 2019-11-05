#include "request/cmd.h"
#include "request/data_in.h"
#include "request/r2t.h"

#include "iscsi_byte.h"
#include "iscsi_pdu.h"
#include "iscsi_target.h"

#include "logger.h"

static int min(int a, int b) {
  return a < b ? a : b;
}

static int divide_round_up(int a, int b) {
  return (a + b - 1) / b;
}

static int iscsi_request_cmd_write(byte* request) {
  return iscsi_byte_bit(request[1], 0);
}

static int iscsi_request_cmd_exp_data_transfer(byte* request) {
  return iscsi_byte_byte2int(request + 20);
}

static byte* iscsi_request_cmd_cdb(byte* request){ 
  return request + 32;
}

int iscsi_request_cmd_process(byte* request, struct iSCSIConnection* connection, struct iSCSIBuffer* response) {
  struct iSCSISession* session = connection->session_reference;
  struct iSCSIConnectionParameter* parameter = iscsi_connection_parameter(connection);

  int transfer_tag = iscsi_session_next_transfer_tag(session);

  int data_seg_length = iscsi_pdu_data_segment_length(request);
  int exp_data_transfer = iscsi_request_cmd_exp_data_transfer(request);
  int byte_left = exp_data_transfer - data_seg_length;
  int next_offset = data_seg_length;

  if (iscsi_request_cmd_write(request) && data_seg_length < exp_data_transfer) {
    if (!iscsi_connection_parameter_initial_r2t(parameter)) {
      int first_data_pdu_length = min(
        iscsi_connection_parameter_first_burst_length(parameter),
        exp_data_transfer
      ) - data_seg_length;
      byte_left -= first_data_pdu_length;
      next_offset += first_data_pdu_length;
    }

    int connection_target_max_receive = iscsi_connection_parameter_max_receive_data_segment_length(parameter);
    int total_r2t = divide_round_up(byte_left, connection_target_max_receive);

    int outgoing_r2t = min(iscsi_connection_parameter_max_outstanding_r2t(parameter), total_r2t);

    // TODO create new transfer target
    struct iSCSITransferEntry* transfer_entry = iscsi_session_transfer_entry(session);
    iscsi_transfer_entry_create(
      transfer_entry,
      iscsi_request_cmd_cdb(request),
      exp_data_transfer,
      connection_target_max_receive,
      transfer_tag
    );

    for (int i = 0; i < outgoing_r2t; ++i) {
      // create r2t PDU
      iscsi_request_r2t_send(
        iscsi_pdu_initiator_task_tag(request),
        transfer_tag,
        i, // r2t sn
        next_offset, // buffer offset
        min(connection_target_max_receive, exp_data_transfer - next_offset),
        response // iSCSIBuffer
      );

      next_offset += connection_target_max_receive;
    }

    // TODO return success status
    
    return 0;
  }

  // execute command
  iscsi_session_execute_command(session, iscsi_request_cmd_cdb(request), response);

  return 0;
}
