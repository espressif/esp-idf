/*
 * SPDX-FileCopyrightText: 2016-2024 Espressif Systems (Shanghai) CO LTD
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
#include "esp_vfs_cdcacm.h"
#include "driver/usb_serial_jtag_vfs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/uart_vfs.h"
#include "driver/usb_serial_jtag.h"

#include "console_private.h"

#if !CONFIG_ESP_CONSOLE_NONE
static const char *TAG = "console.repl";
#endif // !CONFIG_ESP_CONSOLE_NONE

#if CONFIG_ESP_CONSOLE_UART_DEFAULT || CONFIG_ESP_CONSOLE_UART_CUSTOM
static esp_err_t esp_console_repl_uart_delete(esp_console_repl_t *repl);
#endif // CONFIG_ESP_CONSOLE_UART_DEFAULT || CONFIG_ESP_CONSOLE_UART_CUSTOM
#if CONFIG_ESP_CONSOLE_USB_CDC
static esp_err_t esp_console_repl_usb_cdc_delete(esp_console_repl_t *repl);
#endif // CONFIG_ESP_CONSOLE_USB_CDC
#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
static esp_err_t esp_console_repl_usb_serial_jtag_delete(esp_console_repl_t *repl);
#endif //CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG

#if CONFIG_ESP_CONSOLE_USB_CDC
esp_err_t esp_console_new_repl_usb_cdc(const esp_console_dev_usb_cdc_config_t *dev_config, const esp_console_repl_config_t *repl_config, esp_console_repl_t **ret_repl)
{
    esp_err_t ret = ESP_OK;
    esp_console_repl_universal_t *cdc_repl = NULL;
    if (!repl_config || !dev_config || !ret_repl) {
        ret = ESP_ERR_INVALID_ARG;
        goto _exit;
    }
    // allocate memory for console REPL context
    cdc_repl = calloc(1, sizeof(esp_console_repl_universal_t));
    if (!cdc_repl) {
        ret = ESP_ERR_NO_MEM;
        goto _exit;
    }

    /* Minicom, screen, idf_monitor send CR when ENTER key is pressed */
    esp_vfs_dev_cdcacm_set_rx_line_endings(ESP_LINE_ENDINGS_CR);
    /* Move the caret to the beginning of the next line on '\n' */
    esp_vfs_dev_cdcacm_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);

    /* Enable blocking mode on stdin and stdout */
    fcntl(fileno(stdout), F_SETFL, 0);
    fcntl(fileno(stdin), F_SETFL, 0);

    // initialize console, common part
    ret = esp_console_common_init(repl_config->max_cmdline_length, &cdc_repl->repl_com);
    if (ret != ESP_OK) {
        goto _exit;
    }

    // setup history
    ret = esp_console_setup_history(repl_config->history_save_path, repl_config->max_history_len, &cdc_repl->repl_com);
    if (ret != ESP_OK) {
        goto _exit;
    }

    // setup prompt
    esp_console_setup_prompt(repl_config->prompt, &cdc_repl->repl_com);

    /* Fill the structure here as it will be used directly by the created task. */
    cdc_repl->uart_channel = CONFIG_ESP_CONSOLE_UART_NUM;
    cdc_repl->repl_com.state = CONSOLE_REPL_STATE_INIT;
    cdc_repl->repl_com.repl_core.del = esp_console_repl_usb_cdc_delete;

    /* spawn a single thread to run REPL */
    if (xTaskCreatePinnedToCore(esp_console_repl_task, "console_repl", repl_config->task_stack_size,
                                cdc_repl, repl_config->task_priority, &cdc_repl->repl_com.task_hdl, repl_config->task_core_id) != pdTRUE) {
        ret = ESP_FAIL;
        goto _exit;
    }

    *ret_repl = &cdc_repl->repl_com.repl_core;
    return ESP_OK;
_exit:
    if (cdc_repl) {
        esp_console_deinit();
        free(cdc_repl);
    }
    if (ret_repl) {
        *ret_repl = NULL;
    }
    return ret;
}
#endif // CONFIG_ESP_CONSOLE_USB_CDC

