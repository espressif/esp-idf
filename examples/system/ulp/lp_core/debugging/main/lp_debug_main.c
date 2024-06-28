/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "esp_sleep.h"
#include "esp_err.h"
#include "ulp_lp_core.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern const uint8_t ulp_build_debug_bin_start[] asm("_binary_ulp_debug_example_bin_start");
extern const uint8_t ulp_build_debug_bin_end[]   asm("_binary_ulp_debug_example_bin_end");

static void lp_core_init(void)
{
    /* Set LP core wakeup source as the HP CPU */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };

    /* Load LP core firmware */
    ESP_ERROR_CHECK(ulp_lp_core_load_binary(ulp_build_debug_bin_start, (ulp_build_debug_bin_end - ulp_build_debug_bin_start)));

    /* Run LP core */
    ESP_ERROR_CHECK(ulp_lp_core_run(&cfg));

    printf("LP core loaded with firmware and running successfully\n");
}

void app_main(void)
{
    printf("Initializing LP core...\n");
    /* Load LP Core binary and start the coprocessor */
    lp_core_init();

    printf("Do some work on HP core...\n");
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
