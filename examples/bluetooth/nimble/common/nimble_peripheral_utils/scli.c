/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
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

    sscanf(argv[1], "%7s", pkey);
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
        if (sscanf(pkey, "%d", &key) != 1) {
            key = 0;
        }
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

    ESP_ERROR_CHECK(uart_driver_install(uart_num, 256, 0, 8, &uart_queue, 0));
    /* Initialize the console */
    esp_console_config_t console_config = {
        .max_cmdline_args = 8,
        .max_cmdline_length = 256,
    };

    ESP_ERROR_CHECK(esp_console_init(&console_config));

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
                    if (i >= sizeof(linebuf) - 1) {
                        break;
                    }
                    if (linebuf[i] == '\r') {
                        uart_write_bytes(uart_num, "\r\n", 2);
                    } else {
                        uart_write_bytes(uart_num, (char *) &linebuf[i], 1);
                    }
                    i++;
                }
            }

        } while ((i < 255) && (i == 0 || linebuf[i - 1] != '\r'));
        if (stop) {
            break;
        }
        /* Remove the truncating \r\n */
        size_t len = strlen((char *)linebuf);
        if (len > 0) {
            linebuf[len - 1] = '\0';
        }
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

int scli_deinit(void)
{
    if (cli_task == NULL) {
        return ESP_OK;  // Already deinitialized
    }

    // Signal task to exit
    stop = 1;

    // Wait for task to exit (it will clean up UART and console)
    int timeout_ms = 200;
    while (timeout_ms > 0 && cli_task != NULL && eTaskGetState(cli_task) != eDeleted) {
        vTaskDelay(pdMS_TO_TICKS(10));
        timeout_ms -= 10;
    }

    // Force delete if still running (shouldn't happen if task exits properly)
    if (cli_task != NULL && eTaskGetState(cli_task) != eDeleted) {
        vTaskDelete(cli_task);
        // If force-deleted, clean up resources manually
        uart_driver_delete(0);
        esp_console_deinit();
    }
    cli_task = NULL;

    // Clean up queue
    if (cli_handle != NULL) {
        vQueueDelete(cli_handle);
        cli_handle = NULL;
    }

    // Reset stop flag
    stop = 0;

    return ESP_OK;
}
