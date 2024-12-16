/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "lp_core_test_app_prefix1.h"
#include "lp_core_test_app_prefix2.h"
#include "ulp_lp_core.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "test_shared.h"
#include "unity.h"
#include "test_utils.h"

extern const uint8_t lp_core_main_prefix1_bin_start[] asm("_binary_lp_core_test_app_prefix1_bin_start");
extern const uint8_t lp_core_main_prefix1_bin_end[]   asm("_binary_lp_core_test_app_prefix1_bin_end");
extern const uint8_t lp_core_main_prefix2_bin_start[] asm("_binary_lp_core_test_app_prefix2_bin_start");
extern const uint8_t lp_core_main_prefix2_bin_end[]   asm("_binary_lp_core_test_app_prefix2_bin_end");

static void load_and_start_lp_core_firmware(ulp_lp_core_cfg_t* cfg, const uint8_t* firmware_start, const uint8_t* firmware_end)
{
    TEST_ASSERT(ulp_lp_core_load_binary(firmware_start,
                                        (firmware_end - firmware_start)) == ESP_OK);

    TEST_ASSERT(ulp_lp_core_run(cfg) == ESP_OK);
}

TEST_CASE("LP-Core header prefix test", "[lp_core]")
{
    /* Load ULP firmware and start the coprocessor */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
#if ESP_ROM_HAS_LP_ROM
        .skip_lp_rom_boot = true,
#endif //ESP_ROM_HAS_LP_ROM
    };

    /* Load and run the first LP core firmware */
    load_and_start_lp_core_firmware(&cfg, lp_core_main_prefix1_bin_start, lp_core_main_prefix1_bin_end);
    vTaskDelay(10);

    /* Verify the shared variable value */
    TEST_ASSERT_EQUAL(1, ulp1_g_var);

    /* Load and run the second LP core firmware */
    load_and_start_lp_core_firmware(&cfg, lp_core_main_prefix2_bin_start, lp_core_main_prefix2_bin_end);
    vTaskDelay(10);

    /* Verify that a global array can be accessed as an array on the HP CPU */
    for (int i = 0; i < 10; i++) {
        TEST_ASSERT_EQUAL(i, ulp2_g_array[i]);
    }

    /* Verify that the shared variable with the same name is updated once the second LP core binary runs */
    TEST_ASSERT_EQUAL(2, ulp2_g_var);
}
