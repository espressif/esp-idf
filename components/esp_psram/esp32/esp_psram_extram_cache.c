/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/dport_reg.h"
#include "esp_attr.h"
#include "esp_psram.h"
#include "esp_private/esp_psram_extram.h"

#if CONFIG_FREERTOS_UNICORE
#define PSRAM_MODE PSRAM_VADDR_MODE_NORMAL
#else
#define PSRAM_MODE PSRAM_VADDR_MODE_LOWHIGH
#endif

/*
 Before flushing the cache, if psram is enabled as a memory-mapped thing, we need to write back the data in the cache to the psram first,
 otherwise it will get lost. For now, we just read 64/128K of random PSRAM memory to do this.
 Note that this routine assumes some unique mapping for the first 2 banks of the PSRAM memory range, as well as the
 2 banks after the 2 MiB mark.
*/
void IRAM_ATTR esp_psram_extram_writeback_cache(void)
{
    int x;
    volatile int i = 0;
    volatile uint8_t *psram = (volatile uint8_t*)SOC_EXTRAM_DATA_LOW;
    int cache_was_disabled = 0;

    if (!esp_psram_is_initialized()) {
        return;
    }

    //We need cache enabled for this to work. Re-enable it if needed; make sure we
    //disable it again on exit as well.
    if (DPORT_REG_GET_BIT(DPORT_PRO_CACHE_CTRL_REG, DPORT_PRO_CACHE_ENABLE) == 0) {
        cache_was_disabled |= (1 << 0);
        DPORT_SET_PERI_REG_BITS(DPORT_PRO_CACHE_CTRL_REG, 1, 1, DPORT_PRO_CACHE_ENABLE_S);
    }
#ifndef CONFIG_FREERTOS_UNICORE
    if (DPORT_REG_GET_BIT(DPORT_APP_CACHE_CTRL_REG, DPORT_APP_CACHE_ENABLE) == 0) {
        cache_was_disabled |= (1 << 1);
        DPORT_SET_PERI_REG_BITS(DPORT_APP_CACHE_CTRL_REG, 1, 1, DPORT_APP_CACHE_ENABLE_S);
    }
#endif

#if (PSRAM_MODE != PSRAM_VADDR_MODE_LOWHIGH)
    /*
    Single-core and even/odd mode only have 32K of cache evenly distributed over the address lines. We can clear
    the cache by just reading 64K worth of cache lines.
    */
    for (x = 0; x < 1024 * 64; x += 32) {
        i += psram[x];
    }
#else
    /*
    Low/high psram cache mode uses one 32K cache for the lowest 2MiB of SPI flash and another 32K for the highest
    2MiB. Clear this by reading from both regions.
    Note: this assumes the amount of external RAM is >2M. If it is 2M or less, what this code does is undefined. If
    we ever support external RAM chips of 2M or smaller, this may need adjusting.
    */
    for (x = 0; x < 1024 * 64; x += 32) {
        i += psram[x];
        i += psram[x + (1024 * 1024 * 2)];
    }
#endif

    if (cache_was_disabled & (1 << 0)) {
        while (DPORT_GET_PERI_REG_BITS2(DPORT_PRO_DCACHE_DBUG0_REG, DPORT_PRO_CACHE_STATE, DPORT_PRO_CACHE_STATE_S) != 1) ;
        DPORT_SET_PERI_REG_BITS(DPORT_PRO_CACHE_CTRL_REG, 1, 0, DPORT_PRO_CACHE_ENABLE_S);
    }
#ifndef CONFIG_FREERTOS_UNICORE
    if (cache_was_disabled & (1 << 1)) {
        while (DPORT_GET_PERI_REG_BITS2(DPORT_APP_DCACHE_DBUG0_REG, DPORT_APP_CACHE_STATE, DPORT_APP_CACHE_STATE_S) != 1);
        DPORT_SET_PERI_REG_BITS(DPORT_APP_CACHE_CTRL_REG, 1, 0, DPORT_APP_CACHE_ENABLE_S);
    }
#endif
}
