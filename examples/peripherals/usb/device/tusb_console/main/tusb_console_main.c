/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

// DESCRIPTION:
// This example contains minimal code to make ESP32-S2 based device
// recognizable by USB-host devices as a USB Serial Device printing output from
// the application.

#include <stdio.h>
#include <stdlib.h>
#include <sys/reent.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tinyusb.h"
#include "tusb_cdc_acm.h"
#include "tusb_console.h"
#include "sdkconfig.h"

static const char *TAG = "example";

void app_main(void)
{
    /* Setting TinyUSB up */
    ESP_LOGI(TAG, "USB initialization");

    const tinyusb_config_t tusb_cfg = {
        .device_descriptor = NULL,
        .string_descriptor = NULL,
        .external_phy = false, // In the most cases you need to use a `false` value
#if (TUD_OPT_HIGH_SPEED)
        .fs_configuration_descriptor = NULL,
        .hs_configuration_descriptor = NULL,
        .qualifier_descriptor = NULL,
#else
        .configuration_descriptor = NULL,
#endif // TUD_OPT_HIGH_SPEED
    };

    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    tinyusb_config_cdcacm_t acm_cfg = { 0 }; // the configuration uses default values
    ESP_ERROR_CHECK(tusb_cdc_acm_init(&acm_cfg));

    ESP_LOGI(TAG, "USB initialization DONE");
    while (1) {
        ESP_LOGI(TAG, "log -> UART");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        fprintf(stdout, "example: print -> stdout\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        fprintf(stderr, "example: print -> stderr\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        esp_tusb_init_console(TINYUSB_CDC_ACM_0); // log to usb
        ESP_LOGI(TAG, "log -> USB");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        fprintf(stdout, "example: print -> stdout\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        fprintf(stderr, "example: print -> stderr\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        esp_tusb_deinit_console(TINYUSB_CDC_ACM_0); // log to uart
    }
}
