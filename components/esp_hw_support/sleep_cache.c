/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "esp_attr.h"
#include "esp_cpu.h"
#include "esp_private/sleep_cache.h"
#include "esp_private/cache_utils.h"
#include "hal/cache_ll.h"

#if SOC_PMU_SUPPORTED
#include "esp_private/esp_pmu.h"
#endif

static int s_cache_suspend_cnt = 0;
static uint32_t s_cache_state = 0;

// Must be called from critical sections.
void sleep_cache_suspend(void)
{
    s_cache_suspend_cnt++;
    if (s_cache_suspend_cnt == 1) {
#if CONFIG_ESP_SLEEP_CACHE_SAFE_ASSERTION && CONFIG_IDF_TARGET_ESP32P4
        // The implementation of P4 L2 cache suspend is to shut down MSPI AXI instead of shutting down Cache BUS.
        // If the access to external memory hits in the cache, it will not trigger a cache error. So in order to
        // fully check the access to external memory, writeback & invalidate is needed here.
        Cache_WriteBack_Invalidate_All(CACHE_MAP_MASK);
#endif
        spi_flash_disable_cache(esp_cpu_get_core_id(), &s_cache_state);
    }
}

// Must be called from critical sections.
void sleep_cache_resume(void)
{
    s_cache_suspend_cnt--;
    assert(s_cache_suspend_cnt >= 0 && DRAM_STR("cache resume doesn't match suspend ops"));
    if (s_cache_suspend_cnt == 0) {
        spi_flash_restore_cache(esp_cpu_get_core_id(), s_cache_state);
    }
}

#if !CONFIG_SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE && CONFIG_SPIRAM
void sleep_cache_safe_writeback(uint32_t sleep_flags)
{
#if CONFIG_PM_POWER_DOWN_CPU_IN_LIGHT_SLEEP && !SOC_PM_CACHE_RETENTION_BY_PAU && SOC_EXT_MEM_CACHE_TAG_IN_CPU_DOMAIN
#if SOC_PMU_SUPPORTED
    /* When SPIRAM is using, we need to use Cache_WriteBack_All to protect SPIRAM data
        because the cache powers down when we power down the CPU */
    if (sleep_flags & PMU_SLEEP_PD_CPU) {
        if (s_cache_suspend_cnt) {
            spi_flash_restore_cache(esp_cpu_get_core_id(), s_cache_state);
        }
        Cache_WriteBack_All();
        if (s_cache_suspend_cnt) {
            spi_flash_disable_cache(esp_cpu_get_core_id(), &s_cache_state);
        }
    }
#else
    (void)sleep_flags;
#endif
#elif CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_PM_CACHE_RETENTION_BY_PAU
    (void)sleep_flags;
    if (s_cache_suspend_cnt) {
        spi_flash_restore_cache(esp_cpu_get_core_id(), s_cache_state);
    }
    Cache_WriteBack_All(CACHE_MAP_L1_DCACHE);
    if (s_cache_suspend_cnt) {
        spi_flash_disable_cache(esp_cpu_get_core_id(), &s_cache_state);
    }
#else
    (void)sleep_flags;
#endif
}
#endif
