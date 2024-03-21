/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <string.h>
#include <assert.h>
#include <sys/cdefs.h>

#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "linenoise/linenoise.h"
#include "esp_console.h"
#include "console_private.h"

static const char *TAG = "console.repl";

static struct termios s_orig_termios;

/**
 * This function restores the original terminal settings.
 */
static void disable_raw_mode(void)
{
    assert(tcsetattr(STDIN_FILENO, TCSAFLUSH, &s_orig_termios) == 0);
}

/**
 * Depending on if the input is a terminal or a file or pipe, we need to apply different
 * settings to avoid additional processing or buffering getting into our way.
 */
static void prepare_input_stream(void)
{
    // Set stdin to unbuffered
    setvbuf(stdin, NULL, _IONBF, 0);

    const int stdin_fileno = fileno(stdin);

    if (isatty(stdin_fileno)) {
        // Use Termios driver to activate CR-NL translation and deactivate echo and canonical mode
        assert(tcgetattr(stdin_fileno, &s_orig_termios) == 0);
        struct termios raw = s_orig_termios;
        raw.c_iflag |= ICRNL; // we translate to NL because linenoise expects NL
        raw.c_lflag &= ~(ECHO | ICANON); // turn off echo and cononical mode
        assert(tcsetattr(stdin_fileno, TCSAFLUSH, &raw) == 0);

        // Make sure user does not end up with a broken terminal
        assert(atexit(disable_raw_mode) == 0);
    } else {
        // Flush input
        assert(fflush(stdin) == 0);
    }
}

static esp_err_t esp_console_repl_linux_delete(esp_console_repl_t *repl)
{
    esp_err_t ret = ESP_OK;
    esp_console_repl_com_t *repl_com = __containerof(repl, esp_console_repl_com_t, repl_core);
    esp_console_repl_universal_t *linux_repl = __containerof(repl_com, esp_console_repl_universal_t, repl_com);
    // check if already de-initialized
    if (repl_com->state == CONSOLE_REPL_STATE_DEINIT) {
        ESP_LOGE(TAG, "already de-initialized");
        ret = ESP_ERR_INVALID_STATE;
        goto _exit;
    }
    repl_com->state = CONSOLE_REPL_STATE_DEINIT;
    esp_console_deinit();

    free(linux_repl);
_exit:
    return ret;
}

static esp_err_t esp_console_new_repl_linux(const esp_console_repl_config_t *repl_config, esp_console_repl_t **ret_repl)
{
    esp_console_repl_universal_t *linux_repl = NULL;
    if (!repl_config || !ret_repl) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = ESP_OK;
    // allocate memory for console REPL context
    linux_repl = calloc(1, sizeof(esp_console_repl_universal_t));
    if (!linux_repl) {
        ret = ESP_ERR_NO_MEM;
        goto _exit;
    }

    /* Enable blocking mode on stdin and stdout */
    fcntl(fileno(stdout), F_SETFL, 0);
    fcntl(fileno(stdin), F_SETFL, 0);

    // initialize console , common part
    ret = esp_console_common_init(repl_config->max_cmdline_length, &linux_repl->repl_com);
    if (ret != ESP_OK) {
        goto _exit;
    }

    // setup history
    ret = esp_console_setup_history(repl_config->history_save_path, repl_config->max_history_len, &linux_repl->repl_com);
    if (ret != ESP_OK) {
        goto _exit;
    }

    // Make sure the setup works on Linux without buffering or additional processing
    prepare_input_stream();

    // setup prompt
    esp_console_setup_prompt(repl_config->prompt, &linux_repl->repl_com);

    /* Fill the structure here as it will be used directly by the created task. */
    linux_repl->uart_channel = CONFIG_ESP_CONSOLE_UART_NUM;
    linux_repl->repl_com.state = CONSOLE_REPL_STATE_INIT;
    linux_repl->repl_com.repl_core.del = esp_console_repl_linux_delete;

    /* spawn a single thread to run REPL */
    if (xTaskCreate(esp_console_repl_task, "console_repl", repl_config->task_stack_size,
                    linux_repl, repl_config->task_priority, &linux_repl->repl_com.task_hdl) != pdTRUE) {
        ret = ESP_FAIL;
        goto _exit;
    }

    *ret_repl = &linux_repl->repl_com.repl_core;
    return ESP_OK;
_exit:
    if (linux_repl) {
        esp_console_deinit();
        free(linux_repl);
    }
    if (ret_repl) {
        *ret_repl = NULL;
    }
    return ret;
}

#if CONFIG_ESP_CONSOLE_UART_DEFAULT || CONFIG_ESP_CONSOLE_UART_CUSTOM
esp_err_t esp_console_new_repl_uart(const esp_console_dev_uart_config_t *dev_config, const esp_console_repl_config_t *repl_config, esp_console_repl_t **ret_repl)
{
    if (!dev_config) {
        return ESP_ERR_INVALID_ARG;
    }

    return esp_console_new_repl_linux(repl_config, ret_repl);
}
#endif //  CONFIG_ESP_CONSOLE_UART_DEFAULT || CONFIG_ESP_CONSOLE_UART_CUSTOM

#if CONFIG_ESP_CONSOLE_USB_CDC
esp_err_t esp_console_new_repl_usb_cdc(const esp_console_dev_usb_cdc_config_t *dev_config, const esp_console_repl_config_t *repl_config, esp_console_repl_t **ret_repl)
{
    if (!dev_config) {
        return ESP_ERR_INVALID_ARG;
    }

    return esp_console_new_repl_linux(repl_config, ret_repl);
}
#endif // CONFIG_ESP_CONSOLE_USB_CDC

#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
esp_err_t esp_console_new_repl_usb_serial_jtag(const esp_console_dev_usb_serial_jtag_config_t *dev_config, const esp_console_repl_config_t *repl_config, esp_console_repl_t **ret_repl)
{
    if (!dev_config) {
        return ESP_ERR_INVALID_ARG;
    }

    return esp_console_new_repl_linux(repl_config, ret_repl);
}
#endif // CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
