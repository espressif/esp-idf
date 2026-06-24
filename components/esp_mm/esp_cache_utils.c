/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
*
* SPDX-License-Identifier: Apache-2.0
*/

#include <sys/param.h>
#include <inttypes.h>
#include <string.h>
#include "sys/lock.h"
#include "sdkconfig.h"
#include "esp_check.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "esp_memory_utils.h"
#include "soc/soc_caps.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#include "esp_private/esp_cache_private.h"
#include "esp_private/critical_section.h"
#if __riscv
#include "riscv/rv_utils.h"
#endif

#define ALIGN_UP_BY(num, align) (((num) + ((align) - 1)) & ~((align) - 1))

void esp_cache_suspend_ext_mem_cache(void)
{
#if (CONFIG_SPIRAM && SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE)
    /**
     * before suspending the external mem cache, writeback internal mem cache content back to external mem cache
     * to avoid stuck issue caused by internal mem cache auto-writeback
     */
    cache_ll_writeback_all(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA, CACHE_LL_ID_ALL);
#endif
    cache_hal_suspend(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
}

void esp_cache_resume_ext_mem_cache(void)
{
    cache_hal_resume(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
}

/*-----------------------------------------------------------------------------
 *                      Cache Freeze Related
 *----------------------------------------------------------------------------*/
#if SOC_CACHE_FREEZE_SUPPORTED
DEFINE_CRIT_SECTION_LOCK_STATIC(s_spinlock);

void esp_cache_freeze_ext_mem_cache(void)
{
#if (CONFIG_SPIRAM && SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE)
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    int cpuid = xPortGetCoreID();
    uint32_t other_cpuid = (cpuid == 0) ? 1 : 0;
    esp_cpu_stall(other_cpuid);
#else
    //single core mode, don't need to stall other core
#endif
    /**
     * before freezing the external mem cache, writeback internal mem cache content back to external mem cache
     * to avoid stuck issue caused by internal mem cache auto-writeback
     */
    cache_ll_writeback_all(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA, CACHE_LL_ID_ALL);
#endif

    cache_hal_freeze(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);

#if (CONFIG_SPIRAM && SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE)
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    esp_cpu_unstall(other_cpuid);
#else
    //single core mode, don't need to unstall other core
#endif
#endif
}

void esp_cache_unfreeze_ext_mem_cache(void)
{
    cache_hal_unfreeze(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
}

static inline bool s_task_stack_is_sane_when_cache_frozen(void)
{
    const void *sp = (const void *)esp_cpu_get_sp();

    return esp_ptr_in_dram(sp)
#if CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP
           || esp_ptr_in_rtc_dram_fast(sp)
#endif
           ;
}

void esp_cache_freeze_caches_disable_interrupts(void)
{
    assert(s_task_stack_is_sane_when_cache_frozen());
    esp_os_enter_critical_safe(&s_spinlock);

    /**
     * - disable non-iram interrupt on current core
     * - current core call cache freeze
     * - external access from other cores will hang on cache
     */
    esp_intr_noniram_disable();
    esp_cache_freeze_ext_mem_cache();
}

void esp_cache_unfreeze_caches_enable_interrupts(void)
{
    esp_cache_unfreeze_ext_mem_cache();
    esp_intr_noniram_enable();
    esp_os_exit_critical_safe(&s_spinlock);
}
#endif
