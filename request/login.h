#ifndef __ISCSI_REQUEST_LOGIN_H__
#define __ISCSI_REQUEST_LOGIN_H__

#include "iscsi_connection.h"
#include "iscsi_pdu.h"
#include "iscsi_server.h"
#include "iscsi_session.h"
#include "iscsi_type.h"

#include "target/iscsi_target.h"

#include "logger.h"

extern struct iSCSISession ISCSI_DEFAULT_SESSION;
extern struct iSCSITarget ISCSI_DEFAULT_TARGET;

enum LOGIN_STATUS {
  SUCCESS = 0x0000,
  TARGET_MOVED_TEMPORARILY = 0x0101,
  TARGET_MOVED_PERMANENTLY = 0x0102,
  INITIATOR_ERROR = 0x0200,
  AUTHENTICATION_ERROR = 0x0201,
  AUTHORIZATION_ERROR = 0x0202,
  NOT_FOUND = 0x0203,
  TARGET_MOVED = 0x0204,
  UNSUPPORTED_VERSION = 0x0205,
  TOO_MANY_CONNECTION = 0x0206,
  MISSING_PARAMETER = 0x0207,
  CANT_INCLUDE_SESSION = 0x0208,
  SESSION_NOT_SUPPORTED = 0x0209,
  SESSION_NOT_EXIST = 0x020A,
  INVALID_DURING_LOGIN = 0x020B,
  TARGET_ERROR = 0x0300,
  SERVICE_UNAVAILABLE = 0x0301,
  OUT_OF_RESOURCE = 0x0302
};

