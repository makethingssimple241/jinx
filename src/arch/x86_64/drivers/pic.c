#include "pic.h"
#include "kernel/arch.h"

#define UNUSED_PORT                     0x80

#define PIC_MASTER                      0x20
#define PIC_SLAVE                       0xA0

#define PIC_COMMAND_PORT(port)          (port)
#define PIC_DATA_PORT(port)             (port) + 1

#define PIC_MASTER_REMAP_OFFSET         0x20
#define PIC_SLAVE_REMAP_OFFSET          (0x20 + 8)

typedef enum {
    pic_call_address_interval_8 = false,
    pic_call_address_interval_4 = true,
} pic_call_address_interval;

typedef enum {
    pic_trigger_mode_edge_triggered = false,
    pic_trigger_mode_level_triggered = true,
} pic_trigger_mode;

typedef union {
    struct packed {
        bool icw4_needed                                : 1;
        bool single                                     : 1;
        pic_call_address_interval call_address_interval : 1;
        pic_trigger_mode trigger_mode                   : 1;
        bool is_init                                    : 1;
        u8 only_used_in_mcs_80_85                       : 3;
    };

    u8 data;
} pic_icw1;

typedef enum {
    pic_master_icw3_slave_irq_0 = 1 << 0,
    pic_master_icw3_slave_irq_1 = 1 << 1,
    pic_master_icw3_slave_irq_2 = 1 << 2,
    pic_master_icw3_slave_irq_3 = 1 << 3,
    pic_master_icw3_slave_irq_4 = 1 << 4,
    pic_master_icw3_slave_irq_5 = 1 << 5,
    pic_master_icw3_slave_irq_6 = 1 << 6,
    pic_master_icw3_slave_irq_7 = 1 << 7,
} pic_master_icw3_slave_irq;

/// @brief Which IRQ line the slave is connected to the slave
typedef enum {
    pic_slave_icw3_cascade_irq_0 = 0,
    pic_slave_icw3_cascade_irq_1 = 1,
    pic_slave_icw3_cascade_irq_2 = 2,
    pic_slave_icw3_cascade_irq_3 = 3,
    pic_slave_icw3_cascade_irq_4 = 4,
    pic_slave_icw3_cascade_irq_5 = 5,
    pic_slave_icw3_cascade_irq_6 = 6,
    pic_slave_icw3_cascade_irq_7 = 7,
} pic_slave_icw3_cascade;

typedef enum {
    pic_mode_mcs_80_85 = false,
    pic_mode_8086 = true,
} pic_mode;

typedef enum {
    pic_buffering_mode_non_buffered     = 0b00,
    pic_buffering_mode_master_buffered  = 0b10,
    pic_buffering_mode_slave_buffered   = 0b11,
} pic_buffering_mode;

typedef union {
    struct packed {
        pic_mode mode                       : 1;
        bool auto_end_of_interrupt          : 1;
        pic_buffering_mode buffering_mode   : 2;
        bool fully_nested_mode              : 1;
        u8 unused                           : 3;
    };

    u8 data;
} pic_icw4;

#define PIC_DISABLE             0xFF

#define PIC_END_OF_INTERRUPT    0x20

static void wait(void) {
    out_byte(UNUSED_PORT, 0);
}

void pic_init(bool auto_end_of_interrupt) {
    // ICW1
    pic_icw1 icw1 = {0};
    icw1.icw4_needed = true;
    icw1.is_init = true;

    out_byte(PIC_COMMAND_PORT(PIC_MASTER), icw1.data);
    wait();
    out_byte(PIC_COMMAND_PORT(PIC_SLAVE), icw1.data);
    wait();

    // ICW2
    out_byte(PIC_DATA_PORT(PIC_MASTER), PIC_MASTER_REMAP_OFFSET);
    wait();
    out_byte(PIC_DATA_PORT(PIC_SLAVE), PIC_SLAVE_REMAP_OFFSET);
    wait();

    // ICW3
    // Tell master that slave is at IRQ 2
    out_byte(PIC_DATA_PORT(PIC_MASTER), pic_master_icw3_slave_irq_2);
    wait();
    // Tell slave that its master connects to the slave through IRQ 2
    out_byte(PIC_DATA_PORT(PIC_SLAVE), pic_slave_icw3_cascade_irq_2);
    wait();

    // ICW4
    pic_icw4 icw4 = {0};
    icw4.mode = pic_mode_8086;
    icw4.auto_end_of_interrupt = auto_end_of_interrupt;

    out_byte(PIC_DATA_PORT(PIC_MASTER), icw4.data);
    wait();
    out_byte(PIC_DATA_PORT(PIC_SLAVE), icw4.data);
    wait();
}

void pic_deinit(void) {
    out_byte(PIC_DATA_PORT(PIC_MASTER), PIC_DISABLE);
    out_byte(PIC_DATA_PORT(PIC_SLAVE), PIC_DISABLE);
}

void pic_send_end_of_interrupt(u8 irq) {
    // Signal the slave PIC if it involved IRQs that require the slave to handle
    if (irq >= 8) {
        out_byte(PIC_COMMAND_PORT(PIC_SLAVE), PIC_END_OF_INTERRUPT);
    }

    out_byte(PIC_COMMAND_PORT(PIC_MASTER), PIC_END_OF_INTERRUPT);
}
