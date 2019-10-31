#include "iscsi_connection_parameter.h"

#include <stdlib.h>
#include <string.h>

#define MAX_RECEIVE_DATA_SEGEMENT   "MaxRecvDataSegmentLength"
#define MAX_CONNECTIONS             "MaxConnections"
#define INITIAL_R2T                 "InitialR2T"
#define IMMEDIATE_DATA              "ImmediateData"
#define MAX_BURST_LENGTH            "MaxBurstLength"
#define FIRST_BURST_LENGTH          "FirstBurstLength"
#define DATA_PDU_IN_ORDER           "DataPDUInOrder"
#define DATA_SEQUENCE_IN_ORDER      "DataSequenceInOrder"
#define DEFAULT_TIME_2_WAIT         "DefaultTime2Wait"
#define DEFAULT_TIME_2_RETAIN       "DefaultTime2Retain"
#define MAX_OUTSTANDING_R2T         "MaxOutstandingR2T"


enum CONNECTION_PARAMETER_SYMBOL {
  EQUAL,
  KEY,
  SEPARATOR,
  VALUE,
};

static int key_equal(byte* key, byte* prop) {
  int length = strlen(prop);
  for (int i = 0; i < length; ++i) {
    if ((*key++) != (*prop++)) {
      return 0;
    }
  }
  return 1;
}

static inline int string_to_int(byte* s) {
  return atoi((char*) s);
}

static inline byte* int_to_string(int s) {
  static byte buffer[20];
  if (s == 0) {
    buffer[0] = '0';
    buffer[1] = 0;
  } else {
    int i = s, len = 0;
    while (i) {
      ++len;
      i /= 10;
    }
    buffer[len] = 0;
    while (s) {
      buffer[--len] = s % 10 + '0';
      s /= 10;
    }
  }
  return buffer;
}

static byte* bool_to_string(int b) {
  return b ? "YES" : "NO";
}

static int min(int a, int b) {
  return a < b ? a : b;
}

static int max(int a, int b) {
  return a > b ? a : b;
}

static void set_default_parameter(struct iSCSIConnectionParameter* parameter) {
  parameter->max_connections          = ISCSI_CONNECTION_PARAMETER_MAX_CONNECTIONS;
  parameter->initial_r2t              = ISCSI_CONNECTION_PARAMETER_INITIAL_R2T;
  parameter->immediate_data           = ISCSI_CONNECTION_PARAMETER_IMMEDIATE_DATA;
  parameter->max_burst_length         = ISCSI_CONNECTION_PARAMETER_MAX_BURST_LENGTH;
  parameter->first_burst_length       = ISCSI_CONNECTION_PARAMETER_FIRST_BURST_LENGTH;
  parameter->data_pdu_in_order        = ISCSI_CONNECTION_PARAMETER_DATA_PDU_IN_ORDER;
  parameter->data_sequence_in_order   = ISCSI_CONNECTION_PARAMETER_DATA_SEQUENCE_IN_ORDER;
  parameter->default_time_2_wait      = ISCSI_CONNECTION_PARAMETER_DEFAULT_TIME_2_WAIT;
  parameter->default_time_2_retain    = ISCSI_CONNECTION_PARAMETER_DEFAULT_TIME_2_RETAIN;
  parameter->max_outstanding_r2t      = ISCSI_CONNECTION_PARAMETER_MAX_OUTSTANDING_R2T;
}

static inline void iscsi_connection_parameter_write(struct iSCSIConnectionParameter* parameter, byte* data) {
  int length = strlen(data);
  memcpy(parameter->buffer + parameter->length, data, length);
  parameter->length += length;
}

static void update_information(struct iSCSIConnectionParameter* parameter, byte* key, byte* value) {
  if (key_equal(key, MAX_RECEIVE_DATA_SEGEMENT)) {
    parameter->max_receive_data_segment_length = string_to_int(value);
  }
  if (key_equal(key, MAX_CONNECTIONS)) {
    parameter->max_connections = min(string_to_int(value), ISCSI_CONNECTION_PARAMETER_MAX_CONNECTIONS);
  }
  if (key_equal(key, INITIAL_R2T)) {
    parameter->initial_r2t = key_equal(value, "Yes") || ISCSI_CONNECTION_PARAMETER_INITIAL_R2T;
  }
  if (key_equal(key, IMMEDIATE_DATA)) {
    parameter->immediate_data = key_equal(value, "Yes") || ISCSI_CONNECTION_PARAMETER_IMMEDIATE_DATA;
  }
  if (key_equal(key, MAX_BURST_LENGTH)) {
    parameter->max_burst_length = min(string_to_int(value), ISCSI_CONNECTION_PARAMETER_MAX_BURST_LENGTH);
  }
  if (key_equal(key, FIRST_BURST_LENGTH)) {
    parameter->first_burst_length = min(string_to_int(value), ISCSI_CONNECTION_PARAMETER_FIRST_BURST_LENGTH);
  }
  if (key_equal(key, DATA_PDU_IN_ORDER)) {
    parameter->data_pdu_in_order = key_equal(value, "Yes") || ISCSI_CONNECTION_PARAMETER_DATA_PDU_IN_ORDER;
  }
  if (key_equal(key, DATA_SEQUENCE_IN_ORDER)) {
    parameter->data_sequence_in_order = key_equal(value, "Yes") || ISCSI_CONNECTION_PARAMETER_DATA_SEQUENCE_IN_ORDER;
  }
  if (key_equal(key, DEFAULT_TIME_2_WAIT)) {
    parameter->default_time_2_wait = max(string_to_int(value), ISCSI_CONNECTION_PARAMETER_DEFAULT_TIME_2_WAIT);
  }
  if (key_equal(key, DEFAULT_TIME_2_RETAIN)) {
    parameter->default_time_2_retain = max(string_to_int(value), ISCSI_CONNECTION_PARAMETER_DEFAULT_TIME_2_RETAIN);
  }
  if (key_equal(key, MAX_OUTSTANDING_R2T)) {
    parameter->max_outstanding_r2t = min(string_to_int(value), ISCSI_CONNECTION_PARAMETER_MAX_OUTSTANDING_R2T);
  }
}

