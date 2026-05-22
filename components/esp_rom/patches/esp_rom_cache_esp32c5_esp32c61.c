/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_rom_caps.h"
#include "soc/soc_caps.h"
#include "soc/cache_reg.h"
#include "soc/cache_struct.h"
#include "soc/ext_mem_defs.h"
#include "hal/assert.h"
#if CONFIG_IDF_TARGET_ESP32C61
#include "esp32c61/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32C5
#include "esp32c5/rom/cache.h"
#else
#error "This patch is only applied to ESP32C5 or ESP32C61"
#endif

#include "esp_rom_sys.h"

/**
 * For ESP32C61, we will patch:
 *   1. writeback/invalidate not work with high address, https://jira.espressif.com:8443/browse/IDF-11008
 *   2. cache writeback skip, https://jira.espressif.com:8443/browse/DIG-829
 *
 * For ESP32C5, we will only patch cache writeback skip issue
 */
#if CONFIG_IDF_TARGET_ESP32C61

#define CACHE_MAX_SYNC_NUM                       ((CACHE_SYNC_SIZE + 1) >> 1)

/**
 * @brief Sync Cache items
 *
 * @param type   sync type
 * @param addr   address
 * @param bytes  bytes to be synced
 */
__attribute__((always_inline))
static inline void s_cache_sync_items(uint32_t type, uint32_t addr, uint32_t bytes)
{
    REG_WRITE(CACHE_SYNC_ADDR_REG, addr);
    REG_SET_FIELD(CACHE_SYNC_SIZE_REG, CACHE_SYNC_SIZE, bytes);
    REG_SET_BIT(CACHE_SYNC_CTRL_REG, type);
    while (!REG_GET_BIT(CACHE_SYNC_CTRL_REG, CACHE_SYNC_DONE))
        ;
}

__attribute__((always_inline))
static inline void s_cache_sync_items_twice(uint32_t type, uint32_t addr, uint32_t bytes)
{
    REG_WRITE(CACHE_SYNC_ADDR_REG, addr);
    REG_SET_FIELD(CACHE_SYNC_SIZE_REG, CACHE_SYNC_SIZE, bytes);
    REG_SET_BIT(CACHE_SYNC_CTRL_REG, type);
    while (!REG_GET_BIT(CACHE_SYNC_CTRL_REG, CACHE_SYNC_DONE))
        ;

    REG_SET_BIT(CACHE_SYNC_CTRL_REG, type);
    while (!REG_GET_BIT(CACHE_SYNC_CTRL_REG, CACHE_SYNC_DONE))
        ;
}

int Cache_Invalidate_Addr(uint32_t vaddr, uint32_t size)
{
    uint32_t plus = 0;
    uint32_t cache_line_size = 32;
    uint32_t cache_max_sync_size = CACHE_MAX_SYNC_NUM;
    if (size == 0) {
        HAL_ASSERT(false);
    }
    //aligned start address to cache line size
    plus = vaddr & (cache_line_size - 1);
    vaddr -= plus;
    //make the length fit the start address
    size += plus;
    //aligned the length to cache line size(0->0)
    size = (size + cache_line_size - 1) & ~(cache_line_size - 1);

    while (size > 0) {
        //aligned to cache_max_sync_size, (0->cache_max_sync_size)
        uint32_t this_size = ((vaddr + cache_max_sync_size) & ~(cache_max_sync_size - 1)) - vaddr;
        if (this_size > size) {
            this_size = size;
        }
        s_cache_sync_items(CACHE_SYNC_INVALIDATE, vaddr, this_size);
        vaddr += this_size;
        size -= this_size;
    }

    return 0;
}

int Cache_WriteBack_Addr(uint32_t vaddr, uint32_t size)
{
    uint32_t plus = 0;
    uint32_t cache_line_size = 32;
    uint32_t cache_max_sync_size = CACHE_MAX_SYNC_NUM;
    if (size == 0) {
        HAL_ASSERT(false);
    }
    //aligned start address to cache line size
    plus = vaddr & (cache_line_size - 1);
    vaddr -= plus;
    //make the length fit the start address
    size += plus;
    //aligned the length to cache line size(0->0)
    size = (size + cache_line_size - 1) & ~(cache_line_size - 1);

    while (size > 0) {
        //aligned to cache_max_sync_size, (0->cache_max_sync_size)
        uint32_t this_size = ((vaddr + cache_max_sync_size) & ~(cache_max_sync_size - 1)) - vaddr;
        if (this_size > size) {
            this_size = size;
        }
        s_cache_sync_items_twice(CACHE_SYNC_WRITEBACK, vaddr, this_size);
        vaddr += this_size;
        size -= this_size;
    }

    return 0;
}

int Cache_WriteBack_Invalidate_Addr(uint32_t addr, uint32_t size)
{
    uint32_t plus = 0;
    uint32_t cache_line_size = 32;
    uint32_t cache_max_sync_size = CACHE_MAX_SYNC_NUM;
    if (size == 0) {
        HAL_ASSERT(false);
    }
    //aligned start address to cache line size
    plus = vaddr & (cache_line_size - 1);
    vaddr -= plus;
    //make the length fit the start address
    size += plus;
    //aligned the length to cache line size(0->0)
    size = (size + cache_line_size - 1) & ~(cache_line_size - 1);

    while (size > 0) {
        //aligned to cache_max_sync_size, (0->cache_max_sync_size)
        uint32_t this_size = ((vaddr + cache_max_sync_size) & ~(cache_max_sync_size - 1)) - vaddr;
        if (this_size > size) {
            this_size = size;
        }
        s_cache_sync_items_twice(CACHE_SYNC_WRITEBACK_INVALIDATE, vaddr, this_size);
        vaddr += this_size;
        size -= this_size;
    }

    return 0;
}

#ifndef BOOTLOADER_BUILD
void Cache_WriteBack_All(void)
{
    s_cache_sync_items_twice(CACHE_SYNC_WRITEBACK, 0, 0);
}
#endif

void Cache_WriteBack_Invalidate_All(void)
{
    s_cache_sync_items_twice(CACHE_SYNC_WRITEBACK_INVALIDATE, 0, 0);
}

#elif CONFIG_IDF_TARGET_ESP32C5
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
#endif
