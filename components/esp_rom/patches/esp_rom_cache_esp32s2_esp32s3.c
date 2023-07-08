/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/extmem_reg.h"
#if CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/cache.h"
#endif

// this api is renamed for patch
extern uint32_t rom_Cache_Count_Flash_Pages(uint32_t bus, uint32_t * page0_mapped);
uint32_t Cache_Count_Flash_Pages(uint32_t bus, uint32_t * page0_mapped)
{
    uint32_t page0_before_count = *page0_mapped;
    uint32_t flash_pages = 0;
    flash_pages = rom_Cache_Count_Flash_Pages(bus, page0_mapped);

/* No page mapped to page0, in this condition, the rom api will return
 * unexpected value + 1.
 */
    if (page0_before_count == *page0_mapped) {
        flash_pages--;
    }
    return flash_pages;
}
extern uint32_t Cache_Count_Flash_Pages(uint32_t bus, uint32_t * page0_mapped);

#if CONFIG_ESP_ROM_HAS_CACHE_SUSPEND_WAITI_BUG
static inline void Cache_Wait_Idle(int icache)
{
    if (icache) {
        while (REG_GET_FIELD(EXTMEM_CACHE_STATE_REG, EXTMEM_ICACHE_STATE) != 1) {
            ;
        }
    } else {
        while (REG_GET_FIELD(EXTMEM_CACHE_STATE_REG, EXTMEM_DCACHE_STATE) != 1) {
            ;
        }
    }
}
// renamed for patch
extern uint32_t rom_Cache_Suspend_ICache(void);
uint32_t Cache_Suspend_ICache(void)
{
    uint32_t ret = rom_Cache_Suspend_ICache();
    Cache_Wait_Idle(1);
    return ret;
}
extern uint32_t Cache_Suspend_ICache(void);

// renamed for patch
extern uint32_t rom_Cache_Suspend_DCache(void);
uint32_t Cache_Suspend_DCache(void)
{
    uint32_t ret = rom_Cache_Suspend_DCache();
    Cache_Wait_Idle(0);
    return ret;
}
extern uint32_t Cache_Suspend_DCache(void);

#if SOC_CACHE_FREEZE_SUPPORTED
// renamed for patch
extern void rom_Cache_Freeze_ICache_Enable(cache_freeze_mode_t mode);
void Cache_Freeze_ICache_Enable(cache_freeze_mode_t mode)
{
    rom_Cache_Freeze_ICache_Enable(mode);
    Cache_Wait_Idle(1);
}
extern void Cache_Freeze_ICache_Enable(cache_freeze_mode_t mode);

// renamed for patch
extern void rom_Cache_Freeze_DCache_Enable(cache_freeze_mode_t mode);
void Cache_Freeze_DCache_Enable(cache_freeze_mode_t mode)
{
    rom_Cache_Freeze_DCache_Enable(mode);
    Cache_Wait_Idle(0);
}
extern void Cache_Freeze_DCache_Enable(cache_freeze_mode_t mode);
#endif
#endif
