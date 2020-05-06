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
#include <errno.h>

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

void iscsi_target_execute_scsi_command(struct iSCSITarget* target, byte* cdb) {

  logger("[TARGET] start sending cdb to disk\n");

  memset(&target->io_hdr, 0, sizeof(sg_io_hdr_t));
  target->io_hdr = (sg_io_hdr_t) {
    .interface_id          = 'S',
    .cmd_len               = SCSI_CDB_LENGTH,
    .mx_sb_len             = ISCSI_TARGET_SENSE_BUFFER_SIZE,
    .iovec_count           = 0,
    .dxfer_direction       = SG_DXFER_FROM_DEV,
    .dxfer_len             = ISCSI_TARGET_MEMORY_SIZE, // the length of the data transfer
    .dxferp                = (void*) target->buffer,
    .cmdp                  = cdb,
    .sbp                   = (void*) target->sense_buffer,
    .timeout               = ISCSI_TARGET_TIMEOUT_MS,
    .flags                 = SG_FLAG_DIRECT_IO,
    // .flags                 = SG_FLAG_LUN_INHIBIT,
  };

  if (ioctl(target->fd, SG_IO, &target->io_hdr) < 0) {
    logger("[TARGET] ioctl failed in disk: %s\n", target->address);
    logger("[TARGET] the error is %s\n", strerror(errno));
    target->scsi_response = 1;
    return;
  } 

  if ((target->io_hdr.info & SG_INFO_OK_MASK) != SG_INFO_OK) {
    if (target->io_hdr.sb_len_wr > 0) {
      printf("INQUIRY sense data: ");
      for (int k = 0; k < target->io_hdr.sb_len_wr; ++k) {
          if ((k > 0) && (0 == (k % 10)))
            printf("\n  ");
          printf("0x%02x ", target->sense_buffer[k]);
      }
      printf("\n");
    }
    if (target->io_hdr.masked_status)
      printf("INQUIRY SCSI status=0x%x\n", target->io_hdr.status);
    if (target->io_hdr.host_status)
      printf("INQUIRY host_status=0x%x\n", target->io_hdr.host_status);
    if (target->io_hdr.driver_status)
      printf("INQUIRY driver_status=0x%x\n", target->io_hdr.driver_status);
  }

  logger("[TARGET] finish scsi command with status = %d\n", target->io_hdr.status);

  target->scsi_response = 0;
}
