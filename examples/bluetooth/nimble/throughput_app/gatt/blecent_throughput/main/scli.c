/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <ctype.h>
#include "esp_log.h"
#include <string.h>
#include <strings.h>
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
#define PEER_ADDR_PARAM (5000 / portTICK_PERIOD_MS)

static QueueHandle_t cli_handle;

#define CLI_CONN_PARAM_COUNT 6

#define CLI_MSG_TYPE_CONN_PARAM 1
#define CLI_MSG_TYPE_MTU        2
#define CLI_MSG_TYPE_THROUGHPUT 3
#define CLI_MSG_TYPE_YESNO      4
#define CLI_MSG_TYPE_PEER_ADDR  5

#define CLI_PEER_ADDR_LEN       6

struct cli_msg {
    int type;
    union {
        int conn_param[CLI_CONN_PARAM_COUNT];
        int mtu;
        int key[3];
        bool yes;
        uint8_t peer_addr[CLI_PEER_ADDR_LEN];
    } data;
};

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

    struct cli_msg msg = {
        .type = CLI_MSG_TYPE_CONN_PARAM,
    };

    sscanf(argv[1], "%d", &msg.data.conn_param[0]);
    sscanf(argv[2], "%d", &msg.data.conn_param[1]);
    sscanf(argv[3], "%d", &msg.data.conn_param[2]);
    sscanf(argv[4], "%d", &msg.data.conn_param[3]);
    sscanf(argv[5], "%d", &msg.data.conn_param[4]);
    sscanf(argv[6], "%d", &msg.data.conn_param[5]);

    ESP_LOGI("You entered", "%s %d %d %d %d %d %d", argv[0], msg.data.conn_param[0], msg.data.conn_param[1],
             msg.data.conn_param[2], msg.data.conn_param[3], msg.data.conn_param[4], msg.data.conn_param[5]);
    if (cli_handle == NULL) {
        ESP_LOGE("CLI", "Queue not initialized");
        return -1;
    }
    xQueueSend(cli_handle, &msg, 500 / portTICK_PERIOD_MS);
    return 0;
}

static int conn_mtu_handler(int argc, char *argv[])
{
    int ret;
    int mtu;
    ESP_LOGI("MTU Arguments entered", "%d", argc);
    if (argc != 2) {
        return -1;
    }

    ret = sscanf(argv[1], "%d", &mtu);
    if (ret != 1) {
        return -1;
    }
    struct cli_msg msg = {
        .type = CLI_MSG_TYPE_MTU,
        .data.mtu = mtu,
    };
    if (cli_handle) {
        xQueueSend(cli_handle, &msg, 500 / portTICK_PERIOD_MS);
    }
    return 0;
}

static int throughput_demo_handler(int argc, char *argv[])
{
    char pkey[8] = {0};
    struct cli_msg msg = {
        .type = CLI_MSG_TYPE_THROUGHPUT,
    };

    if (argc != 4) {
        return -1;
    }

    if (cli_handle == NULL) {
        ESP_LOGE("CLI", "Queue not initialized");
        return -1;
    }

    if (sscanf(argv[1], "%7s", pkey) != 1) {
        ESP_LOGE("CLI", "Failed to parse throughput type");
        return -1;
    }

    if (strcmp(pkey, "read") == 0) {
        msg.data.key[0] = 1;
    } else if (strcmp(pkey, "write") == 0) {
        msg.data.key[0] = 2;
    } else if (strcmp(pkey, "notify") == 0) {
        msg.data.key[0] = 3;
    } else {
        msg.data.key[0] = 0;
    }

    sscanf(argv[2], "%d", &msg.data.key[1]);
    sscanf(argv[3], "%d", &msg.data.key[2]);
    ESP_LOGI("Throughput demo handler", "%s %s %d %d", argv[0], argv[1], msg.data.key[1], msg.data.key[2]);
    xQueueSend(cli_handle, &msg, 500 / portTICK_PERIOD_MS);

    return 0;
}

static int yesno_handler(int argc, char *argv[])
{
    char yesno[4] = {0};
    bool yes;

    if (argc != 2) {
        return -1;
    }

    if (sscanf(argv[1], "%3s", yesno) != 1) {
        ESP_LOGE("CLI", "Failed to parse yes/no argument");
        return -1;
    }

    if (strcmp(yesno, "Yes") == 0 || strcmp(yesno, "YES") == 0 || strcmp(yesno, "yes") == 0) {
        yes = 1;
    } else if (strcmp(yesno, "No") == 0 || strcmp(yesno, "NO") == 0 || strcmp(yesno, "no") == 0) {
        yes = 0;
    } else {
        yes = 0;  /* invalid input */
    }

    ESP_LOGI("User entered", "%s %s", argv[0], yesno);
    /* Use struct cli_msg to exactly match the queue item size (sizeof(struct cli_msg))
     * and avoid stack buffer over-read from sending undersized raw buffers. */
    struct cli_msg msg = {
        .type = CLI_MSG_TYPE_YESNO,
        .data.yes = yes,
    };
    if (cli_handle) {
        xQueueSend(cli_handle, &msg, 500 / portTICK_PERIOD_MS);
    }
    return 0;
}

