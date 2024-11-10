#include "gop.h"

#include "core/memory.h"

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
} pc_screen_font_header;

typedef struct {
    byte *framebuffer;
    uint width;
    uint height;
    uint pitch;
    uint bytes_per_pixel;

    byte *font;
    uint text_width;
    uint text_height;
    uint text_x;
    uint text_y;
} gop_driver;

extern byte _binary_thirdparty_font_psfu_start[];

gop_driver gop = {0};

static void gop_scroll(uint lines) {
    pc_screen_font_header *header = (pc_screen_font_header *)gop.font;

    uint line_area = header->height * gop.pitch;
    uint invisible_area_after_scroll = lines * line_area;
    uint visible_area_after_scroll = (gop.text_y - lines) * line_area;

    memory_move(gop.framebuffer, gop.framebuffer + invisible_area_after_scroll, visible_area_after_scroll);
    memory_set(gop.framebuffer + visible_area_after_scroll, 0, invisible_area_after_scroll);

    gop.text_y -= lines;
}

static void gop_new_line(void) {
    gop.text_x = 0;
    ++gop.text_y;

    if (gop.text_y >= gop.text_height) {
        gop_scroll(1);
    }
}

void gop_init(const gop_init_info *info) {
    gop.framebuffer = info->framebuffer;
    gop.width = info->width;
    gop.height = info->height;
    gop.pitch = info->pitch;
    gop.bytes_per_pixel = info->bytes_per_pixel;

    gop.font = _binary_thirdparty_font_psfu_start;

    pc_screen_font_header *font_header = (pc_screen_font_header *)gop.font;
    gop.text_width = info->width / font_header->width;
    gop.text_height = info->height / font_header->height;
    gop.text_x = 0;
    gop.text_y = 0;
}

void gop_put_character(char c) {
    pc_screen_font_header *font_header = (pc_screen_font_header *)gop.font;

    if (c <= 0 || c >= font_header->number_of_glyphs) {
        gop_put_character('?');
        return;
    }

    switch (c) {
    case '\n':
        gop_new_line();
        return;
    case '\r':
        gop.text_x = 0;
        return;
    default:
        break;
    }

    byte *glyph = gop.font + font_header->header_size + c * font_header->bytes_per_glyph;
    uint x_offset = gop.text_x * font_header->width;
    uint y_offset = gop.text_y * font_header->height;
    uint bytes_per_row = font_header->width / 8;

    for (uint y = 0; y < font_header->height; ++y) {
        uint leftmost_bit_mask = 1 << (font_header->width - 1);

        for (uint x = 0; x < 8; ++x) {
            uint offset = (y_offset + y) * gop.pitch + (x_offset + x) * gop.bytes_per_pixel;
            
            // FIXME: Assume RGB
            for (uint ch = 0; ch < 4; ++ch) {
                gop.framebuffer[offset + ch] =
                    glyph[y * bytes_per_row] & (leftmost_bit_mask >> x) ? 0xFF : 0x00;
            }
        }
    }

    if (gop.text_x >= gop.text_width) {
        gop_new_line();
    } else {
        ++gop.text_x;
    }
}

void gop_put_string(const char *s) {
    char c;
    while ((c = *s)) {
        ++s;
        gop_put_character(c);
    }
}
