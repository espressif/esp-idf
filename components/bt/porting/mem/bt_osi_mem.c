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

#if CONFIG_BT_NIMBLE_MEM_DEBUG

#define NIMBLE_MEM_DBG_INFO_MAX    1024*3
typedef struct {
    void *p;
    int size;
    const char *func;
    int line;
} nimble_mem_dbg_info_t;

static uint32_t nimble_mem_dbg_count = 0;
static nimble_mem_dbg_info_t nimble_mem_dbg_info[NIMBLE_MEM_DBG_INFO_MAX];
static uint32_t nimble_mem_dbg_current_size = 0;
static uint32_t nimble_mem_dbg_max_size = 0;

#define NIMBLE_MEM_DBG_MAX_SECTION_NUM 5
typedef struct {
    bool used;
    uint32_t max_size;
} nimble_mem_dbg_max_size_section_t;
static nimble_mem_dbg_max_size_section_t nimble_mem_dbg_max_size_section[NIMBLE_MEM_DBG_MAX_SECTION_NUM];

void nimble_mem_dbg_init(void)
{
    int i;

    for (i = 0; i < NIMBLE_MEM_DBG_INFO_MAX; i++) {
        nimble_mem_dbg_info[i].p = NULL;
        nimble_mem_dbg_info[i].size = 0;
        nimble_mem_dbg_info[i].func = NULL;
        nimble_mem_dbg_info[i].line = 0;
    }
    nimble_mem_dbg_count = 0;
    nimble_mem_dbg_current_size = 0;
    nimble_mem_dbg_max_size = 0;

    for (i = 0; i < NIMBLE_MEM_DBG_MAX_SECTION_NUM; i++){
        nimble_mem_dbg_max_size_section[i].used = false;
        nimble_mem_dbg_max_size_section[i].max_size = 0;
    }
}

void nimble_mem_dbg_record(void *p, int size, const char *func, int line)
{
    int i;

    if (!p || size == 0) {
        ESP_LOGE("BT_NIMBLE_MEM", "%s invalid !!\n", __func__);
        return;
    }

    for (i = 0; i < NIMBLE_MEM_DBG_INFO_MAX; i++) {
        if (nimble_mem_dbg_info[i].p == NULL) {
            nimble_mem_dbg_info[i].p = p;
            nimble_mem_dbg_info[i].size = size;
            nimble_mem_dbg_info[i].func = func;
            nimble_mem_dbg_info[i].line = line;
            nimble_mem_dbg_count++;
            break;
        }
    }

    if (i >= NIMBLE_MEM_DBG_INFO_MAX) {
        ESP_LOGE("BT_NIMBLE_MEM", "%s full %s %d !!\n", __func__, func, line);
    }

    nimble_mem_dbg_current_size += size;
    if(nimble_mem_dbg_max_size < nimble_mem_dbg_current_size) {
        nimble_mem_dbg_max_size = nimble_mem_dbg_current_size;
    }

    for (i = 0; i < NIMBLE_MEM_DBG_MAX_SECTION_NUM; i++){
        if (nimble_mem_dbg_max_size_section[i].used) {
            if(nimble_mem_dbg_max_size_section[i].max_size < nimble_mem_dbg_current_size) {
                nimble_mem_dbg_max_size_section[i].max_size = nimble_mem_dbg_current_size;
            }
        }
    }
}

void nimble_mem_dbg_clean(void *p, const char *func, int line)
{
    int i;

    if (!p) {
        ESP_LOGE("BT_NIMBLE_MEM", "%s %d free->%p invalid\n", func, line, p);
        return;
    }

    for (i = 0; i < NIMBLE_MEM_DBG_INFO_MAX; i++) {
        if (nimble_mem_dbg_info[i].p == p) {
            nimble_mem_dbg_current_size -= nimble_mem_dbg_info[i].size;
            nimble_mem_dbg_info[i].p = NULL;
            nimble_mem_dbg_info[i].size = 0;
            nimble_mem_dbg_info[i].func = NULL;
            nimble_mem_dbg_info[i].line = 0;
            nimble_mem_dbg_count--;
            break;
        }
    }

    if (i >= NIMBLE_MEM_DBG_INFO_MAX) {
        ESP_LOGE("BT_NIMBLE_MEM", "%s full %s %d !!\n", __func__, func, line);
    }
}

void nimble_mem_dbg_show(void)
{
    int i;

    for (i = 0; i < NIMBLE_MEM_DBG_INFO_MAX; i++) {
        if (nimble_mem_dbg_info[i].p || nimble_mem_dbg_info[i].size != 0 ) {
            ESP_LOGE("BT_NIMBLE_MEM", "--> p %p, s %d, f %s, l %d\n",
                     nimble_mem_dbg_info[i].p, nimble_mem_dbg_info[i].size,
                     nimble_mem_dbg_info[i].func, nimble_mem_dbg_info[i].line);
        }
    }
    ESP_LOGE("BT_NIMBLE_MEM", "--> count %ld\n", nimble_mem_dbg_count);
    ESP_LOGE("BT_NIMBLE_MEM", "--> size %ldB\n--> max size %ldB\n",
             nimble_mem_dbg_current_size, nimble_mem_dbg_max_size);
}

uint32_t nimble_mem_dbg_get_max_size(void)
{
    return nimble_mem_dbg_max_size;
}

uint32_t nimble_mem_dbg_get_current_size(void)
{
    return nimble_mem_dbg_current_size;
}

