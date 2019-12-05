#include "request/login.h"

#include "iscsi_pdu.h"
#include "iscsi_logger.h"

extern struct iSCSISession ISCSI_DEFAULT_SESSION;
extern struct iSCSITarget ISCSI_DEFAULT_TARGET;

static inline int iscsi_pdu_login_transit(byte* buffer) {
  return buffer[1] >> 7;
}
static inline int iscsi_pdu_login_continue(byte* buffer) {
  return (buffer[1] >> 6) & 0x1;
}
static inline enum LOGIN_STATE iscsi_pdu_login_csg(byte* buffer) {
  return (buffer[1] >> 2) & 0x3;
}
static inline enum LOGIN_STATE iscsi_pdu_login_nsg(byte* buffer) {
  return buffer[1] & 0x3;
}
static inline long long iscsi_pdu_login_ISID(byte* buffer) {
  return ((long long) buffer[8] << 40)
    | ((long long) buffer[ 9] << 32)
    | ((long long) buffer[10] << 24)
    | ((long long) buffer[11] << 16)
    | ((long long) buffer[12] <<  8)
    | ((long long) buffer[13]      );
}
static inline int iscsi_pdu_login_TSIH(byte* buffer) {
  return ((int) buffer[14] << 8) | buffer[15];
}
static inline int iscsi_pdu_login_CID(byte* buffer) {
  return ((int) buffer[20] << 8) | buffer[21];
}
static inline int iscsi_pdu_login_ExpCmdSN(byte* buffer) {
  return ((int) buffer[28] << 24)
    | ((int) buffer[29] << 16)
    | ((int) buffer[30] <<  8)
    | ((int) buffer[31]      );
}

// setter

static inline void iscsi_pdu_login_set_TSIH(byte* buffer, int TSIH) {
  buffer[14] = (TSIH >> 8) & 0xFF;
  buffer[15] = (TSIH     ) & 0xFF;
}

static inline void iscsi_pdu_login_set_response_ExpCmdSN(byte* buffer, int exp_cmd_sn) {
  buffer[28] = (exp_cmd_sn >> 24) & 0xFF;
  buffer[29] = (exp_cmd_sn >> 16) & 0xFF;
  buffer[30] = (exp_cmd_sn >>  8) & 0xFF;
  buffer[31] = (exp_cmd_sn      ) & 0xFF;
}

static inline void iscsi_pdu_login_set_response_status(byte* buffer, enum LOGIN_STATUS status) {
  buffer[36] = (status >> 8) & 0xFF;
  buffer[37] = (status     ) & 0xFF;
}

// session startup function

static enum LOGIN_STATUS setup_normal_session(byte* _request, struct iSCSISession* session) {
  session->is_discovery = 0;

  /*
  // TODO multiple targets support
  struct iSCSITarget* target = iscsi_target_mangaer_find(
    iscsi_connection_parameter_get("TargetName")
  );
  */
  struct iSCSITarget* target = &ISCSI_DEFAULT_TARGET;
  
  if (target == NULL) {
    return NOT_FOUND;
  }

  /*
  // TODO target authentication
  if (!iscsi_target_authenticate(target)) {
    return AUTHENTICATION_ERROR;
  }
  */

  session->target = target;

  return SUCCESS;
}

static enum LOGIN_STATUS setup_session(byte* request, struct iSCSIConnection* connection) {
  struct iSCSIConnectionParameter* parameter = iscsi_connection_parameter(connection);
  if (!iscsi_connection_parameter_get(parameter, "InitiatorName")) {
    logger("[LOGIN] No InitiatorName in the parameter\n");
    return INITIATOR_ERROR;
  }

