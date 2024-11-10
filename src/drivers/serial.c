#include "serial.h"
#include "kernel/arch.h"

#define SERIAL_DIVISOR_LOW_WHEN_DLAB_OPEN(port)     (port) + 0
#define SERIAL_DIVISOR_HIGH_WHEN_DLAB_OPEN(port)    (port) + 1
#define SERIAL_FIFO_CONTROL_REGISTER(port)          (port) + 2
#define SERIAL_LINE_CONTROL_REGISTER(port)          (port) + 3
#define SERIAL_MODEM_CONTROL_REGISTER(port)         (port) + 4
#define SERIAL_LINE_STATUS_REGISTER(port)           (port) + 5

#define SERIAL_TEST                                 0xAE

typedef enum {
    character_length_5  = 0b00,
    character_length_6  = 0b01,
    character_length_7  = 0b10,
    character_length_8  = 0b11,
} character_length;

typedef enum {
    stop_bit_1          = 0,
    stop_bit_1_5_or_2   = 1
} stop_bit;

typedef enum {
    parity_bit_none     = 0b0,
    parity_bit_odd      = 0b001,
    parity_bit_even     = 0b011,
    parity_bit_mark     = 0b101,
    parity_bit_space    = 0b111
} parity_bit;

typedef union {
    packed struct {
        character_length character_length   : 2;
        stop_bit stop_bit                   : 1;
        parity_bit parity_bit               : 3;
        bool break_enable_bit               : 1;
        bool divisor_latch_access_bit       : 1;
    };
    u16 data;
} line_control_register;

typedef enum {
    interrupt_trigger_level_1_byte      = 0b00,
    interrupt_trigger_level_4_bytes     = 0b01,
    interrupt_trigger_level_8_bytes     = 0b10,
    interrupt_trigger_level_14_bytes    = 0b11,
} interrupt_trigger_level;

typedef union {
    packed struct {
        bool enable_fifo                                : 1;
        bool clear_receive_fifo                         : 1;
        bool clear_transmit_fifo                        : 1;
        bool dma_mode_select                            : 1;
        u8 reserved                                     : 2;
        interrupt_trigger_level interrupt_trigger_level : 2;
    };
    u16 data;
} fifo_control_register;

typedef union {
    packed struct {
        bool data_terminal_ready    : 1;
        bool request_to_send        : 1;
        bool unused_in_pc           : 1;
        bool enable_irq_in_pc       : 1;
        bool loop                   : 1;
        u8 unused                   : 3;
    };
    u16 data;
} modem_control_register;

typedef union {
    packed struct {
        bool data_ready                         : 1;
        bool overun_error                       : 1;
        bool parity_error                       : 1;
        bool framing_error                      : 1;
        bool break_indicator                    : 1;
        /// @brief Whether data can be sent
        bool transmitter_holding_register_empty : 1;
        bool transmitter_empty                  : 1;
        bool impending_error                    : 1;
    };
    u16 data;
} line_status_register;

static bool serial_can_send_data(u16 port) {
    line_status_register line_status;
    line_status.data = in_byte(SERIAL_LINE_STATUS_REGISTER(port));
    return line_status.transmitter_holding_register_empty;
}

bool serial_init(const serial_init_info *info) {
    if (info->port != COM1 && info->port != COM2) {
        return false;
    }

    line_control_register line_control = {0};

    // Enable the divisor latch access bit (DLAB)
    line_control.divisor_latch_access_bit = true;
    out_byte(SERIAL_LINE_CONTROL_REGISTER(info->port), line_control.data);

    // Send the least significant byte of the divisor value to the port
    out_byte(SERIAL_DIVISOR_LOW_WHEN_DLAB_OPEN(info->port), info->divisor >> 8);

    // Send the most significant byte of the divisor value to the port
    out_byte(SERIAL_DIVISOR_HIGH_WHEN_DLAB_OPEN(info->port), info->divisor);

    // Disable the DLAB
    line_control.character_length = character_length_8;
    line_control.divisor_latch_access_bit = false;
    out_byte(SERIAL_LINE_CONTROL_REGISTER(info->port), line_control.data);

    fifo_control_register fifo_control = {0};
    fifo_control.enable_fifo = true;
    fifo_control.clear_receive_fifo = true;
    fifo_control.clear_transmit_fifo = true;
    fifo_control.interrupt_trigger_level = interrupt_trigger_level_14_bytes;
    out_byte(SERIAL_FIFO_CONTROL_REGISTER(info->port), line_control.data);

    modem_control_register modem_control = {0};

    // Enable loopback mode for testing
    modem_control.request_to_send = true;
    modem_control.unused_in_pc = true;
    modem_control.enable_irq_in_pc = true;
    modem_control.loop = true;
    out_byte(SERIAL_MODEM_CONTROL_REGISTER(info->port), modem_control.data);

    // Send a test byte which we will recieve because loop bit is on
    out_byte(info->port, SERIAL_TEST);

    // If what is sent is not being sent back, the serial port is faulty
    if (in_byte(info->port) != SERIAL_TEST) {
        return false;
    }

    // Disable loopback mode
    modem_control.data_terminal_ready = true;
    modem_control.loop = false;
    out_byte(SERIAL_MODEM_CONTROL_REGISTER(info->port), modem_control.data);

    return true;
}

void serial_write_character(u16 port, char c) {
    while (!serial_can_send_data(port));
    out_byte(port, c);
}

void serial_write_string(u16 port, const char *s) {
    char c;
    while ((c = *s)) {
        ++s;
        serial_write_character(port, c);
    }
}
