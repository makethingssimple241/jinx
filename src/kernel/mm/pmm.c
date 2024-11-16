#include "pmm.h"
#include "core/string.h"
#include "drivers/gop.h"
#include "kernel/arch.h"

typedef struct {
    u64 physical_address, length;
} memory_region;

struct pmm {
    usize kernel_address_space_start;
};

static struct pmm pmm = {0};

void pmm_init(struct limine_memmap_response *memory_map_response) {
    char buffer[256];

    gop_put_string("Memory map:\n");

    const struct limine_memmap_entry *entries = *memory_map_response->entries;

    for (usize i = 0; i < memory_map_response->entry_count; ++i) {
        struct limine_memmap_entry entry = entries[i];

        if (entry.type != LIMINE_MEMMAP_USABLE && entry.type != LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE) {
            continue;
        }

        string_format(buffer, sizeof(buffer), "  base = 0x%x64 length = %u64\n", entry.base, entry.length);
        gop_put_string(buffer);

        while (entry.length > 0 && entry.length % page_size == 0) {
            u64 physical_address = entry.base;

            entry.base += page_size;
            entry.length -= page_size;
        }
    }
}

u64 pmm_alloc(usize size) {
    return 0;
}

void pmm_free(u64 physical_address, usize size) {
    
}

usize pmm_get_kernel_address_space_start(void) {
    return pmm.kernel_address_space_start;
}

void pmm_set_kernel_address_space_start(usize virtual_address) {
    pmm.kernel_address_space_start = virtual_address;
}
