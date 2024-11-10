#pragma once

#include "core/base.h"

void out_byte(u16 port, byte b);
byte in_byte(u16 port);
void enable_interrupts(void);
void disable_interrupts(void);
void halt(void);

noreturn void panic(const char *message);

void arch_init(void);
