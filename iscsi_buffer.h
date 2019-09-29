#ifndef __ISCSI_RECEIVE_BUFFER_H__
#define __ISCSI_RECEIVE_BUFFER_H__

#include <string.h>

#include "iscsi_type.h"
#include "iscsi_define.h"

struct iSCSIBuffer {
  byte data[ISCSI_BUFFER_SIZE];
  int length;
};

void create_new_iscsi_buffer(struct iSCSIBuffer* buffer);

int iscsi_buffer_receive(
  struct iSCSIBuffer* buffer, 
  byte* receive,
  int length
);

void iscsi_buffer_flush(struct iSCSIBuffer* buffer, int length);

static inline int iscsi_buffer_free_space(struct iSCSIBuffer* buffer) {
  return ISCSI_BUFFER_SIZE - buffer->length;
}

#endif // __ISCSI_RECEIVE_BUFFER_H__