void nimble_mem_dbg_set_section_start(uint8_t index)
{
    if (index >= NIMBLE_MEM_DBG_MAX_SECTION_NUM) {
        ESP_LOGE("BT_NIMBLE_MEM", "Then range of index should be between 0 and %d, current index is %d.\n",
                            NIMBLE_MEM_DBG_MAX_SECTION_NUM - 1, index);
        return;
    }

    if (nimble_mem_dbg_max_size_section[index].used) {
        ESP_LOGW("BT_NIMBLE_MEM", "This index(%d) has been started, restart it.\n", index);
    }

    nimble_mem_dbg_max_size_section[index].used = true;
    nimble_mem_dbg_max_size_section[index].max_size = nimble_mem_dbg_current_size;
}

void nimble_mem_dbg_set_section_end(uint8_t index)
{
    if (index >= NIMBLE_MEM_DBG_MAX_SECTION_NUM) {
        ESP_LOGE("BT_NIMBLE_MEM", "Then range of index should be between 0 and %d, current index is %d.\n",
                            NIMBLE_MEM_DBG_MAX_SECTION_NUM - 1, index);
        return;
    }

    if (!nimble_mem_dbg_max_size_section[index].used) {
        ESP_LOGE("BT_NIMBLE_MEM", "This index(%d) has not been started.\n", index);
        return;
    }

    nimble_mem_dbg_max_size_section[index].used = false;
}

uint32_t nimble_mem_dbg_get_max_size_section(uint8_t index)
{
    if (index >= NIMBLE_MEM_DBG_MAX_SECTION_NUM){
        ESP_LOGE("BT_NIMBLE_MEM", "Then range of index should be between 0 and %d, current index is %d.\n",
                            NIMBLE_MEM_DBG_MAX_SECTION_NUM - 1, index);
        return 0;
    }

    return nimble_mem_dbg_max_size_section[index].max_size;
}

void *nimble_mem_dbg_realloc(void *ptr, size_t new_size, const char *func, int line)
{
    size_t old_size = 0;
    int i;

    void *new_ptr = realloc(ptr, new_size);
    if (new_ptr == NULL && new_size > 0) {
        // realloc failed, keep old ptr record
        return NULL;
    }

    // Find and clean old record if ptr is not NULL
    if (ptr != NULL) {
        for (i = 0; i < NIMBLE_MEM_DBG_INFO_MAX; i++) {
            if (nimble_mem_dbg_info[i].p == ptr) {
                old_size = nimble_mem_dbg_info[i].size;
                nimble_mem_dbg_current_size -= old_size;

                nimble_mem_dbg_info[i].p = NULL;
                nimble_mem_dbg_info[i].size = 0;
                nimble_mem_dbg_info[i].func = NULL;
                nimble_mem_dbg_info[i].line = 0;
                nimble_mem_dbg_count--;
                break;
            }
        }
    }

    // Record the new allocation if new_size > 0
    if (new_ptr != NULL && new_size > 0) {
        for (i = 0; i < NIMBLE_MEM_DBG_INFO_MAX; i++) {
            if (nimble_mem_dbg_info[i].p == NULL) {
                nimble_mem_dbg_info[i].p = new_ptr;
                nimble_mem_dbg_info[i].size = new_size;
                nimble_mem_dbg_info[i].func = func;
                nimble_mem_dbg_info[i].line = line;
                nimble_mem_dbg_count++;
                break;
            }
        }

        if (i >= NIMBLE_MEM_DBG_INFO_MAX) {
            ESP_LOGE("BT_NIMBLE_MEM", "%s full %s %d !!\n", __func__, func, line);
        }

        nimble_mem_dbg_current_size += new_size;
        if (nimble_mem_dbg_max_size < nimble_mem_dbg_current_size) {
            nimble_mem_dbg_max_size = nimble_mem_dbg_current_size;
        }

        for (i = 0; i < NIMBLE_MEM_DBG_MAX_SECTION_NUM; i++) {
            if (nimble_mem_dbg_max_size_section[i].used &&
                nimble_mem_dbg_max_size_section[i].max_size < nimble_mem_dbg_current_size) {
                nimble_mem_dbg_max_size_section[i].max_size = nimble_mem_dbg_current_size;
            }
        }
    }

    return new_ptr;
}
#endif // CONFIG_BT_NIMBLE_MEM_DEBUG

#if !CONFIG_BT_NIMBLE_LOW_SPEED_MODE
IRAM_ATTR
#endif
void *bt_osi_mem_malloc(size_t size)
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

#if !CONFIG_BT_NIMBLE_LOW_SPEED_MODE
IRAM_ATTR
#endif
void *bt_osi_mem_calloc(size_t n, size_t size)
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
#if CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
    if (mem_ptr) {
        controller_mem_used_size += heap_caps_get_allocated_size(mem_ptr);
    }
#endif // CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
    return mem_ptr;
}

#if !CONFIG_BT_NIMBLE_LOW_SPEED_MODE
IRAM_ATTR
#endif
void *bt_osi_mem_calloc_internal(size_t n, size_t size)
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

#if !CONFIG_BT_NIMBLE_LOW_SPEED_MODE
IRAM_ATTR
#endif
void bt_osi_mem_free_internal(void *ptr)
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

#if !CONFIG_BT_NIMBLE_LOW_SPEED_MODE
IRAM_ATTR
#endif
void bt_osi_mem_free(void *ptr)
{
#if CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
    if (ptr) {
        size_t alloc_size = heap_caps_get_allocated_size(ptr);
        // assert(host_mem_used_size >= alloc_size);
        host_mem_used_size -= alloc_size;
    }
#endif // CONFIG_BT_LE_USED_MEM_STATISTICS_ENABLED
    if (ptr) {
        heap_caps_free(ptr);
    }
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
