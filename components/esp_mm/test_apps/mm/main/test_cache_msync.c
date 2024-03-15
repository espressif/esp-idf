/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <sys/param.h>
#include <string.h>
#include "inttypes.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_rom_sys.h"
#include "esp_memory_utils.h"
#include "esp_heap_caps.h"
#include "esp_mmu_map.h"
#include "esp_cache.h"
#include "esp_timer.h"
#include "esp_partition.h"
#include "esp_flash.h"
#include "test_mm_utils.h"
#include "soc/ext_mem_defs.h"

const static char *TAG = "CACHE_TEST";

#define TEST_NUM                10
#define TEST_BUF                {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9}

#define TEST_OFFSET       0x100000
#if CONFIG_IDF_TARGET_ESP32S2
#define TEST_SYNC_START   (SOC_DPORT_CACHE_ADDRESS_LOW + TEST_OFFSET)
#elif CONFIG_IDF_TARGET_ESP32S3
#define TEST_SYNC_START   (SOC_DRAM0_CACHE_ADDRESS_LOW + TEST_OFFSET)
#elif CONFIG_IDF_TARGET_ESP32P4
#define TEST_SYNC_START   (SOC_DRAM_PSRAM_ADDRESS_LOW + TEST_OFFSET)
#endif
#define TEST_SYNC_SIZE    0x8000

#define RECORD_TIME_PREPARE()   uint32_t __t1, __t2
#define RECORD_TIME_START()     do {__t1 = esp_cpu_get_cycle_count();} while(0)
#define RECORD_TIME_END(p_time) do{__t2 = esp_cpu_get_cycle_count(); p_time = (__t2 - __t1);} while(0)
#define GET_US_BY_CCOUNT(t)     ((double)(t)/CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ)

static void s_test_with_msync_cb(void *arg)
{
    (void)arg;
    esp_cache_msync((void *)TEST_SYNC_START, TEST_SYNC_SIZE, ESP_CACHE_MSYNC_FLAG_INVALIDATE);
}

TEST_CASE("test cache msync short enough to be in an ISR", "[cache]")
{
    uint32_t sync_time = 0;
    uint32_t sync_time_us = 200;
    RECORD_TIME_PREPARE();

#if CONFIG_SPIRAM
    //prepare the cache
    TEST_ESP_OK(test_set_buffer_dirty(TEST_SYNC_START, TEST_SYNC_SIZE));
#endif

    //do once to record time
    RECORD_TIME_START();
    TEST_ESP_OK(esp_cache_msync((void *)TEST_SYNC_START, TEST_SYNC_SIZE, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_INVALIDATE | ESP_CACHE_MSYNC_FLAG_UNALIGNED));
    RECORD_TIME_END(sync_time);
    sync_time_us = GET_US_BY_CCOUNT(sync_time);
    printf("sync_time_us: %"PRId32"\n", sync_time_us);

    esp_timer_handle_t timer;
    const esp_timer_create_args_t oneshot_timer_args = {
        .callback = &s_test_with_msync_cb,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_ISR,
        .name = "test_ro_suspend"
    };
    TEST_ESP_OK(esp_timer_create(&oneshot_timer_args, &timer));

#if CONFIG_SPIRAM
    //prepare the cache
    TEST_ESP_OK(test_set_buffer_dirty(TEST_SYNC_START, TEST_SYNC_SIZE));
#endif

    //start timer
    uint32_t period = sync_time_us * 2;
    TEST_ESP_OK(esp_timer_start_periodic(timer, period));

    //10ms
    esp_rom_delay_us(10 * 1000);
    TEST_ESP_OK(esp_timer_stop(timer));

    ESP_LOGI(TAG, "Finish");
    TEST_ESP_OK(esp_timer_delete(timer));
}

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS && CONFIG_SPIRAM_RODATA
static const esp_partition_t *s_get_partition(void)
{
    //Find the "storage1" partition defined in `partitions.csv`
    const esp_partition_t *result = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage1");
    if (!result) {
        ESP_LOGE(TAG, "Can't find the partition, please define it correctly in `partitions.csv`");
        abort();
    }
    return result;
}

