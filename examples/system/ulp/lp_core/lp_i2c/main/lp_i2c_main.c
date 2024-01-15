/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "esp_sleep.h"
#include "lp_core_main.h"
#include "ulp_lp_core.h"
#include "lp_core_i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern const uint8_t lp_core_main_bin_start[] asm("_binary_lp_core_main_bin_start");
extern const uint8_t lp_core_main_bin_end[]   asm("_binary_lp_core_main_bin_end");

static void lp_core_init(void)
{
    esp_err_t ret = ESP_OK;

    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };

    ret = ulp_lp_core_load_binary(lp_core_main_bin_start, (lp_core_main_bin_end - lp_core_main_bin_start));
    if (ret != ESP_OK) {
        printf("LP Core load failed\n");
        abort();
    }

    ret = ulp_lp_core_run(&cfg);
    if (ret != ESP_OK) {
        printf("LP Core run failed\n");
        abort();
    }

    printf("LP core loaded with firmware successfully\n");
}

static void lp_i2c_init(void)
{
    esp_err_t ret = ESP_OK;

    /* Initialize LP I2C with default configuration */
    const lp_core_i2c_cfg_t i2c_cfg = LP_CORE_I2C_DEFAULT_CONFIG();
    ret = lp_core_i2c_master_init(LP_I2C_NUM_0, &i2c_cfg);
    if (ret != ESP_OK) {
        printf("LP I2C init failed\n");
        abort();
    }

    printf("LP I2C initialized successfully\n");
}

void app_main(void)
{
    /* If user is using USB-serial-jtag then idf monitor needs some time to
    *  re-connect to the USB port. We wait 1 sec here to allow for it to make the reconnection
    *  before we print anything. Otherwise the chip will go back to sleep again before the user
    *  has time to monitor any output.
    */
    vTaskDelay(pdMS_TO_TICKS(1000));

    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    if (cause != ESP_SLEEP_WAKEUP_ULP) {
        printf("Not an LP core wakeup. Cause = %d\n", cause);
        printf("Initializing...\n");

        /* Initialize LP_I2C from the main processor */
        lp_i2c_init();

        /* Load LP Core binary and start the coprocessor */
        lp_core_init();
    } else if (cause == ESP_SLEEP_WAKEUP_ULP) {
        printf("LP core woke up the main CPU\n");
        printf("Lux = %ld\n", ulp_lux);
    }

    /* Setup wakeup triggers */
    ESP_ERROR_CHECK(esp_sleep_enable_ulp_wakeup());

    /* Enter Deep Sleep */
    printf("Entering deep sleep...\n");
    esp_deep_sleep_start();
}
