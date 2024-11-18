/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <inttypes.h>
#include <sys/time.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "unity.h"
#include "soc/soc_caps.h"
#include "esp_rom_caps.h"
#include "lp_core_test_app.h"
#include "ulp_lp_core.h"
#include "ulp_lp_core_memory_shared.h"
#include "test_shared.h"

#if ESP_ROM_HAS_LP_ROM
#include "lp_core_test_app_lp_rom.h"
#endif

extern const uint8_t lp_core_main_bin_start[] asm("_binary_lp_core_test_app_bin_start");
extern const uint8_t lp_core_main_bin_end[]   asm("_binary_lp_core_test_app_bin_end");

extern const uint8_t lp_core_panic_bin_start[] asm("_binary_lp_core_test_app_panic_bin_start");
extern const uint8_t lp_core_panic_bin_end[]   asm("_binary_lp_core_test_app_panic_bin_end");

extern const uint8_t lp_core_shared_mem_bin_start[] asm("_binary_lp_core_test_app_shared_mem_bin_start");
extern const uint8_t lp_core_shared_mem_bin_end[]   asm("_binary_lp_core_test_app_shared_mem_bin_end");

#if ESP_ROM_HAS_LP_ROM
extern const uint8_t lp_core_lp_rom_bin_start[] asm("_binary_lp_core_test_app_lp_rom_bin_start");
extern const uint8_t lp_core_lp_rom_bin_end[]   asm("_binary_lp_core_test_app_lp_rom_bin_end");
#endif

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

TEST_CASE("LP-Core Shared-mem", "[lp_core]")
{
    /* Load ULP firmware and start the coprocessor */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };

    TEST_ASSERT(ulp_lp_core_load_binary(lp_core_shared_mem_bin_start, (lp_core_shared_mem_bin_end - lp_core_shared_mem_bin_start)) == ESP_OK);

    printf("HP shared memory address: %p\n", ulp_lp_core_memory_shared_cfg_get());

    volatile uint8_t* shared_mem = (uint8_t*)ulp_lp_core_memory_shared_cfg_get();
    for (int i = 0; i < sizeof(ulp_lp_core_memory_shared_cfg_t); i++) {
        shared_mem[i] = SHARED_MEM_INIT_VALUE;
    }

    TEST_ASSERT(ulp_lp_core_run(&cfg) == ESP_OK);
    // Actual test output on UART is checked by pytest, not unity test-case
    // We simply wait to allow the lp-core to run once
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    // Check that ULP set the shared memory to 0xAA, and it did not get overwritten by anything
    for (int i = 0; i < sizeof(ulp_lp_core_memory_shared_cfg_t); i++) {
        TEST_ASSERT_EQUAL(SHARED_MEM_END_VALUE, shared_mem[i]);
    }

    printf("HP shared memory test passed\n");
}

#if ESP_ROM_HAS_LP_ROM
TEST_CASE("LP-Core LP-ROM", "[lp_core]")
{
    /* Load ULP firmware and start the coprocessor */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };

    TEST_ASSERT(ulp_lp_core_load_binary(lp_core_lp_rom_bin_start, (lp_core_lp_rom_bin_end - lp_core_lp_rom_bin_start)) == ESP_OK);

    TEST_ASSERT(ulp_lp_core_run(&cfg) == ESP_OK);
    // Actual test output on UART is checked by pytest, not unity test-case
    // We simply wait to allow the lp-core to run once
    while (!ulp_lp_rom_test_finished) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    printf("LP ROM test passed\n");
}
#endif
