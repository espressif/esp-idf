/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "unity_test_utils_memory.h"
#include "esp_log.h"
#include <strings.h>

static const char* TAG = "event_test_app";

void setUp(void)
{
    // If heap tracing is enabled in kconfig, leak trace the test
    unity_utils_set_leak_level(0);
    unity_utils_record_free_mem();
}

void tearDown(void)
{
    unity_utils_evaluate_leaks();
}

TEST_CASE("fls(0) = 0", "[fls]")
{
    TEST_ASSERT_EQUAL(0, fls(0));
}

TEST_CASE("fls(1) = 1", "[fls]")
{
    TEST_ASSERT_EQUAL(1, fls(1));
}

TEST_CASE("fls(0xF000) = 16", "[fls]")
{
    TEST_ASSERT_EQUAL(16, fls(0xF000));
}

TEST_CASE("fls(0x80000000) = 32", "[fls]")
{
    TEST_ASSERT_EQUAL(32, fls(0x80000000));
}

void app_main(void)
{
    ESP_LOGI(TAG, "Running linux test app");
    unity_run_menu();
}
