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
    ESP_RETURN_ON_FALSE_ISR(mmu_hal_check_valid_ext_vaddr_region(0, (uint32_t)addr, size, MMU_VADDR_DATA), ESP_ERR_INVALID_ARG, TAG, "invalid address");
    bool both_dir = (flags & ESP_CACHE_MSYNC_FLAG_DIR_C2M) && (flags & ESP_CACHE_MSYNC_FLAG_DIR_M2C);
    ESP_RETURN_ON_FALSE_ISR(!both_dir, ESP_ERR_INVALID_ARG, TAG, "both C2M and M2C directions are selected, you should only select one");
    uint32_t data_cache_line_size = cache_hal_get_cache_line_size(CACHE_TYPE_DATA);
    if ((flags & ESP_CACHE_MSYNC_FLAG_UNALIGNED) == 0) {
        bool aligned_addr = (((uint32_t)addr % data_cache_line_size) == 0) && ((size % data_cache_line_size) == 0);
        ESP_RETURN_ON_FALSE_ISR(aligned_addr, ESP_ERR_INVALID_ARG, TAG, "start address, end address or the size is(are) not aligned with the data cache line size (%d)B", data_cache_line_size);
    }

    uint32_t vaddr = (uint32_t)addr;
    if (flags & ESP_CACHE_MSYNC_FLAG_DIR_M2C) {
        ESP_EARLY_LOGD(TAG, "M2C DIR");

        esp_os_enter_critical_safe(&s_spinlock);
        //Add preload feature / flag here, IDF-7800
        cache_hal_invalidate_addr(vaddr, size);
        esp_os_exit_critical_safe(&s_spinlock);

    } else {
        ESP_EARLY_LOGD(TAG, "C2M DIR");

#if SOC_CACHE_WRITEBACK_SUPPORTED

        esp_os_enter_critical_safe(&s_spinlock);
        cache_hal_writeback_addr(vaddr, size);
        esp_os_exit_critical_safe(&s_spinlock);

        if (flags & ESP_CACHE_MSYNC_FLAG_INVALIDATE) {
            esp_os_enter_critical_safe(&s_spinlock);
            cache_hal_invalidate_addr(vaddr, size);
            esp_os_exit_critical_safe(&s_spinlock);
        }
#endif
    }

    return ESP_OK;
}
