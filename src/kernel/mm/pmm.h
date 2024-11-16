/// @brief The physical memory manager
/// @details The physical memory manager (PMM) is responsible for allocating and freeing pages.

#pragma once

#include "core/base.h"

#include <limine.h>

#define page_size   4096

void pmm_init(struct limine_memmap_response *memory_map_response);

/// @param size The size to allocate in bytes
/// @returns A page-aligned physical address
u64 pmm_alloc(usize size);
/// @param size The size of the block to free in bytes
void pmm_free(u64 physical_address, usize size);

usize pmm_get_kernel_address_space_start(void);
void pmm_set_kernel_address_space_start(usize virtual_address);
