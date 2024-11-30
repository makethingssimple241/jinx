/// @brief The physical memory manager
/// @details The physical memory manager (PMM) is responsible for allocating and freeing pages.

#pragma once

#include "core/base.h"

#define page_size   4096

void pmm_init(void);

/// @param size The size to allocate in bytes
/// @returns The physical address of the page allocated
physical_address pmm_alloc(usize size);
/// @param physical_address The physical address of the page to free
/// @param size The size of the block to free in bytes
void pmm_free(physical_address physical_address, usize size);

void *pmm_get_kernel_address_space_start(void);
