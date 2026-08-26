/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "soc/cache_reg.h"
#include "rom/cache.h"
#include "esp_attr.h"
#include "esp_rom_sys.h"

// esp32p4 and esp32s31 need msp parameters in apis.
int Cache_WriteBack_Addr(uint32_t map, uint32_t addr, uint32_t size)
{
    uint32_t plus;
    uint32_t cache_line_size = 0;
    /* writeback readonly cache is invalid */
    if (map & CACHE_MAP_L1_ICACHE_MASK) {
        return ESP_ROM_ERR_INVALID_ARG;
    }

    if ((map & CACHE_MAP_DCACHE_MASK) == 0) {
        return ESP_ROM_ERR_INVALID_ARG;
    }
#if CONFIG_IDF_TARGET_ESP32P4
    /* esp32p4 will check l2 cache */
    if (map & CACHE_MAP_L1_DCACHE) {
        cache_line_size = rom_cache_internal_table_ptr->l1_dcache_line_size();
    }
    if (map & CACHE_MAP_L2_CACHE) {
        cache_line_size = (cache_line_size > rom_cache_internal_table_ptr->l2_cache_line_size()) ?
            cache_line_size : rom_cache_internal_table_ptr->l2_cache_line_size();
    }
#else
    cache_line_size = rom_cache_internal_table_ptr->l1_dcache_line_size();
#endif

    plus = addr & (cache_line_size - 1);
    addr -= plus;
    size += plus;
    size = (size + cache_line_size - 1) & ~(cache_line_size - 1);
    REG_WRITE(CACHE_SYNC_MAP_REG, map);
    REG_WRITE(CACHE_SYNC_ADDR_REG, addr);
    REG_WRITE(CACHE_SYNC_SIZE_REG, size);
    REG_WRITE(CACHE_SYNC_CTRL_REG, CACHE_WRITEBACK_ENA);
    while (!REG_GET_BIT(CACHE_SYNC_CTRL_REG, CACHE_SYNC_DONE));

    REG_WRITE(CACHE_SYNC_CTRL_REG, CACHE_WRITEBACK_ENA);
    while (!REG_GET_BIT(CACHE_SYNC_CTRL_REG, CACHE_SYNC_DONE));

    return 0;
}

#ifndef BOOTLOADER_BUILD
int Cache_WriteBack_All(uint32_t map)
{
    /* writeback readonly cache is invalid */
    if (map & CACHE_MAP_L1_ICACHE_MASK) {
        return ESP_ROM_ERR_INVALID_ARG;
    }

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
#endif

int Cache_WriteBack_Invalidate_Addr(uint32_t map, uint32_t addr, uint32_t size)
{
    uint32_t plus;
    uint32_t cache_line_size = 0;
    /* writeback readonly cache is invalid */
    if (map & CACHE_MAP_L1_ICACHE_MASK) {
        return ESP_ROM_ERR_INVALID_ARG;
    }

    if ((map & CACHE_MAP_DCACHE_MASK) == 0) {
        return ESP_ROM_ERR_INVALID_ARG;
    }
#if CONFIG_IDF_TARGET_ESP32P4
    /* esp32p4 will check l2 cache */
    if (map & CACHE_MAP_L1_DCACHE) {
        cache_line_size = rom_cache_internal_table_ptr->l1_dcache_line_size();
    }
    if (map & CACHE_MAP_L2_CACHE) {
        cache_line_size = (cache_line_size > rom_cache_internal_table_ptr->l2_cache_line_size()) ?
            cache_line_size : rom_cache_internal_table_ptr->l2_cache_line_size();
    }
#else
    cache_line_size = rom_cache_internal_table_ptr->l1_dcache_line_size();
#endif

    plus = addr & (cache_line_size - 1);
    addr -= plus;
    size += plus;
    size = (size + cache_line_size - 1) & ~(cache_line_size - 1);

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
    /* writeback readonly cache is invalid */
    if (map & CACHE_MAP_L1_ICACHE_MASK) {
        return ESP_ROM_ERR_INVALID_ARG;
    }

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
