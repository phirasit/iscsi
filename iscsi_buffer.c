#include <assert.h>
#include "iscsi_buffer.h"
#include "iscsi_logger.h"

#include <string.h>
#include <semaphore.h>
#include <unistd.h>

void iscsi_buffer_new(struct iSCSIBuffer* buffer) {
  // constructor for iSCSIBuffer
  sem_init(&buffer->lock, 0, 1);
}

void iscsi_buffer_acquire_lock(struct iSCSIBuffer* buffer) {
  // logger("[BUFFER] %p: wait for lock\n", buffer);
  sem_wait(&buffer->lock);
  // logger("[BUFFER] %p: lock acquired\n", buffer);
}

byte* iscsi_buffer_acquire_lock_for_length(struct iSCSIBuffer* buffer, int length) {
  if (length > ISCSI_BUFFER_SIZE) {
    return NULL;
  }
  logger("[BUFFER] ask for buffer length = %d\n", length);
start:
  iscsi_buffer_acquire_lock(buffer); 
  if (iscsi_buffer_free_space(buffer) < length) {
    iscsi_buffer_release_lock(buffer, 0);
    usleep(100);
    goto start;
  }
  logger("[BUFFER] buffer length (%d) is allocated\n", length);
  memset(buffer->data + buffer->length, 0, length);
  return buffer->data + buffer->length;
}

void iscsi_buffer_release_lock(struct iSCSIBuffer* buffer, int offset) {
  buffer->length += offset;
  // logger("[BUFFER] %p: lock release with offset %d\n", buffer, offset);
  sem_post(&buffer->lock);
  if (offset) logger("[BUFFER] %p: lock release with offset %d\n", buffer, offset);
}

int iscsi_buffer_receive(struct iSCSIBuffer* buffer, byte* receive, int length) {
  if (length > ISCSI_BUFFER_SIZE) {
    return -1;
  }

  byte* buffer_data = iscsi_buffer_acquire_lock_for_length(buffer, length);
  memcpy(buffer_data, receive, length);
  iscsi_buffer_release_lock(buffer, length);

  return 0;
}

void iscsi_buffer_flush(struct iSCSIBuffer* buffer, int length) {
  assert (length <= buffer->length);
  logger("lock is being asked\n");
  iscsi_buffer_acquire_lock(buffer);
  // logger_hex_array(buffer->data, length);
  logger("lock is acquired\n");
  for (int i = 0; i < buffer->length - length; ++i) {
    buffer->data[i] = buffer->data[i + length];
  }
  // memcpy(buffer->data + length, buffer->data, buffer->length - length);
  iscsi_buffer_release_lock(buffer, -length);
  logger("lock is released\n");
}
