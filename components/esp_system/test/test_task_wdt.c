/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdbool.h>
#include "unity.h"
#include "esp_rom_sys.h"
#include "esp_task_wdt.h"
#include "test_utils.h"

#define TASK_WDT_TIMEOUT_MS     1000

static bool timeout_flag;

void esp_task_wdt_isr_user_handler(void)
{
    timeout_flag = true;
}

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32C2)
//IDF-5055
TEST_CASE("Task WDT task timeout", "[task_wdt]")
{
    timeout_flag = false;
    esp_task_wdt_config_t twdt_config = {
        .timeout_ms = TASK_WDT_TIMEOUT_MS,
        .idle_core_mask = 0,
        .trigger_panic = false,
    };
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_init(&twdt_config));
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_add(NULL));
    // Short delay to allow timeout to occur
    esp_rom_delay_us(TASK_WDT_TIMEOUT_MS * 1000);
    TEST_ASSERT_EQUAL(true, timeout_flag);
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_delete(NULL));
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_deinit());
}
#endif //!TEMPORARY_DISABLED_FOR_TARGETS(ESP32C2)

TEST_CASE("Task WDT task feed", "[task_wdt]")
{
    timeout_flag = false;
    esp_task_wdt_config_t twdt_config = {
        .timeout_ms = TASK_WDT_TIMEOUT_MS,
        .idle_core_mask = 0,
        .trigger_panic = false,
    };
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_init(&twdt_config));
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_add(NULL));
    // Feed the watchdog after a short delay
    esp_rom_delay_us((TASK_WDT_TIMEOUT_MS * 1000) / 2);
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_reset());
    esp_rom_delay_us((TASK_WDT_TIMEOUT_MS * 1000) / 2);
    TEST_ASSERT_EQUAL(false, timeout_flag);
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_delete(NULL));
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_deinit());
}

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32C2)
//IDF-5055
TEST_CASE("Task WDT user timeout", "[task_wdt]")
{
    const char *user_name = "test_user";
    esp_task_wdt_user_handle_t user_handle;
    timeout_flag = false;
    esp_task_wdt_config_t twdt_config = {
        .timeout_ms = TASK_WDT_TIMEOUT_MS,
        .idle_core_mask = 0,
        .trigger_panic = false,
    };
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_init(&twdt_config));
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_add_user(user_name, &user_handle));
    // Short delay to allow timeout to occur
    esp_rom_delay_us(TASK_WDT_TIMEOUT_MS * 1000);
    TEST_ASSERT_EQUAL(true, timeout_flag);
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_delete_user(user_handle));
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_deinit());
}
#endif //!TEMPORARY_DISABLED_FOR_TARGETS(ESP32C2)

TEST_CASE("Task WDT user feed", "[task_wdt]")
{
    const char *user_name = "test_user";
    esp_task_wdt_user_handle_t user_handle;
    timeout_flag = false;
    esp_task_wdt_config_t twdt_config = {
        .timeout_ms = TASK_WDT_TIMEOUT_MS,
        .idle_core_mask = 0,
        .trigger_panic = false,
    };
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_init(&twdt_config));
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_add_user(user_name, &user_handle));
    // Feed the watchdog after a short delay
    esp_rom_delay_us((TASK_WDT_TIMEOUT_MS * 1000) / 2);
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_reset_user(user_handle));
    esp_rom_delay_us((TASK_WDT_TIMEOUT_MS * 1000) / 2);
    TEST_ASSERT_EQUAL(false, timeout_flag);
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_delete_user(user_handle));
    TEST_ASSERT_EQUAL(ESP_OK, esp_task_wdt_deinit());
}
