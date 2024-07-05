/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* LP core build system example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
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
    /* If user is using USB-serial-jtag then idf monitor needs some time to
    *  re-connect to the USB port. We wait 1 sec here to allow for it to make the reconnection
    *  before we print anything. Otherwise the chip will go back to sleep again before the user
    *  has time to monitor any output.
    */
    vTaskDelay(pdMS_TO_TICKS(1000));

    init_ulp_program();

    while(ulp_sum == 0) {
        // Wait for ULP to finish its boot up and calculate sum
    }
    printf("Sum calculated by ULP using external library func: %"PRIi32"\n", ulp_sum);
    printf("Example finished\n");
    vTaskDelay(portMAX_DELAY);
}

static void init_ulp_program(void)
{
    esp_err_t err = ulp_lp_core_load_binary(ulp_build_system_example_bin_start, (ulp_build_system_example_bin_end - ulp_build_system_example_bin_start));
    ESP_ERROR_CHECK(err);

    /* Start the program */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };

    err = ulp_lp_core_run(&cfg);
    ESP_ERROR_CHECK(err);
}
