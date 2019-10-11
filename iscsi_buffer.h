#ifndef __ISCSI_RECEIVE_BUFFER_H__
#define __ISCSI_RECEIVE_BUFFER_H__

#include <string.h>
#include <semaphore.h>

#include "iscsi_type.h"
#include "iscsi_define.h"

struct iSCSIBuffer {
  int length;
  sem_t lock;
  byte data[ISCSI_BUFFER_SIZE];
};

void iscsi_buffer_flush(struct iSCSIBuffer* buffer, int length);
void iscsi_buffer_new(struct iSCSIBuffer* buffer);
int iscsi_buffer_receive(struct iSCSIBuffer* buffer, byte* receive, int length);

void iscsi_buffer_acquire_lock(struct iSCSIBuffer* buffer);
byte* iscsi_buffer_acquire_lock_for_length(struct iSCSIBuffer* buffer, int length);
void iscsi_buffer_release_lock(struct iSCSIBuffer* buffer, int offset);

static inline int iscsi_buffer_length(struct iSCSIBuffer* buffer) {
  return buffer->length;
}

static inline int iscsi_buffer_free_space(struct iSCSIBuffer* buffer) {
  return ISCSI_BUFFER_SIZE - buffer->length;
}

static inline byte* iscsi_buffer_data(struct iSCSIBuffer* buffer) {
  return buffer->data;
}

static inline int iscsi_buffer_data_length(struct iSCSIBuffer* buffer) {
  return buffer->length;
}

#endif // __ISCSI_RECEIVE_BUFFER_H__

