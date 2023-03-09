/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "lp_core_test_app.h"
#include "ulp_lp_core.h"
#include "test_shared.h"
#include "unity.h"
#include "esp_sleep.h"
#include "esp_timer.h"

extern const uint8_t lp_core_main_bin_start[] asm("_binary_lp_core_test_app_bin_start");
extern const uint8_t lp_core_main_bin_end[]   asm("_binary_lp_core_test_app_bin_end");
static bool  firmware_loaded = false;

static void load_and_start_lp_core_firmware(void)
{
    if (!firmware_loaded) {

        ulp_lp_core_cfg_t cfg = {
            .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
        };

        TEST_ASSERT(ulp_lp_core_load_binary(lp_core_main_bin_start,
                            (lp_core_main_bin_end - lp_core_main_bin_start)) == ESP_OK);

        TEST_ASSERT(ulp_lp_core_run(&cfg) == ESP_OK);

        firmware_loaded = true;
    }
}

TEST_CASE("LP core and main CPU are able to exchange data", "[lp_core]")
{
    const uint32_t test_data = 0x12345678;

    /* Load ULP RISC-V firmware and start the coprocessor */
    load_and_start_lp_core_firmware();

    /* Setup test data */
    ulp_test_data_in = test_data ^ XOR_MASK;
    ulp_main_cpu_command = LP_CORE_READ_WRITE_TEST;

    /* Wait till we receive the correct command response */
    while (ulp_command_resp != LP_CORE_READ_WRITE_TEST) {
    }

    /* Verify test data */
    TEST_ASSERT(ulp_command_resp == LP_CORE_READ_WRITE_TEST);

    /* Wait till we receive COMMAND_OK reply */
    while (ulp_main_cpu_reply != LP_CORE_COMMAND_OK) {
    }

    printf("data out: 0x%" PRIx32 ", expected: 0x%" PRIx32 " \n", ulp_test_data_out, test_data);
    TEST_ASSERT(test_data == ulp_test_data_out);

    /* Clear test data */
    ulp_main_cpu_command = LP_CORE_NO_COMMAND;
    ulp_command_resp = LP_CORE_NO_COMMAND;
}

TEST_CASE("Test LP core delay", "[lp_core]")
{
    int64_t start, diff;
    const uint32_t delay_period_us = 5000000;
    const uint32_t delta_us = 500000; // RTC FAST is not very accurate

    /* Load ULP RISC-V firmware and start the coprocessor */
    load_and_start_lp_core_firmware();

    /* Setup test data */
    ulp_test_data_in = delay_period_us;
    ulp_main_cpu_command = LP_CORE_DELAY_TEST;

    /* Wait till we receive the correct command response */
    while (ulp_command_resp != LP_CORE_DELAY_TEST) {
    }

    start = esp_timer_get_time();

    /* Wait till we receive COMMAND_OK reply */
    while (ulp_main_cpu_reply != LP_CORE_COMMAND_OK) {
    }

    diff = esp_timer_get_time() - start;

    printf("Waited for %" PRIi64 "us, expected: %" PRIi32 "us\n", diff, delay_period_us);
    TEST_ASSERT_INT_WITHIN(delta_us, delay_period_us, diff);

    /* Clear test data */
    ulp_main_cpu_command = LP_CORE_NO_COMMAND;
    ulp_command_resp = LP_CORE_NO_COMMAND;
}


static void do_ulp_wakeup_deepsleep(lp_core_test_commands_t ulp_cmd)
{
    /* Load ULP RISC-V firmware and start the ULP RISC-V Coprocessor */
    load_and_start_lp_core_firmware();

    /* Setup wakeup triggers */
    TEST_ASSERT(esp_sleep_enable_ulp_wakeup() == ESP_OK);

    /* Setup test data */
    ulp_main_cpu_command = ulp_cmd;

    /* Enter Deep Sleep */
    esp_deep_sleep_start();
    UNITY_TEST_FAIL(__LINE__, "Should not get here!");
}


static void check_reset_reason_ulp_wakeup(void)
{
    TEST_ASSERT_EQUAL(ESP_SLEEP_WAKEUP_ULP, esp_sleep_get_wakeup_cause());
}

static void do_ulp_wakeup_after_short_delay_deepsleep(void)
{
    do_ulp_wakeup_deepsleep(LP_CORE_DEEP_SLEEP_WAKEUP_SHORT_DELAY_TEST);
}

static void do_ulp_wakeup_after_long_delay_deepsleep(void)
{
    do_ulp_wakeup_deepsleep(LP_CORE_DEEP_SLEEP_WAKEUP_LONG_DELAY_TEST);
}

TEST_CASE_MULTIPLE_STAGES("LP-core is able to wakeup main CPU from deep sleep after a short delay", "[ulp]",
        do_ulp_wakeup_after_short_delay_deepsleep,
        check_reset_reason_ulp_wakeup);

/* Certain erroneous wake-up triggers happen only after sleeping for a few seconds  */
TEST_CASE_MULTIPLE_STAGES("LP-core is able to wakeup main CPU from deep sleep after a long delay", "[ulp]",
        do_ulp_wakeup_after_long_delay_deepsleep,
        check_reset_reason_ulp_wakeup);
