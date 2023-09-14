/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_rom_caps.h"
#include "soc/soc_caps.h"
#include "hal/mmu_hal.h"
#include "hal/cache_hal.h"
#include "esp_cache.h"
#include "esp_private/critical_section.h"

static const char *TAG = "cache";
DEFINE_CRIT_SECTION_LOCK_STATIC(s_spinlock);


esp_err_t esp_cache_msync(void *addr, size_t size, int flags)
{
    ESP_RETURN_ON_FALSE_ISR(addr, ESP_ERR_INVALID_ARG, TAG, "null pointer");

    uint32_t addr_end = 0;
    bool ovf = __builtin_add_overflow((uint32_t)addr, size, &addr_end);
    ESP_EARLY_LOGV(TAG, "addr_end: 0x%x\n", addr_end);
    ESP_RETURN_ON_FALSE_ISR(!ovf, ESP_ERR_INVALID_ARG, TAG, "wrong size, total size overflow");

    bool both_dir = (flags & ESP_CACHE_MSYNC_FLAG_DIR_C2M) && (flags & ESP_CACHE_MSYNC_FLAG_DIR_M2C);
    ESP_RETURN_ON_FALSE_ISR(!both_dir, ESP_ERR_INVALID_ARG, TAG, "both C2M and M2C directions are selected, you should only select one");

    uint32_t vaddr = (uint32_t)addr;
    bool valid = false;
    uint32_t cache_level = 0;
    uint32_t cache_id = 0;
    valid = cache_hal_vaddr_to_cache_level_id(vaddr, size, &cache_level, &cache_id);
    ESP_RETURN_ON_FALSE_ISR(valid, ESP_ERR_INVALID_ARG, TAG, "invalid addr or null pointer");

    uint32_t data_cache_line_size = cache_hal_get_cache_line_size(CACHE_TYPE_DATA, cache_level);
    if ((flags & ESP_CACHE_MSYNC_FLAG_UNALIGNED) == 0) {
        bool aligned_addr = (((uint32_t)addr % data_cache_line_size) == 0) && ((size % data_cache_line_size) == 0);
        ESP_RETURN_ON_FALSE_ISR(aligned_addr, ESP_ERR_INVALID_ARG, TAG, "start address: 0x%x, or the size: 0x%x is(are) not aligned with the data cache line size (0x%x)B", (uint32_t)addr, size, data_cache_line_size);
    }

    if (flags & ESP_CACHE_MSYNC_FLAG_DIR_M2C) {
        ESP_EARLY_LOGV(TAG, "M2C DIR");

        esp_os_enter_critical_safe(&s_spinlock);
        //Add preload feature / flag here, IDF-7800
        valid = cache_hal_invalidate_addr(vaddr, size);
        esp_os_exit_critical_safe(&s_spinlock);
        assert(valid);
    } else {
        ESP_EARLY_LOGV(TAG, "C2M DIR");

#if SOC_CACHE_WRITEBACK_SUPPORTED

        esp_os_enter_critical_safe(&s_spinlock);
        valid = cache_hal_writeback_addr(vaddr, size);
        esp_os_exit_critical_safe(&s_spinlock);
        assert(valid);

        if (flags & ESP_CACHE_MSYNC_FLAG_INVALIDATE) {
            esp_os_enter_critical_safe(&s_spinlock);
            valid &= cache_hal_invalidate_addr(vaddr, size);
            esp_os_exit_critical_safe(&s_spinlock);
        }
        assert(valid);
#endif
    }

    return ESP_OK;
}
