/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <stdint.h>
#include "soc/soc_caps.h"
#include "esp_rom_caps.h"
#include "soc/extmem_reg.h"
#include "xtensa/xtruntime.h"
#if CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/cache.h"
#endif

#define ALIGN_UP(addr, align) (((addr) + (align)-1) & ~((align)-1))
#define ALIGN_DOWN(addr, align)  ((addr) & ~((align) - 1))

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

#if ESP_ROM_HAS_CACHE_SUSPEND_WAITI_BUG
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
#endif  //#if SOC_CACHE_FREEZE_SUPPORTED
#endif  //#if ESP_ROM_HAS_CACHE_SUSPEND_WAITI_BUG

#if ESP_ROM_HAS_CACHE_WRITEBACK_BUG
/* Defined in esp_rom_cache_writeback_esp32s3.S */
extern void cache_writeback_items_freeze(uint32_t addr, uint32_t items);
// renamed for patch
extern int rom_Cache_WriteBack_Addr(uint32_t addr, uint32_t size);
int Cache_WriteBack_Addr(uint32_t addr, uint32_t size)
{
    /* Do special processing for unaligned memory at the start and end of the cache writeback memory.
     * 1. Disable the interrupt to prevent the current CPU accessing the same cacheline.
     * 2. Enable dcache freeze to prevent the another CPU accessing the same cacheline.
     */
    uint32_t irq_status;
    uint32_t start_len, end_len;
    uint32_t start, end;
    uint32_t dcache_line_size;
    uint32_t autoload;
    int ret = 0;
    start = addr;
    end = addr + size;
    dcache_line_size = Cache_Get_DCache_Line_Size();

    if (size == 0) {
        return 0;
    }

    /*the start address is unaligned*/
    if (start & (dcache_line_size -1)) {
        addr = ALIGN_UP(start, dcache_line_size);
        start_len = addr - start;
        size = (size < start_len) ? 0 : (size - start_len);

        /*writeback start unaligned mem, one cacheline*/
        irq_status = XTOS_SET_INTLEVEL(XCHAL_NMILEVEL);//mask all interrupts
        cache_writeback_items_freeze(start, 1);
        XTOS_RESTORE_INTLEVEL(irq_status);

        if (size == 0) {
            return 0;
        }
    }

    /*the end address is unaligned*/
    if (end & (dcache_line_size -1)) {
        end = ALIGN_DOWN(end, dcache_line_size);
        end_len = addr + size - end;
        size = (size - end_len);

        /*writeback end unaligned mem, one cacheline*/
        irq_status = XTOS_SET_INTLEVEL(XCHAL_NMILEVEL);//mask all interrupts
        cache_writeback_items_freeze(end, 1);
        XTOS_RESTORE_INTLEVEL(irq_status);

        if (size == 0) {
            return 0;
        }
    }

    /*suspend autoload, avoid load cachelines being written back*/
    autoload = Cache_Suspend_DCache_Autoload();
    ret = rom_Cache_WriteBack_Addr(addr, size);
    Cache_Resume_DCache_Autoload(autoload);

    return ret;
}
extern int Cache_WriteBack_Addr(uint32_t addr, uint32_t size);
#endif  //#if ESP_ROM_HAS_CACHE_WRITEBACK_BUG
