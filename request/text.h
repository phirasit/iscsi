#ifndef __ISCSI_REQUEST_TEXT_H__
#define __ISCSI_REQUEST_TEXT_H__

#include <stdio.h>

#include "iscsi_buffer.h"
#include "iscsi_byte.h"
#include "iscsi_connection.h"
#include "iscsi_session.h"
#include "iscsi_type.h"
#include "iscsi_pdu.h"

// TODO change to proper function
byte int_value[20];
static byte* int_to_byte(int val) {
  snprintf(int_value, 20, "%d", val);
  return int_value;
}

static inline int iscsi_pdu_text_continue(byte* buffer) {
  return iscsi_byte_bit(buffer[2], 6);
}

static inline int iscsi_pdu_text_initiator_task_tag(byte* buffer) {
  return iscsi_byte_byte2int(buffer + 16);
}

int iscsi_request_text_process(byte* request, struct iSCSIConnection* connection, struct iSCSIBuffer* response) {
  struct iSCSISession* session = connection->session_reference;

  byte* ptr = iscsi_buffer_acquire_lock_for_length(response, BASIC_HEADER_SEGMENT_LENGTH);

  iscsi_pdu_generate_from_buffer(ptr, request);
  iscsi_pdu_set_final(ptr, 1);

  // pdu is not the last pdu
  if (iscsi_pdu_text_continue(request)) {
    // update the parameter and wait for a new one
    // TargetName={target_name}
    iscsi_connection_parameter_update(&connection->parameter, 
      int_to_byte(iscsi_pdu_text_initiator_task_tag(request)), 
      iscsi_pdu_data(request)
    );

    iscsi_buffer_release_lock(response, BASIC_HEADER_SEGMENT_LENGTH);

    return BASIC_HEADER_SEGMENT_LENGTH;
  }

  if (iscsi_connection_parameter_get(&connection->parameter, "SendTargets") != NULL) {
    // TODO: add target to parameter list
  } else if (session->is_discovery || !iscsi_connection_parameter_vendor_specific(&connection->parameter)) {
    // vendor specific: start with "X-" or "X#"
    // TODO: add reject target to response data
    // {target_name}=Reject
  } else {
    // send information from the target
    /*
    int length = iscsi_target_get_text_response(
      session->target, 
      iscsi_pdu_data(request), // TODO check this
      length - BASIC_HEADER_SEGMENT_LENGTH
    ); 
    */
    int length = 0;

    if (length < 0) {
      iscsi_buffer_release_lock(response, length);
      return length;
    }

    iscsi_pdu_set_data_segment_length(ptr, length);
  }
 
  iscsi_buffer_release_lock(response, iscsi_pdu_length(ptr));
  return iscsi_pdu_length(ptr);
}

#endif // __ISCSI_REQUEST_TEXT_H__
