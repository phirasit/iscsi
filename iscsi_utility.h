#ifndef __ISCSI_UTILITY_H__
#define __ISCSI_UTILITY_H__

#include "iscsi_type.h"

int divide_round_up(int a, int b);
int min(int a, int b);

int max(int a, int b);

int string_to_int(byte* s);

byte* int_to_string(int s);

#endif // __ISCSI_UTILITY_H__