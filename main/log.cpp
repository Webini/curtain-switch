#include "log.h"

const size_t _log_printf(const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  char temp[64];
  char* buffer = temp;
  size_t len = vsnprintf(temp, sizeof(temp), format, arg);
  va_end(arg);
  if (len > sizeof(temp) - 1) {
    buffer = new char[len + 1];
    if (!buffer) {
      return 0;
    }
    va_start(arg, format);
    vsnprintf(buffer, len + 1, format, arg);
    va_end(arg);
  }
  Serial.print("["); 
  Serial.print(millis()); 
  Serial.print("]");
  Serial.println(buffer);
  if (buffer != temp) {
    //free(buffer);
    delete[] buffer;
  }
  return len;
}