#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
esp_err_t esp_console_new_repl_usb_serial_jtag(const esp_console_dev_usb_serial_jtag_config_t *dev_config, const esp_console_repl_config_t *repl_config, esp_console_repl_t **ret_repl)
{
    esp_console_repl_universal_t *usb_serial_jtag_repl = NULL;
    if (!repl_config || !dev_config || !ret_repl) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = ESP_OK;
    // allocate memory for console REPL context
    usb_serial_jtag_repl = calloc(1, sizeof(esp_console_repl_universal_t));
    if (!usb_serial_jtag_repl) {
        ret = ESP_ERR_NO_MEM;
        goto _exit;
    }

    /* Minicom, screen, idf_monitor send CR when ENTER key is pressed */
    usb_serial_jtag_vfs_set_rx_line_endings(ESP_LINE_ENDINGS_CR);
    /* Move the caret to the beginning of the next line on '\n' */
    usb_serial_jtag_vfs_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);

    /* Enable blocking mode on stdin and stdout */
    fcntl(fileno(stdout), F_SETFL, 0);
    fcntl(fileno(stdin), F_SETFL, 0);

    usb_serial_jtag_driver_config_t usb_serial_jtag_config = USB_SERIAL_JTAG_DRIVER_CONFIG_DEFAULT();

    /* Install USB-SERIAL-JTAG driver for interrupt-driven reads and writes */
    ret = usb_serial_jtag_driver_install(&usb_serial_jtag_config);
    if (ret != ESP_OK) {
        goto _exit;
    }

    // initialize console, common part
    ret = esp_console_common_init(repl_config->max_cmdline_length, &usb_serial_jtag_repl->repl_com);
    if (ret != ESP_OK) {
        goto _exit;
    }

    /* Tell vfs to use usb-serial-jtag driver */
    usb_serial_jtag_vfs_use_driver();

    // setup history
    ret = esp_console_setup_history(repl_config->history_save_path, repl_config->max_history_len, &usb_serial_jtag_repl->repl_com);
    if (ret != ESP_OK) {
        goto _exit;
    }

    // setup prompt
    esp_console_setup_prompt(repl_config->prompt, &usb_serial_jtag_repl->repl_com);

    /* Fill the structure here as it will be used directly by the created task. */
    usb_serial_jtag_repl->uart_channel = CONFIG_ESP_CONSOLE_UART_NUM;
    usb_serial_jtag_repl->repl_com.state = CONSOLE_REPL_STATE_INIT;
    usb_serial_jtag_repl->repl_com.repl_core.del = esp_console_repl_usb_serial_jtag_delete;

    /* spawn a single thread to run REPL */
    if (xTaskCreatePinnedToCore(esp_console_repl_task, "console_repl", repl_config->task_stack_size,
                                usb_serial_jtag_repl, repl_config->task_priority, &usb_serial_jtag_repl->repl_com.task_hdl, repl_config->task_core_id) != pdTRUE) {
        ret = ESP_FAIL;
        goto _exit;
    }

    *ret_repl = &usb_serial_jtag_repl->repl_com.repl_core;
    return ESP_OK;
_exit:
    if (usb_serial_jtag_repl) {
        esp_console_deinit();
        free(usb_serial_jtag_repl);
    }
    if (ret_repl) {
        *ret_repl = NULL;
    }
    return ret;
}
#endif // CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG

#if CONFIG_ESP_CONSOLE_UART_DEFAULT || CONFIG_ESP_CONSOLE_UART_CUSTOM
esp_err_t esp_console_new_repl_uart(const esp_console_dev_uart_config_t *dev_config, const esp_console_repl_config_t *repl_config, esp_console_repl_t **ret_repl)
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

    /* Minicom, screen, idf_monitor send CR when ENTER key is pressed */
    uart_vfs_dev_port_set_rx_line_endings(dev_config->channel, ESP_LINE_ENDINGS_CR);
    /* Move the caret to the beginning of the next line on '\n' */
    uart_vfs_dev_port_set_tx_line_endings(dev_config->channel, ESP_LINE_ENDINGS_CRLF);

    /* Configure UART. Note that REF_TICK/XTAL is used so that the baud rate remains
     * correct while APB frequency is changing in light sleep mode.
     */
#if SOC_UART_SUPPORT_REF_TICK
    uart_sclk_t clk_source = UART_SCLK_REF_TICK;
    // REF_TICK clock can't provide a high baudrate
    if (dev_config->baud_rate > 1 * 1000 * 1000) {
        clk_source = UART_SCLK_DEFAULT;
        ESP_LOGW(TAG, "light sleep UART wakeup might not work at the configured baud rate");
    }
#elif SOC_UART_SUPPORT_XTAL_CLK
    uart_sclk_t clk_source = UART_SCLK_XTAL;
