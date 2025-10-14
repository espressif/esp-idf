/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_attr.h"
#include "esp_heap_caps.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include <assert.h>

static uint8_t log_count;
#if CONFIG_BT_LE_MEM_CHECK_ENABLED
static uint16_t mem_count_limit = 0;
static uint16_t curr_mem_count;
#endif // CONFIG_BT_LE_MEM_CHECK_ENABLED
IRAM_ATTR void *bt_osi_mem_malloc(size_t size)
{
    void *mem = NULL;
#ifdef CONFIG_BT_NIMBLE_MEM_ALLOC_MODE_INTERNAL
    mem = heap_caps_malloc(size, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#elif CONFIG_BT_NIMBLE_MEM_ALLOC_MODE_EXTERNAL
    mem =  heap_caps_malloc(size, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT);
#elif CONFIG_BT_NIMBLE_MEM_ALLOC_MODE_IRAM_8BIT
    mem =  heap_caps_malloc_prefer(size, 2, MALLOC_CAP_INTERNAL|MALLOC_CAP_IRAM_8BIT, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#else
    mem =  malloc(size);
#endif
    if (!mem) {
        log_count ++;
        if ((log_count % 100) == 0) {
            ESP_EARLY_LOGI("ESP_LOG_INFO","malloc failed (size %zu)",size);
            log_count = 0;
        }
       assert(mem != NULL);
    }
    return mem;
}

IRAM_ATTR void *bt_osi_mem_calloc(size_t n, size_t size)
{
#ifdef CONFIG_BT_NIMBLE_MEM_ALLOC_MODE_INTERNAL
    return heap_caps_calloc(n, size, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#elif CONFIG_BT_NIMBLE_MEM_ALLOC_MODE_EXTERNAL
    return heap_caps_calloc(n, size, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT);
#elif CONFIG_BT_NIMBLE_MEM_ALLOC_MODE_IRAM_8BIT
    return heap_caps_calloc_prefer(n, size, 2, MALLOC_CAP_INTERNAL|MALLOC_CAP_IRAM_8BIT, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#else
    return calloc(n, size);
#endif
}

IRAM_ATTR void *bt_osi_mem_malloc_internal(size_t size)
{
#if CONFIG_BT_LE_MEM_CHECK_ENABLED
    if (mem_count_limit) {
        if (curr_mem_count > mem_count_limit) {
            return NULL;
        }
        curr_mem_count ++;
    }
#endif  // CONFIG_BT_LE_MEM_CHECK_ENABLED
    return heap_caps_malloc(size, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT|MALLOC_CAP_DMA);
}

IRAM_ATTR void *bt_osi_mem_calloc_internal(size_t n, size_t size)
{
#if CONFIG_BT_LE_MEM_CHECK_ENABLED
    if (mem_count_limit) {
        if (curr_mem_count > mem_count_limit) {
            return NULL;
        }
        curr_mem_count ++;
    }
#endif  // CONFIG_BT_LE_MEM_CHECK_ENABLED
    return heap_caps_calloc(n, size, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT|MALLOC_CAP_DMA);
}

IRAM_ATTR void bt_osi_mem_free(void *ptr)
{
    heap_caps_free(ptr);
}

#if CONFIG_BT_LE_MEM_CHECK_ENABLED
void bt_osi_mem_count_limit_set(uint16_t count_limit)
{
    mem_count_limit = count_limit;
    curr_mem_count = 0;
}
#endif // CONFIG_BT_LE_MEM_CHECK_ENABLED
