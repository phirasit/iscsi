#include "iscsi_target.h"

#include "iscsi_buffer.h"
#include "iscsi_connection.h"
#include "iscsi_define.h"
#include "iscsi_pdu.h"

#include <sys/ioctl.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "iscsi_logger.h"

void iscsi_target_create_default() {
  struct iSCSITarget* target = &ISCSI_DEFAULT_TARGET;
  target->address = "/dev/null";
}

void iscsi_target_execute_scsi_command(struct iSCSITarget* target, byte* cdb, struct iSCSIBuffer* response) {
  int fd = open(target->address, O_RDWR);
  if (fd < 0) {
    logger("cannot open: %s", target->address);
    return;
  }

  memset(&target->io_hdr, 0, sizeof(sg_io_hdr_t));
  target->io_hdr.interface_id          = 'S';
  target->io_hdr.dxfer_direction       = SG_DXFER_TO_DEV; // change to the correct direction
  target->io_hdr.cmd_len               = SCSI_CDB_LENGTH;
  target->io_hdr.mx_sb_len             = ISCSI_TARGET_SENSE_BUFFER_SIZE;
  target->io_hdr.iovec_count           = 0;
  target->io_hdr.dxfer_len             = ISCSI_TARGET_MEMORY_SIZE; // the length of the data transfer
  target->io_hdr.dxferp                = (void*) target->buffer;
  target->io_hdr.cmdp                  = cdb;
  target->io_hdr.sbp                   = (void*) target->sense_buffer; 
  target->io_hdr.timeout               = ISCSI_TARGET_TIMEOUT_MS;
  target->io_hdr.flags                 = SG_FLAG_DIRECT_IO;

  if (ioctl(fd, SG_IO, &target->io_hdr) < 0) {
    logger("ioctl failed for disk: %s", target->address);
    close(fd);
    return;
  } 

  logger("finish scsi command");

  // TODO create response text

  close(fd);
}

/*
void iscsi_target_cmd_get_r2t_pdu(byte* command, struct iSCSIConnection* connection, struct iSCSIBuffer* response) {
  struct iSCSISession* session = connection->session_reference;

  int transfer_tag = iscsi_session_next_transfer_tag(session);

  int data_seg_length = scsi_command_data_length(command);
  int exp_data_transfer = scsi_command_exp_data_transfer_length(command);
  int byte_left = exp_data_transfer - data_seg_length;
  int next_offset = data_seg_length;

  if (scsi_command_write(command) && data_seg_length < exp_data_transfer) {
    if (!iscsi_session_is_initalize_r2t(session)) {
      int first_data_pdu_length = min(iscsi_session_first_burst_length(session), exp_data_transfer) - data_seg_length;
      byte_left -= first_data_pdu_length;
      next_offset += first_data_pdu_length;
    } 

    int connection_target_max_receive = iscsi_connection_target_max_receive_data_seg_length(connection);
    int total_r2t = int_ceil(byte_left, connection_target_max_receive);

    int outgoing_r2t = min(iscsi_session_max_outstainding_r2t(session), total_r2t);

    for (int i = 0; i < outgoing_r2t; ++i) {
      // create r2t PDU
      iscsi_request_r2t_send(
        iscsi_command_initiator_task_tag(command),
        transfer_tag,
        i, // r2t sn
        next_offset, // buffer offset
        min(iscsi_connection_target_max_receive_data_seg_length(connection), iscsi_command_expected_data_length(command) - next_offset),
        response // iSCSIBuffer
      );

      next_offset += iscsi_connection_target_max_receive_data_seg_length(connection);
    }

    return;
  }

  if (iscsi_session_is_preceeding_command_pending(session, iscsi_command_cmd_sn(command))) {
    // session.DelayedCommands.Add(command);
    iscsi_command_add_delayed_command(session->delayed_command, command);
  } else {
    // commandsToExecute.Add(command);
    iscsi_command_execute_command(command);
  }
}
*/

/*
void iscsi_target_data_out_get_r2t_pdu(byte* request, struct iSCSIConnection* connection, struct iSCSIBuffer* response) {
  struct iSCSISession* session = connection->session_reference;

  struct iSCSITransferEntry* transfer_entry = NULL;
  if (iscsi_pdu_target_transfer_tag(request) != DEFAULT_TARGET_TRANSFER_TAG) {
    transfer_entry = iscsi_connection_get_transfer_entry(connection, iscsi_pdu_target_transfer_tag(request));
  } else if (!iscsi_session_is_initalize_r2t(session)) {
    transfer_entry = iscsi_connection_get_transfer_entry_using_task_tag(connection, iscsi_pdu_initiator_task_tag(request));
  }

  if (transfer_entry == NULL) {
    // TODO throw error
  }

  int offset = iscsi_pdu_data_out_buffer_offset(request);
  int total_length = iscsi_transfer_entry_expected_data_transfer_length(transfer_entry);

  // TODO store data in buffer
  // Store segment (we only execute the command after receiving all of its data)
  // Array.Copy(request.Data, 0, transfer.Command.Data, offset, request.DataSegmentLength);

  if (offset + iscsi_pdu_data_segment_length(request) == total_length) {
    // last data-out PDU
    iscsi_connection_remove_transfer(
      connection, 
      iscsi_pdu_data_out_initiator_task_tag(request), 
      iscsi_pdu_data_out_target_transfer_tag(request)
    );

    // TODO session.CommandsInTransfer.Remove(transfer.Command.CmdSN);

    int cmd_sn = iscsi_transfer_entry_cmd_sn(transfer_entry);

    if (iscsi_session_is_preceeding_command_pending(session, cmd_sn)) {
      iscsi_delayed_command_add(session->delayed_command, iscsi_trasnfer_entry_command(transfer_entry));
    } else {
      iscsi_command_execute_command(iscsi_trasnfer_entry_command(transfer_entry));
      iscsi_session_execute_command(session);
    }

  } else if (iscsi_pdu_final(request)) {
    if (iscsi_transfer_entry_next_r2t_sn(transfer_entry) < iscsi_transfer_total_r2t_sn(transfer_entry)) {
      int next_offset = iscsi_transfer_entry_next_offset(transfer_entry);
      iscsi_request_r2t_send(
        iscsi_pdu_initiator_task_tag(request),
        iscsi_pdu_target_transfer_tag(request),
        iscsi_transfer_entry_next_r2t_sn(transfer_entry),
        next_offset,
        min(iscsi_connection_target_max_receive_data_seg_length(connection), total_length - next_offset),
        response
      );

      iscsi_transfer_entry_increase_r2t_sn(transfer_entry);
      iscsi_transfer_entry_increase_next_offset(transfer_entry, iscsi_connection_target_max_receive_data_seg_length(connection));
    }
  }
}
*/
