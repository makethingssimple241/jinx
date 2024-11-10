#include "string.h"

usize int_to_string(char *buffer, usize buffer_size, int signed_value, uint radix) {
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

    used += u64_to_string(buffer ? buffer + used : null, buffer_size, value, radix);
    return used;
}

usize u64_to_string(char *buffer, usize buffer_size, u64 value, uint radix) {
    char scratch_buffer[16];
    usize used = 0;

    if (value == 0) {
        scratch_buffer[used++] = '0';
    }

    while (value) {
        u64 digit = value % radix;
        value /= radix;
        scratch_buffer[used++] = digit >= 10 ? digit - 10 + 'a' : digit + '0';
    }

    if (buffer && used < buffer_size) {
        for (usize i = 0; i < used; i++) {
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

typedef enum {
    string_format_int_length_default,
    string_format_int_length_64bit,
} string_format_int_length;

static string_format_int_length get_string_format_int_length(const char **fmt) {
    char next = (*fmt)[0];
    if (next) {
        char next2 = (*fmt)[1];

        if (next == '6' && next2 == '4') {
            *fmt += 2;
            return string_format_int_length_64bit;
        }
    }

    return string_format_int_length_default;
}

usize string_format_v(char *buffer, usize buffer_size, const char *fmt, va_list args) {
    usize used = 0; 
    char c;

    while ((c = *fmt)) {
        ++fmt;

        if (c != '%') {
            if (buffer && used < buffer_size - 1) {
                buffer[used++] = c;
            } else {
                ++used;
            }
            
            continue;
        }

        c = *(fmt++);

        size_t remaining_buffer_size = used < buffer_size - 1 ? buffer_size - used : 0;

        switch (c) {
        case 'd':
        case 'i':
            used += int_to_string(buffer && used < buffer_size - 1 ? buffer + used : null, remaining_buffer_size, va_arg(args, int), 10);
            break;
        case 's': {
            const char *s = va_arg(args, const char *);
            char c = '\0';

            while ((c = *s++)) {
                if (buffer && used < buffer_size - 1) {
                    buffer[used] = c;
                }

                ++used;
            }
        } break;
        case 'b':
        case 'u':
        case 'x': {
            char *buffer_ptr = buffer && used < buffer_size - 1 ? buffer + used : null;
            uint radix = 10;

            switch (c) {
            case 'b':
                radix = 2;
                break;
            case 'o':
                radix = 8;
            case 'u':
                radix = 10;
                break;
            case 'x':
                radix = 16;
                break;
            default:
                break;
            }

            switch (get_string_format_int_length(&fmt)) {
            case string_format_int_length_default:
                used += u64_to_string(buffer_ptr, remaining_buffer_size, va_arg(args, uint), radix);
                break;
            case string_format_int_length_64bit:
                used += u64_to_string(buffer_ptr, remaining_buffer_size, va_arg(args, u64), radix);
                break;
            default:
                break;
            }
        } break;
        case '\0':
        case '%':
            if (buffer && used < buffer_size - 1) {
                buffer[used++] = '%';
            } else {
                ++used;
            }

            break;
        default:
            if (buffer && used < buffer_size - 1) {
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
            buffer[used++] = '\0';
        } else {
            buffer[buffer_size - 1] = '\0';
            ++used;
        }
    }

    return used;
}
