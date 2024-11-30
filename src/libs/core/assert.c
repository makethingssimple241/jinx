#include "assert.h"
#include "string.h"

#include <stdarg.h>

void panic(const char *message);

void assert(bool condition, const char *fmt, ...) {
    if (condition) {
        return;
    }

    va_list args;
    va_start(args, fmt);
    char message[256];
    string_format_v(message, sizeof(message), fmt, args);
    va_end(args);

    panic(message);
}