void iscsi_connection_parameter_generate_operational_data(struct iSCSIConnectionParameter* parameter) {
  parameter->length = 0;
  iscsi_connection_parameter_update(parameter, MAX_CONNECTIONS,        int_to_string(iscsi_connection_parameter_max_connections(parameter)));
  iscsi_connection_parameter_update(parameter, INITIAL_R2T,            bool_to_string(iscsi_connection_parameter_initial_r2t(parameter)));
  iscsi_connection_parameter_update(parameter, IMMEDIATE_DATA,         bool_to_string(iscsi_connection_parameter_immeidate_data(parameter)));
  iscsi_connection_parameter_update(parameter, MAX_BURST_LENGTH,       int_to_string(iscsi_connection_parameter_max_burst_length(parameter)));
  iscsi_connection_parameter_update(parameter, FIRST_BURST_LENGTH,     int_to_string(iscsi_connection_parameter_first_burst_length(parameter)));
  iscsi_connection_parameter_update(parameter, DATA_PDU_IN_ORDER,      bool_to_string(iscsi_connection_parameter_data_pdu_in_order(parameter)));
  iscsi_connection_parameter_update(parameter, DATA_SEQUENCE_IN_ORDER, bool_to_string(iscsi_connection_parameter_data_sequence_in_order(parameter)));
  iscsi_connection_parameter_update(parameter, DEFAULT_TIME_2_WAIT,    int_to_string(iscsi_connection_parameter_default_time_2_wait(parameter)));
  iscsi_connection_parameter_update(parameter, DEFAULT_TIME_2_RETAIN,  int_to_string(iscsi_connection_parameter_default_time_2_retain(parameter)));
  iscsi_connection_parameter_update(parameter, MAX_OUTSTANDING_R2T,    int_to_string(iscsi_connection_parameter_max_outstanding_r2t(parameter)));
}

void iscsi_connection_parameter_create(struct iSCSIConnectionParameter* parameter, byte* data, int length) {
  if (length > ISCSI_CONNECTION_PARAMETER_BUFFER_SIZE) {
    length = ISCSI_CONNECTION_PARAMETER_BUFFER_SIZE;
  }

  memcpy(parameter->buffer, data, length);
  parameter->length = length;

  set_default_parameter(parameter);

  byte* key = data;
  for (int i = 1; i < length; ++i) {
    if (data[i-1] == 0) {
      key = data + i;
    }
    if (data[i] == EQUAL) {
      update_information(parameter, key, data + i + 1);
    }
  }
}

static inline enum CONNECTION_PARAMETER_SYMBOL next_key(enum CONNECTION_PARAMETER_SYMBOL symbol, byte b) {
  if (b == '=') return EQUAL;
  if (b == ',' || b == '\0') return SEPARATOR;
  if (symbol == EQUAL) return VALUE;
  if (symbol == SEPARATOR) return KEY;
  return symbol;
}

void iscsi_connection_parameter_update(struct iSCSIConnectionParameter* parameter, byte* key, byte* value) {
  if (parameter->length > 0) {
    iscsi_connection_parameter_write(parameter, "\0");
  }
  iscsi_connection_parameter_write(parameter, key);
  iscsi_connection_parameter_write(parameter, "=");
  iscsi_connection_parameter_write(parameter, value);
  update_information(parameter, key, value);
}

byte* iscsi_parameter_get(byte* data, int length, byte* key) {
  enum CONNECTION_PARAMETER_SYMBOL symbol = KEY;
  int count = 0, still_valid = 1;

  for (int i = 0; i < length; ++i) {
    symbol = next_key(symbol, data[i]);

    if (symbol == EQUAL && still_valid) {
      return data + i+1;
    }

    if (symbol == SEPARATOR) {
      count = 0;
      still_valid = 1;
    }

    if (symbol == KEY) {
      if (data[i] == key[count]) {
        ++count;
      } else {
        still_valid = 0;
      }
    }
  }
  return NULL;
}

byte* iscsi_connection_parameter_get(struct iSCSIConnectionParameter* parameter, byte* key) {
  return iscsi_parameter_get(parameter->buffer, parameter->length, key);
}

byte* iscsi_connection_parameter_data(struct iSCSIConnectionParameter* parameter) {
  return parameter->buffer;
}

int iscsi_connection_parameter_length(struct iSCSIConnectionParameter* parameter) {
  return parameter->length;
}


static inline int iscsi_connection_parameter_is_vendor_specific(byte* key) {
  return key[0] == 'X' && (key[1] == '-' || key[1] == '#');
}

int iscsi_connection_parameter_vendor_specific(struct iSCSIConnectionParameter* parameter) {
  if (parameter->length > 1 && iscsi_connection_parameter_is_vendor_specific(parameter->buffer)) {
    return 0;
  }

  for (int i = 0; i < parameter->length - 2; ++i) {
    if (parameter->buffer[i] == ',' && iscsi_connection_parameter_is_vendor_specific(parameter->buffer + i + 1)) {
      return 0;
    }
  }

  return 1;
}

/*
foreach(string key in requestParameters.Keys)
{
  // RFC 3720: Implementers may introduce new keys by prefixing them with "X-" [..] or X# if registered with IANA.
  if (!(key.StartsWith("X-") || key.StartsWith("X#")))
  {
    return false;   
  }
}
return (requestParameters.Count > 0);
*/
