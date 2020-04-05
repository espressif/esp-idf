// Copyright 2016-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdint.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_console.h"
#include "esp_vfs_dev.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "linenoise/linenoise.h"

static const char *TAG = "console.repl";

#define CONSOLE_PROMPT_LEN_MAX (32)

typedef enum {
    CONSOLE_REPL_STATE_DEINIT,
    CONSOLE_REPL_STATE_INIT,
    CONSOLE_REPL_STATE_START,
} repl_state_t;

static repl_state_t s_repl_state =  CONSOLE_REPL_STATE_DEINIT;

/**
 * @brief Prompt to be printed before each line.
 *
 */
static char s_prompt[CONSOLE_PROMPT_LEN_MAX];

/**
 * @brief path to save history commands in file system
 *
 */
static const char *s_history_save_path = NULL;

/**
 * @brief default uart channel number
 *
 */
static int s_uart_channel = -1;

/**
 * @brief REPL task handle
 *
 */
static TaskHandle_t s_repl_task_hdl = NULL;

static void esp_console_repl_thread(void *param)
{
    // waiting for task notify
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    while (s_repl_state == CONSOLE_REPL_STATE_START) {
        char *line = linenoise(s_prompt);
        if (line == NULL) {
            ESP_LOGD(TAG, "empty line");
            /* Ignore empty lines */
            continue;
        }
        /* Add the command to the history */
        linenoiseHistoryAdd(line);
        /* Save command history to filesystem */
        if (s_history_save_path) {
            linenoiseHistorySave(s_history_save_path);
        }

        /* Try to run the command */
        int ret;
        esp_err_t err = esp_console_run(line, &ret);
        if (err == ESP_ERR_NOT_FOUND) {
            printf("Unrecognized command\n");
        } else if (err == ESP_ERR_INVALID_ARG) {
            // command was empty
        } else if (err == ESP_OK && ret != ESP_OK) {
            printf("Command returned non-zero error code: 0x%x (%s)\n", ret, esp_err_to_name(ret));
        } else if (err != ESP_OK) {
            printf("Internal error: %s\n", esp_err_to_name(err));
        }
        /* linenoise allocates line buffer on the heap, so need to free it */
        linenoiseFree(line);
    }
    ESP_LOGD(TAG, "The End");
    vTaskDelete(NULL);
}

esp_err_t esp_console_repl_init(const esp_console_repl_config_t *config)
{
    esp_err_t ret = ESP_OK;
    if (!config) {
        ret = ESP_ERR_INVALID_ARG;
        goto _exit;
    }

    // check if already initialized
    if (s_repl_state != CONSOLE_REPL_STATE_DEINIT) {
        ESP_LOGE(TAG, "already initialized");
        ret = ESP_ERR_INVALID_STATE;
        goto _exit;
    }

    /* Drain stdout before reconfiguring it */
    fflush(stdout);
    fsync(fileno(stdout));

    /* Disable buffering on stdin */
    setvbuf(stdin, NULL, _IONBF, 0);

    /* Minicom, screen, idf_monitor send CR when ENTER key is pressed */
    esp_vfs_dev_uart_set_rx_line_endings(ESP_LINE_ENDINGS_CR);
    /* Move the caret to the beginning of the next line on '\n' */
    esp_vfs_dev_uart_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);

    /* Configure UART. Note that REF_TICK is used so that the baud rate remains
     * correct while APB frequency is changing in light sleep mode.
     */
    const uart_config_t uart_config = {
        .baud_rate = config->device.uart.baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .source_clk = UART_SCLK_REF_TICK,
    };
    /* Install UART driver for interrupt-driven reads and writes */
    ret = uart_driver_install(config->device.uart.channel, 256, 0, 0, NULL, 0);
    if (ret != ESP_OK) {
        goto _exit;
    }
    s_uart_channel = config->device.uart.channel;
    uart_param_config(s_uart_channel, &uart_config);
    uart_set_pin(s_uart_channel, config->device.uart.tx_gpio, config->device.uart.rx_gpio, -1, -1);

    /* Tell VFS to use UART driver */
    esp_vfs_dev_uart_use_driver(s_uart_channel);

    /* Initialize the console */
    esp_console_config_t console_config = ESP_CONSOLE_CONFIG_DEFAULT();
#if CONFIG_LOG_COLORS
    console_config.hint_color = atoi(LOG_COLOR_CYAN);
