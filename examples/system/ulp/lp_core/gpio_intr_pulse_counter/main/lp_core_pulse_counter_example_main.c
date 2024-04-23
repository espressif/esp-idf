/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* LP core gpio example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <inttypes.h>
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ulp_lp_core.h"
#include "ulp_main.h"
#include "lp_core_uart.h"

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

    /* Initialize selected GPIO as RTC IO, enable input/output, disable pullup and pulldown */
    printf("Using pin %d as pulse counter input\n", CONFIG_EXAMPLE_PULSE_COUNT_PIN);
    rtc_gpio_init(CONFIG_EXAMPLE_PULSE_COUNT_PIN);
    rtc_gpio_set_direction(CONFIG_EXAMPLE_PULSE_COUNT_PIN, RTC_GPIO_MODE_INPUT_OUTPUT);
    rtc_gpio_pulldown_dis(CONFIG_EXAMPLE_PULSE_COUNT_PIN);
    rtc_gpio_pullup_dis(CONFIG_EXAMPLE_PULSE_COUNT_PIN);

    printf("ULP will wake up processor after every %d pulses\n", CONFIG_EXAMPLE_PULSE_COUNT_WAKEUP_LIMIT);

    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    /* not a wakeup from ULP, load the firmware */
    if (cause != ESP_SLEEP_WAKEUP_ULP) {
        printf("Not a ULP wakeup, initializing it! \n");
        init_ulp_program();
    } else {
        printf("ULP woke up the main CPU!\n");
        printf("Pulse count: %"PRIu32"\n", ulp_pulse_count);
    }

    /* Go back to sleep, only the ULP will run */
    printf("Entering in deep sleep\n\n");

    /* Small delay to ensure the messages are printed */
    ESP_ERROR_CHECK( esp_sleep_enable_ulp_wakeup());

    esp_deep_sleep_start();
}

static void init_ulp_program(void)
{
    lp_core_uart_cfg_t uart_cfg = LP_CORE_UART_DEFAULT_CONFIG();

    ESP_ERROR_CHECK(lp_core_uart_init(&uart_cfg));

    esp_err_t err = ulp_lp_core_load_binary(ulp_main_bin_start, (ulp_main_bin_end - ulp_main_bin_start));
    ESP_ERROR_CHECK(err);

    /* Start the program */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };

    err = ulp_lp_core_run(&cfg);
    ESP_ERROR_CHECK(err);
}
