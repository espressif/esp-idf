/*
 * SPDX-FileCopyrightText: 2016-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/cdefs.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_console.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_stdio.h"
#include "console_private.h"

#if CONFIG_ESP_CONSOLE_UART
#include "driver/uart_vfs.h"

static const char *TAG = "console.repl.chip";

static esp_err_t esp_console_repl_uart_delete_legacy(esp_console_repl_t *repl)
{
    esp_console_repl_com_t *repl_com = (esp_console_repl_com_t*)repl;
    esp_console_repl_universal_t *uart_repl = (esp_console_repl_universal_t*)repl_com;

    // check if already de-initialized
    if (repl_com->state == CONSOLE_REPL_STATE_DEINIT) {
        ESP_LOGE(TAG, "already de-initialized");
        return ESP_ERR_INVALID_STATE;
    }

    const esp_err_t ret = esp_console_common_deinit(repl_com);
    if (ret != ESP_OK) {
        return ret;
    }

    esp_console_deinit();
    uart_vfs_dev_port_deinit(uart_repl->dev_config);
    /* free dev_config since it was malloc in esp_console_new_repl_uart_legacy */
    free(uart_repl->dev_config);
    free(uart_repl);

    return ESP_OK;
}

static esp_err_t esp_console_new_repl_uart_legacy(const esp_console_dev_uart_config_t *dev_config, const esp_console_repl_config_t *repl_config, esp_console_repl_t **ret_repl)
{
    esp_err_t ret = ESP_OK;
    esp_console_repl_universal_t *uart_repl = NULL;
    if (!repl_config || !dev_config || !ret_repl) {
        ret = ESP_ERR_INVALID_ARG;
        goto _exit;
    }
    // allocate memory for console REPL context
    uart_repl = calloc(1, sizeof(esp_console_repl_universal_t));
    if (!uart_repl) {
        ret = ESP_ERR_NO_MEM;
        goto _exit;
    }

    /* Drain stdout before reconfiguring it */
    fflush(stdout);
    fsync(fileno(stdout));

    ret = uart_vfs_dev_port_init(dev_config, ESP_LINE_ENDINGS_CR, ESP_LINE_ENDINGS_CRLF);
    if (ret != ESP_OK) {
        goto _exit;
    }

    // initialize console, common part
    ret = esp_console_common_init(repl_config->max_cmdline_length, &uart_repl->repl_com);
    if (ret != ESP_OK) {
        goto _exit;
    }

    // setup history
    ret = esp_console_setup_history(repl_config->history_save_path, repl_config->max_history_len, &uart_repl->repl_com);
    if (ret != ESP_OK) {
        goto _exit;
    }

#if CONFIG_LIBC_PICOLIBC // TODO IDF-14901
#if !CONFIG_LIBC_PICOLIBC_NEWLIB_COMPATIBILITY
#define tls_stdin linenoise_stdin
#define tls_stdout linenoise_stdout
#endif
    extern __thread FILE *tls_stdin;
    extern __thread FILE *tls_stdout;

    // Workaround for Picolibc to use thread-local stdio streams when the console is not the default one.
    // Need to set linenoise_stdin/linenoise_stdout to correct values that will be used by the esp_console_repl_task
    // before esp_console_setup_prompt() call, because it uses them. After that, we can restore the original values.
    if (dev_config->channel != CONFIG_ESP_CONSOLE_UART_NUM) {
        char path[CONSOLE_PATH_MAX_LEN] = { 0 };
        snprintf(path, CONSOLE_PATH_MAX_LEN, "/dev/uart/%d", dev_config->channel);
        uart_repl->repl_com._stdin = fopen(path, "r");
        uart_repl->repl_com._stdout = fopen(path, "w");
    }
    FILE *tmp_stdin = stdin;
    FILE *tmp_stdout = stdout;
    if (uart_repl->repl_com._stdin) {
        tls_stdin = uart_repl->repl_com._stdin;
        tls_stdout = uart_repl->repl_com._stdout;
    }
#endif

    // setup prompt
    ret = esp_console_setup_prompt(repl_config->prompt, &uart_repl->repl_com);
    if (ret != ESP_OK) {
        goto _exit;
    }

#if CONFIG_LIBC_PICOLIBC // TODO IDF-14901
    if (uart_repl->repl_com._stdin) {
        // Restore the original values of tls_stdin and tls_stdout just in case.
        tls_stdin = tmp_stdin;
        tls_stdout = tmp_stdout;
    }
#endif

    /* we have to make a copy of the dev_config because it will be used to init the
     * uart driver in the repl task. We have no guarantee that dev_config will still
     * be valid then */
    esp_console_dev_uart_config_t *copy_config = malloc(sizeof(esp_console_dev_uart_config_t));
    if (copy_config == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto _exit;
    }
    memcpy(copy_config, dev_config, sizeof(esp_console_dev_uart_config_t));

    /* Fill the structure here as it will be used directly by the created task. */
    uart_repl->dev_config = copy_config;
    uart_repl->repl_com.state = CONSOLE_REPL_STATE_INIT;
    uart_repl->repl_com.repl_core.del = esp_console_repl_uart_delete_legacy;

    /* Spawn a single thread to run REPL, we need to pass `uart_repl` to it as
     * it also requires the uart channel. */
    if (xTaskCreatePinnedToCore(esp_console_repl_task, "console_repl", repl_config->task_stack_size,
                                uart_repl, repl_config->task_priority, &uart_repl->repl_com.task_hdl, repl_config->task_core_id) != pdTRUE) {
        ret = ESP_FAIL;
        goto _exit;
    }

    *ret_repl = &uart_repl->repl_com.repl_core;
    return ESP_OK;
_exit:
    if (uart_repl) {
        esp_console_deinit();
        free(uart_repl);
    }
    if (ret_repl) {
        *ret_repl = NULL;
    }
    return ret;
}

