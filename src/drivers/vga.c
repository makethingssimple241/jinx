#include "vga.h"

#include <core/memory.h>

#define PSF_FONT_MAGIC  0x864ab572

typedef struct {
    u32 magic;
    u32 version;
    u32 header_size;
    u32 flags;
    u32 number_of_glyphs;
    u32 bytes_per_glyph;
    u32 height;
    u32 width;
} pc_screen_font;

typedef struct {
    vga_init_info info;
    uint text_width;
    uint text_height;
    uint text_x;
    uint text_y;
} vga_driver;

extern byte _binary_thirdparty_font_psfu_start[];

pc_screen_font *font = {0};
vga_driver vga = {0};

static void vga_scroll(uint lines) {
    uint line_area = font->height * vga.info.width * vga.info.bytes_per_pixel;
    uint invisible_area_after_scroll = lines * line_area;
    uint visible_area_after_scroll = (vga.text_y - lines) * line_area;

    memory_move(vga.info.framebuffer, vga.info.framebuffer + invisible_area_after_scroll, visible_area_after_scroll);
    memory_set(vga.info.framebuffer + visible_area_after_scroll, 0, invisible_area_after_scroll);
    
    vga.text_y -= lines;
}

static void vga_new_line(void) {
    vga.text_x = 0;
    ++vga.text_y;

    if (vga.text_y > vga.text_height) {
        vga_scroll(1);
    }
}

void vga_init(const vga_init_info *info) {
    font = (pc_screen_font *)&_binary_thirdparty_font_psfu_start;
    vga.info = *info;
    vga.text_width = info->width / font->width;
    vga.text_height = info->height / font->height;
    vga.text_x = 0;
    vga.text_y = 0;
}

void vga_put_character(char c) {
    if (c <= 0 || c >= font->number_of_glyphs) {
        vga_put_character('?');
        return;
    }

    if (c == '\n') {
        vga_new_line();
        return;
    }

    byte *glyph = _binary_thirdparty_font_psfu_start + font->header_size + c * font->bytes_per_glyph;
    uint x_offset = vga.text_x * font->width;
    uint y_offset = vga.text_y * font->height;
    uint bytes_per_row = font->width / 8;

    for (uint y = 0; y < font->height; ++y) {
        byte mask = 1 << (font->width - 1);

        for (uint x = 0; x < 8; ++x) {
            uint offset = ((y_offset + y) * vga.info.width + x_offset + x) * vga.info.bytes_per_pixel;
            
            // FIXME: Assume RGB
            for (uint ch = 0; ch < 4; ++ch) {
                vga.info.framebuffer[offset + ch] =
                    glyph[y * bytes_per_row] & mask ? 0xFF : 0x00;
            }

            mask >>= 1;
        }
    }

    if (vga.text_x >= vga.text_width) {
        vga_new_line();
    } else {
        ++vga.text_x;
    }
}

void vga_put_string(const char *s) {
    char c;
    while ((c = *s)) {
        ++s;
        vga_put_character(c);
    }
}
