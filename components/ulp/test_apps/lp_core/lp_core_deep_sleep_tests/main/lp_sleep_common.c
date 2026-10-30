/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "esp_sleep.h"
#include "esp_rom_caps.h"
#include "driver/gpio.h"
#include "ulp_lp_core.h"
#include "lp_sleep_common.h"

void lp_sleep_keep_pin(gpio_num_t pin)
{
    /* Boot isolates every pad. LP peripheral pins have to keep the active
     * configuration while HP is in deep sleep. */
    TEST_ESP_OK(gpio_sleep_sel_dis(pin));
}

void lp_sleep_run(const uint8_t *bin_start, const uint8_t *bin_end, uint32_t wakeup_source, bool load_binary)
{
    if (load_binary) {
        TEST_ESP_OK(ulp_lp_core_load_binary(bin_start, bin_end - bin_start));
    }

    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = wakeup_source,
    };
    if (wakeup_source & ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER) {
        cfg.lp_timer_sleep_duration_us = LP_SLEEP_TIMER_US;
    }
#if ESP_ROM_HAS_LP_ROM
    /* ROM UART init uses XTAL, which is off in deep sleep. */
    cfg.skip_lp_rom_boot = true;
#endif
    TEST_ESP_OK(ulp_lp_core_run(&cfg));
}

void hp_core_deep_sleep_enter(void)
{
    /* Pytest waits for this line before it lets the peer continue. */
    printf("SLEEPER_SLEEP\n");
    fflush(stdout);
    vTaskDelay(pdMS_TO_TICKS(50));
    TEST_ESP_OK(esp_sleep_enable_ulp_wakeup());
    esp_deep_sleep_start();
    TEST_FAIL_MESSAGE("Should have entered deep sleep");
}

void lp_sleep_expect_ulp_wakeup(void)
{
    uint32_t causes = esp_sleep_get_wakeup_causes();
    printf("Wakeup causes: 0x%" PRIx32 "\n", causes);
    TEST_ASSERT_MESSAGE(causes & BIT(ESP_SLEEP_WAKEUP_ULP), "HP did not wake from the LP core");
}
