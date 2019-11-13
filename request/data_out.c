#include "request/data_out.h"

#include "request/r2t.h"
#include "iscsi_byte.h"
#include "iscsi_pdu.h"
#include "iscsi_logger.h"
#include "iscsi_transfer_entry.h"
#include "iscsi_utility.h"

static int iscsi_request_data_out_buffer_offset(byte* request) {
  return iscsi_byte_byte2int(request + 40);
}

int iscsi_request_data_out_process(byte* request, struct iSCSIConnection* connection, struct iSCSIBuffer* response) {
  struct iSCSISession* session = connection->session_reference;
  struct iSCSIConnectionParameter* parameter = iscsi_connection_parameter(connection);

  struct iSCSITransferEntry* transfer_entry = NULL;
  if (iscsi_pdu_target_transfer_tag(request) != DEFAULT_TARGET_TRANSFER_TAG) {
    transfer_entry = iscsi_session_get_transfer_entry(session, iscsi_pdu_target_transfer_tag(request));
  } else if (!iscsi_connection_parameter_initial_r2t(parameter)) {
    // TODO implement this
    /*
    transfer_entry = iscsi_connection_get_transfer_entry_using_task_tag(connection, iscsi_pdu_initiator_task_tag(request));
    */
  }

  if (transfer_entry == NULL) {
    // TODO throw error
    logger("no transfer entry");
    return 0;
  }

  int offset = iscsi_request_data_out_buffer_offset(request);
  int data_length = iscsi_pdu_data_segment_length(request);
  int total_length = iscsi_transfer_entry_data_length(transfer_entry);

  // TODO store data in buffer
  // Store segment (we only execute the command after receiving all of its data)
  memcpy(
    iscsi_transfer_entry_data(transfer_entry) + offset,
    iscsi_pdu_data(request),
    data_length
  );

  if (offset + data_length == total_length) {
    // TODO last data-out PDU
    /*
    iscsi_session_remove_transfer(
      session, 
      iscsi_pdu_data_out_initiator_task_tag(request), 
      iscsi_pdu_data_out_target_transfer_tag(request)
    );
    */

    // execute command
    iscsi_session_execute_transfer_entry(session, transfer_entry, response);

    /*
    if (iscsi_session_is_preceeding_command_pending(session, cmd_sn)) {
      iscsi_delayed_command_add(session->delayed_command, iscsi_trasnfer_entry_command(transfer_entry));
    } else {
      iscsi_command_execute_command(iscsi_trasnfer_entry_command(transfer_entry));
      iscsi_session_execute_command(session);
    }
    */
  } else if (iscsi_pdu_final(request)) {

    // send more r2t for more data
    struct iSCSIConnectionParameter* parameter = iscsi_connection_parameter(connection);
    int max_receive_data_seg_length = iscsi_connection_parameter_max_receive_data_segment_length(parameter);
    int next_r2t_sn = iscsi_transfer_entry_next_r2t_sn(transfer_entry);
    int total_r2t_sn = iscsi_transfer_entry_total_r2t_sn(transfer_entry);

    if (next_r2t_sn < total_r2t_sn) {
      int next_offset = iscsi_transfer_entry_next_offset(transfer_entry);

      iscsi_request_r2t_send(
        iscsi_pdu_initiator_task_tag(request),
        iscsi_pdu_target_transfer_tag(request),
        next_r2t_sn,
        next_offset,
        min(max_receive_data_seg_length, total_length - next_offset),
        response
      );

      iscsi_transfer_entry_increase_r2t_sn(transfer_entry);
    }
  }

  return 0;
}

