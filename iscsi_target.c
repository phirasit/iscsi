#include "iscsi_target.h"

#include "iscsi_buffer.h"
#include "iscsi_connection.h"
#include "iscsi_define.h"
#include "iscsi_pdu.h"

#include <stdlib.h>

#include <sys/ioctl.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "iscsi_logger.h"

void iscsi_target_create_default(struct iSCSITarget* target) {
  target->address = ISCSI_DEFAULT_TARGET_PATH;
  logger("[TARGET] opening %s\n", target->address);
  target->fd = open(target->address, O_RDWR);
  if (target->fd < 0) {
    logger("[TARGET] Cannot open: %s\n", target->address);
    exit(1);
  }
}

int iscsi_target_execute_scsi_command(struct iSCSITarget* target, byte* cdb) {
  
  memset(&target->io_hdr, 0, sizeof(sg_io_hdr_t));
  target->io_hdr = (sg_io_hdr_t) {
    .interface_id          = 'S',
    .dxfer_direction       = SG_DXFER_TO_FROM_DEV, // temporary
    .cmd_len               = SCSI_CDB_LENGTH,
    .mx_sb_len             = ISCSI_TARGET_SENSE_BUFFER_SIZE,
    .iovec_count           = 0,
    .dxfer_len             = ISCSI_TARGET_MEMORY_SIZE, // the length of the data transfer
    .dxferp                = (void*) target->buffer,
    .cmdp                  = cdb,
    .sbp                   = (void*) target->sense_buffer,
    .timeout               = ISCSI_TARGET_TIMEOUT_MS,
    .flags                 = SG_FLAG_DIRECT_IO,
  };

  if (ioctl(target->fd, SG_IO, &target->io_hdr) < 0) {
    logger("[TARGET] ioctl failed in disk: %s\n", target->address);
    return 1;
  } 

  logger("[TARGET] finish scsi command with status = %d\n", target->io_hdr.status);
  
  return 0;
}
