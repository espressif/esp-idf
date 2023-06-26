/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <esp_log.h>
#include <esp_console.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <driver/uart.h>

#include <protocomm.h>
#include <protocomm_console.h>

#include "protocomm_priv.h"

#define LINE_BUF_SIZE 256
static const char *TAG = "protocomm_console";

static uint32_t session_id = PROTOCOMM_NO_SESSION_ID;
static protocomm_t *pc_console   = NULL; /* The global protocomm instance for console */
static TaskHandle_t console_task = NULL;

esp_err_t protocomm_console_stop(protocomm_t *pc)
{
    if (pc != pc_console) {
        ESP_LOGE(TAG, "Incorrect stop request");
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI(TAG, "Stopping console...");
    xTaskNotifyGive(console_task);
    return ESP_OK;
}

static ssize_t hex2bin(const char *hexstr, uint8_t *bytes)
{
    size_t hexstrLen = strlen(hexstr);
    ssize_t bytesLen = hexstrLen / 2;

    int count = 0;
    const char *pos = hexstr;

    for(count = 0; count < bytesLen; count++) {
        sscanf(pos, "%2hhx", &bytes[count]);
        pos += 2;
    }

    return bytesLen;
}

static bool stopped(void)
{
    uint32_t flag = 0;
    xTaskNotifyWait(0, 0, &flag, (TickType_t) 10/portTICK_PERIOD_MS);
    return (flag != 0);
}

static void protocomm_console_task(void *arg)
{
    int uart_num = (int) arg;
    uint8_t linebuf[LINE_BUF_SIZE];
    int i, cmd_ret;
    esp_err_t ret;
    QueueHandle_t uart_queue;
    uart_event_t event;

    ESP_LOGD(TAG, "Initializing UART on port %d", uart_num);
    uart_driver_install(uart_num, LINE_BUF_SIZE, 0, 8, &uart_queue, 0);
    /* Initialize the console */
    esp_console_config_t console_config = {
            .max_cmdline_args = 8,
            .max_cmdline_length = LINE_BUF_SIZE,
    };

    esp_console_init(&console_config);
    esp_console_register_help_command();

    while (!stopped()) {
        uart_write_bytes(uart_num, "\n>> ", 4);
        memset(linebuf, 0, sizeof(linebuf));
        i = 0;
        do {
            ret = xQueueReceive(uart_queue, (void * )&event, (TickType_t) 10/portTICK_PERIOD_MS);
            if (ret != pdPASS) {
                if (stopped()) {
                    break;
                } else {
                    continue;
                }
            }
            if (event.type == UART_DATA) {
                while (uart_read_bytes(uart_num, (uint8_t *) &linebuf[i], 1, 0) && (i < LINE_BUF_SIZE)) {
                    if (linebuf[i] == '\r') {
                        uart_write_bytes(uart_num, "\r\n", 2);
                    } else {
                        uart_write_bytes(uart_num, (char *) &linebuf[i], 1);
                    }
                    i++;
                }
            }
            if ((i > 0) && (linebuf[i-1] == '\r')) {
                break;
            }
        } while (i < LINE_BUF_SIZE);
        if (stopped()) {
            break;
        }
        ret = esp_console_run((char *) linebuf, &cmd_ret);
        if (ret < 0) {
            ESP_LOGE(TAG, "Console dispatcher error");
            break;
        }
    }

    pc_console = NULL;
    esp_console_deinit();

    ESP_LOGI(TAG, "Console stopped");
    vTaskDelete(NULL);
}

static int common_cmd_handler(int argc, char** argv)
{
    if (argc < 3) {
        return ESP_ERR_INVALID_ARG;
    }

    int i, ret;

    uint32_t cur_session_id = atoi(argv[1]);

    uint8_t *buf = (uint8_t *) malloc(strlen(argv[2]));
    if (buf == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory");
        return ESP_ERR_NO_MEM;
    }
    uint8_t *outbuf;
    ssize_t outlen;
    ssize_t len = hex2bin(argv[2], buf);

    if (cur_session_id != session_id) {
        if (pc_console->sec && pc_console->sec->new_transport_session) {
            ret = pc_console->sec->new_transport_session(pc_console->sec_inst, cur_session_id);
            if (ret == ESP_OK) {
                session_id = cur_session_id;
            }
        }
    }

    ret = protocomm_req_handle(pc_console, argv[0], cur_session_id, buf, len, &outbuf, &outlen);
    free(buf);

    if (ret == ESP_OK) {
        printf("\r\n");
        for (i = 0; i < outlen; i++) {
            printf("%02x", outbuf[i]);
        }
        printf("\r\n");

        /* Transport is responsible for freeing the transmit buffer */
        free(outbuf);

        return ESP_OK;
    } else {
        return ret;
    }
}

static esp_err_t protocomm_console_add_endpoint(const char *ep_name, protocomm_req_handler_t req_handler, void *priv_data)
{
    (void) req_handler;
    (void) priv_data;

    esp_err_t ret;
    esp_console_cmd_t cmd;
    memset(&cmd, 0, sizeof(cmd));

    cmd.command = ep_name;
    cmd.help = "";
    cmd.func = common_cmd_handler;

    ret = esp_console_cmd_register(&cmd);

    return ret;
}

static esp_err_t protocomm_console_remove_endpoint(const char *ep_name)
{
    /* Command deletion happens internally in esp_console_deinit function */

    return ESP_OK;
}

esp_err_t protocomm_console_start(protocomm_t *pc, const protocomm_console_config_t *config)
{
    if (pc == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (pc_console != NULL) {
        if (pc_console == pc) {
            return ESP_ERR_INVALID_STATE;
        }
        else {
            return ESP_ERR_NOT_SUPPORTED;
        }
    }


    if (xTaskCreate(protocomm_console_task, "protocomm_console",
                    config->stack_size, NULL, config->task_priority, &console_task) != pdPASS) {
        return ESP_FAIL;
    }

    pc->add_endpoint = protocomm_console_add_endpoint;
    pc->remove_endpoint = protocomm_console_remove_endpoint;
    pc_console = pc;
    return ESP_OK;
}
