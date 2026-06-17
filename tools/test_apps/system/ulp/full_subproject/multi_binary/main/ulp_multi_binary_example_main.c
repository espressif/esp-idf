/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <inttypes.h>
#include <stdio.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ulp_lp_core.h"
#include "add.h"
#include "multiply.h"

extern const uint8_t add_bin_start[] asm("_binary_add_bin_start");
extern const uint8_t add_bin_end[] asm("_binary_add_bin_end");
extern const uint8_t multiply_bin_start[] asm("_binary_multiply_bin_start");
extern const uint8_t multiply_bin_end[] asm("_binary_multiply_bin_end");

static void run_ulp_binary(const uint8_t *binary_start, const uint8_t *binary_end)
{
    ESP_ERROR_CHECK(ulp_lp_core_load_binary(binary_start, binary_end - binary_start));

    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };

    ESP_ERROR_CHECK(ulp_lp_core_run(&cfg));

    /* The LP core halts when the ULP program returns. */
    vTaskDelay(pdMS_TO_TICKS(100));
}

void app_main(void)
{
    printf("Running ULP add binary\n");
    run_ulp_binary(add_bin_start, add_bin_end);
    printf("ULP add result: %" PRIi32 "\n", (int32_t)ulp_add_result);

    printf("Running ULP multiply binary\n");
    run_ulp_binary(multiply_bin_start, multiply_bin_end);
    printf("ULP multiply result: %" PRIi32 "\n", (int32_t)ulp_multiply_result);

    printf("Example finished\n");
}
