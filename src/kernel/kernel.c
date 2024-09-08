#include <core/base.h>
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

    for (uint i = 0; i < 100; ++i) {
        vga_put_formatted_string("scroll test %u\n", i);
    }

    vga_put_formatted_string("%d should be equal to \"-100\"\n", -100);
    vga_put_string("time to hang?\n");

    halt();
}
