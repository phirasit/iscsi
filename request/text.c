#include "request/text.h"

#include <stdio.h>
#include <string.h>

#include "iscsi_byte.h"
#include "iscsi_pdu.h"
#include "iscsi_session.h"

#include "logger.h"

// TODO change to proper function
static byte* int_to_byte(int val) {
  static byte int_value[20];
  snprintf(int_value, 20, "%d", val);
  return int_value;
}

static inline int iscsi_pdu_text_continue(byte* buffer) {
  return iscsi_byte_bit(buffer[2], 1);
}

static inline int iscsi_pdu_text_initiator_task_tag(byte* buffer) {
  return iscsi_byte_byte2int(buffer + 16);
}

int iscsi_request_text_process(byte* request, struct iSCSIConnection* connection, struct iSCSIBuffer* response) {
  struct iSCSISession* session = connection->session_reference;
  struct iSCSIConnectionParameter* parameter = iscsi_connection_get_parameter(connection);

  logger("TEXT REQUEST\n");

  // pdu is not the last pdu
  if (iscsi_pdu_text_continue(request)) {
    // TODO update the parameter and wait for a new one
    // TargetName={target_name}
    /*
    iscsi_connection_parameter_update(&connection->parameter, 
      int_to_byte(iscsi_pdu_text_initiator_task_tag(request)), 
      iscsi_pdu_data(request)
    );
    */

    logger("has continue");

    byte* buffer = iscsi_buffer_acquire_lock_for_length(response, BASIC_HEADER_SEGMENT_LENGTH);

    iscsi_pdu_generate_from_buffer(buffer, request);
    iscsi_pdu_set_opcode(buffer, TEXT_RES);
    iscsi_pdu_set_final(buffer, 1);
    iscsi_buffer_release_lock(response, BASIC_HEADER_SEGMENT_LENGTH);

    return BASIC_HEADER_SEGMENT_LENGTH;
  }

  if (iscsi_parameter_get(
    iscsi_pdu_data(request),
    iscsi_pdu_data_segment_length(request),
    "SendTargets"
  ) != NULL) {
    // return all targets
    // TODO use real value
    static byte response_text[] = "TargetName=iqn.2019-10.chula.ac.th:iscsi-disk.sn.12345";
    static int text_length = STRLEN(response_text);

    int length = BASIC_HEADER_SEGMENT_LENGTH + text_length;
    int padded_length = iscsi_pdu_padded_length(length);

    logger("send all targets: %s(%d)", response_text, text_length);

    byte* buffer = iscsi_buffer_acquire_lock_for_length(response, padded_length);

    iscsi_pdu_generate_from_buffer(buffer, request);
    iscsi_pdu_set_opcode(buffer, TEXT_RES);
    iscsi_pdu_set_final(buffer, 1);

    iscsi_pdu_set_data_segment_length(buffer, text_length);
    memcpy(iscsi_pdu_data(buffer), response_text, text_length);
    iscsi_pdu_pad0(buffer, length);

    iscsi_buffer_release_lock(response, padded_length);
    return padded_length;

  } else if (session->is_discovery || !iscsi_connection_parameter_vendor_specific(parameter)) {
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

    // iscsi_pdu_set_data_segment_length(buffer, length);
  }
 
  return 0;
}
