/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "esp_sleep.h"
#include "esp_err.h"
#include "lp_core_main.h"
#include "ulp_lp_core.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern const uint8_t lp_core_main_bin_start[] asm("_binary_lp_core_main_bin_start");
extern const uint8_t lp_core_main_bin_end[]   asm("_binary_lp_core_main_bin_end");


static void lp_core_init(void)
{
    /* Set LP core wakeup source as the HP CPU */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };

    /* Load LP core firmware */
    ESP_ERROR_CHECK(ulp_lp_core_load_binary(lp_core_main_bin_start, (lp_core_main_bin_end - lp_core_main_bin_start)));

    /* Run LP core */
    ESP_ERROR_CHECK(ulp_lp_core_run(&cfg));

    // Give the LP core time to start up
    vTaskDelay(pdMS_TO_TICKS(100));

    printf("LP core loaded with firmware and running successfully\n");
}

#define INTERRUPT_COUNT 10

void app_main(void)
{
    /* Load LP Core binary and start the coprocessor */
    lp_core_init();

    for (int i = 0; i < INTERRUPT_COUNT; i++) {
        /* In addition to waking the LP source up, the HP-LP communication bit can also be used to trigger a PMU interrupt on the LP Core */
        ulp_lp_core_sw_intr_trigger();
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    printf("Triggered %d interrupts on the LP-Core, LP-Core received %"PRIu32" interrupts\n", INTERRUPT_COUNT, ulp_lp_core_pmu_intr_count);

}
