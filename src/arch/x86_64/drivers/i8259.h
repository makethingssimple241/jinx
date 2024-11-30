/// @brief Driver for legacy programmable interrupt controller (PIC) i8259

#pragma once

#include "core/base.h"

void i8259_init(bool auto_end_of_interrupt);
void i8259_deinit(void);
void i8259_send_end_of_interrupt(u8 irq);
