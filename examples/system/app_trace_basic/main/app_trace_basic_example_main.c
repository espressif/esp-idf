/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* Application Trace Basic Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/uart_pins.h"
#include "esp_log.h"
#include "esp_app_trace.h"

static const char *TAG = "example";

#if !CONFIG_APPTRACE_DEST_JTAG
/* Override default uart config to use console pins as a uart channel */
esp_apptrace_config_t esp_apptrace_get_user_params(void)
{
    esp_apptrace_config_t config = APPTRACE_UART_CONFIG_DEFAULT();
    config.dest_cfg.uart.uart_num = 0;
    config.dest_cfg.uart.tx_pin_num = U0TXD_GPIO_NUM;
    config.dest_cfg.uart.rx_pin_num = U0RXD_GPIO_NUM;
    return config;
}
#endif

void app_main(void)
{
    if (esp_apptrace_get_destination() == ESP_APPTRACE_DEST_JTAG) {
        ESP_LOGI(TAG, "Waiting for OpenOCD connection");

        while (!esp_apptrace_host_is_connected()) {
            vTaskDelay(1);
        }
    } else { // UART
        // Before sending data, wait for the UART host to be ready
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    ESP_LOGI(TAG, "Sending example data to the host...");

    for (unsigned int cnt = 1; cnt < 51; ++cnt) {
        char buf[32] = {0};
        snprintf(buf, sizeof(buf), "Apptrace test data[%d]:%d\n", cnt, cnt * cnt);
        esp_err_t res = esp_apptrace_write(buf, strlen(buf), ESP_APPTRACE_TMO_INFINITE);
        if (res != ESP_OK) {
            ESP_LOGE(TAG, "Failed to write data to host [0x%x] (%s)", res, esp_err_to_name(res));
        }
        esp_apptrace_flush( 1000);
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }

    ESP_LOGI(TAG, "Done!");
}
