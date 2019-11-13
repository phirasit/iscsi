#ifndef __ISCSI_LOGGER_H__
#define __ISCSI_LOGGER_H__

#include <stdio.h>
#include <stdarg.h>

#include "iscsi_type.h"

int logger(char* format, ...);
void logger_hex_array(byte* buffer, int length);

#endif // __ISCSI_LOGGER_H__