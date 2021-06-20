#ifndef log_h
#define log_h

#include <Arduino.h>
#include "parameters.h"

#ifdef DEBUG
  #define log_printf(...) _log_printf(__VA_ARGS__)
#else
  #define log_printf(...)((void) 0)
#endif

const size_t _log_printf(const char *format, ...);

#endif
