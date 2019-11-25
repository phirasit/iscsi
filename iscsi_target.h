#ifndef __ISCSI_TARGET_H__
#define __ISCSI_TARGET_H__

#include "iscsi_define.h"
#include "iscsi_type.h"

#include <scsi/sg.h>
#include <scsi/scsi.h>

struct iSCSITarget {
  char* address;
  sg_io_hdr_t io_hdr;
  byte buffer[ISCSI_TARGET_MEMORY_SIZE];
  byte sense_buffer[ISCSI_TARGET_SENSE_BUFFER_SIZE];
};

struct iSCSITarget ISCSI_DEFAULT_TARGET;

// inline functions

static inline byte* iscsi_target_buffer(struct iSCSITarget* target) {
  return target->buffer;
}

static inline sg_io_hdr_t* iscsi_target_sg_io_hdr(struct iSCSITarget* target) {
  return &target->io_hdr;
}
// function prototypes

void iscsi_target_create_default(struct iSCSITarget* target);
int iscsi_target_execute_scsi_command(struct iSCSITarget* target, byte* cdb);

// DEPRICATE
// void iscsi_target_cmd_get_r2t_pdu(byte* command, struct iSCSIConnection* connection, struct iSCSIBuffer* response);
// void iscsi_target_data_out_get_r2t_pdu(byte* request, struct iSCSIConnection* connection, struct iSCSIBuffer* response);

#endif // __ISCSI_TARGET_H__
