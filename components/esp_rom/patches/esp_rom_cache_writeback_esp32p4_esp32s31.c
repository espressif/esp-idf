/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "soc/cache_reg.h"
#include "rom/cache.h"

// esp32p4 and esp32s31 need msp parameters in apis.
int Cache_WriteBack_Addr(uint32_t map, uint32_t addr, uint32_t size)
{
    REG_WRITE(CACHE_SYNC_MAP_REG, map);
    REG_WRITE(CACHE_SYNC_ADDR_REG, addr);
    REG_WRITE(CACHE_SYNC_SIZE_REG, size);
    REG_WRITE(CACHE_SYNC_CTRL_REG, CACHE_WRITEBACK_ENA);
    while (!REG_GET_BIT(CACHE_SYNC_CTRL_REG, CACHE_SYNC_DONE));

    REG_WRITE(CACHE_SYNC_CTRL_REG, CACHE_WRITEBACK_ENA);
    while (!REG_GET_BIT(CACHE_SYNC_CTRL_REG, CACHE_SYNC_DONE));

    return 0;
}

int Cache_WriteBack_All(uint32_t map)
{
    if ((map & CACHE_MAP_DCACHE_MASK) == 0) {
        return ESP_ROM_ERR_INVALID_ARG;
    }

    REG_WRITE(CACHE_SYNC_MAP_REG, map);
    REG_WRITE(CACHE_SYNC_ADDR_REG, 0);
    REG_WRITE(CACHE_SYNC_SIZE_REG, 0);
    REG_WRITE(CACHE_SYNC_CTRL_REG, CACHE_WRITEBACK_ENA);
    while (!REG_GET_BIT(CACHE_SYNC_CTRL_REG, CACHE_SYNC_DONE));

    REG_WRITE(CACHE_SYNC_CTRL_REG, CACHE_WRITEBACK_ENA);
    while (!REG_GET_BIT(CACHE_SYNC_CTRL_REG, CACHE_SYNC_DONE));

    return 0;
}

int Cache_WriteBack_Invalidate_Addr(uint32_t map, uint32_t addr, uint32_t size)
{
    REG_WRITE(CACHE_SYNC_MAP_REG, map);
    REG_WRITE(CACHE_SYNC_ADDR_REG, addr);
    REG_WRITE(CACHE_SYNC_SIZE_REG, size);
    REG_WRITE(CACHE_SYNC_CTRL_REG, CACHE_WRITEBACK_INVALIDATE_ENA);
    while (!REG_GET_BIT(CACHE_SYNC_CTRL_REG, CACHE_SYNC_DONE));

    REG_WRITE(CACHE_SYNC_CTRL_REG, CACHE_WRITEBACK_INVALIDATE_ENA);
    while (!REG_GET_BIT(CACHE_SYNC_CTRL_REG, CACHE_SYNC_DONE));

    return 0;
}

int Cache_WriteBack_Invalidate_All(uint32_t map)
{
    if ((map & CACHE_MAP_DCACHE_MASK) == 0) {
        return ESP_ROM_ERR_INVALID_ARG;
    }

    REG_WRITE(CACHE_SYNC_MAP_REG, map);
    REG_WRITE(CACHE_SYNC_ADDR_REG, 0);
    REG_WRITE(CACHE_SYNC_SIZE_REG, 0);
    REG_WRITE(CACHE_SYNC_CTRL_REG, CACHE_WRITEBACK_INVALIDATE_ENA);
    while (!REG_GET_BIT(CACHE_SYNC_CTRL_REG, CACHE_SYNC_DONE));

    REG_WRITE(CACHE_SYNC_CTRL_REG, CACHE_WRITEBACK_INVALIDATE_ENA);
    while (!REG_GET_BIT(CACHE_SYNC_CTRL_REG, CACHE_SYNC_DONE));

    return 0;
}