static int peer_addr_handler(int argc, char *argv[])
{
    struct cli_msg msg = {
        .type = CLI_MSG_TYPE_PEER_ADDR,
    };

    if (argc != 2) {
        return -1;
    }

    /* val[0] holds the least significant byte, so parse in reverse order. */
    if (sscanf(argv[1], "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
               &msg.data.peer_addr[5], &msg.data.peer_addr[4],
               &msg.data.peer_addr[3], &msg.data.peer_addr[2],
               &msg.data.peer_addr[1], &msg.data.peer_addr[0]) != CLI_PEER_ADDR_LEN) {
        ESP_LOGE("CLI", "Failed to parse peer address");
        return -1;
    }

    ESP_LOGI("User entered", "%s %s", argv[0], argv[1]);
    if (cli_handle == NULL) {
        ESP_LOGE("CLI", "Queue not initialized");
        return -1;
    }
    xQueueSend(cli_handle, &msg, 500 / portTICK_PERIOD_MS);
    return 0;
}

int scli_receive_peer_addr(uint8_t peer_addr[6])
{
    struct cli_msg msg;

    if (cli_handle == NULL) {
        return 0;
    }
    if (xQueueReceive(cli_handle, &msg, PEER_ADDR_PARAM) != pdTRUE) {
        return 0;
    }
    if (msg.type != CLI_MSG_TYPE_PEER_ADDR) {
        return 0;
    }

    memcpy(peer_addr, msg.data.peer_addr, CLI_PEER_ADDR_LEN);
    return 1;
}

int scli_receive_yesno(bool *console_key)
{
    struct cli_msg msg;
    int ret;

    if (cli_handle == NULL) {
        return pdFALSE;
    }
    ret = xQueueReceive(cli_handle, &msg, YES_NO_PARAM);
    if (ret == pdPASS) {
        *console_key = msg.data.yes;
    }
    return ret;
}

int scli_receive_key(int console_key[6])
{
    struct cli_msg msg;
    if (cli_handle == NULL) {
        return 0;
    }
    if (xQueueReceive(cli_handle, &msg, BLE_RX_PARAM) != pdTRUE) {
        return 0;
    }

    switch (msg.type) {
    case CLI_MSG_TYPE_MTU:
        console_key[0] = msg.data.mtu;
        return 1;
    case CLI_MSG_TYPE_CONN_PARAM:
        memcpy(console_key, msg.data.conn_param,
               sizeof(msg.data.conn_param));
        return 1;
    case CLI_MSG_TYPE_THROUGHPUT:
        memcpy(console_key, msg.data.key, sizeof(msg.data.key));
        return 1;
    default:
        return 0;
    }
}

int cli_receive_key(int console_key[6])
{
    struct cli_msg msg;
    if (cli_handle == NULL) {
        return 0;
    }
    if (xQueueReceive(cli_handle, &msg, BLE_RX_TIMEOUT) != pdTRUE) {
        return 0;
    }
    switch (msg.type) {
    case CLI_MSG_TYPE_CONN_PARAM:
        memcpy(console_key, msg.data.conn_param, sizeof(msg.data.conn_param));
        return 1;
    case CLI_MSG_TYPE_THROUGHPUT:
        memcpy(console_key, msg.data.key, sizeof(msg.data.key));
        return 1;
    case CLI_MSG_TYPE_MTU:
        console_key[0] = msg.data.mtu;
        return 1;
    default:
        return 0;
    }
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
    {
        .command = "peer",
        .help = "Connect only to this peer address: peer xx:xx:xx:xx:xx:xx",
        .func = &peer_addr_handler,
    },
};

void ble_register_cli(void)
{
    int cmds_num = sizeof(cmds) / sizeof(esp_console_cmd_t);
    int i;
    for (i = 0; i < cmds_num; i++) {
        esp_console_cmd_register(&cmds[i]);
    }

    cli_handle = xQueueCreate(1, sizeof(struct cli_msg));
    if (cli_handle == NULL) {
        return;
    }

    ESP_LOGI("CLI", "BLE CLI registered ");
    return;
}
