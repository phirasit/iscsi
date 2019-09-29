#include "iscsi_buffer.h"

void create_new_iscsi_buffer(struct iSCSIBuffer* buffer) {
  // constructor for iSCSIBuffer
}

int iscsi_buffer_receive(struct iSCSIBuffer* buffer, byte* receive, int length) {
  if (buffer->length + length < ISCSI_BUFFER_SIZE) {
    memcpy(buffer->data + buffer->length, receive, length);
    buffer->length += length;
    return 0;
  } else {
    return -1;
  }
}

void iscsi_buffer_flush(struct iSCSIBuffer* buffer, int length) {
  memcpy(buffer->data, buffer->data + length, ISCSI_BUFFER_SIZE - length);
  buffer->length -= length;
}
