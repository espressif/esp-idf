/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_openthread_cli.h"

#include <stdio.h>
#include <string.h>

#include "openthread/cli.h"

#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_openthread.h"
#include "esp_openthread_common_macro.h"
#include "esp_openthread_task_queue.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "linenoise/linenoise.h"

#define OT_CLI_MAX_LINE_LENGTH 256

static TaskHandle_t s_cli_task;

static int cli_output_callback(void *context, const char *format, va_list args)
{
    char prompt_check[3];
    int ret = 0;

    vsnprintf(prompt_check, sizeof(prompt_check), format, args);
    if (!strncmp(prompt_check, "> ", sizeof(prompt_check)) && s_cli_task) {
        xTaskNotifyGive(s_cli_task);
    } else {
        ret = vprintf(format, args);
    }
    return ret;
}

void esp_openthread_cli_init(void)
{
    otCliInit(esp_openthread_get_instance(), cli_output_callback, NULL);
}

void line_handle_task(void *context)
{
    char *line = (char *)context;

    otCliInputLine(line);
    free(line);
}

esp_err_t esp_openthread_cli_input(const char *line)
{
    char *line_copy = strdup(line);

    ESP_RETURN_ON_FALSE(line_copy != NULL, ESP_ERR_NO_MEM, OT_PLAT_LOG_TAG, "Failed to copy OpenThread CLI line input");

    return esp_openthread_task_queue_post(line_handle_task, line_copy);
}

static void ot_cli_loop(void *context)
{
    const char *prompt = "> ";

    linenoiseSetMultiLine(true);
    linenoiseHistorySetMaxLen(100);
    linenoiseSetMaxLineLen(OT_CLI_MAX_LINE_LENGTH);
    linenoiseAllowEmpty(false);

    if (linenoiseProbe()) {
        linenoiseSetDumbMode(1);
    }

    while (true) {
        char *line = linenoise(prompt);
        if (line && strnlen(line, OT_CLI_MAX_LINE_LENGTH)) {
            printf("\r\n");
            esp_openthread_cli_input(line);
            linenoiseHistoryAdd(line);
            xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
        }
        linenoiseFree(line);
    }
}

void esp_openthread_cli_create_task()
{
    xTaskCreate(ot_cli_loop, "ot_cli", 4096, xTaskGetCurrentTaskHandle(), 4, &s_cli_task);

    return;
}
