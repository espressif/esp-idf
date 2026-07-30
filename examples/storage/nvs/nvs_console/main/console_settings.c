/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "esp_system.h"
#include "esp_log.h"
#include "esp_console.h"
#include "esp_stdio.h"
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"
#include "console_settings.h"

#define CONSOLE_MAX_CMDLINE_ARGS 8
#define CONSOLE_MAX_CMDLINE_LENGTH 256
#define CONSOLE_PROMPT_MAX_LEN (32)

static const char *TAG = "console_settings";
static char prompt[CONSOLE_PROMPT_MAX_LEN];

void initialize_console_peripheral(void)
{
    /* Drain stdout before reconfiguring it */
    fflush(stdout);
    fsync(fileno(stdout));

    /* Install UART / USB Serial/JTAG / USB CDC driver based on menuconfig */
    ESP_ERROR_CHECK(esp_stdio_install_io_driver());

    /* Disable buffering on stdin */
    setvbuf(stdin, NULL, _IONBF, 0);
}

void initialize_console_library(const char *history_path)
{
    /* Initialize the console */
    esp_console_config_t console_config = {
        .max_cmdline_args = CONSOLE_MAX_CMDLINE_ARGS,
        .max_cmdline_length = CONSOLE_MAX_CMDLINE_LENGTH,
#if CONFIG_LOG_COLORS
        .hint_color = atoi(LOG_COLOR_CYAN)
#endif
    };
    ESP_ERROR_CHECK(esp_console_init(&console_config));

    /* Configure linenoise line completion library */
    linenoiseSetMultiLine(1);

    /* Tell linenoise where to get command completions and hints */
    linenoiseSetCompletionCallback(&esp_console_get_completion);
    linenoiseSetHintsCallback((linenoiseHintsCallback*) &esp_console_get_hint);

    /* Set command history size */
    linenoiseHistorySetMaxLen(100);

    /* Set command maximum length */
    linenoiseSetMaxLineLen(console_config.max_cmdline_length);

    /* Don't return empty lines */
    linenoiseAllowEmpty(false);

#if CONFIG_CONSOLE_STORE_HISTORY
    /* Load command history from filesystem */
    if (history_path) {
        linenoiseHistoryLoad(history_path);
    }
#endif

    /* Figure out if the terminal supports escape sequences */
    int probe_status = linenoiseProbe();
    if (probe_status) { /* zero indicates success */
        ESP_LOGW(TAG, "Your terminal application does not support escape sequences.\n"
                 "Line editing and history features are disabled.\n"
                 "On Windows, try using Windows Terminal or Putty instead.");
        linenoiseSetDumbMode(1);
    }
}

char *setup_prompt(const char *prompt_str)
{
    const char *prompt_temp = "esp>";
    if (prompt_str) {
        prompt_temp = prompt_str;
    }

    /* Set command line prompt */
    if (linenoiseIsDumbMode()) {
        /* Since the terminal doesn't support escape sequences,
         * don't use color codes in the prompt */
        snprintf(prompt, CONSOLE_PROMPT_MAX_LEN - 1, "%s ", prompt_temp);
    } else {
        snprintf(prompt, CONSOLE_PROMPT_MAX_LEN - 1, LOG_COLOR_I "%s " LOG_RESET_COLOR, prompt_temp);
    }

    return prompt;
}
