/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
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
#include "esp_heap_caps.h"
#include "esp_rom_caps.h"
#include "soc/soc_caps.h"
#include "hal/mmu_hal.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#include "esp_cache.h"
#include "esp_compiler.h"
#include "esp_private/esp_cache_private.h"
#include "esp_private/critical_section.h"

static const char *TAG = "cache";

#define ALIGN_UP_BY(num, align) (((num) + ((align) - 1)) & ~((align) - 1))

DEFINE_CRIT_SECTION_LOCK_STATIC(s_spinlock);
#if CONFIG_ESP_MM_CACHE_MSYNC_C2M_CHUNKED_OPS
static _lock_t s_mutex;
#endif

#if SOC_CACHE_WRITEBACK_SUPPORTED
static void s_c2m_ops(uint32_t vaddr, size_t size)
{
#if CONFIG_ESP_MM_CACHE_MSYNC_C2M_CHUNKED_OPS
    if (!xPortInIsrContext()) {
        bool valid = true;
        size_t offset = 0;
        while (offset < size) {
            size_t chunk_len = ((size - offset) > CONFIG_ESP_MM_CACHE_MSYNC_C2M_CHUNKED_OPS_MAX_LEN) ? CONFIG_ESP_MM_CACHE_MSYNC_C2M_CHUNKED_OPS_MAX_LEN : (size - offset);
            esp_os_enter_critical_safe(&s_spinlock);
            valid &= cache_hal_writeback_addr(vaddr + offset, chunk_len);
            esp_os_exit_critical_safe(&s_spinlock);
            offset += chunk_len;
        }
        assert(valid);
    } else
#endif
    {
        // Value unused if asserts are disabled
        bool __attribute__((unused)) valid = false;
        esp_os_enter_critical_safe(&s_spinlock);
        valid = cache_hal_writeback_addr(vaddr, size);
        esp_os_exit_critical_safe(&s_spinlock);
        assert(valid);
    }
}
#endif

//no ops if ISR context or critical section context
static void s_acquire_mutex_from_task_context(void)
{
#if CONFIG_ESP_MM_CACHE_MSYNC_C2M_CHUNKED_OPS
    if (xPortCanYield()) {
        _lock_acquire(&s_mutex);
        ESP_LOGD(TAG, "mutex is taken");
    }
#endif  //#if CONFIG_ESP_MM_CACHE_MSYNC_C2M_CHUNKED_OPS
}

//no ops if ISR context or critical section context
static void s_release_mutex_from_task_context(void)
{
#if CONFIG_ESP_MM_CACHE_MSYNC_C2M_CHUNKED_OPS
    if (xPortCanYield()) {
        _lock_release(&s_mutex);
        ESP_LOGD(TAG, "mutex is free");
    }
#endif  //#if CONFIG_ESP_MM_CACHE_MSYNC_C2M_CHUNKED_OPS
}

