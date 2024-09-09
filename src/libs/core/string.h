#pragma once

#include "base.h"

#include <stdarg.h>

usize int_to_string(char *buffer, int signed_value, uint radix);
usize uint_to_string(char *buffer, uint value, uint radix);
usize string_format(char *buffer, usize buffer_size, const char *fmt, ...);
usize string_format_v(char *buffer, usize buffer_size, const char *fmt, va_list args);
