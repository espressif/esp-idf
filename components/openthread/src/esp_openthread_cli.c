/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "openthread/cli.h"

#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_openthread.h"
#include "esp_openthread_common_macro.h"
#include "esp_openthread_cli.h"
#include "esp_openthread_task_queue.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "linenoise/linenoise.h"
#include "esp_console.h"
#define OT_CLI_MAX_LINE_LENGTH      256
#define ESP_CONSOLE_PREFIX          "esp "
#define ESP_CONSOLE_PREFIX_LENGTH   4

static TaskHandle_t s_cli_task;

static ssize_t ot_cli_read_bytes(int fd, void *buf, size_t max_bytes)
{
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);

    int nread = select(fd + 1, &read_fds, NULL, NULL, NULL);
    if (nread <= 0) {
        return -1;
    }

    if (FD_ISSET(fd, &read_fds)) {
        return read(fd, buf, max_bytes);
    }

    return -1;
}

static void ot_cli_set_read_characteristics(void)
{
    int stdin_fileno = fileno(stdin);
    int flags = fcntl(stdin_fileno, F_GETFL);
    if (flags == -1) {
        ESP_LOGW(OT_PLAT_LOG_TAG, "Failed to get stdin flags: %d", errno);
    } else {
        flags |= O_NONBLOCK;
        if (fcntl(stdin_fileno, F_SETFL, flags) == -1) {
            ESP_LOGW(OT_PLAT_LOG_TAG, "Failed to set stdin non-blocking: %d", errno);
        }
    }
    linenoiseSetReadFunction(ot_cli_read_bytes);
}

static int cli_output_callback(void *context, const char *format, va_list args)
{
    char prompt_check[3];
    int ret = 0;
    va_list args_copy;

    va_copy(args_copy, args);
    vsnprintf(prompt_check, sizeof(prompt_check), format, args_copy);
    va_end(args_copy);

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

    esp_err_t ret = esp_openthread_task_queue_post(line_handle_task, line_copy);
    if (ret != ESP_OK) {
        free(line_copy);
    }
    return ret;
}

static int ot_cli_console_callback(int argc, char **argv)
{
    ESP_RETURN_ON_FALSE(argv[1] != NULL && strlen(argv[1]) > 0, ESP_FAIL, OT_PLAT_LOG_TAG, "Invalid OpenThread command");
    char cli_cmd[OT_CLI_MAX_LINE_LENGTH] = {0};
    strncpy(cli_cmd, argv[1], sizeof(cli_cmd) - strlen(cli_cmd) - 1);
    for (int i = 2; i < argc; i++) {
        strncat(cli_cmd, " ", sizeof(cli_cmd) - strlen(cli_cmd) - 1);
        strncat(cli_cmd, argv[i], sizeof(cli_cmd) - strlen(cli_cmd) - 1);
    }
    s_cli_task = xTaskGetCurrentTaskHandle();
    if (esp_openthread_cli_input(cli_cmd) == ESP_OK) {
        xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
    } else {
        printf("Openthread task is busy, failed to run command: %s\n", cli_cmd);
    }
    s_cli_task = NULL;
    return 0;
}

esp_err_t esp_openthread_cli_console_command_register(void)
{
    esp_console_cmd_t cmd = {
        .command = CONFIG_OPENTHREAD_CONSOLE_COMMAND_PREFIX,
        .help = "Execute `"CONFIG_OPENTHREAD_CONSOLE_COMMAND_PREFIX" ...` to run openthread cli",
        .hint = NULL,
        .func = ot_cli_console_callback,
    };
    return esp_console_cmd_register(&cmd);
}

esp_err_t esp_openthread_cli_console_command_unregister(void)
{
    return esp_console_cmd_deregister(CONFIG_OPENTHREAD_CONSOLE_COMMAND_PREFIX);
}

static void ot_cli_loop(void *context)
{
    int ret = 0;
    const char *prompt = "> ";
    esp_console_config_t console_config = ESP_CONSOLE_CONFIG_DEFAULT();
    console_config.max_cmdline_length = OT_CLI_MAX_LINE_LENGTH;

    console_config.hint_color = -1;
    ret = esp_console_init(&console_config);
    if (ret != ESP_OK) {
        ESP_LOGE(OT_PLAT_LOG_TAG, "Failed to initialize console: %s", esp_err_to_name(ret));
        vTaskDelete(NULL);
        return;
    }

    linenoiseSetMultiLine(true);
    linenoiseHistorySetMaxLen(100);
    linenoiseSetMaxLineLen(OT_CLI_MAX_LINE_LENGTH);
    linenoiseAllowEmpty(false);

    if (linenoiseProbe()) {
        linenoiseSetDumbMode(1);
    }
    ot_cli_set_read_characteristics();

    while (true) {
        char *line = linenoise(prompt);
        if (line == NULL) {
            continue;
        }
        if (strnlen(line, OT_CLI_MAX_LINE_LENGTH)) {
            printf("\r\n");
            if (memcmp(line, ESP_CONSOLE_PREFIX, ESP_CONSOLE_PREFIX_LENGTH) == 0) {
                esp_err_t err = esp_console_run(line + ESP_CONSOLE_PREFIX_LENGTH, &ret);
                if (err == ESP_ERR_NOT_FOUND) {
                    printf("Unrecognized command\n");
                } else if (err == ESP_ERR_INVALID_ARG) {
                    // command was empty
                    printf("Command is empty\n");
                } else if (err == ESP_OK && ret != ESP_OK) {
                    printf("Command returned non-zero error code: 0x%x (%s)\n", ret, esp_err_to_name(ret));
                } else if (err != ESP_OK) {
                    printf("Internal error: %s\n", esp_err_to_name(err));
                }
            } else {
                if (esp_openthread_cli_input(line) == ESP_OK) {
                    xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
                } else {
                    printf("Openthread task is busy, failed to run command: %s\n", line);
                }
            }
            linenoiseHistoryAdd(line);
        }
        linenoiseFree(line);
    }
}

void esp_openthread_cli_create_task(void)
{
    BaseType_t ret = xTaskCreate(ot_cli_loop, "ot_cli", 4096, xTaskGetCurrentTaskHandle(), 4, &s_cli_task);
    ESP_RETURN_ON_FALSE(ret == pdPASS, , OT_PLAT_LOG_TAG, "Failed to create OpenThread CLI task");
}
