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
#if CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
static size_t controller_mem_used_size = 0;
static size_t host_mem_used_size = 0;
#endif // CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
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
#if CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
    if(mem) {
         host_mem_used_size += heap_caps_get_allocated_size(mem);
    }
#endif // CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
    return mem;
}

IRAM_ATTR void *bt_osi_mem_calloc(size_t n, size_t size)
{
    void *mem = NULL;
#ifdef CONFIG_BT_NIMBLE_MEM_ALLOC_MODE_INTERNAL
    mem = heap_caps_calloc(n, size, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#elif CONFIG_BT_NIMBLE_MEM_ALLOC_MODE_EXTERNAL
    mem = heap_caps_calloc(n, size, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT);
#elif CONFIG_BT_NIMBLE_MEM_ALLOC_MODE_IRAM_8BIT
    mem = heap_caps_calloc_prefer(n, size, 2, MALLOC_CAP_INTERNAL|MALLOC_CAP_IRAM_8BIT, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#else
    mem = calloc(n, size);
#endif
#if CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
    if(mem) {
         host_mem_used_size += heap_caps_get_allocated_size(mem);
    }
#endif // CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
    return mem;
}

IRAM_ATTR void *bt_osi_mem_malloc_internal(size_t size)
{
    void *mem_ptr;
#if CONFIG_BT_LE_MEM_CHECK_ENABLED
    if (mem_count_limit) {
        if (curr_mem_count > mem_count_limit) {
            return NULL;
        }
        curr_mem_count ++;
    }
#endif  // CONFIG_BT_LE_MEM_CHECK_ENABLED
    mem_ptr = heap_caps_malloc(size, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT|MALLOC_CAP_DMA);
#if CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
    if (mem_ptr) {
        controller_mem_used_size += heap_caps_get_allocated_size(mem_ptr);
    }
#endif // CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
    return mem_ptr;
}

IRAM_ATTR void *bt_osi_mem_calloc_internal(size_t n, size_t size)
{
    void *mem_ptr;
#if CONFIG_BT_LE_MEM_CHECK_ENABLED
    if (mem_count_limit) {
        if (curr_mem_count > mem_count_limit) {
            return NULL;
        }
        curr_mem_count ++;
    }
#endif  // CONFIG_BT_LE_MEM_CHECK_ENABLED
    mem_ptr = heap_caps_calloc(n, size, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT|MALLOC_CAP_DMA);
#if CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
    if (mem_ptr) {
        controller_mem_used_size += heap_caps_get_allocated_size(mem_ptr);
    }
#endif // CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
    return mem_ptr;
}

IRAM_ATTR void bt_osi_mem_free_internal(void *ptr)
{
#if CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
    if (ptr) {
        size_t alloc_size = heap_caps_get_allocated_size(ptr);
        // assert(controller_mem_used_size >= alloc_size);
        controller_mem_used_size -= alloc_size;
    }
#endif // CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
    heap_caps_free(ptr);
}

IRAM_ATTR void bt_osi_mem_free(void *ptr)
{
#if CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
    if (ptr) {
        size_t alloc_size = heap_caps_get_allocated_size(ptr);
        // assert(host_mem_used_size >= alloc_size);
        host_mem_used_size -= alloc_size;
    }
#endif // CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
    heap_caps_free(ptr);
}

#if CONFIG_BT_LE_MEM_CHECK_ENABLED
void bt_osi_mem_count_limit_set(uint16_t count_limit)
{
    mem_count_limit = count_limit;
    curr_mem_count = 0;
}
#endif // CONFIG_BT_LE_MEM_CHECK_ENABLED

#if CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
size_t
bt_osi_mem_internal_used_size_get(void)
{
    return controller_mem_used_size;
}

size_t
bt_osi_mem_used_size_get(void)
{
    return host_mem_used_size;
}

uint32_t esp_ble_controller_used_heap_size_get(void)
{
    return bt_osi_mem_internal_used_size_get();
}

#if CONFIG_BT_NIMBLE_ENABLED
uint32_t esp_host_used_heap_size_get(void)
{
    return bt_osi_mem_used_size_get();
}
#endif // CONFIG_BT_NIMBLE_ENABLED
#endif // CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
