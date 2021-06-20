#ifndef log_h
#define log_h

#include <Arduino.h>

#define log_printf(...) _log_printf(__VA_ARGS__)

const size_t _log_printf(const char *format, ...);

#endif