  if (iscsi_pdu_login_TSIH(request) == 0) {
    /*
    // TODO create a new session for a new connection
    // right now just use the pre-defined session
    iscsi_session_new(connection->session_reference, iscsi_pdu_login_ISID(request));
    */
    iscsi_session_create(&ISCSI_DEFAULT_SESSION);
    connection->session_reference = &ISCSI_DEFAULT_SESSION;
    // setup_normal_session(request, connection->session_reference);

    // connection->session->CommandNumberStart = 1
    connection->session_reference->ExpCmdSN = iscsi_pdu_login_ExpCmdSN(request);
    connection->connection_id = iscsi_pdu_login_CID(request);

    if (strcmp(iscsi_connection_parameter_get(parameter, "SessionType"), "Discovery") == 0) {
      connection->session_reference->is_discovery = 1;
    } else {
      if (iscsi_connection_parameter_get(parameter, "TargetName") != NULL) {
        // TODO fix this
        return setup_normal_session(request, connection->session_reference);
      } else {
        return INITIATOR_ERROR;
      }
    }
  } else {

    /*
    // TODO search iscsi session
    struct iSCSISession* session = iscsi_session_manager_search(
      login_ISID(request),
      iscsi_pdu_login_TSIH(request)
    );
    */
    struct iSCSISession* session = &ISCSI_DEFAULT_SESSION;

    /*
    // TODO check session ISID and TSIH
    if (iscsi_session_ISID(session) != login_ISID(request) || iscsi_session_TSIH(session) != iscsi_pdu_login_TSIH(request)) {
      return SESSION_NOT_EXIST;
    }
    */

    if (iscsi_session_full(session)) {
      return TOO_MANY_CONNECTION;
    }

    iscsi_session_add_connection(session, connection);
    connection->connection_id = iscsi_pdu_login_CID(request);
  }

  return SUCCESS;
}

static int response_partial_login(byte* request, struct iSCSIConnection* connection, struct iSCSIBuffer* response) {
  logger("[LOGIN] Entering partial login\n");
  byte* buffer = iscsi_buffer_acquire_lock_for_length(response, BASIC_HEADER_SEGMENT_LENGTH);
  iscsi_pdu_generate_from_buffer(buffer, request);
  iscsi_pdu_set_response_header(buffer, connection);
  iscsi_pdu_set_opcode(buffer, LOGIN_RES);
  iscsi_pdu_set_final(buffer, 0);
  if (iscsi_pdu_login_transit(request)) {
    iscsi_pdu_login_set_response_status(buffer, INITIATOR_ERROR);
  } else {
    iscsi_pdu_login_set_response_status(buffer, SUCCESS);
  }
  iscsi_buffer_release_lock(response, BASIC_HEADER_SEGMENT_LENGTH);
  return BASIC_HEADER_SEGMENT_LENGTH;
}

static int response_final_with_parameters(struct iSCSIBuffer* response, byte* request, struct iSCSIConnection* connection, enum LOGIN_STATUS status, byte* parameter, int length) {
  int total_packet_size = BASIC_HEADER_SEGMENT_LENGTH + length;
  int total_padded_size = iscsi_pdu_padded_length(total_packet_size);

  byte* buffer = iscsi_buffer_acquire_lock_for_length(response, total_padded_size);

  iscsi_pdu_generate_from_buffer(buffer, request);
  iscsi_pdu_set_response_header(buffer, connection);
  iscsi_pdu_set_bit(buffer, 1, 0);
  iscsi_pdu_set_opcode(buffer, LOGIN_RES);
  iscsi_pdu_set_final(buffer, 1);
  iscsi_pdu_login_set_response_status(buffer, status);
  iscsi_pdu_login_set_TSIH(buffer, connection->session_reference->TSIH);
  iscsi_pdu_set_data_segment_length(buffer, length);
  iscsi_pdu_pad0(buffer, total_packet_size);

  memcpy(
    iscsi_pdu_data(buffer), 
    parameter,
    length
  );

  iscsi_buffer_release_lock(response, total_padded_size);
  return total_padded_size;
}

