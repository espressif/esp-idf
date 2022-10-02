/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <ctype.h>
#include "esp_log.h"
#include <string.h>
#include <esp_log.h>
#include <esp_console.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <driver/uart.h>
#include "esp_peripheral.h"

#define BLE_RX_TIMEOUT (30000 / portTICK_PERIOD_MS)

static TaskHandle_t cli_task;
static QueueHandle_t cli_handle;
static int stop;

static int enter_passkey_handler(int argc, char *argv[])
{
    int key;
    char pkey[8];
    int num;

    if (argc != 2) {
        return -1;
    }

    sscanf(argv[1], "%s", pkey);
    ESP_LOGI("You entered", "%s %s", argv[0], argv[1]);
    num = pkey[0];

    if (isalpha(num)) {
        if ((strcasecmp(pkey, "Y") == 0) || (strcasecmp(pkey, "Yes") == 0)) {
            key = 1;
            xQueueSend(cli_handle, &key, 0);
        } else {
            key = 0;
            xQueueSend(cli_handle, &key, 0);
        }
    } else {
        sscanf(pkey, "%d", &key);
        xQueueSend(cli_handle, &key, 0);
    }

    return 0;
}

int scli_receive_key(int *console_key)
{
    return xQueueReceive(cli_handle, console_key, BLE_RX_TIMEOUT);
}

static esp_console_cmd_t cmds[] = {
    {
        .command = "key",
        .help = "",
        .func = enter_passkey_handler,
    },
};

static int ble_register_cli(void)
{
    int cmds_num = sizeof(cmds) / sizeof(esp_console_cmd_t);
    int i;
    for (i = 0; i < cmds_num; i++) {
        esp_console_cmd_register(&cmds[i]);
    }
    return 0;
}

static void scli_task(void *arg)
{
    int uart_num = (int) arg;
    uint8_t linebuf[256];
    int i, cmd_ret;
    esp_err_t ret;
    QueueHandle_t uart_queue;
    uart_event_t event;

    uart_driver_install(uart_num, 256, 0, 8, &uart_queue, 0);
    /* Initialize the console */
    esp_console_config_t console_config = {
        .max_cmdline_args = 8,
        .max_cmdline_length = 256,
    };

    esp_console_init(&console_config);

    while (!stop) {
        i = 0;
        memset(linebuf, 0, sizeof(linebuf));
        do {
            ret = xQueueReceive(uart_queue, (void * )&event, (TickType_t)portMAX_DELAY);
            if (ret != pdPASS) {
                if (stop == 1) {
                    break;
                } else {
                    continue;
                }
            }
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
        } while ((i < 255) && linebuf[i - 1] != '\r');
        if (stop) {
            break;
        }
        /* Remove the truncating \r\n */
        linebuf[strlen((char *)linebuf) - 1] = '\0';
        ret = esp_console_run((char *) linebuf, &cmd_ret);
        if (ret < 0) {
            break;
        }
    }
    vTaskDelete(NULL);
}

int scli_init(void)
{
    /* Register CLI "key <value>" to accept input from user during pairing */
    ble_register_cli();

    xTaskCreate(scli_task, "scli_cli", 4096, (void *) 0, 3, &cli_task);
    if (cli_task == NULL) {
        return ESP_FAIL;
    }
    cli_handle = xQueueCreate( 1, sizeof(int) );
    if (cli_handle == NULL) {
        return ESP_FAIL;
    }
    return ESP_OK;
}
