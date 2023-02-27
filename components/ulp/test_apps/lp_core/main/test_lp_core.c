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
}
