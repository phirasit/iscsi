#include "iscsi_transfer_entry.h"

#include <string.h>

void iscsi_transfer_entry_create(
  struct iSCSITransferEntry* entry,
  byte* cdb,
  int total_length,
  int max_receive_data_segment,
  int transfer_tag
) {
  memcpy(iscsi_transfer_entry_cdb(entry), cdb, SCSI_CDB_LENGTH);
  entry->total_length = total_length;
  entry->transfer_tag = transfer_tag;
}

