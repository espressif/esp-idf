/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_attr.h"
#include "esp_heap_caps.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include <assert.h>
#include <stdint.h>

#if CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
static size_t controller_mem_used_size = 0;
#endif // CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED

#if CONFIG_BT_LE_MEM_CHECK_ENABLED
static uint16_t mem_count_limit = 0;
static uint16_t curr_mem_count;
#endif // CONFIG_BT_LE_MEM_CHECK_ENABLED
/* Updated with GCC atomic builtins since allocation failures can race.
 * DRAM_ATTR ensures the variable is reachable when flash cache is disabled. */
static DRAM_ATTR uint32_t log_count = 0;

/* Both helpers are placed in IRAM (matching their callers) so the compiler
 * cannot emit them into flash even if it decides not to inline them.
 * ESP_DRAM_LOGI places the format string in DRAM, keeping the whole log path
 * flash-cache-safe. */
#if !CONFIG_BT_NIMBLE_LOW_SPEED_MODE
IRAM_ATTR
#endif
static void bt_osi_log_alloc_failure(size_t bytes)
{
    uint32_t count = __atomic_add_fetch(&log_count, 1, __ATOMIC_RELAXED);
    if (count == 1 || (count % 100) == 0) {
        ESP_DRAM_LOGI("BT_OSI_MEM", "alloc failed (size %zu)", bytes);
    }
}


