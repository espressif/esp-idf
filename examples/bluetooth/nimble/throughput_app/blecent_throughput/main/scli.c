/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <ctype.h>
#include "esp_log.h"
#include <string.h>
#include <esp_log.h>
#include <esp_console.h>
#include "esp_vfs_dev.h"
#include "linenoise/linenoise.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <driver/uart.h>

#define BLE_RX_TIMEOUT (30000 / portTICK_PERIOD_MS)
#define BLE_RX_PARAM   (10000 / portTICK_PERIOD_MS)
#define YES_NO_PARAM   (5000 / portTICK_PERIOD_MS)

static QueueHandle_t cli_handle;
static int key[2];
static int conn_param[10];
static int mtu;

#define CONSOLE_PROMPT_LEN_MAX (32)

typedef enum {
    CONSOLE_REPL_STATE_DEINIT,
    CONSOLE_REPL_STATE_INIT,
    CONSOLE_REPL_STATE_START,
} repl_state_t;

static int conn_param_handler(int argc, char *argv[])
{
    ESP_LOGI("Conn param Arguments entered", "%d", argc);
    if (argc != 7) {
        return -1;
    }

    sscanf(argv[1], "%d", &conn_param[0]);
    sscanf(argv[2], "%d", &conn_param[1]);
    sscanf(argv[3], "%d", &conn_param[2]);
    sscanf(argv[4], "%d", &conn_param[3]);
    sscanf(argv[5], "%d", &conn_param[4]);
    sscanf(argv[6], "%d", &conn_param[5]);

    ESP_LOGI("You entered", "%s %d %d %d %d %d %d", argv[0], conn_param[0], conn_param[1],
             conn_param[2], conn_param[3], conn_param[4], conn_param[5]);
    xQueueSend(cli_handle, &conn_param[0], 500 / portTICK_PERIOD_MS);
    return 0;
}

static int conn_mtu_handler(int argc, char *argv[])
{
    ESP_LOGI("MTU Arguments entered", "%d", argc);
    if (argc != 2) {
        return -1;
    }

    sscanf(argv[1], "%d", &mtu);
    ESP_LOGI("You entered", "%s %d", argv[0], mtu);
    xQueueSend(cli_handle, &mtu, 500 / portTICK_PERIOD_MS);
    return 0;
}

static int throughput_demo_handler(int argc, char *argv[])
{
    char pkey[8];

    if (argc != 3) {
        return -1;
    }

    sscanf(argv[1], "%s", pkey);

    if (strcmp(pkey, "read") == 0) {
        key[0] = 1;
    } else if (strcmp(pkey, "write") == 0) {
        key[0] = 2;
    } else if (strcmp(pkey, "notify") == 0) {
        key[0] = 3;
    } else {
        key[0] = 0;
    }

    sscanf(argv[2], "%d", &key[1]);

    ESP_LOGI("Throughput demo handler", "%s %s %d", argv[0], argv[1], key[1]);
    xQueueSend(cli_handle, &key[0], 500 / portTICK_PERIOD_MS);

    return 0;
}

static int yesno_handler(int argc, char *argv[])
{
    char yesno[4];
    bool yes;

    if (argc != 2) {
        return -1;
    }

    sscanf(argv[1], "%s", yesno);

    if (strcmp(yesno, "Yes") || strcmp (yesno, "YES") || strcmp(yesno, "yes")) {
        yes = 1;
    } else {
        yes = 0;
    }

    ESP_LOGI("User entered", "%s %s", argv[0], yesno);
    xQueueSend(cli_handle, &yes, 500 / portTICK_PERIOD_MS);
    return 0;
}

int scli_receive_yesno(bool *console_key)
{
    return xQueueReceive(cli_handle, console_key, YES_NO_PARAM);
}

int scli_receive_key(int *console_key)
{
    return xQueueReceive(cli_handle, console_key, BLE_RX_PARAM);
}

int cli_receive_key(int *console_key)
{
    return xQueueReceive(cli_handle, console_key, BLE_RX_TIMEOUT);
}

void scli_reset_queue(void)
{
    xQueueReset(cli_handle);
}

static esp_console_cmd_t cmds[] = {
    {
        .command = "conn",
        .help = "Set connection parameters: min itvl, max itvl, latency,"
        "supervision timeout, min CE, max CE",
        .func = &conn_param_handler,
    },
    {
        .command = "MTU",
        .help = "Set MTU value",
        .func = &conn_mtu_handler,
    },
    {
        .command = "throughput",
        .help = "Enter read/write/notify and time (in seconds)",
        .func = &throughput_demo_handler,
    },
    {
        .command = "Insert",
        .help = "Enter Insert Yes for YES or Insert No for NO",
        .func = &yesno_handler,
    },
};

void ble_register_cli(void)
{
    int cmds_num = sizeof(cmds) / sizeof(esp_console_cmd_t);
    int i;
    for (i = 0; i < cmds_num; i++) {
        esp_console_cmd_register(&cmds[i]);
    }

    cli_handle = xQueueCreate( 1, sizeof(int) * 6);
    if (cli_handle == NULL) {
        return;
    }

    ESP_LOGI("CLI", "BLE CLI registered ");
    return;
}