esp_err_t esp_cache_msync(void *addr, size_t size, int flags)
{
    ESP_RETURN_ON_FALSE_ISR(addr, ESP_ERR_INVALID_ARG, TAG, "null pointer");

    uint32_t addr_end = 0;
    bool ovf = __builtin_add_overflow((uint32_t)addr, size, &addr_end);
    ESP_EARLY_LOGV(TAG, "addr_end: 0x%" PRIx32, addr_end);
    ESP_RETURN_ON_FALSE_ISR(!ovf, ESP_ERR_INVALID_ARG, TAG, "wrong size, total size overflow");

    bool both_dir = (flags & ESP_CACHE_MSYNC_FLAG_DIR_C2M) && (flags & ESP_CACHE_MSYNC_FLAG_DIR_M2C);
    bool both_type = (flags & ESP_CACHE_MSYNC_FLAG_TYPE_DATA) && (flags & ESP_CACHE_MSYNC_FLAG_TYPE_INST);
    ESP_RETURN_ON_FALSE_ISR(!both_dir && !both_type, ESP_ERR_INVALID_ARG, TAG, "both C2M and M2C directions, or both data and instruction type are selected, you should only select one direction or one type");

    uint32_t vaddr = (uint32_t)addr;
    bool valid = false;
    uint32_t cache_level = 0;
    uint32_t cache_id = 0;
    valid = cache_hal_vaddr_to_cache_level_id(vaddr, size, &cache_level, &cache_id);
    ESP_RETURN_ON_FALSE_ISR(valid, ESP_ERR_INVALID_ARG, TAG, "invalid addr or null pointer");

    cache_type_t cache_type = CACHE_TYPE_DATA;
    if (flags & ESP_CACHE_MSYNC_FLAG_TYPE_INST) {
        cache_type = CACHE_TYPE_INSTRUCTION;
    }
    uint32_t cache_line_size = cache_hal_get_cache_line_size(cache_level, cache_type);
    if ((flags & ESP_CACHE_MSYNC_FLAG_UNALIGNED) == 0) {
        bool aligned_addr = (((uint32_t)addr % cache_line_size) == 0) && ((size % cache_line_size) == 0);
        ESP_RETURN_ON_FALSE_ISR(aligned_addr, ESP_ERR_INVALID_ARG, TAG, "start address: 0x%" PRIx32 ", or the size: 0x%" PRIx32 " is(are) not aligned with cache line size (0x%" PRIx32 ")B", (uint32_t)addr, (uint32_t)size, cache_line_size);
    }

    s_acquire_mutex_from_task_context();
    if (flags & ESP_CACHE_MSYNC_FLAG_DIR_M2C) {
        ESP_EARLY_LOGV(TAG, "M2C DIR");

        if (flags & ESP_CACHE_MSYNC_FLAG_UNALIGNED) {
            ESP_RETURN_ON_FALSE_ISR(false, ESP_ERR_INVALID_ARG, TAG, "M2C direction doesn't allow ESP_CACHE_MSYNC_FLAG_UNALIGNED");
        }

        esp_os_enter_critical_safe(&s_spinlock);
        //Add preload feature / flag here, IDF-7800
        valid = cache_hal_invalidate_addr(vaddr, size);
        esp_os_exit_critical_safe(&s_spinlock);
        assert(valid);
    } else {
        ESP_EARLY_LOGV(TAG, "C2M DIR");
        if (flags & ESP_CACHE_MSYNC_FLAG_TYPE_INST) {
            ESP_RETURN_ON_FALSE_ISR(false, ESP_ERR_INVALID_ARG, TAG, "C2M direction doesn't support instruction type");
        }

#if SOC_CACHE_WRITEBACK_SUPPORTED
        s_c2m_ops(vaddr, size);

        if (flags & ESP_CACHE_MSYNC_FLAG_INVALIDATE) {
            esp_os_enter_critical_safe(&s_spinlock);
            valid &= cache_hal_invalidate_addr(vaddr, size);
            esp_os_exit_critical_safe(&s_spinlock);
        }
        assert(valid);
#endif  //#if SOC_CACHE_WRITEBACK_SUPPORTED
    }
    s_release_mutex_from_task_context();

    return ESP_OK;
}

//The esp_cache_aligned_malloc function is marked deprecated but also called by other
//(also deprecated) functions in this file. In order to work around that generating warnings, it's
//split into a non-deprecated internal function and the stubbed external deprecated function.
static esp_err_t esp_cache_aligned_malloc_internal(size_t size, uint32_t heap_caps, void **out_ptr, size_t *actual_size)
{
    ESP_RETURN_ON_FALSE_ISR(out_ptr, ESP_ERR_INVALID_ARG, TAG, "null pointer");
    uint32_t valid_caps = MALLOC_CAP_SPIRAM | MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA;
    ESP_RETURN_ON_FALSE_ISR((heap_caps & valid_caps) > 0, ESP_ERR_INVALID_ARG, TAG, "not supported cap matches");

    uint32_t cache_level = CACHE_LL_LEVEL_INT_MEM;
    uint32_t data_cache_line_size = 0;
    void *ptr = NULL;

    if (heap_caps & MALLOC_CAP_SPIRAM) {
        cache_level = CACHE_LL_LEVEL_EXT_MEM;
    }

    data_cache_line_size = cache_hal_get_cache_line_size(cache_level, CACHE_TYPE_DATA);
    if (data_cache_line_size == 0) {
        //default alignment
        data_cache_line_size = 4;
    }

    size = ALIGN_UP_BY(size, data_cache_line_size);
    ptr = heap_caps_aligned_alloc(data_cache_line_size, size, (uint32_t)heap_caps);
    if (!ptr) {
        return ESP_ERR_NO_MEM;
    }

    *out_ptr = ptr;
    if (actual_size) {
        *actual_size = size;
    }

    return ESP_OK;
}

