/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "esp_sleep.h"
#include "lp_core_main.h"
#include "ulp_lp_core.h"
#include "lp_core_spi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern const uint8_t lp_core_main_bin_start[] asm("_binary_lp_core_main_bin_start");
extern const uint8_t lp_core_main_bin_end[]   asm("_binary_lp_core_main_bin_end");

#define LP_SPI_MOSI_PIN 7
#define LP_SPI_MISO_PIN 6
#define LP_SPI_SCLK_PIN 8
#define LP_SPI_CS_PIN   4

#define LP_CORE_WAKEUP_PERIOD_US 1*1000*1000

static void lp_core_init(void)
{
    esp_err_t ret = ESP_OK;

    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER,
        .lp_timer_sleep_duration_us = LP_CORE_WAKEUP_PERIOD_US
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

static void lp_spi_init(void)
{
    lp_spi_host_t host_id = 0;

    lp_spi_bus_config_t bus_config = {
        .miso_io_num = LP_SPI_MISO_PIN,
        .mosi_io_num = LP_SPI_MOSI_PIN,
        .sclk_io_num = LP_SPI_SCLK_PIN,
    };

    /* Base LP SPI device settings */
    lp_spi_device_config_t device = {
        .cs_io_num = LP_SPI_CS_PIN,
        .clock_speed_hz = 10 * 1000, // 10 MHz
        .duty_cycle = 128,  // 50% duty cycle
    };

    ESP_ERROR_CHECK(lp_core_lp_spi_bus_initialize(host_id, &bus_config));

    /* Add LP SPI device */
    ESP_ERROR_CHECK(lp_core_lp_spi_bus_add_device(host_id, &device));
    printf("LP SPI initialized successfully\n");
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

        /* Initialize LP_SPI from the main processor */
        lp_spi_init();

        /* Load LP Core binary and start the coprocessor */
        lp_core_init();
    } else if (cause == ESP_SLEEP_WAKEUP_ULP) {
        printf("LP core woke up the main CPU\n");
        printf("Temperature %.2f degree celsius, humidity %.2f%%RH\n", ulp_temperature / 100.0, ulp_humidity / 1024.0);
    }

    /* Setup wakeup triggers */
    ESP_ERROR_CHECK(esp_sleep_enable_ulp_wakeup());

    /* Enter Deep Sleep */
    printf("Entering deep sleep...\n");
    esp_deep_sleep_start();
}
