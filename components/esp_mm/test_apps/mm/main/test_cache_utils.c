/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
*
* SPDX-License-Identifier: Apache-2.0
*/

#include <sys/param.h>
#include <string.h>
#include "sdkconfig.h"
#include "inttypes.h"
#include "unity.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_cpu.h"
#include "esp_private/esp_cache_private.h"
#include "esp_private/cache_utils.h"
#include "test_utils.h"

#if !CONFIG_FREERTOS_UNICORE

#define RECORD_TIME_PREPARE()   uint32_t __t1, __t2
#define RECORD_TIME_START()     do {__t1 = esp_cpu_get_cycle_count();} while(0)
#define RECORD_TIME_END(p_time) do{__t2 = esp_cpu_get_cycle_count(); p_time = (__t2 - __t1);} while(0)
#define GET_US_BY_CCOUNT(t)     ((double)(t)/CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ)

static void IRAM_ATTR NOINLINE_ATTR s_test_func_freeze_unfreeze(void)
{
    esp_cache_freeze_caches_disable_interrupts();
    for (int i = 0; i < 100; i++) {
        asm volatile("nop");
    }
    esp_cache_unfreeze_caches_enable_interrupts();
}

static void IRAM_ATTR NOINLINE_ATTR s_test_func_disable_enable(void)
{
    spi_flash_disable_interrupts_caches_and_other_cpu();
    for (int i = 0; i < 100; i++) {
        asm volatile("nop");
    }
    spi_flash_enable_interrupts_caches_and_other_cpu();
}

TEST_CASE("test esp_cache_freeze_caches_disable_interrupts speed", "[cache]")
{
    uint32_t cache_disable_enable_time = 0;
    uint32_t cache_freeze_unfreeze_time = 0;

    s_test_func_freeze_unfreeze();
    s_test_func_disable_enable();

    RECORD_TIME_PREPARE();
    RECORD_TIME_START();
    s_test_func_freeze_unfreeze();
    RECORD_TIME_END(cache_freeze_unfreeze_time);
    uint32_t cache_freeze_unfreeze_time_us = GET_US_BY_CCOUNT(cache_freeze_unfreeze_time);
    IDF_LOG_PERFORMANCE("Cache freeze time", "cache_freeze_unfreeze_time: %"PRId32" cpu cycles, %"PRId32" us", cache_freeze_unfreeze_time, cache_freeze_unfreeze_time_us);

    RECORD_TIME_START();
    s_test_func_disable_enable();
    RECORD_TIME_END(cache_disable_enable_time);
    uint32_t cache_disable_enable_time_us = GET_US_BY_CCOUNT(cache_disable_enable_time);
    IDF_LOG_PERFORMANCE("Cache disable time", "cache_disable_enable_time: %"PRId32", cpu cycles, %"PRId32" us", cache_disable_enable_time, cache_disable_enable_time_us);

    TEST_ASSERT(cache_freeze_unfreeze_time < cache_disable_enable_time);
}
#endif  // #if !CONFIG_FREERTOS_UNICORE
