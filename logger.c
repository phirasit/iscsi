#include "logger.h"

#define BYTE_PER_LINE 16

int logger(char* format, ...) {
   va_list arg;
   int done;

   va_start (arg, format);
   done = vfprintf (stderr, format, arg);
   va_end (arg);

   return done;
}

void logger_hex_array(byte* buffer, int length) {
  for (int i = 0; i < length; ++i) {
    if (i % BYTE_PER_LINE == 0) logger("\n");
    logger("%02x ", buffer[i]);
  }
  logger("\n");
}
