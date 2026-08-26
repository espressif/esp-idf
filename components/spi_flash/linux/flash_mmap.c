/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "spi_flash_mmap.h"

uint32_t spi_flash_mmap_get_free_pages(spi_flash_mmap_memory_t memory)
{
    (void) memory;
    return 10;
}
