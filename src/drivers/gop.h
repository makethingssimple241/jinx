/// @brief Graphics Output Protocol driver

#pragma once

#include "core/base.h"

typedef struct {
    byte *framebuffer;
    uint width;
    uint height;
    uint pitch;
    uint bytes_per_pixel;
} gop_init_info;

void gop_init(const gop_init_info *info);
void gop_put_character(char c);
void gop_put_string(const char *s);