//this is the deprecated stub for the above function
esp_err_t esp_cache_aligned_malloc(size_t size, uint32_t heap_caps, void **out_ptr, size_t *actual_size)
{
    return esp_cache_aligned_malloc_internal(size, heap_caps, out_ptr, actual_size);
}

esp_err_t esp_cache_aligned_malloc_prefer(size_t size, void **out_ptr, size_t *actual_size, size_t flag_nums, ...)
{
    ESP_RETURN_ON_FALSE_ISR(out_ptr, ESP_ERR_INVALID_ARG, TAG, "null pointer");

    esp_err_t ret = ESP_FAIL;
    va_list argp;
    uint32_t flags = 0;
    va_start(argp, flag_nums);
    *out_ptr = NULL;

    while (flag_nums--) {
        flags = va_arg(argp, int);
        ret = esp_cache_aligned_malloc_internal(size, flags, out_ptr, actual_size);
        if (ret == ESP_OK) {
            break;
        }
    }

    va_end(argp);
    return ret;
}

esp_err_t esp_cache_aligned_calloc(size_t n, size_t size, uint32_t heap_caps, void **out_ptr, size_t *actual_size)
{
    ESP_RETURN_ON_FALSE_ISR(out_ptr, ESP_ERR_INVALID_ARG, TAG, "null pointer");

    esp_err_t ret = ESP_FAIL;
    size_t size_bytes = 0;
    bool ovf = false;

    ovf = __builtin_mul_overflow(n, size, &size_bytes);
    ESP_RETURN_ON_FALSE_ISR(!ovf, ESP_ERR_INVALID_ARG, TAG, "wrong size, total size overflow");

    void *ptr = NULL;
    ret = esp_cache_aligned_malloc_internal(size_bytes, heap_caps, &ptr, actual_size);
    if (ret == ESP_OK) {
        memset(ptr, 0, size_bytes);
        *out_ptr = ptr;
    }

    return ret;
}

esp_err_t esp_cache_aligned_calloc_prefer(size_t n, size_t size, void **out_ptr, size_t *actual_size, size_t flag_nums, ...)
{
    ESP_RETURN_ON_FALSE_ISR(out_ptr, ESP_ERR_INVALID_ARG, TAG, "null pointer");

    esp_err_t ret = ESP_FAIL;
    size_t size_bytes = 0;
    bool ovf = false;

    *out_ptr = NULL;
    ovf = __builtin_mul_overflow(n, size, &size_bytes);
    ESP_RETURN_ON_FALSE_ISR(!ovf, ESP_ERR_INVALID_ARG, TAG, "wrong size, total size overflow");

    void *ptr = NULL;
    va_list argp;
    va_start(argp, flag_nums);
    int arg;
    for (int i = 0; i < flag_nums; i++) {
        arg = va_arg(argp, int);
        ret = esp_cache_aligned_malloc_internal(size_bytes, arg, &ptr, actual_size);
        if (ret == ESP_OK) {
            ESP_COMPILER_DIAGNOSTIC_PUSH_IGNORE("-Wanalyzer-null-argument")
            memset(ptr, 0, size_bytes);
            *out_ptr = ptr;
            ESP_COMPILER_DIAGNOSTIC_POP("-Wanalyzer-null-argument")
            break;
        }

    }
    va_end(argp);

    return ret;
}

esp_err_t esp_cache_get_alignment(uint32_t heap_caps, size_t *out_alignment)
{
    ESP_RETURN_ON_FALSE(out_alignment, ESP_ERR_INVALID_ARG, TAG, "null pointer");

    uint32_t cache_level = CACHE_LL_LEVEL_INT_MEM;
    uint32_t data_cache_line_size = 0;

    if (heap_caps & MALLOC_CAP_SPIRAM) {
        cache_level = CACHE_LL_LEVEL_EXT_MEM;
    }

    data_cache_line_size = cache_hal_get_cache_line_size(cache_level, CACHE_TYPE_DATA);

    *out_alignment = data_cache_line_size;

    return ESP_OK;
}
