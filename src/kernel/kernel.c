#include "arch.h"

#include "core/base.h"
#include "core/string.h"
#include "drivers/gop.h"
#include "drivers/serial.h"

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

void run_tests() {
    char buffer[256];
    uint scroll_tests = 100;

    for (uint i = 0; i < scroll_tests; ++i) {
        string_format(buffer, 256, "scroll test %u\n", i);
        gop_put_string(buffer);
    }

    string_format(buffer, 256, "%u (0x%x) lines of scroll tests completed\n", scroll_tests, scroll_tests);
    gop_put_string(buffer);

    usize length = string_format(null, 256, "%u (0x%x) lines of scroll tests completed", scroll_tests, scroll_tests);
    string_format(buffer, 256, "The previous line should have %u characters (excluding the null-termination character)\n", length);
    gop_put_string(buffer);

    serial_write_string(COM1, "Kernel initialization completed\n");
}

void kernel_main(void) {
    if (!framebuffer_request.response || framebuffer_request.response->framebuffer_count < 1 || !framebuffer_request.response->framebuffers[0]->address) {
        while (true) {}
    }

    struct limine_framebuffer *framebuffer =
        framebuffer_request.response->framebuffers[0];

    gop_init_info gop_init_info = {0};
    gop_init_info.framebuffer = framebuffer->address;
    gop_init_info.width = framebuffer->width;
    gop_init_info.height = framebuffer->height;
    gop_init_info.pitch = framebuffer->pitch;
    gop_init_info.bytes_per_pixel = framebuffer->bpp / 8;
    gop_init(&gop_init_info);

    serial_init_info serial_init_info = {0};
    serial_init_info.port = COM1;
    serial_init_info.divisor = 3;
    if (!serial_init(&serial_init_info)) {
        panic("Failed to initialize COM1 serial port\n");
    }

    arch_init();

    run_tests();

    while (true) {
        
    }
}