#if !CONFIG_BT_NIMBLE_LOW_SPEED_MODE
IRAM_ATTR
#endif
void *bt_osi_mem_malloc(size_t size)
{
    void *mem = NULL;
#if CONFIG_BT_LE_MEM_CHECK_ENABLED
    if (mem_count_limit) {
        if (curr_mem_count > mem_count_limit) {
            return NULL;
        }
        curr_mem_count++;
    }
#endif  // CONFIG_BT_LE_MEM_CHECK_ENABLED
#ifdef CONFIG_BT_NIMBLE_MEM_ALLOC_MODE_INTERNAL
    mem = heap_caps_malloc(size, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#elif CONFIG_BT_NIMBLE_MEM_ALLOC_MODE_EXTERNAL
    mem = heap_caps_malloc(size, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT);
#elif CONFIG_BT_NIMBLE_MEM_ALLOC_MODE_IRAM_8BIT
    mem = heap_caps_malloc_prefer(size, 2, MALLOC_CAP_INTERNAL|MALLOC_CAP_IRAM_8BIT, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#elif CONFIG_BT_NIMBLE_MEM_ALLOC_MODE_DEFAULT
    mem = malloc(size);
#else
    /* Bluedroid / controller-only: NimBLE mem-mode Kconfig not present.
     * Preserve historical INTERNAL alloc used by msys and controller paths. */
    mem = heap_caps_malloc(size, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#endif
    if (!mem) {
        bt_osi_log_alloc_failure(size);
    }
#if CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
    if(mem) {
         controller_mem_used_size += heap_caps_get_allocated_size(mem);
    }
#endif // CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
    return mem;
}

#if !CONFIG_BT_NIMBLE_LOW_SPEED_MODE
IRAM_ATTR
#endif
void *bt_osi_mem_calloc(size_t n, size_t size)
{
    void *mem = NULL;
#if CONFIG_BT_LE_MEM_CHECK_ENABLED
    if (mem_count_limit) {
        if (curr_mem_count > mem_count_limit) {
            return NULL;
        }
        curr_mem_count++;
    }
#endif  // CONFIG_BT_LE_MEM_CHECK_ENABLED
#ifdef CONFIG_BT_NIMBLE_MEM_ALLOC_MODE_INTERNAL
    mem = heap_caps_calloc(n, size, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#elif CONFIG_BT_NIMBLE_MEM_ALLOC_MODE_EXTERNAL
    mem = heap_caps_calloc(n, size, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT);
#elif CONFIG_BT_NIMBLE_MEM_ALLOC_MODE_IRAM_8BIT
    mem = heap_caps_calloc_prefer(n, size, 2, MALLOC_CAP_INTERNAL|MALLOC_CAP_IRAM_8BIT, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#elif CONFIG_BT_NIMBLE_MEM_ALLOC_MODE_DEFAULT
    mem = calloc(n, size);
#else
    /* Bluedroid / controller-only: keep INTERNAL (see bt_osi_mem_malloc). */
    mem = heap_caps_calloc(n, size, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#endif
    if (!mem) {
        size_t total = (size && n > SIZE_MAX / size) ? SIZE_MAX : n * size;
        bt_osi_log_alloc_failure(total);
    }
#if CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
    if(mem) {
         controller_mem_used_size += heap_caps_get_allocated_size(mem);
    }
#endif // CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
    return mem;
}

#if !CONFIG_BT_NIMBLE_LOW_SPEED_MODE
IRAM_ATTR
#endif
void *bt_osi_mem_realloc(void *ptr, size_t size)
{
    void *mem = NULL;
#if CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
    size_t old_size = 0;
    if (ptr) {
        old_size = heap_caps_get_allocated_size(ptr);
    }
#endif

#ifdef CONFIG_BT_NIMBLE_MEM_ALLOC_MODE_INTERNAL
    mem = heap_caps_realloc(ptr, size, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#elif CONFIG_BT_NIMBLE_MEM_ALLOC_MODE_EXTERNAL
    mem = heap_caps_realloc(ptr, size, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT);
#elif CONFIG_BT_NIMBLE_MEM_ALLOC_MODE_IRAM_8BIT
    mem = heap_caps_realloc_prefer(ptr, size, 2,
                                   MALLOC_CAP_INTERNAL|MALLOC_CAP_IRAM_8BIT,
                                   MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#elif CONFIG_BT_NIMBLE_MEM_ALLOC_MODE_DEFAULT
    mem = realloc(ptr, size);
#else
    /* Bluedroid / controller-only: keep INTERNAL (see bt_osi_mem_malloc). */
    mem = heap_caps_realloc(ptr, size, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#endif
    if (!mem && size != 0) {
        bt_osi_log_alloc_failure(size);
    }

#if CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
    if (mem) {
        size_t new_size = heap_caps_get_allocated_size(mem);
        controller_mem_used_size = controller_mem_used_size - old_size + new_size;
    } else if (ptr && size == 0) {
        controller_mem_used_size -= old_size;
    }
#endif

    return mem;
}

#if !CONFIG_BT_NIMBLE_LOW_SPEED_MODE
IRAM_ATTR
#endif
void *bt_osi_mem_malloc_internal(size_t size)
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
    if (!mem_ptr) {
        bt_osi_log_alloc_failure(size);
    }
#if CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
    if (mem_ptr) {
        controller_mem_used_size += heap_caps_get_allocated_size(mem_ptr);
    }
#endif // CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
    return mem_ptr;
}


void *
bt_osi_mem_calloc_internal(size_t n, size_t size)
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
    if (!mem_ptr) {
        size_t total = (size && n > SIZE_MAX / size) ? SIZE_MAX : n * size;
        bt_osi_log_alloc_failure(total);
    }
#if CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
    if (mem_ptr) {
        controller_mem_used_size += heap_caps_get_allocated_size(mem_ptr);
    }
#endif // CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
    return mem_ptr;
}

void
bt_osi_mem_free_internal(void *ptr)
{
#if CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
    if (ptr) {
        size_t alloc_size = heap_caps_get_allocated_size(ptr);
        // assert(controller_mem_used_size >= alloc_size);
        controller_mem_used_size -= alloc_size;
    }
#endif // CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
    if (ptr) {
        heap_caps_free(ptr);
    }
}

void
bt_osi_mem_free(void *ptr)
{
    bt_osi_mem_free_internal(ptr);
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

uint32_t esp_ble_controller_used_heap_size_get(void)
{
    return bt_osi_mem_internal_used_size_get();
}
#endif // CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
