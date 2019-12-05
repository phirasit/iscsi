#include "request/cmd.h"
#include "request/data_in.h"
#include "request/r2t.h"

#include "iscsi_byte.h"
#include "iscsi_logger.h"
#include "iscsi_pdu.h"
#include "iscsi_target.h"
#include "iscsi_utility.h"

#include <scsi/scsi.h>

static int iscsi_request_cmd_read(byte* request) {
  return iscsi_byte_byte2bit(request[1], 1);
}

static int iscsi_request_cmd_write(byte* request) {
  return iscsi_byte_byte2bit(request[1], 2);
}

static int iscsi_request_cmd_exp_data_transfer(byte* request) {
  return iscsi_byte_byte2int(request + 20);
}

static byte* iscsi_request_cmd_cdb(byte* request){ 
  return request + 32;
}

static void iscsi_request_cmd_set_response(byte* buffer, int response) {
  buffer[2] = (byte) response;
}

static void iscsi_request_cmd_set_status(byte* buffer, int status) {
  buffer[3] = (byte) status;
}

static void iscsi_request_cmd_set_sense_buffer(byte* buffer, byte* sense_buffer, int length) {
  iscsi_pdu_set_data_segment_length(buffer, 2 + length);
  iscsi_byte_short2byte(buffer + BASIC_HEADER_SEGMENT_LENGTH, length);
  memcpy(buffer + (BASIC_HEADER_SEGMENT_LENGTH + 2), sense_buffer, length);
}

int iscsi_request_cmd_process(byte* request, struct iSCSIConnection* connection, struct iSCSIBuffer* response) {
  struct iSCSISession* session = connection->session_reference;
  struct iSCSIConnectionParameter* parameter = iscsi_connection_parameter(connection);

  int transfer_tag = iscsi_session_next_transfer_tag(session);

  int data_seg_length = iscsi_pdu_data_segment_length(request);
  int exp_data_transfer = iscsi_request_cmd_exp_data_transfer(request);
  int byte_left = exp_data_transfer - data_seg_length;
  int next_offset = data_seg_length;

  logger("[REQUEST CMD] IS WRITE REQUEST: %d\n", iscsi_request_cmd_write(request));
  logger("[REQUEST CMD] IS READ REQUEST: %d\n", iscsi_request_cmd_read(request));
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
  int expect_data_transfer_length = iscsi_request_cmd_exp_data_transfer(request);
  // TODO change allocate memory to expect_data_transfer_length
  int scsi_response = iscsi_session_execute_command(session, iscsi_request_cmd_cdb(request), response);


  // data from target
  struct iSCSITarget* target = iscsi_session_target(session);
  sg_io_hdr_t* io_hdr = iscsi_target_sg_io_hdr(target);
  int target_status = io_hdr->status;
  int sense_buffer_length = io_hdr->sb_len_wr;
  byte* sense_buffer = (byte*) io_hdr->sbp;

  int total_length = iscsi_pdu_calc_size(0, 2 + sense_buffer_length);
  byte* buffer = iscsi_buffer_acquire_lock_for_length(response, total_length);
  iscsi_pdu_set_opcode(buffer, SCSI_CMD_RES);
  iscsi_pdu_set_final(buffer, 1);
  iscsi_request_cmd_set_response(buffer, scsi_response);
  iscsi_request_cmd_set_status(buffer, target_status);
  iscsi_request_cmd_set_sense_buffer(buffer, sense_buffer, sense_buffer_length); // write sense buffer
  iscsi_pdu_set_response_header(buffer, connection);

  iscsi_buffer_release_lock(response, total_length);

  if (iscsi_request_cmd_read(request)) {
    int initiator_task_tag = iscsi_pdu_initiator_task_tag(request);
    iscsi_request_data_in_send(initiator_task_tag, expect_data_transfer_length, connection, target, response);
  }

  return 0;
}