#endif
    ret = esp_console_init(&console_config);
    if (ret != ESP_OK) {
        goto _console_del;
    }

    ret = esp_console_register_help_command();
    if (ret != ESP_OK) {
        goto _console_del;
    }

    ret = esp_console_register_quit_command();
    if (ret != ESP_OK) {
        goto _console_del;
    }

    /* Configure linenoise line completion library */
    /* Enable multiline editing. If not set, long commands will scroll within single line */
    linenoiseSetMultiLine(1);

    /* Tell linenoise where to get command completions and hints */
    linenoiseSetCompletionCallback(&esp_console_get_completion);
    linenoiseSetHintsCallback((linenoiseHintsCallback *)&esp_console_get_hint);

    if (config->history_save_path) {
        s_history_save_path = config->history_save_path;
        /* Load command history from filesystem */
        linenoiseHistoryLoad(s_history_save_path);
    }

    /* Set command history size */
    if (linenoiseHistorySetMaxLen(config->max_history_len) != 1) {
        ESP_LOGE(TAG, "set max history length to %d failed", config->max_history_len);
        ret = ESP_FAIL;
        goto _console_del;
    }

    /* set command line prompt */
    const char *prompt_temp = "esp>";
    if (config->prompt) {
        prompt_temp = config->prompt;
    }
    snprintf(s_prompt, CONSOLE_PROMPT_LEN_MAX - 1, LOG_COLOR_I "%s " LOG_RESET_COLOR, prompt_temp);

    printf("\r\n"
           "Type 'help' to get the list of commands.\r\n"
           "Use UP/DOWN arrows to navigate through command history.\r\n"
           "Press TAB when typing command name to auto-complete.\r\n");

    /* Figure out if the terminal supports escape sequences */
    int probe_status = linenoiseProbe();
    if (probe_status) {
        /* zero indicates success */
        printf("\r\n"
               "Your terminal application does not support escape sequences.\n\n"
               "Line editing and history features are disabled.\n\n"
               "On Windows, try using Putty instead.\r\n");
        linenoiseSetDumbMode(1);
#if CONFIG_LOG_COLORS
        /* Since the terminal doesn't support escape sequences,
         * don't use color codes in the s_prompt.
         */
        snprintf(s_prompt, CONSOLE_PROMPT_LEN_MAX - 1, "%s ", prompt_temp);
#endif //CONFIG_LOG_COLORS
    }

    /* spawn a single thread to run REPL */
    if (xTaskCreate(esp_console_repl_thread, "console_repl", config->task_stack_size,
                    NULL, config->task_priority, &s_repl_task_hdl) != pdTRUE) {
        ret = ESP_FAIL;
        goto _console_del;
    }
    s_repl_state = CONSOLE_REPL_STATE_INIT;

    return ret;

_console_del:
    esp_console_deinit();
    esp_vfs_dev_uart_use_nonblocking(s_uart_channel);
    uart_driver_delete(s_uart_channel);
    s_uart_channel = -1;
    s_repl_state = CONSOLE_REPL_STATE_DEINIT;
_exit:
    return ret;
}

esp_err_t esp_console_repl_deinit(void)
{
    esp_err_t ret = ESP_OK;

    // check if already de-initialized
    if (s_repl_state == CONSOLE_REPL_STATE_DEINIT) {
        ESP_LOGE(TAG, "not initialized yet");
        ret = ESP_ERR_INVALID_STATE;
        goto _exit;
    }

    s_repl_state = CONSOLE_REPL_STATE_DEINIT;
    esp_console_deinit();
    esp_vfs_dev_uart_use_nonblocking(s_uart_channel);
    uart_driver_delete(s_uart_channel);
    s_uart_channel = -1;
    s_repl_task_hdl = NULL;
    s_history_save_path = NULL;
_exit:
    return ret;
}

esp_err_t esp_console_repl_start(void)
{
    esp_err_t ret = ESP_OK;

    // check if already initialized
    if (s_repl_state != CONSOLE_REPL_STATE_INIT) {
        ret = ESP_ERR_INVALID_STATE;
        goto _exit;
    }

    s_repl_state = CONSOLE_REPL_STATE_START;
    xTaskNotifyGive(s_repl_task_hdl);

_exit:
    return ret;
}

/* handle 'quit' command */
static int do_cmd_quit(int argc, char **argv)
{
    printf("ByeBye\r\n");
    esp_console_repl_deinit();
    return 0;
}

esp_err_t esp_console_register_quit_command(void)
{
    esp_console_cmd_t command = {
        .command = "quit",
        .help = "Quit REPL environment",
        .func = &do_cmd_quit
    };
    return esp_console_cmd_register(&command);
}