static int response_final_login(byte* request, struct iSCSIConnection* connection, struct iSCSIBuffer* response) {
  static byte security_negotiation_response[] = "AuthMethod=None";
  enum LOGIN_STATUS status = SUCCESS;
  struct iSCSIConnectionParameter* parameter = iscsi_connection_parameter(connection);
  byte* buffer;

  logger("[LOGIN] Final with pdu_login_csg: %d\n", iscsi_pdu_login_csg(request));
  switch (iscsi_pdu_login_csg(request)) {
    case SECURITY_NEGOTIATION:
      if (iscsi_pdu_login_transit(request)) {
        if (iscsi_pdu_login_nsg(request) == FULL_FEATURE) {
          connection->session_reference->is_full_feature_phase = 1;
          status = SUCCESS;
        } else if (iscsi_pdu_login_transit(request) != SECURITY_NEGOTIATION) {
          status = INITIATOR_ERROR;
        }
      }

      return response_final_with_parameters(
        response,
        request,
        connection,
        status,
        security_negotiation_response,
        strlen(security_negotiation_response)
      );

    case LOGIN_OPERATIONAL:

      if (iscsi_pdu_login_transit(request)) {
        if (iscsi_pdu_login_nsg(request) == FULL_FEATURE) {
          connection->session_reference->is_full_feature_phase = 1;
          logger("[LOGIN] now session has now enter full feature phase\n");
        } else {
          status = INITIATOR_ERROR;
        }
      }

      iscsi_connection_parameter_generate_operational_data(parameter);

      return response_final_with_parameters(
        response,
        request,
        connection,
        status,
        iscsi_connection_parameter_data(parameter),
        iscsi_connection_parameter_length(parameter)
      );

    case FULL_FEATURE:
      buffer = iscsi_buffer_acquire_lock_for_length(response, BASIC_HEADER_SEGMENT_LENGTH);
      iscsi_pdu_generate_from_buffer(buffer, request);
      iscsi_pdu_set_opcode(buffer, LOGIN_RES);
      iscsi_pdu_login_set_response_status(buffer, INITIATOR_ERROR);
      iscsi_buffer_release_lock(response, BASIC_HEADER_SEGMENT_LENGTH);
      return BASIC_HEADER_SEGMENT_LENGTH;
  }

  return -1; // IMPOSSIBLE
}

// functions

int iscsi_request_login_reject(byte* request, struct iSCSIConnection* connection, struct iSCSIBuffer* response) {
  if (connection->session_reference == NULL) {
    return PDU_ERROR;
  }

  byte* buffer = iscsi_buffer_acquire_lock_for_length(response, BASIC_HEADER_SEGMENT_LENGTH);
  iscsi_pdu_set_opcode(buffer, LOGIN_RES);
  iscsi_pdu_login_set_TSIH(buffer, connection->session_reference->TSIH);
  iscsi_buffer_release_lock(response, BASIC_HEADER_SEGMENT_LENGTH);

  return BASIC_HEADER_SEGMENT_LENGTH;
}

int iscsi_request_login_process(byte* request, struct iSCSIConnection* connection, struct iSCSIBuffer* response) {
  logger("[LOGIN] Start login process\n");

  // update parameter 
  iscsi_connection_parameter_create(
    iscsi_connection_parameter(connection),
    iscsi_pdu_data(request),
    iscsi_pdu_data_segment_length(request)
  );

  if (iscsi_pdu_login_continue(request)) {
    return response_partial_login(request, connection, response);
  } else {
    if (connection->session_reference == NULL) {
      enum LOGIN_STATUS status = setup_session(request, connection);
      if (status != SUCCESS) {
        byte* buffer = iscsi_buffer_acquire_lock_for_length(response, BASIC_HEADER_SEGMENT_LENGTH);
        iscsi_pdu_generate_from_buffer(buffer, request);
        iscsi_pdu_set_opcode(buffer, LOGIN_RES);
        iscsi_pdu_login_set_response_status(buffer, status);
        iscsi_buffer_release_lock(response, BASIC_HEADER_SEGMENT_LENGTH);
        return BASIC_HEADER_SEGMENT_LENGTH;
      }
    }

    return response_final_login(request, connection, response);
  }
}

