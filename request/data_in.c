#include "data_in.h"

void iscsi_request_data_in_send(struct iSCSIConnection* connection, struct iSCSITarget* target, struct iSCSIBuffer* resopnse) {
  // TODO create data-in PDU for read operations
  struct iSCSIConnectionParameter* parameter = iscsi_connection_parameter(connection);
  int max_receive_length = iscsi_connection_parameter_max_receive_data_segment_length(parameter);
  int total_length = 0; // TODO this
  int num_pdu = (total_length + max_receive_length - 1) / max_receive_length;

  for (int i = 0; i < num_pdu; ++i) {
  }
}
