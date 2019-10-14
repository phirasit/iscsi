#include <string.h>

#include "iscsi_connection_parameter.h"
#include "logger.h"

enum CONNECTION_PARAMETER_SYMBOL {
  EQUAL,
  KEY,
  SEPARATOR,
  VALUE,
};

static inline enum CONNECTION_PARAMETER_SYMBOL next_key(enum CONNECTION_PARAMETER_SYMBOL symbol, byte b) {
  if (b == '=') return EQUAL;
  if (b == ',' || b == '\0') return SEPARATOR;
  if (symbol == EQUAL) return VALUE;
  if (symbol == SEPARATOR) return KEY;
  return symbol;
}

static inline void iscsi_connection_parameter_write(struct iSCSIConnectionParameter* parameter, byte* data) {
  int length = strlen(data);
  memcpy(parameter->buffer + parameter->length, data, length);
  parameter->length += length;
}

void iscsi_connection_parameter_create(struct iSCSIConnectionParameter* parameter, byte* data, int length) {
  if (length > ISCSI_CONNECTION_PARAMETER_BUFFER_SIZE) {
    length = ISCSI_CONNECTION_PARAMETER_BUFFER_SIZE;
  }
  memcpy(parameter->buffer, data, length);
  parameter->length = length;
}

void iscsi_connection_parameter_update(struct iSCSIConnectionParameter* parameter, byte* key, byte* value) {
  if (parameter->length > 0) {
    iscsi_connection_parameter_write(parameter, "\0");
  }
  iscsi_connection_parameter_write(parameter, key);
  iscsi_connection_parameter_write(parameter, "=");
  iscsi_connection_parameter_write(parameter, value);
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
