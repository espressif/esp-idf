/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "soc/cache_reg.h"
#include "rom/cache.h"

// esp32c5, esp32c61 and esp32h4 do not need msp parameters in apis.
int Cache_WriteBack_Addr(uint32_t addr, uint32_t size)
{
    uint32_t plus;
    uint32_t cache_line_size = MIN_CACHE_LINE_SIZE;

    plus = addr & (cache_line_size - 1);
    addr -= plus;
    size += plus;
    size = (size + cache_line_size - 1) & ~(cache_line_size - 1);

    REG_WRITE(CACHE_SYNC_MAP_REG, CACHE_MAP_FLASH_CACHE);
    REG_WRITE(CACHE_SYNC_ADDR_REG, addr);
    REG_WRITE(CACHE_SYNC_SIZE_REG, size);
    REG_WRITE(CACHE_SYNC_CTRL_REG, CACHE_WRITEBACK_ENA);
    while (!REG_GET_BIT(CACHE_SYNC_CTRL_REG, CACHE_SYNC_DONE));

    REG_WRITE(CACHE_SYNC_CTRL_REG, CACHE_WRITEBACK_ENA);
    while (!REG_GET_BIT(CACHE_SYNC_CTRL_REG, CACHE_SYNC_DONE));

    return 0;
}

#ifndef BOOTLOADER_BUILD
void Cache_WriteBack_All(void)
{
    REG_WRITE(CACHE_SYNC_MAP_REG, CACHE_MAP_FLASH_CACHE);
    REG_WRITE(CACHE_SYNC_ADDR_REG, 0);
    REG_WRITE(CACHE_SYNC_SIZE_REG, 0);
    REG_WRITE(CACHE_SYNC_CTRL_REG, CACHE_WRITEBACK_ENA);
    while (!REG_GET_BIT(CACHE_SYNC_CTRL_REG, CACHE_SYNC_DONE));

    REG_WRITE(CACHE_SYNC_CTRL_REG, CACHE_WRITEBACK_ENA);
    while (!REG_GET_BIT(CACHE_SYNC_CTRL_REG, CACHE_SYNC_DONE));
}
#endif

int Cache_WriteBack_Invalidate_Addr(uint32_t addr, uint32_t size)
{
    uint32_t plus;
    uint32_t cache_line_size = MIN_CACHE_LINE_SIZE;

    plus = addr & (cache_line_size - 1);
    addr -= plus;
    size += plus;
    size = (size + cache_line_size - 1) & ~(cache_line_size - 1);

    REG_WRITE(CACHE_SYNC_MAP_REG, CACHE_MAP_FLASH_CACHE);
    REG_WRITE(CACHE_SYNC_ADDR_REG, addr);
    REG_WRITE(CACHE_SYNC_SIZE_REG, size);
    REG_WRITE(CACHE_SYNC_CTRL_REG, CACHE_WRITEBACK_INVALIDATE_ENA);
    while (!REG_GET_BIT(CACHE_SYNC_CTRL_REG, CACHE_SYNC_DONE));

    REG_WRITE(CACHE_SYNC_CTRL_REG, CACHE_WRITEBACK_INVALIDATE_ENA);
    while (!REG_GET_BIT(CACHE_SYNC_CTRL_REG, CACHE_SYNC_DONE));

    return 0;
}

void Cache_WriteBack_Invalidate_All(void)
{
    REG_WRITE(CACHE_SYNC_MAP_REG, CACHE_MAP_FLASH_CACHE);
    REG_WRITE(CACHE_SYNC_ADDR_REG, 0);
    REG_WRITE(CACHE_SYNC_SIZE_REG, 0);
    REG_WRITE(CACHE_SYNC_CTRL_REG, CACHE_WRITEBACK_INVALIDATE_ENA);
    while (!REG_GET_BIT(CACHE_SYNC_CTRL_REG, CACHE_SYNC_DONE));

    REG_WRITE(CACHE_SYNC_CTRL_REG, CACHE_WRITEBACK_INVALIDATE_ENA);
    while (!REG_GET_BIT(CACHE_SYNC_CTRL_REG, CACHE_SYNC_DONE));
}
