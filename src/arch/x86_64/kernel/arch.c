#include "kernel/arch.h"
#include "drivers/serial.h"
#include "drivers/vga.h"

void idt_init(void);

void panic(void) {
    vga_put_string("Kernel panic\n");
    serial_write_string(COM1, "Kernel panic\n");
}

void arch_init(void) {
    idt_init();
}