enum LOGIN_STATE {
  SECURITY_NEGOTIATION = 0,
  LOGIN_OPERATIONAL = 1,
  FULL_FEATURE = 3
};

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
  logger("setup session\n");
  if (!iscsi_connection_parameter_get(&connection->parameter, "InitiatorName")) {
    logger("no InitiatorName\n");
    return INITIATOR_ERROR;
  }

  if (iscsi_pdu_login_TSIH(request) == 0) {
    /*
    // TODO create a new session for a new connection
    // right now just use the pre-defined session
    iscsi_session_new(connection->session_reference, iscsi_pdu_login_ISID(request));
    */
    connection->session_reference = &ISCSI_DEFAULT_SESSION;
    // setup_normal_session(request, connection->session_reference);

    // connection->session->CommandNumberStart = 1
    connection->session_reference->ExpCmdSN = iscsi_pdu_login_ExpCmdSN(request);
    connection->connection_id = iscsi_pdu_login_CID(request);

    if (strcmp(iscsi_connection_parameter_get(&connection->parameter, "SessionType"), "Discovery") == 0) {
      connection->session_reference->is_discovery = 1;
    } else {
      if (iscsi_connection_parameter_get(&connection->parameter, "TargetName") != NULL) {
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
  logger("partial login\n");
  byte* buffer = iscsi_buffer_acquire_lock_for_length(response, BASIC_HEADER_SEGMENT_LENGTH);
  iscsi_pdu_generate_from_buffer(buffer, request);
  iscsi_pdu_set_opcode(buffer, LOGIN_RES);
  iscsi_pdu_set_final(buffer, 0);
  // iscsi_pdu_login_set_response_ExpCmdSN(response, iscsi_pdu_login_ExpCmdSN(request));
  if (iscsi_pdu_login_transit(request)) {
    iscsi_pdu_login_set_response_status(buffer, INITIATOR_ERROR);
  } else {
    iscsi_pdu_login_set_response_status(buffer, SUCCESS);
  }
  iscsi_buffer_release_lock(response, BASIC_HEADER_SEGMENT_LENGTH);
  return BASIC_HEADER_SEGMENT_LENGTH;
}

static int response_with_request_parameters(byte* request, struct iSCSIConnection* connection, enum LOGIN_STATUS status, struct iSCSIBuffer* response) {
  // create respose packet
  int parameter_length = iscsi_connection_parameter_length(&connection->parameter);
  int total_packet_size = BASIC_HEADER_SEGMENT_LENGTH + parameter_length;

  byte* buffer = iscsi_buffer_acquire_lock_for_length(response, total_packet_size);
  iscsi_pdu_generate_from_buffer(buffer, request);
  iscsi_pdu_login_set_response_status(buffer, status);
  iscsi_pdu_set_data_segment_length(buffer, parameter_length);
  iscsi_buffer_release_lock(response, total_packet_size);

  return total_packet_size;
}

static int response_final_login(byte* request, struct iSCSIConnection* connection, struct iSCSIBuffer* response) {
  enum LOGIN_STATUS status = SUCCESS;
  byte* ptr;
  switch (iscsi_pdu_login_csg(request)) {
    case SECURITY_NEGOTIATION:
      if (iscsi_pdu_login_transit(request)) {
        if (iscsi_pdu_login_nsg(request) == FULL_FEATURE) {
          connection->session_reference->is_full_feature_phase = 1;
          status = SUCCESS;
        } else if (iscsi_pdu_login_transit(request) != SECURITY_NEGOTIATION) {
          // iscsi_pdu_login_set_response_status(response, INITIATOR_ERROR);
          status = INITIATOR_ERROR;
        }
      }

      // TODO copy parameters to response package

      return response_with_request_parameters(request, connection, status, response);

    case LOGIN_OPERATIONAL:
      if (iscsi_pdu_login_transit(request)) {
        if (iscsi_pdu_login_nsg(request) == FULL_FEATURE) {
          connection->session_reference->is_full_feature_phase = 1;
        } else {
          // iscsi_pdu_login_set_response_status(response, INITIATOR_ERROR);
          status = INITIATOR_ERROR;
        }
      }

      /*
      // TODO update parameters & copy to response package
      iscsi_connection_parameter_update(
        connection, 
        iscsi_pdu_data(request), 
        iscsi_pdu_data_segment_length(request)
      );
      */

      return response_with_request_parameters(request, connection, status, response);

    case FULL_FEATURE:
      ptr = iscsi_buffer_acquire_lock_for_length(response, BASIC_HEADER_SEGMENT_LENGTH);
      iscsi_pdu_generate_from_buffer(ptr, request);
      iscsi_pdu_login_set_response_status(ptr, INITIATOR_ERROR);
      iscsi_buffer_release_lock(response, BASIC_HEADER_SEGMENT_LENGTH);
      return BASIC_HEADER_SEGMENT_LENGTH;
  }

  return -1; // IMPOSSIBLE
}

int iscsi_request_login_reject(byte* request, struct iSCSIConnection* connection, struct iSCSIBuffer* response) {
  if (connection->session_reference == NULL) {
    return PDU_ERROR;
  }

  byte* ptr = iscsi_buffer_acquire_lock_for_length(response, BASIC_HEADER_SEGMENT_LENGTH);
  iscsi_pdu_set_opcode(ptr, LOGIN_RES);
  iscsi_pdu_login_set_TSIH(ptr, connection->session_reference->TSIH);
  iscsi_buffer_release_lock(response, BASIC_HEADER_SEGMENT_LENGTH);

  return BASIC_HEADER_SEGMENT_LENGTH;
}

int iscsi_request_login_process(byte* request, struct iSCSIConnection* connection, struct iSCSIBuffer* response) {
  logger("login process\n");

  iscsi_connection_parameter_create(
    &connection->parameter, 
    iscsi_pdu_data(request),
    iscsi_pdu_data_segment_length(request)
  );

  if (iscsi_pdu_login_continue(request)) {
    return response_partial_login(request, connection, response);
  } else {
    if (connection->session_reference == NULL) {
      enum LOGIN_STATUS status = setup_session(request, connection);
      logger("setup session status %d\n", status);
      if (status != SUCCESS) {
        byte* buffer = iscsi_buffer_acquire_lock_for_length(response, BASIC_HEADER_SEGMENT_LENGTH);
        iscsi_pdu_generate_from_buffer(buffer, request);
        iscsi_pdu_set_opcode(buffer, LOGIN_RES);
        iscsi_pdu_set_final(buffer, 1);
        iscsi_pdu_login_set_response_status(buffer, status);
        iscsi_buffer_release_lock(response, BASIC_HEADER_SEGMENT_LENGTH);
        return BASIC_HEADER_SEGMENT_LENGTH;
      }
    }

    return response_final_login(request, connection, response);
  }
}

#endif // __ISCSI_REQUEST_LOGIN_H__