TEST_CASE("test cache msync work with Flash operation when XIP from PSRAM", "[cache]")
{
    uint32_t sync_time = 0;
    RECORD_TIME_PREPARE();

    //prepare the cache
    TEST_ESP_OK(test_set_buffer_dirty(TEST_SYNC_START, TEST_SYNC_SIZE));

    //do once to record time
    RECORD_TIME_START();
    TEST_ESP_OK(esp_cache_msync((void *)TEST_SYNC_START, TEST_SYNC_SIZE, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_INVALIDATE));
    RECORD_TIME_END(sync_time);
    uint32_t sync_time_us = GET_US_BY_CCOUNT(sync_time);
    printf("sync_time_us: %"PRId32"\n", sync_time_us);

    //Get the partition used for SPI1 erase operation
    const esp_partition_t *part = s_get_partition();
    ESP_LOGI(TAG, "found partition '%s' at offset 0x%"PRIx32" with size 0x%"PRIx32, part->label, part->address, part->size);
    //Erase whole region
    TEST_ESP_OK(esp_flash_erase_region(part->flash_chip, part->address, part->size));

    esp_timer_handle_t timer;
    const esp_timer_create_args_t oneshot_timer_args = {
        .callback = &s_test_with_msync_cb,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_ISR,
        .name = "test_ro_suspend"
    };
    TEST_ESP_OK(esp_timer_create(&oneshot_timer_args, &timer));

    //prepare the cache
    TEST_ESP_OK(test_set_buffer_dirty(TEST_SYNC_START, TEST_SYNC_SIZE));

    //start timer
    uint32_t period = sync_time_us * 2;
    TEST_ESP_OK(esp_timer_start_periodic(timer, period));

    //erase
    ESP_ERROR_CHECK(esp_flash_erase_region(part->flash_chip, part->address, part->size));

    TEST_ESP_OK(esp_timer_stop(timer));
    ESP_LOGI(TAG, "Finish");
    TEST_ESP_OK(esp_timer_delete(timer));
}
#endif  //#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS && CONFIG_SPIRAM_RODATA

#if CONFIG_SPIRAM
/*---------------------------------------------------------------
        Test esp_cache_msync with PSRAM stack
---------------------------------------------------------------*/
static void test_msync_on_psram(void *arg)
{
    SemaphoreHandle_t test_semphr = *(SemaphoreHandle_t *)arg;
    extern int _instruction_reserved_end;
    extern int _rodata_reserved_end;
    esp_rom_printf("_instruction_reserved_end: %p\n", &_instruction_reserved_end);
    esp_rom_printf("_rodata_reserved_end: %p\n", &_rodata_reserved_end);

    StackType_t *start_addr_stack = esp_cpu_get_sp();
    TEST_ASSERT(esp_ptr_external_ram(start_addr_stack));

    TEST_ESP_OK(test_set_buffer_dirty(TEST_SYNC_START, TEST_SYNC_SIZE));

    uint32_t sync_time = 0;
    RECORD_TIME_PREPARE();

    printf("doing msync...\n");
    RECORD_TIME_START();
    TEST_ESP_OK(esp_cache_msync((void *)TEST_SYNC_START, 0x8000, ESP_CACHE_MSYNC_FLAG_INVALIDATE));
    RECORD_TIME_END(sync_time);
    printf("msync done\n");

    uint32_t sync_time_us = GET_US_BY_CCOUNT(sync_time);
    printf("sync_time_us: %"PRId32"\n", sync_time_us);

    xSemaphoreGive(test_semphr);
    vTaskDelete(NULL);
}

TEST_CASE("test cache msync work with PSRAM stack", "[cache]")
{
    SemaphoreHandle_t test_semphr = xSemaphoreCreateBinary();
    TEST_ASSERT(test_semphr);

    int size_stack = 1024 * 4;
    StackType_t *stack_for_task = (StackType_t *) heap_caps_calloc(1, size_stack, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    printf("init_task: current addr_stack = %p, stack_for_task = %p\n", esp_cpu_get_sp(), stack_for_task);
    static StaticTask_t task_buf;
    xTaskCreateStaticPinnedToCore(test_msync_on_psram, "test_msync_on_psram", size_stack, &test_semphr, 5, stack_for_task, &task_buf, 0);

    xSemaphoreTake(test_semphr, portMAX_DELAY);
    vSemaphoreDelete(test_semphr);
    free(stack_for_task);
}
#endif  //#if CONFIG_SPIRAM

TEST_CASE("test cache msync unaligned flag not work with M2C direction", "[cache]")
{
    TEST_ASSERT(esp_cache_msync((void *)TEST_SYNC_START, 0x8000, ESP_CACHE_MSYNC_FLAG_UNALIGNED | ESP_CACHE_MSYNC_FLAG_DIR_M2C) == ESP_ERR_INVALID_ARG);
}
