#pragma once

#include "../base.h"

/// @brief This can be a physical address or a virtual address depending on how the bitmap allocator is used
typedef u64 bitmap_allocator_address;

typedef struct {
    byte *bitmap;
    usize number_of_entries;
    bitmap_allocator_address memory_base;
    usize block_size;
} bitmap_allocator;

typedef struct {
    bitmap_allocator_address base;
    usize size;
} memory_region;

/// @param bitmap A virtual address which points to a block of memory which is at least of <tt>size / block_size / 8</tt> bytes
/// @param memory_base The beginning of the address space for the allocator to allocate
/// @param size The size of the address space for the allocator to allocate
bitmap_allocator bitmap_allocator_init(byte *bitmap, bitmap_allocator_address memory_base, usize size, usize block_size);
void bitmap_allocator_use_memory_map(bitmap_allocator *self, memory_region regions[], usize region_count);
bitmap_allocator_address bitmap_allocator_alloc(bitmap_allocator *self, usize size);
void bitmap_allocator_free(bitmap_allocator *self, bitmap_allocator_address address, usize size);
