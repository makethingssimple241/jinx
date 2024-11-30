#include "i8259.h"
#include "kernel/arch.h"

#define UNUSED_PORT                     0x80

#define i8259_MASTER                    0x20
#define i8259_SLAVE                     0xA0

#define i8259_COMMAND_PORT(port)        (port)
#define i8259_DATA_PORT(port)           (port) + 1

#define i8259_MASTER_REMAP_OFFSET       0x20
#define i8259_SLAVE_REMAP_OFFSET        (0x20 + 8)

typedef enum {
    i8259_call_address_interval_8 = false,
    i8259_call_address_interval_4 = true,
} i8259_call_address_interval;

typedef enum {
    i8259_trigger_mode_edge_triggered = false,
    i8259_trigger_mode_level_triggered = true,
} i8259_trigger_mode;

typedef union {
    struct packed {
        bool icw4_needed                                : 1;
        bool single                                     : 1;
        i8259_call_address_interval call_address_interval : 1;
        i8259_trigger_mode trigger_mode                   : 1;
        bool is_init                                    : 1;
        u8 only_used_in_mcs_80_85                       : 3;
    };

    u8 data;
} i8259_icw1;

typedef enum {
    i8259_master_icw3_slave_irq_0 = 1 << 0,
    i8259_master_icw3_slave_irq_1 = 1 << 1,
    i8259_master_icw3_slave_irq_2 = 1 << 2,
    i8259_master_icw3_slave_irq_3 = 1 << 3,
    i8259_master_icw3_slave_irq_4 = 1 << 4,
    i8259_master_icw3_slave_irq_5 = 1 << 5,
    i8259_master_icw3_slave_irq_6 = 1 << 6,
    i8259_master_icw3_slave_irq_7 = 1 << 7,
} i8259_master_icw3_slave_irq;

/// @brief Which IRQ line the slave is connected to the slave
typedef enum {
    i8259_slave_icw3_cascade_irq_0 = 0,
    i8259_slave_icw3_cascade_irq_1 = 1,
    i8259_slave_icw3_cascade_irq_2 = 2,
    i8259_slave_icw3_cascade_irq_3 = 3,
    i8259_slave_icw3_cascade_irq_4 = 4,
    i8259_slave_icw3_cascade_irq_5 = 5,
    i8259_slave_icw3_cascade_irq_6 = 6,
    i8259_slave_icw3_cascade_irq_7 = 7,
} i8259_slave_icw3_cascade;

typedef enum {
    i8259_mode_mcs_80_85 = false,
    i8259_mode_8086 = true,
} i8259_mode;

typedef enum {
    i8259_buffering_mode_non_buffered     = 0b00,
    i8259_buffering_mode_master_buffered  = 0b10,
    i8259_buffering_mode_slave_buffered   = 0b11,
} i8259_buffering_mode;

typedef union {
    struct packed {
        i8259_mode mode                     : 1;
        bool auto_end_of_interrupt          : 1;
        i8259_buffering_mode buffering_mode : 2;
        bool fully_nested_mode              : 1;
        u8 unused                           : 3;
    };

    u8 data;
} i8259_icw4;

#define i8259_DISABLE             0xFF

#define i8259_END_OF_INTERRUPT    0x20

static void wait(void) {
    out_byte(UNUSED_PORT, 0);
}

void i8259_init(bool auto_end_of_interrupt) {
    // ICW1
    i8259_icw1 icw1 = {0};
    icw1.icw4_needed = true;
    icw1.is_init = true;

    out_byte(i8259_COMMAND_PORT(i8259_MASTER), icw1.data);
    wait();
    out_byte(i8259_COMMAND_PORT(i8259_SLAVE), icw1.data);
    wait();

    // ICW2
    out_byte(i8259_DATA_PORT(i8259_MASTER), i8259_MASTER_REMAP_OFFSET);
    wait();
    out_byte(i8259_DATA_PORT(i8259_SLAVE), i8259_SLAVE_REMAP_OFFSET);
    wait();

    // ICW3
    // Tell master that slave is at IRQ 2
    out_byte(i8259_DATA_PORT(i8259_MASTER), i8259_master_icw3_slave_irq_2);
    wait();
    // Tell slave that its master connects to the slave through IRQ 2
    out_byte(i8259_DATA_PORT(i8259_SLAVE), i8259_slave_icw3_cascade_irq_2);
    wait();

    // ICW4
    i8259_icw4 icw4 = {0};
    icw4.mode = i8259_mode_8086;
    icw4.auto_end_of_interrupt = auto_end_of_interrupt;

    out_byte(i8259_DATA_PORT(i8259_MASTER), icw4.data);
    wait();
    out_byte(i8259_DATA_PORT(i8259_SLAVE), icw4.data);
    wait();
}

void i8259_deinit(void) {
    out_byte(i8259_DATA_PORT(i8259_MASTER), i8259_DISABLE);
    out_byte(i8259_DATA_PORT(i8259_SLAVE), i8259_DISABLE);
}

void i8259_send_end_of_interrupt(u8 irq) {
    // Signal the slave PIC if it involved IRQs that require the slave to handle
    if (irq >= 8) {
        out_byte(i8259_COMMAND_PORT(i8259_SLAVE), i8259_END_OF_INTERRUPT);
    }

    out_byte(i8259_COMMAND_PORT(i8259_MASTER), i8259_END_OF_INTERRUPT);
}
