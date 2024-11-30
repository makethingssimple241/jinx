#include "bitmap.h"
#include "../assert.h"

#define invalid_index       -1
#define invalid_address     -1

static void bitmap_allocator_mark_used(const bitmap_allocator *self, usize i) {
    assert(i < self->number_of_entries, "Index out of range");

    usize byte_index = i / 8;
    usize bit_index = i % 8;
    self->bitmap[byte_index] |= (1 << bit_index);
}

static void bitmap_allocator_mark_unused(const bitmap_allocator *self, usize i) {
    assert(i < self->number_of_entries, "Index out of range");

    usize byte_index = i / 8;
    usize bit_index = i % 8;
    self->bitmap[byte_index] &= ~(1 << bit_index);
}

static bool bitmap_allocator_unused(const bitmap_allocator *self, usize i) {
    assert(i < self->number_of_entries, "Index out of range");

    usize byte_index = i / 8;
    usize bit_index = i % 8;
    return ((self->bitmap[byte_index] >> bit_index) & 0b1) == 0;
}

static usize bitmap_allocator_get_index_of_address(const bitmap_allocator *self, u64 address) {
    return (address - self->memory_base) / self->block_size;
}

static u64 bitmap_allocator_get_address_of_index(const bitmap_allocator *self, usize i) {
    return self->memory_base + i * self->block_size;
}

bitmap_allocator bitmap_allocator_init(byte *bitmap, bitmap_allocator_address memory_base, usize size, usize block_size) {
    bitmap_allocator self = {0};
    self.bitmap = bitmap;
    self.number_of_entries = size / block_size;
    self.memory_base = memory_base;
    self.block_size = block_size;

    // Mark all regions as used first.
    // bitmap_allocator_use_memory_map will mark entries which point to areas in unused areas as in the memory map as unused.
    for (usize i = 0; i < self.number_of_entries; ++i) {
        bitmap_allocator_mark_used(&self, i);
    }

    return self;
}

void bitmap_allocator_use_memory_map(bitmap_allocator *self, memory_region regions[], usize region_count) {
    for (usize region_index = 0; region_index < region_count; ++region_index) {
        memory_region region = regions[region_index];

        if (region.size % self->block_size != 0) {
            region.size -= self->block_size - region.size % self->block_size;
        }

        usize region_start_block_index = bitmap_allocator_get_index_of_address(self, region.base);

        for (usize i = 0; i < region.size / self->block_size; ++i) {
            bitmap_allocator_mark_unused(self, region_start_block_index + i);
        }
    }
}

bitmap_allocator_address bitmap_allocator_alloc(bitmap_allocator *self, usize size) {
    usize number_of_blocks = size / self->block_size;

    if (size % self->block_size) {
        ++number_of_blocks;
    }

    usize start_index = invalid_index;

    for (usize i = 0; i < self->number_of_entries; ++i) {
        // If the current block is not free, skip it and continue to search
        if (!bitmap_allocator_unused(self, i)) {
            continue;
        }

        // If the current block is free, start searching
        start_index = i;
        usize number_of_blocks_visited = 1;

        while (number_of_blocks_visited < number_of_blocks) {
            // If there are insufficient continuous blocks, skip these blocks
            if (!bitmap_allocator_unused(self, ++i)) {
                start_index = invalid_index;
                break;
            }

            ++number_of_blocks_visited;
        }

        if (start_index != invalid_index) {
            for (usize i = start_index; i < start_index + number_of_blocks; ++i) {
                bitmap_allocator_mark_used(self, i);
            }
            
            break;
        }
    }

    if (start_index == invalid_index) {
        return invalid_address;
    }

    return bitmap_allocator_get_address_of_index(self, start_index);
}

void bitmap_allocator_free(bitmap_allocator *self, bitmap_allocator_address address, usize size) {
    usize index = bitmap_allocator_get_index_of_address(self, address);
    usize number_of_blocks = size / self->block_size;
    usize end_index = index + number_of_blocks;

    if (size % self->block_size) {
        ++number_of_blocks;
    }

    for (; index < end_index; ++index) {
        bitmap_allocator_mark_unused(self, index);
    }
}
