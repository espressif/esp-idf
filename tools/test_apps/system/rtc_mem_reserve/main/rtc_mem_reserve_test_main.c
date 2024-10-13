/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * This is an integration test for bootloader RTC reserve memory + ULP
 * These both rely on the same RTC memory region, so we need to make sure they don't
 * cause any conflict.
 */

#include "sdkconfig.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "bootloader_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ulp_lp_core.h"
#include "ulp_main.h"
#include "unity.h"
#include "unity_test_runner.h"
#include "esp_cache.h"

#define TEST_MAGIC_VALUE_A 0x42987561
#define TEST_MAGIC_VALUE_B 0x32234701
#define TEST_MAGIC_VALUE_C 0x62312178
#define UNINITIALIZED_HEAP_VAL 0xCECECECE // From CONFIG_HEAP_POISONING_COMPREHENSIVE

extern const uint8_t ulp_main_bin_start[] asm("_binary_ulp_main_bin_start");
extern const uint8_t ulp_main_bin_end[]   asm("_binary_ulp_main_bin_end");

extern rtc_retain_mem_t* bootloader_common_get_rtc_retain_mem(void);

#define ULP_SLEEP_DURATION_US 10000
#define HEAP_ALLOC_SIZE 0x1000

static void init_ulp_program(void)
{
    esp_err_t err = ulp_lp_core_load_binary(ulp_main_bin_start, (ulp_main_bin_end - ulp_main_bin_start));
    ESP_ERROR_CHECK(err);

    /* Start the program */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER,
        .lp_timer_sleep_duration_us = ULP_SLEEP_DURATION_US,
    };

    err = ulp_lp_core_run(&cfg);
    ESP_ERROR_CHECK(err);
}

static volatile RTC_DATA_ATTR uint32_t rtc_data_var = TEST_MAGIC_VALUE_B;
static volatile RTC_IRAM_ATTR uint32_t rtc_text_var = TEST_MAGIC_VALUE_C;

static bool ulp_is_running(uint32_t *counter_variable)
{
    uint32_t start_cnt = *counter_variable;

    /* Wait a few ULP wakeup cycles to ensure ULP has run */
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    uint32_t end_cnt = *counter_variable;
    printf("start run count: %" PRIu32 ", end run count %" PRIu32 "\n", start_cnt, end_cnt);

    /* If the ulp is running the counter should have been incremented */
    return (start_cnt != end_cnt);
}

TEST_CASE("ULP do not corrupt RTC memory used by APP", "[rtc_mem_reserve]")
{
    rtc_retain_mem_t* mem = bootloader_common_get_rtc_retain_mem();
    uint32_t* _rtc_vars = (uint32_t*) mem->custom;
    uint32_t *rtc_heap = heap_caps_malloc(HEAP_ALLOC_SIZE, MALLOC_CAP_RTCRAM);

    printf("Checking RTC reserve memory at %p\n", _rtc_vars);
    printf("Checking RTC heap memory at %p\n", rtc_heap);
    printf("Checking RTC data var at %p\n", &rtc_data_var);
    printf("Checking RTC text var at %p\n", &rtc_text_var);

    TEST_ASSERT_EACH_EQUAL_HEX(TEST_MAGIC_VALUE_A, _rtc_vars, CONFIG_BOOTLOADER_CUSTOM_RESERVE_RTC_SIZE / sizeof(uint32_t));
    TEST_ASSERT_EQUAL(TEST_MAGIC_VALUE_B, rtc_data_var);
    TEST_ASSERT_EQUAL(TEST_MAGIC_VALUE_C, rtc_text_var);
    TEST_ASSERT_EACH_EQUAL_HEX(UNINITIALIZED_HEAP_VAL, rtc_heap, HEAP_ALLOC_SIZE / sizeof(uint32_t));

    init_ulp_program();

    // Let the ULP run for a few times to see
    // if it corrupts the RTC memory or not
    TEST_ASSERT(ulp_is_running(&ulp_run_counter));

    TEST_ASSERT_EACH_EQUAL_HEX(TEST_MAGIC_VALUE_A, _rtc_vars, CONFIG_BOOTLOADER_CUSTOM_RESERVE_RTC_SIZE / sizeof(uint32_t));
    TEST_ASSERT_EQUAL(TEST_MAGIC_VALUE_B, rtc_data_var);
    TEST_ASSERT_EQUAL(TEST_MAGIC_VALUE_C, rtc_text_var);
    TEST_ASSERT_EACH_EQUAL_HEX(UNINITIALIZED_HEAP_VAL, rtc_heap, HEAP_ALLOC_SIZE / sizeof(uint32_t));
}

void app_main(void)
{
    rtc_retain_mem_t* mem = bootloader_common_get_rtc_retain_mem();
    uint32_t* _rtc_vars = (uint32_t*) mem->custom;

    esp_reset_reason_t reset_reason = esp_reset_reason();
    printf("RTC reserve memory placed at %p\n", _rtc_vars);
    printf("RTC data var placed at %p\n", &rtc_data_var);

    if (reset_reason == ESP_RST_POWERON) {
        printf("First boot, set RTC values and restart\n");
        for(int i = 0; i < CONFIG_BOOTLOADER_CUSTOM_RESERVE_RTC_SIZE / sizeof(uint32_t); i++) {
            _rtc_vars[i] = TEST_MAGIC_VALUE_A;
        }

        TEST_ASSERT_EACH_EQUAL_HEX(TEST_MAGIC_VALUE_A, _rtc_vars, CONFIG_BOOTLOADER_CUSTOM_RESERVE_RTC_SIZE / sizeof(uint32_t));
        TEST_ASSERT_EQUAL(TEST_MAGIC_VALUE_B, rtc_data_var);
        TEST_ASSERT_EQUAL(TEST_MAGIC_VALUE_C, rtc_text_var);

        esp_restart();
    }

    unity_run_menu();

}
