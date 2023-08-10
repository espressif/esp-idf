/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <unity.h>
#include <spi_flash_mmap.h>
#include <esp_attr.h>
#include <esp_flash_encrypt.h>
#include "esp_memory_utils.h"

#include "esp_private/cache_utils.h"

//TODO: IDF-6730, migrate this test to test_app

static QueueHandle_t result_queue;

static IRAM_ATTR void cache_test_task(void *arg)
{
    bool do_disable = (bool)arg;
    bool result;
    if(do_disable) {
        spi_flash_disable_interrupts_caches_and_other_cpu();
    }
    result = spi_flash_cache_enabled();
    if (do_disable) {
        spi_flash_enable_interrupts_caches_and_other_cpu();
    }

    TEST_ASSERT( xQueueSendToBack(result_queue, &result, 0) );
    vTaskDelete(NULL);
}

TEST_CASE("spi_flash_cache_enabled() works on both CPUs", "[spi_flash][esp_flash]")
{
    result_queue = xQueueCreate(1, sizeof(bool));

    for(int cpu = 0; cpu < portNUM_PROCESSORS; cpu++) {
        for(int disable = 0; disable <= 1; disable++) {
            bool do_disable = disable;
            bool result;
            printf("Testing cpu %d disabled %d\n", cpu, do_disable);

            xTaskCreatePinnedToCore(cache_test_task, "cache_check_task",
                                    2048, (void *)do_disable, configMAX_PRIORITIES-1, NULL, cpu);

            TEST_ASSERT( xQueueReceive(result_queue, &result, 2) );
            TEST_ASSERT_EQUAL(!do_disable, result);
        }
    }
    vTaskDelay(10);

    vQueueDelete(result_queue);
}

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32S2)


// This needs to sufficiently large array, otherwise it may end up in
// DRAM (e.g. size <= 8 bytes && ARCH == RISCV)
static const uint32_t s_in_rodata[8] = { 0x12345678, 0xfedcba98 };

static void reset_after_invalid_cache(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_PANIC, esp_reset_reason());
}

static void IRAM_ATTR cache_access_test_func(void* arg)
{
    /* Assert that the array s_in_rodata is in DROM. If not, this test is
     * invalid as disabling the cache wouldn't have any effect. */
    TEST_ASSERT(esp_ptr_in_drom(s_in_rodata));

    spi_flash_disable_interrupts_caches_and_other_cpu();
    volatile uint32_t* src = (volatile uint32_t*) s_in_rodata;
    uint32_t v1 = src[0];
    uint32_t v2 = src[1];
    bool cache_enabled = spi_flash_cache_enabled();
    spi_flash_enable_interrupts_caches_and_other_cpu();
    printf("%d %lx %lx\n", cache_enabled, v1, v2);
    vTaskDelete(NULL);
}

#if CONFIG_IDF_TARGET_ESP32
#define CACHE_ERROR_REASON "Cache disabled,SW_RESET"
#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C2 || CONFIG_IDF_TARGET_ESP32P4
#define CACHE_ERROR_REASON "Cache error,RTC_SW_CPU_RST"
#elif CONFIG_IDF_TARGET_ESP32S3
#define CACHE_ERROR_REASON "Cache disabled,RTC_SW_CPU_RST"
#elif CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32H2
#define CACHE_ERROR_REASON "Cache error,SW_CPU"
#endif


// These tests works properly if they resets the chip with the
// "Cache disabled but cached memory region accessed" reason and the correct CPU is logged.
static void invalid_access_to_cache_pro_cpu(void)
{
    xTaskCreatePinnedToCore(&cache_access_test_func, "ia", 2048, NULL, 5, NULL, 0);
    vTaskDelay(1000/portTICK_PERIOD_MS);
}

TEST_CASE_MULTIPLE_STAGES("invalid access to cache raises panic (PRO CPU)", "[spi_flash][reset="CACHE_ERROR_REASON"]", invalid_access_to_cache_pro_cpu, reset_after_invalid_cache);

#ifndef CONFIG_FREERTOS_UNICORE

static void invalid_access_to_cache_app_cpu(void)
{
    xTaskCreatePinnedToCore(&cache_access_test_func, "ia", 2048, NULL, 5, NULL, 1);
    vTaskDelay(1000/portTICK_PERIOD_MS);
}

TEST_CASE_MULTIPLE_STAGES("invalid access to cache raises panic (APP CPU)", "[spi_flash][reset="CACHE_ERROR_REASON"]", invalid_access_to_cache_app_cpu, reset_after_invalid_cache);

#endif // !CONFIG_FREERTOS_UNICORE
#endif // !TEMPORARY_DISABLED_FOR_TARGETS(ESP32S2)
