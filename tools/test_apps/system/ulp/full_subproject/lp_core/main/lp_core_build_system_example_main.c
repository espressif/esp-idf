/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "esp_sleep.h"
#include "ulp_lp_core.h"
#include "ulp_build_system_example.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern const uint8_t ulp_build_system_example_bin_start[] asm("_binary_ulp_build_system_example_bin_start");
extern const uint8_t ulp_build_system_example_bin_end[]   asm("_binary_ulp_build_system_example_bin_end");

static void init_ulp_program(void);

void app_main(void)
{
    vTaskDelay(pdMS_TO_TICKS(1000));

    init_ulp_program();

    /* Wait for ULP to complete. The ULP variables are in RTC memory and
     * not declared volatile, so a delay is needed to prevent the compiler
     * from optimizing the reads into a tight loop. */
    vTaskDelay(pdMS_TO_TICKS(100));

    printf("Sum calculated by ULP (ulp_sum component): %" PRIi32 "\n", ulp_sum);
    printf("Product calculated by ULP (ulp_math component): %" PRIi32 "\n", ulp_product);

    printf("Example finished\n");
    vTaskDelay(portMAX_DELAY);
}

static void init_ulp_program(void)
{
    esp_err_t err = ulp_lp_core_load_binary(ulp_build_system_example_bin_start,
                                             (ulp_build_system_example_bin_end - ulp_build_system_example_bin_start));
    ESP_ERROR_CHECK(err);

    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };

    err = ulp_lp_core_run(&cfg);
    ESP_ERROR_CHECK(err);
}
