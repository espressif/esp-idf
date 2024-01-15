/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* ULP riscv example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <inttypes.h>
#include "esp_sleep.h"
#include "ulp_riscv.h"
#include "ulp_adc.h"
#include "ulp_main.h"
#include "ulp/example_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


extern const uint8_t ulp_main_bin_start[] asm("_binary_ulp_main_bin_start");
extern const uint8_t ulp_main_bin_end[]   asm("_binary_ulp_main_bin_end");

static void init_ulp_program(void);

void app_main(void)
{
    /* If user is using USB-serial-jtag then idf monitor needs some time to
    *  re-connect to the USB port. We wait 1 sec here to allow for it to make the reconnection
    *  before we print anything. Otherwise the chip will go back to sleep again before the user
    *  has time to monitor any output.
    */
    vTaskDelay(pdMS_TO_TICKS(1000));

    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();

    /* not a wakeup from ULP, load the firmware */
    if ((cause != ESP_SLEEP_WAKEUP_ULP) && (cause != ESP_SLEEP_WAKEUP_TIMER)) {
        printf("Not a ULP-RISC-V wakeup (cause = %d), initializing it! \n", cause);
        init_ulp_program();
    }

    /* ULP Risc-V read and detected a temperature above the limit */
    if (cause == ESP_SLEEP_WAKEUP_ULP) {
        printf("ULP-RISC-V woke up the main CPU\n");
        printf("Threshold: high = %"PRIu32"\n", ulp_adc_threshold);
        printf("Value = %"PRIu32" was above threshold\n", ulp_wakeup_result);
    }

    /* Go back to sleep, only the ULP Risc-V will run */
    printf("Entering in deep sleep\n\n");

    /* RTC peripheral power domain needs to be kept on to keep SAR ADC related configs during sleep */
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);

    ESP_ERROR_CHECK( esp_sleep_enable_ulp_wakeup());

    esp_deep_sleep_start();
}

static void init_ulp_program(void)
{
    ulp_adc_cfg_t cfg = {
        .adc_n    = EXAMPLE_ADC_UNIT,
        .channel  = EXAMPLE_ADC_CHANNEL,
        .width    = EXAMPLE_ADC_WIDTH,
        .atten    = EXAMPLE_ADC_ATTEN,
        .ulp_mode = ADC_ULP_MODE_RISCV,
    };

    ESP_ERROR_CHECK(ulp_adc_init(&cfg));

    esp_err_t err = ulp_riscv_load_binary(ulp_main_bin_start, (ulp_main_bin_end - ulp_main_bin_start));
    ESP_ERROR_CHECK(err);

    /* The first argument is the period index, which is not used by the ULP-RISC-V timer
     * The second argument is the period in microseconds, which gives a wakeup time period of: 20ms
     */
    ulp_set_wakeup_period(0, 20000);

    /* Start the program */
    err = ulp_riscv_run();
    ESP_ERROR_CHECK(err);
}
