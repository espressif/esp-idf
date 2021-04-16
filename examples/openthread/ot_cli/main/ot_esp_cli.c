// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <unistd.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_openthread.h"
#include "esp_openthread_lock.h"
#include "esp_openthread_types.h"
#include "sdkconfig.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"
#include "hal/uart_types.h"
#include "openthread/cli.h"
#include "openthread/instance.h"
#include "openthread/tasklet.h"

#define TAG "ot_esp_cli"

extern void otAppCliInit(otInstance *instance);

static void ot_task_worker(void *aContext)
{
    esp_openthread_platform_config_t config = {
        .radio_config =
        {
            .radio_mode = RADIO_MODE_UART_RCP,
            .radio_uart_config =
            {
                .port = 1,
                .uart_config =
                {
                    .baud_rate = 115200,
                    .data_bits = UART_DATA_8_BITS,
                    .parity = UART_PARITY_DISABLE,
                    .stop_bits = UART_STOP_BITS_1,
                    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
                    .rx_flow_ctrl_thresh = 0,
                    .source_clk = UART_SCLK_APB,
                },
                .rx_pin = 4,
                .tx_pin = 5,
            },
        },
        .host_config =
        {
            .host_connection_mode = HOST_CONNECTION_MODE_UART,
            .host_uart_config =
            {
                .port = 0,
                .uart_config =
                {
                    .baud_rate = 115200,
                    .data_bits = UART_DATA_8_BITS,
                    .parity = UART_PARITY_DISABLE,
                    .stop_bits = UART_STOP_BITS_1,
                    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
                    .rx_flow_ctrl_thresh = 0,
                    .source_clk = UART_SCLK_APB,
                },
                .rx_pin = UART_PIN_NO_CHANGE,
                .tx_pin = UART_PIN_NO_CHANGE,
            },
        },
    };
    esp_openthread_mainloop_context_t mainloop;

    ESP_ERROR_CHECK(esp_openthread_platform_init(&config));
    otInstance *instance = otInstanceInitSingle();
    assert(instance != NULL);

    esp_openthread_lock_acquire(portMAX_DELAY);
    otAppCliInit(instance);
    esp_openthread_lock_release();

    while (true) {
        FD_ZERO(&mainloop.read_fds);
        FD_ZERO(&mainloop.write_fds);
        FD_ZERO(&mainloop.error_fds);

        mainloop.max_fd = -1;
        mainloop.timeout.tv_sec = 10;
        mainloop.timeout.tv_usec = 0;

        esp_openthread_lock_acquire(portMAX_DELAY);
        esp_openthread_platform_update(&mainloop);
        if (otTaskletsArePending(instance)) {
            mainloop.timeout.tv_sec = 0;
            mainloop.timeout.tv_usec = 0;
        }
        esp_openthread_lock_release();

        if (select(mainloop.max_fd + 1, &mainloop.read_fds, &mainloop.write_fds, &mainloop.error_fds,
                   &mainloop.timeout) >= 0) {
            esp_openthread_lock_acquire(portMAX_DELAY);
            otTaskletsProcess(instance);
            if (esp_openthread_platform_process(instance, &mainloop)) {
                ESP_LOGE(TAG, "esp_openthread_platform_process failed");
            }
            esp_openthread_lock_release();
        } else {
            ESP_LOGE(TAG, "OpenThread system polling failed");
            break;
        }
    }

    otInstanceFinalize(instance);
    esp_openthread_platform_deinit();
    vTaskDelete(NULL);
}

void app_main(void)
{
    xTaskCreate(ot_task_worker, "ot_cli_main", 10240, xTaskGetCurrentTaskHandle(), 5, NULL);
}
