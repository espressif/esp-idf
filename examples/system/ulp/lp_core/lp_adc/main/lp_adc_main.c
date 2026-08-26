/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "lp_core_main.h"
#include "ulp_lp_core.h"
#include "lp_core_uart.h"
#include "ulp_lp_core_lp_adc_shared.h"

/* LP ADC1 configuration */
#define EXAMPLE_LP_ADC1_CHAN0   CONFIG_EXAMPLE_LP_ADC1_CHANNEL_0_SELECT
#define EXAMPLE_LP_ADC1_CHAN1   CONFIG_EXAMPLE_LP_ADC1_CHANNEL_1_SELECT
#define EXAMPLE_ADC_ATTEN       ADC_ATTEN_DB_12

/* LP core binary */
extern const uint8_t lp_core_main_bin_start[] asm("_binary_lp_core_main_bin_start");
extern const uint8_t lp_core_main_bin_end[]   asm("_binary_lp_core_main_bin_end");

static void lp_uart_init(void)
{
    lp_core_uart_cfg_t cfg = LP_CORE_UART_DEFAULT_CONFIG();

    ESP_ERROR_CHECK(lp_core_uart_init(&cfg));

    printf("LP UART initialized successfully\n");
}

static void lp_core_init(void)
{
    /* Set LP core wakeup source as the LP timer */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER,
        .lp_timer_sleep_duration_us = 1000000,
    };

    /* Load LP core firmware */
    ESP_ERROR_CHECK(ulp_lp_core_load_binary(lp_core_main_bin_start, (lp_core_main_bin_end - lp_core_main_bin_start)));

    /* Run LP core */
    ESP_ERROR_CHECK(ulp_lp_core_run(&cfg));

    printf("LP core loaded with firmware and running successfully\n");
}

void app_main(void)
{
    /* LP ADC1 Init */
    ESP_ERROR_CHECK(lp_core_lp_adc_init(ADC_UNIT_1));

    /* LP ADC1 channel config */
    const lp_core_lp_adc_chan_cfg_t config = {
        .atten = EXAMPLE_ADC_ATTEN,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(lp_core_lp_adc_config_channel(ADC_UNIT_1, EXAMPLE_LP_ADC1_CHAN0, &config));
    ESP_ERROR_CHECK(lp_core_lp_adc_config_channel(ADC_UNIT_1, EXAMPLE_LP_ADC1_CHAN1, &config));

    /* Initialize LP_UART to print the ADC values to the LP core console */
    lp_uart_init();

    /* Load LP Core binary and start the coprocessor */
    lp_core_init();

    /* Enable ULP wakeup */
    ESP_ERROR_CHECK(esp_sleep_enable_ulp_wakeup());

    /* Enter Deep Sleep */
    printf("Entering deep sleep...\n");
    esp_deep_sleep_start();
}
