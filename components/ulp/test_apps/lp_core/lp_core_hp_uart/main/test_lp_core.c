/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <inttypes.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "unity.h"
#include "soc/soc_caps.h"
#include "esp_rom_caps.h"
#include "lp_core_test_app.h"
#include "ulp_lp_core.h"

extern const uint8_t lp_core_main_bin_start[] asm("_binary_lp_core_test_app_bin_start");
extern const uint8_t lp_core_main_bin_end[]   asm("_binary_lp_core_test_app_bin_end");

extern const uint8_t lp_core_panic_bin_start[] asm("_binary_lp_core_test_app_panic_bin_start");
extern const uint8_t lp_core_panic_bin_end[]   asm("_binary_lp_core_test_app_panic_bin_end");

static void load_and_start_lp_core_firmware(ulp_lp_core_cfg_t* cfg, const uint8_t* firmware_start, const uint8_t* firmware_end)
{
    TEST_ASSERT(ulp_lp_core_load_binary(firmware_start,
                                        (firmware_end - firmware_start)) == ESP_OK);

    TEST_ASSERT(ulp_lp_core_run(cfg) == ESP_OK);

}

TEST_CASE("lp-print can output to hp-uart", "[lp_core]")
{
    /* Load ULP firmware and start the coprocessor */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };

    load_and_start_lp_core_firmware(&cfg, lp_core_main_bin_start, lp_core_main_bin_end);

    // Actual test output on UART is checked by pytest, not unity test-case
    // We simply wait to allow the lp-core to run once
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

TEST_CASE("LP-Core panic", "[lp_core]")
{
    /* Load ULP firmware and start the coprocessor */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };

    load_and_start_lp_core_firmware(&cfg, lp_core_panic_bin_start, lp_core_panic_bin_end);

    // Actual test output on UART is checked by pytest, not unity test-case
    // We simply wait to allow the lp-core to run once
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
