#include "pmm.h"
#include "core/allocators/bitmap.h"
#include "core/memory.h"
#include "core/string.h"
#include "drivers/gop.h"
#include "kernel/arch.h"

#include <limine.h>

struct pmm {
    byte *bitmap;
    bitmap_allocator allocator;
    void *kernel_address_space_start;
};

static struct pmm pmm = {0};

extern volatile struct limine_hhdm_request hhdm_request;
extern volatile struct limine_memmap_request memmap_request;

void pmm_init(void) {
    struct limine_hhdm_response *hhdm_response = hhdm_request.response;
    if (!hhdm_response) {
        panic("pmm_init: cannot get higher-half direct map response");
    }

    const struct limine_memmap_response *memmap_response = memmap_request.response;
    if (!memmap_response) {
        panic("pmm_init: cannot get memory map response");
    }

    pmm.kernel_address_space_start = (void *)hhdm_response->offset;

    gop_put_string("Memory map:\n");

    const struct limine_memmap_entry *entries = *memmap_response->entries;

    char buffer[256] = {0};

    u64 min_physical_address = -1, max_physical_address = 0;
    memory_region regions[256];
    usize region_count = 0;

    for (usize i = 0; i < memmap_response->entry_count; ++i) {
        struct limine_memmap_entry entry = entries[i];

        if (entry.type != LIMINE_MEMMAP_USABLE && entry.type != LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE) {
            continue;
        }

        string_format(buffer, sizeof(buffer), "  base = 0x%x64 length = %u64\n", entry.base, entry.length);
        gop_put_string(buffer);

        if (entry.base < min_physical_address) {
            min_physical_address = entry.base;
        }

        if (entry.base + entry.length > max_physical_address) {
            max_physical_address = entry.base + entry.length;
        }

        if (region_count > len(regions)) {
            panic("pmm_init: too many regions");
        }

        regions[region_count++] = (memory_region){
            .base = entry.base,
            .size = entry.length};
    }

    usize size = max_physical_address - min_physical_address;

    usize bitmap_size = size / page_size / 8;
    
    // Allocate space for the bitmap
    for (usize i = 0; i < region_count; ++i) {
        memory_region *region = &regions[i];

        if (region->size >= bitmap_size) {
            pmm.bitmap = pmm.kernel_address_space_start + region->base;
            region->base += bitmap_size;
            region->size -= bitmap_size;
        }
        
        if (pmm.bitmap) {
            if (region->size == 0) {
                panic("TODO: Remove entry");
            }

            break;
        }
    }

    if (!pmm.bitmap) {
        panic("pmm_init: cannot find a memory region for the bitmap");
    }

    memory_set(pmm.bitmap, 0, bitmap_size);

    pmm.allocator = bitmap_allocator_init(pmm.bitmap, min_physical_address, size, page_size);
    bitmap_allocator_use_memory_map(&pmm.allocator, regions, region_count);
}

physical_address pmm_alloc(usize size) {
    return bitmap_allocator_alloc(&pmm.allocator, size);
}

void pmm_free(physical_address physical_address, usize size) {
    bitmap_allocator_free(&pmm.allocator, physical_address, size);
}

void *pmm_get_kernel_address_space_start(void) {
    return pmm.kernel_address_space_start;
}
