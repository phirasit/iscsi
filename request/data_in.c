#include "request/data_in.h"

#include "iscsi_byte.h"
#include "iscsi_pdu.h"
#include "iscsi_logger.h"
#include "iscsi_type.h"
#include "iscsi_utility.h"

static inline void iscsi_request_data_in_set_status(byte* buffer, byte S) {
  // set scsi status bit
  iscsi_byte_bit2byte(buffer + 1, 7, 1);
  // set status value
  buffer[3] = S;
}

static inline void iscsi_request_data_in_set_acknowledge(byte* buffer, int acknowledge) {
  iscsi_byte_bit2byte(buffer + 1, 1, acknowledge);
}

static inline void iscsi_request_data_in_set_data_sn(byte* buffer, int data_sn) {
  iscsi_byte_int2byte(buffer + 36, data_sn);  
}

static inline void iscsi_request_data_in_set_buffer_offset(byte* buffer, int offset) {
  iscsi_byte_int2byte(buffer + 40, offset);
}

void iscsi_request_data_in_send(struct iSCSIConnection* connection, int initiator_task_tag, int expected_data_transfer_length, struct iSCSITarget* target, struct iSCSIBuffer* response) {
  // TODO create data-in PDU for read operations

  // get total length and number of PDUs
  struct iSCSIConnectionParameter* parameter = iscsi_connection_parameter(connection);
  int max_receive_length = iscsi_connection_parameter_max_receive_data_segment_length(parameter);
  int num_pdu = divide_round_up(expected_data_transfer_length, max_receive_length); 

  // get others parameters
  byte* target_data = iscsi_target_buffer(target);
  byte status = 0; // Assume there is no error TODO fix this if there is an error
  int acknowledge = 0; // TODO this
  int target_trasfer_tag = DEFAULT_TARGET_TRANSFER_TAG; // TODO this


  logger("[REQUEST DATA IN] Start sending Data-In PDUs\n");
  logger("[REQUEST DATA IN] Total expected data length = %d\n", expected_data_transfer_length);
  logger("[REQUEST DATA IN] PDU data length = %d\n", max_receive_length);

  int offset = 0;
  for (int i = 0; i == 0 || offset < expected_data_transfer_length; ++i) {
    // create pdu
    int data_length = min(expected_data_transfer_length - offset, max_receive_length);
    int final = offset + data_length == expected_data_transfer_length;
    int pdu_length = BASIC_HEADER_SEGMENT_LENGTH + data_length;
    int total_length = iscsi_pdu_padded_length(pdu_length);

    byte* buffer = iscsi_buffer_acquire_lock_for_length(response, total_length);
      
    logger("[REQUEST DATA IN] build buffer: %d with offset = %d / %d\n", i, offset, expected_data_transfer_length);
    logger("[REQUEST DATA IN] data_length = %d\n", data_length);
    logger("[REQUEST DATA IN] address %p\n", buffer);
    
    iscsi_pdu_set_opcode(buffer, SCSI_DATA_IN);
    iscsi_pdu_set_final(buffer, final);
    iscsi_request_data_in_set_acknowledge(buffer, acknowledge);
    iscsi_request_data_in_set_status(buffer, status);

    iscsi_pdu_set_initiator_task_tag(buffer, initiator_task_tag);
    iscsi_pdu_set_target_transfer_tag(buffer, target_trasfer_tag);
    iscsi_pdu_set_response_header(buffer, connection);
    iscsi_request_data_in_set_data_sn(buffer, i);
    iscsi_request_data_in_set_buffer_offset(buffer, offset);
    iscsi_pdu_set_data_segment(buffer, target_data + offset, data_length);
    iscsi_pdu_pad0(buffer, pdu_length);

    iscsi_buffer_release_lock(response, total_length);
    offset += data_length;

    // advance stat sn
    // iscsi_connection_advance_stat_sn(connection);
  }
  
  logger("[REQUEST DATA IN] finish create data-in PDUs\n");
}
