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
#include "lp_core_uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ulp_lp_core_lp_uart_shared.h"


extern const uint8_t lp_core_main_bin_start[] asm("_binary_lp_core_main_bin_start");
extern const uint8_t lp_core_main_bin_end[]   asm("_binary_lp_core_main_bin_end");

static void lp_uart_init(void)
{
    lp_core_uart_cfg_t cfg = LP_CORE_UART_DEFAULT_CONFIG();

    ESP_ERROR_CHECK(lp_core_uart_init(&cfg));

    uart_wakeup_cfg_t wake_up_cfg = {
        .wakeup_mode = UART_WK_MODE_CHAR_SEQ,
        .wake_chars_seq = "he**o"
    };
    ESP_ERROR_CHECK(lp_core_uart_wakeup_setup(&wake_up_cfg));

    printf("LP UART initialized successfully\n");
}

static void lp_core_init(void)
{
    /* Set LP core wakeup source as the HP CPU */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_LP_UART,
    };

    /* Load LP core firmware */
    ESP_ERROR_CHECK(ulp_lp_core_load_binary(lp_core_main_bin_start, (lp_core_main_bin_end - lp_core_main_bin_start)));

    /* Run LP core */
    ESP_ERROR_CHECK(ulp_lp_core_run(&cfg));

    printf("LP core loaded with firmware and running successfully\n");
}

void app_main(void)
{

    printf("Initializing...\n");

    /* Initialize LP_UART */
    lp_uart_init();

    /* Load LP Core binary and start the coprocessor */
    lp_core_init();

    while(1){
        vTaskDelay(pdMS_TO_TICKS(5000));
        printf("Main processor running\r\n");
    }

}