#else
#error "No UART clock source is aware of DFS"
#endif // SOC_UART_SUPPORT_xxx
    const uart_config_t uart_config = {
        .baud_rate = dev_config->baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .source_clk = clk_source,
    };

    uart_param_config(dev_config->channel, &uart_config);
    uart_set_pin(dev_config->channel, dev_config->tx_gpio_num, dev_config->rx_gpio_num, -1, -1);

    /* Install UART driver for interrupt-driven reads and writes */
    ret = uart_driver_install(dev_config->channel, 256, 0, 0, NULL, 0);
    if (ret != ESP_OK) {
        goto _exit;
    }

    /* Tell VFS to use UART driver */
    uart_vfs_dev_use_driver(dev_config->channel);

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

    // setup prompt
    esp_console_setup_prompt(repl_config->prompt, &uart_repl->repl_com);

    /* Fill the structure here as it will be used directly by the created task. */
    uart_repl->uart_channel = dev_config->channel;
    uart_repl->repl_com.state = CONSOLE_REPL_STATE_INIT;
    uart_repl->repl_com.repl_core.del = esp_console_repl_uart_delete;

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
        uart_driver_delete(dev_config->channel);
        free(uart_repl);
    }
    if (ret_repl) {
        *ret_repl = NULL;
    }
    return ret;
}
#endif // CONFIG_ESP_CONSOLE_UART_DEFAULT || CONFIG_ESP_CONSOLE_UART_CUSTOM

#if CONFIG_ESP_CONSOLE_UART_DEFAULT || CONFIG_ESP_CONSOLE_UART_CUSTOM
static esp_err_t esp_console_repl_uart_delete(esp_console_repl_t *repl)
{
    esp_err_t ret = ESP_OK;
    esp_console_repl_com_t *repl_com = __containerof(repl, esp_console_repl_com_t, repl_core);
    esp_console_repl_universal_t *uart_repl = __containerof(repl_com, esp_console_repl_universal_t, repl_com);
    // check if already de-initialized
    if (repl_com->state == CONSOLE_REPL_STATE_DEINIT) {
        ESP_LOGE(TAG, "already de-initialized");
        ret = ESP_ERR_INVALID_STATE;
        goto _exit;
    }
    repl_com->state = CONSOLE_REPL_STATE_DEINIT;
    esp_console_deinit();
    uart_vfs_dev_use_nonblocking(uart_repl->uart_channel);
    uart_driver_delete(uart_repl->uart_channel);
    free(uart_repl);
_exit:
    return ret;
}
#endif // CONFIG_ESP_CONSOLE_UART_DEFAULT || CONFIG_ESP_CONSOLE_UART_CUSTOM

#if CONFIG_ESP_CONSOLE_USB_CDC
static esp_err_t esp_console_repl_usb_cdc_delete(esp_console_repl_t *repl)
{
    esp_err_t ret = ESP_OK;
    esp_console_repl_com_t *repl_com = __containerof(repl, esp_console_repl_com_t, repl_core);
    esp_console_repl_universal_t *cdc_repl = __containerof(repl_com, esp_console_repl_universal_t, repl_com);
    // check if already de-initialized
    if (repl_com->state == CONSOLE_REPL_STATE_DEINIT) {
        ESP_LOGE(TAG, "already de-initialized");
        ret = ESP_ERR_INVALID_STATE;
        goto _exit;
    }
    repl_com->state = CONSOLE_REPL_STATE_DEINIT;
    esp_console_deinit();
    free(cdc_repl);
_exit:
    return ret;
}
#endif // CONFIG_ESP_CONSOLE_USB_CDC

#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
static esp_err_t esp_console_repl_usb_serial_jtag_delete(esp_console_repl_t *repl)
{
    esp_err_t ret = ESP_OK;
    esp_console_repl_com_t *repl_com = __containerof(repl, esp_console_repl_com_t, repl_core);
    esp_console_repl_universal_t *usb_serial_jtag_repl = __containerof(repl_com, esp_console_repl_universal_t, repl_com);
    // check if already de-initialized
    if (repl_com->state == CONSOLE_REPL_STATE_DEINIT) {
        ESP_LOGE(TAG, "already de-initialized");
        ret = ESP_ERR_INVALID_STATE;
        goto _exit;
    }
    repl_com->state = CONSOLE_REPL_STATE_DEINIT;
    esp_console_deinit();
    usb_serial_jtag_vfs_use_nonblocking();
    usb_serial_jtag_driver_uninstall();
    free(usb_serial_jtag_repl);
_exit:
    return ret;
}
#endif // CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
