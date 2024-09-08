#pragma once

#include <core/base.h>

typedef struct {
    byte *framebuffer;
    uint width;
    uint height;
    uint bytes_per_pixel;
} vga_init_info;

void vga_init(const vga_init_info *info);
void vga_put_character(char c);
void vga_put_string(const char *s);
void vga_put_formatted_string(const char *fmt, ...);
