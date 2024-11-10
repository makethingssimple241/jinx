#pragma once

#include "core/base.h"

void pic_init(bool auto_end_of_interrupt);
void pic_deinit(void);
void pic_send_end_of_interrupt(u8 irq);
