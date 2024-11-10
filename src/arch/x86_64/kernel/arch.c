#include "idt.h"
#include "kernel/arch.h"
#include "../drivers/pic.h"
#include "drivers/serial.h"
#include "drivers/gop.h"

void panic(const char *message) {
    gop_put_string("\nKernel panic - ");
    serial_write_string(COM1, "\nKernel panic - ");
    gop_put_string(message);
    serial_write_string(COM1, message);
    gop_put_character('\n');
    serial_write_character(COM1, '\n');

    disable_interrupts();

    while (true) {
        halt();
    }
}

void arch_init(void) {
    disable_interrupts();
    
    idt_init();

    pic_init(true);
    serial_write_string(COM1, "PIC initialized\n");
    
    enable_interrupts();
}
