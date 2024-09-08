#include <core/base.h>
#include <core/string.h>
#include <drivers/serial.h>
#include <drivers/vga.h>

// MARK: Bootloader

#include <limine.h>

__attribute__((used, section(".requests_start_marker")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".requests")))
static volatile LIMINE_BASE_REVISION(2);

__attribute__((used, section(".requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

__attribute__((used, section(".requests_end_marker")))
static volatile LIMINE_REQUESTS_END_MARKER;

// MARK: Kernel

extern void halt(void);

void kernel_main(void) {
    if (framebuffer_request.response == null || framebuffer_request.response->framebuffer_count < 1) {
        while (true) {}
    }

    struct limine_framebuffer *framebuffer =
        framebuffer_request.response->framebuffers[0];

    vga_init_info vga_init_info;
    vga_init_info.framebuffer = framebuffer->address;
    vga_init_info.width = framebuffer->width;
    vga_init_info.height = framebuffer->height;
    // FIXME: Assume framebuffer bits per pixel is a multiple of 8 bits
    vga_init_info.bytes_per_pixel = framebuffer->bpp / 8;
    vga_init(&vga_init_info);

    serial_init_info serial_init_info;
    serial_init_info.port = COM1;
    serial_init_info.divisor = 3;
    if (!serial_init(&serial_init_info)) {
        vga_put_string("Failed to initialize COM1 serial port\n");
    }

    char buffer[256];
    uint scroll_tests = 100;

    for (uint i = 0; i < scroll_tests; ++i) {
        string_format(buffer, "scroll test %u\n", i);
        vga_put_string(buffer);
    }

    string_format(buffer, "%u (0x%x) lines of scroll tests completed\n", scroll_tests, scroll_tests);
    vga_put_string(buffer);

    usize length = string_format(null, "%u (0x%x) lines of scroll tests completed", scroll_tests, scroll_tests);
    string_format(buffer, "The previous line should have %u characters (excluding the null-termination character)\n", length);
    vga_put_string(buffer);

    serial_write_string(COM1, "Kernel initialization completed\n");

    halt();
}
