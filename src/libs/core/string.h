#pragma once

#include "base.h"

#include <stdarg.h>

usize int_to_string(char *buffer, usize buffer_size, int signed_value, uint radix);
usize u64_to_string(char *buffer, usize buffer_size, u64 value, uint radix);
usize string_format(char *buffer, usize buffer_size, const char *fmt, ...);
usize string_format_v(char *buffer, usize buffer_size, const char *fmt, va_list args);
