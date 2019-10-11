#include "iscsi_buffer.h"
#include "logger.h"

#include <semaphore.h>
#include <unistd.h>

void iscsi_buffer_new(struct iSCSIBuffer* buffer) {
  // constructor for iSCSIBuffer
  sem_init(&buffer->lock, 0, 1);
}

void iscsi_buffer_acquire_lock(struct iSCSIBuffer* buffer) {
  logger("buffer %p: wait for lock\n", buffer);
  sem_wait(&buffer->lock);
  logger("buffer %p: lock acquired\n", buffer);
}

byte* iscsi_buffer_acquire_lock_for_length(struct iSCSIBuffer* buffer, int length) {
  if (length > ISCSI_BUFFER_SIZE) {
    return NULL;
  }
start:
  iscsi_buffer_acquire_lock(buffer); 
  if (iscsi_buffer_free_space(buffer) < length) {
    iscsi_buffer_release_lock(buffer, 0);
    sleep(50);
    goto start;
  }
  return buffer->data + buffer->length;
}

void iscsi_buffer_release_lock(struct iSCSIBuffer* buffer, int offset) {
  buffer->length += offset;
  sem_post(&buffer->lock);
  logger("buffer %p: lock release\n", buffer);
}

int iscsi_buffer_receive(struct iSCSIBuffer* buffer, byte* receive, int length) {
  if (length > ISCSI_BUFFER_SIZE) {
    return -1;
  }

  byte* ptr = iscsi_buffer_acquire_lock_for_length(buffer, length);
  memcpy(ptr, receive, length);
  iscsi_buffer_release_lock(buffer, length);

  logger_hex_array(ptr, length);

  return 0;
}

void iscsi_buffer_flush(struct iSCSIBuffer* buffer, int length) {
  iscsi_buffer_acquire_lock(buffer);
  logger_hex_array(buffer->data, length);
  memcpy(buffer->data, buffer->data + length, buffer->length - length);
  iscsi_buffer_release_lock(buffer, -length);
}
