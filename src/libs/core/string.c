#include "string.h"

usize int_to_string(char *buffer, int signed_value, uint radix) {
    usize used = 0;
    bool negative = radix == 10 && signed_value < 0;
    unsigned int value;

    if (negative) {
        if (buffer) {
            buffer[used++] = '-';
        }
        value = -signed_value;
    } else {
        value = signed_value;
    }

    used += uint_to_string(buffer ? buffer + used : null, value, radix);
    return used;
}

usize uint_to_string(char *buffer, uint value, uint radix) {
    char scratch_buffer[16];
    u32 used = 0;

    if (value == 0) {
        scratch_buffer[used++] = '0';
    }

    while (value) {
        u32 digit = value % radix;
        value /= radix;
        scratch_buffer[used++] = digit >= 10 ? digit - 10 + 'a' : digit + '0';
    }

    if (buffer) {
        for (u32 i = 0; i < used; i++) {
            buffer[i] = scratch_buffer[used - 1 - i];
        }
    }

    return used;
}

usize string_format(char *buffer, usize buffer_size, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    usize used = string_format_v(buffer, buffer_size, fmt, args);
    va_end(args);
    return used;
}

usize string_format_v(char *buffer, usize buffer_size, const char *fmt, va_list args) {
    usize used = 0; 
    char c;

    while ((c = *fmt)) {
        ++fmt;

        if (c != '%') {
            if (buffer && used < buffer_size) {
                buffer[used++] = c;
            } else {
                ++used;
            }
            
            continue;
        }

        c = *(fmt++);

        switch (c) {
        case 'd':
        case 'i':
            used += int_to_string(buffer && used < buffer_size ? buffer + used : null, va_arg(args, int), 10);
            break;
        case 'u':
            used += uint_to_string(buffer && used < buffer_size ? buffer + used : null, va_arg(args, uint), 10);
            break;
        case 'x':
            used += uint_to_string(buffer && used < buffer_size ? buffer + used : null, va_arg(args, uint), 16);
            break;
        case '\0':
        case '%':
            if (buffer && used < buffer_size) {
                buffer[used++] = '%';
            } else {
                ++used;
            }

            break;
        default:
            if (buffer && used < buffer_size) {
                buffer[used++] = '%';
                buffer[used++] = c;
            } else {
                used += 2;
            }

            break;
        }
    }

    if (buffer) {
        if (used < buffer_size) {
            buffer[used] = '\0';
        } else {
            buffer[buffer_size - 1] = '\0';
        }
    }
    return used;
}
