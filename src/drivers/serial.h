#pragma once

#include <core/base.h>

#define COM1    0x3F8
#define COM2    0x2F8

typedef struct {
    u16 port;
    u16 divisor;
} serial_init_info;

bool serial_init(const serial_init_info *info);
void serial_write_character(u16 port, char c);
void serial_write_string(u16 port, const char *s);