esp_err_t esp_console_new_repl_uart(const esp_console_dev_uart_config_t *dev_config, const esp_console_repl_config_t *repl_config, esp_console_repl_t **ret_repl)
{
    return esp_console_new_repl_uart_legacy(dev_config, repl_config, ret_repl);
}

void esp_console_repl_uart_setup_std_stream(const esp_console_dev_uart_config_t *dev_config)
{
    const int uart_channel = dev_config->channel;
    if (uart_channel != CONFIG_ESP_CONSOLE_UART_NUM) {
        char path[CONSOLE_PATH_MAX_LEN] = { 0 };
        snprintf(path, CONSOLE_PATH_MAX_LEN, "/dev/uart/%d", uart_channel);

        FILE *temp_stdin = fopen(path, "r");
        FILE *temp_stdout = fopen(path, "w");
        if (temp_stdin == NULL || temp_stdout == NULL) {
            ESP_LOGE(TAG, "Failed to open UART device file for channel %d", uart_channel);
            if (temp_stdin) {
                fclose(temp_stdin);
            }
            if (temp_stdout) {
                fclose(temp_stdout);
            }
        } else {
            stdin = temp_stdin;
            stdout = temp_stdout;
            stderr = stdout;
        }
    }
}
#endif // CONFIG_ESP_CONSOLE_UART

#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
esp_err_t esp_console_new_repl_usb_serial_jtag(const esp_console_dev_usb_serial_jtag_config_t *dev_config, const esp_console_repl_config_t *repl_config, esp_console_repl_t **ret_repl)
{
    (void)dev_config;
    return esp_console_new_repl_stdio(repl_config, ret_repl);
}
#endif // CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG

#if CONFIG_ESP_CONSOLE_USB_CDC
esp_err_t esp_console_new_repl_usb_cdc(const esp_console_dev_usb_cdc_config_t *dev_config, const esp_console_repl_config_t *repl_config, esp_console_repl_t **ret_repl)
{
    (void)dev_config;
    return esp_console_new_repl_stdio(repl_config, ret_repl);
}
#endif // CONFIG_ESP_CONSOLE_USB_CDC

void esp_console_setup_standard_stream(void *dev_config)
{
#if CONFIG_ESP_CONSOLE_UART
    esp_console_repl_uart_setup_std_stream((esp_console_dev_uart_config_t *)dev_config);
#endif // CONFIG_ESP_CONSOLE_UART
}
