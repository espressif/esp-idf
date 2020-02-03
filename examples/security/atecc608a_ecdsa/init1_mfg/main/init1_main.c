/*
 *  Key generation application
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */
#include <string.h>

#include "esp_log.h"
#include <esp_console.h>
#include <driver/uart.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include "init1_commands.h"

#define CMD_BUFFER_SIZE 1024

const char* APP_TAG = "CMD Handler";
QueueHandle_t uart_queue;

#ifdef CONFIG_MFG_DEBUGGING
#define ESP_MFG_DEBUG ESP_LOGI
#else
#define ESP_MFG_DEBUG(...)
#endif /* MFG_DEBUG */

static void scli_loop() 
{    
    int uart_num = 0, i, cmd_ret;
    uint8_t linebuf[CMD_BUFFER_SIZE];
    esp_err_t ret;
    uart_event_t event;

    uart_driver_install(uart_num, CMD_BUFFER_SIZE, 0, 32, &uart_queue, 0);
    printf("Initialising Command line: >>");
    fflush(stdout);
    while (true) {
        uart_write_bytes(uart_num, "\n>>\n", 4);
        bzero(linebuf, sizeof(linebuf));
        i = 0;
        do {
            ret = xQueueReceive(uart_queue, (void * )&event, (portTickType)portMAX_DELAY);
            if (ret != pdPASS) continue;

            if (event.type == UART_DATA) {
                while (uart_read_bytes(uart_num, (uint8_t *) &linebuf[i], 1, 0)) {
                    if (linebuf[i] == '\r') {
                        uart_write_bytes(uart_num, "\r\n", 2);
                    } else {
                        uart_write_bytes(uart_num, (char *) &linebuf[i], 1);
                    }
                    i++;
                }
            }
        } while ((i < CMD_BUFFER_SIZE - 1) && linebuf[i-1] != '\r');

        /* Remove the truncating \r\n */
        linebuf[strlen((char *)linebuf) - 1] = '\0';
        printf("\n");

        esp_console_run((char *) linebuf, &cmd_ret);
    }
}

static void scli_task(void *arg)
{
    esp_console_config_t console_config = {
            .max_cmdline_args = 8,
            .max_cmdline_length = CMD_BUFFER_SIZE,
    };
    esp_console_init(&console_config);
    esp_console_register_help_command();
    
    if (register_command_handler() == ESP_OK)
        scli_loop();
    else {
        ESP_MFG_DEBUG(APP_TAG,"Failed to registering all commands");
    }
    
    ESP_MFG_DEBUG(APP_TAG, "Stopping the CLI");
    vTaskDelete(NULL);
}

void app_main() 
{
    BaseType_t cli_task = xTaskCreate(scli_task, "scli_task", 20 * 1024, NULL, configMAX_PRIORITIES - 5, NULL);
    if (cli_task != pdPASS) {
        ESP_MFG_DEBUG(APP_TAG, "Couldn't create scli thread");
    }
}
