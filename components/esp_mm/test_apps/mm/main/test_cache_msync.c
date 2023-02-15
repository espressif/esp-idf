/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
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
#include "esp_rom_sys.h"
#include "esp_memory_utils.h"
#include "esp_heap_caps.h"
#include "esp_mmu_map.h"
#include "esp_cache.h"
#include "esp_timer.h"
#include "esp_partition.h"
#include "esp_flash.h"

const static char *TAG = "CACHE_TEST";

#define TEST_NUM                10
#define TEST_BUF                {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9}


#if CONFIG_IDF_TARGET_ESP32S2
#define TEST_SYNC_START   0x3F500000
#define TEST_SYNC_SIZE    0xA80000
#elif CONFIG_IDF_TARGET_ESP32S3
#define TEST_SYNC_START   0x3C000000
#define TEST_SYNC_SIZE    0x2000000
#endif


#define RECORD_TIME_PREPARE()   uint32_t __t1, __t2
#define RECORD_TIME_START()     do {__t1 = esp_cpu_get_cycle_count();} while(0)
#define RECORD_TIME_END(p_time) do{__t2 = esp_cpu_get_cycle_count(); p_time = (__t2 - __t1);} while(0)
#define GET_US_BY_CCOUNT(t)     ((double)(t)/CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ)

static const uint8_t s_test_buf[TEST_NUM] = TEST_BUF;
static DRAM_ATTR bool diff_res;
static DRAM_ATTR uint32_t s_check_times = 0;

static void NOINLINE_ATTR IRAM_ATTR s_test_rodata_cb(void *arg)
{
    bool sync_flag = *(bool *)arg;

    if (sync_flag) {
        uint8_t cmp_buf[TEST_NUM] = TEST_BUF;
        for (int i = 0; i < TEST_NUM; i++) {
            if (cmp_buf[i] != s_test_buf[i]) {
                diff_res |= true;
            }
        }

        s_check_times++;
    }
}

/**
 * This test tests if the esp_cache_msync() suspending CPU->Cache access is short enough
 * 1. Register an IRAM callback, but access rodata inside the callback
 * 2. esp_cache_msync() will suspend the CPU access to the cache
 * 3. Therefore the rodata access in `s_test_rodata_cb()` should be blocked, most of the times
 * 4. Note if the callback frequency is less, there might be few successful rodata access, as code execution needs time
 */
TEST_CASE("test cache msync short enough when suspending an ISR", "[cache]")
{
    uint32_t sync_time = 0;
    uint32_t sync_time_us = 20;
    RECORD_TIME_PREPARE();

    //Do msync first, as the first writeback / invalidate takes long time, next msyncs will be shorter and they keep unchanged almost
    RECORD_TIME_START();
    TEST_ESP_OK(esp_cache_msync((void *)TEST_SYNC_START, TEST_SYNC_SIZE, ESP_CACHE_MSYNC_FLAG_INVALIDATE));
    RECORD_TIME_END(sync_time);
    sync_time_us = GET_US_BY_CCOUNT(sync_time);
    printf("first sync_time_us: %"PRId32"\n", sync_time_us);

    RECORD_TIME_START();
    TEST_ESP_OK(esp_cache_msync((void *)TEST_SYNC_START, TEST_SYNC_SIZE, ESP_CACHE_MSYNC_FLAG_INVALIDATE));
    RECORD_TIME_END(sync_time);
    sync_time_us = GET_US_BY_CCOUNT(sync_time);
    printf("sync_time_us: %"PRId32"\n", sync_time_us);

    bool sync_flag = false;
    esp_timer_handle_t timer;
    const esp_timer_create_args_t oneshot_timer_args = {
        .callback = &s_test_rodata_cb,
        .arg = &sync_flag,
        .dispatch_method = ESP_TIMER_ISR,
        .name = "test_ro_suspend"
    };
    TEST_ESP_OK(esp_timer_create(&oneshot_timer_args, &timer));

    uint32_t period = sync_time_us / 2;
    TEST_ESP_OK(esp_timer_start_periodic(timer, period));

    RECORD_TIME_START();
    sync_flag = true;
    TEST_ESP_OK(esp_cache_msync((void *)TEST_SYNC_START, TEST_SYNC_SIZE, ESP_CACHE_MSYNC_FLAG_INVALIDATE | ESP_CACHE_MSYNC_FLAG_UNALIGNED));
    sync_flag = false;
    RECORD_TIME_END(sync_time);

    TEST_ESP_OK(esp_timer_stop(timer));
    printf("s_check_times: %"PRId32"\n", s_check_times);
    sync_time_us = GET_US_BY_CCOUNT(sync_time);
    printf("sync time: %"PRId32" us\n", sync_time_us);
    TEST_ASSERT((s_check_times < (sync_time_us / period)));
    TEST_ASSERT(diff_res == false);

    ESP_LOGI(TAG, "Finish");
    TEST_ESP_OK(esp_timer_delete(timer));
}


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

    RECORD_TIME_START();
    TEST_ESP_OK(esp_cache_msync((void *)TEST_SYNC_START, TEST_SYNC_SIZE, ESP_CACHE_MSYNC_FLAG_INVALIDATE | ESP_CACHE_MSYNC_FLAG_UNALIGNED));
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

    uint32_t period = sync_time_us * 2;
    TEST_ESP_OK(esp_timer_start_periodic(timer, period));

    //1ms
    esp_rom_delay_us(1000);
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

    RECORD_TIME_START();
    TEST_ESP_OK(esp_cache_msync((void *)TEST_SYNC_START, TEST_SYNC_SIZE, ESP_CACHE_MSYNC_FLAG_INVALIDATE));
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

    uint32_t period = sync_time_us * 2;
    TEST_ESP_OK(esp_timer_start_periodic(timer, period));

    ESP_ERROR_CHECK(esp_flash_erase_region(part->flash_chip, part->address, part->size));

    TEST_ESP_OK(esp_timer_stop(timer));
    ESP_LOGI(TAG, "Finish");
    TEST_ESP_OK(esp_timer_delete(timer));
}
#endif  //#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS && CONFIG_SPIRAM_RODATA
