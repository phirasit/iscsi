#include "request/nop.h"

#include "iscsi_connection_parameter.h"
#include "iscsi_pdu.h"
#include "iscsi_server.h"
#include "iscsi_utility.h"

int iscsi_request_nop_out_process(byte* request, struct iSCSIConnection* connection, struct iSCSIBuffer* response) {
  struct iSCSIConnectionParameter* parameter = iscsi_connection_parameter(connection);
  int max_receive_data_segment_length = iscsi_connection_parameter_max_receive_data_segment_length(parameter);
  int data_length = min(max_receive_data_segment_length, iscsi_pdu_data_segment_length(request));
  int total_length = BASIC_HEADER_SEGMENT_LENGTH + data_length;

  logger("[NOP OUT] this is a nop-out PDU\n");
  // TODO check validity of initiator_task_tag and target_transfer_tag
  int valid = 1;

  byte* buffer = iscsi_buffer_acquire_lock_for_length(response, total_length);

  iscsi_pdu_set_opcode(buffer, NOP_IN);
  iscsi_pdu_set_final(buffer, 1);
  
  if (valid) {
    iscsi_pdu_set_initiator_task_tag(buffer, iscsi_pdu_initiator_task_tag(request));
    iscsi_pdu_set_target_transfer_tag(buffer, iscsi_pdu_target_transfer_tag(request));
  } else {
    iscsi_pdu_set_initiator_task_tag(buffer, DEFAULT_INITIAL_TASK_TAG);
    iscsi_pdu_set_target_transfer_tag(buffer, DEFAULT_TARGET_TRANSFER_TAG);
    // TODO: decrease stat sn
  }

  iscsi_pdu_set_response_header(buffer, connection);
  iscsi_pdu_set_data_segment(buffer, iscsi_pdu_data(request), data_length);
  iscsi_pdu_pad0(buffer, total_length - BASIC_HEADER_SEGMENT_LENGTH - iscsi_pdu_data_segment_length(request));

  iscsi_buffer_release_lock(response, total_length);

  return total_length;
}
