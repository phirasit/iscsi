#ifndef __ISCSI_TRANSFER_ENTRY_H__
#define __ISCSI_TRANSFER_ENTRY_H__

#include "iscsi_define.h"
#include "iscsi_type.h"

struct iSCSITransferEntry {
  byte data[ISCSI_TRANSFER_ENTRY_SIZE];
  byte cdb[SCSI_CDB_LENGTH];
  int total_length;
  int offset;
  int r2t_sn;
  int total_r2t_sn;

  int transfer_tag;
};

static inline byte* iscsi_transfer_entry_data(struct iSCSITransferEntry* entry) {
  return entry->data;
}

static inline int iscsi_transfer_entry_data_length(struct iSCSITransferEntry* entry) {
  return entry->total_length;
}

static inline byte* iscsi_transfer_entry_cdb(struct iSCSITransferEntry* entry) {
  return entry->cdb;
}

static inline int iscsi_transfer_entry_next_r2t_sn(struct iSCSITransferEntry* entry) {
  return entry->r2t_sn; 
}

static inline int iscsi_transfer_entry_total_r2t_sn(struct iSCSITransferEntry* entry) {
  return entry->total_r2t_sn;
}

static inline int iscsi_transfer_entry_next_offset(struct iSCSITransferEntry* entry) {
  return entry->offset * entry->r2t_sn;
}

static inline void iscsi_transfer_entry_increase_r2t_sn(struct iSCSITransferEntry* entry) {
  ++entry->r2t_sn;
}

static inline int iscsi_transfer_entry_transfer_tag(struct iSCSITransferEntry* entry) {
  return entry->transfer_tag;
}

void iscsi_transfer_entry_create(
  struct iSCSITransferEntry* entry,
  byte* cdb,
  int total_length,
  int max_receive_data_segment,
  int transfer_tag
);

#endif // __ISCSI_TRANSFER_ENTRY_H__
