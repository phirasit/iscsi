#ifndef __ISCSI_CONNECTION_PARAMETER_H__
#define __ISCSI_CONNECTION_PARAMETER_H__

#include "iscsi_type.h"

struct iSCSIConnection;

void iscsi_connection_parameter_update(struct iSCSIConnection* connection, byte* key, byte* value);
byte* iscsi_connection_parameter_get(struct iSCSIConnection* connection, byte* key);
int iscsi_connection_parameter_length(struct iSCSIConnection* connection);

int iscsi_connection_parameter_vendor_specific(struct iSCSIConnection* connection);
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
#endif // __ISCSI_CONNECTION_PARAMETER_H__
