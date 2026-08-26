/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "driver/uart_vfs.h"
#include "driver/uart.h"

#if CONFIG_EXAMPLE_GET_AP_INFO_FROM_STDIN
static const char *TAG = "power_save";
static char stdin_ssid[32];
static char stdin_password[64];

void get_ap_info_from_stdin(void)
{
    // Initialize VFS & UART so we can use std::cout/cin
    setvbuf(stdin, NULL, _IONBF, 0);
    /* Install UART driver for interrupt-driven reads and writes */
    ESP_ERROR_CHECK( uart_driver_install( (uart_port_t)CONFIG_ESP_CONSOLE_UART_NUM,
            256, 0, 0, NULL, 0) );

    /* Tell VFS to use UART driver */
    uart_vfs_dev_use_driver(CONFIG_ESP_CONSOLE_UART_NUM);
    uart_vfs_dev_port_set_rx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM, ESP_LINE_ENDINGS_CR);
    /* Move the caret to the beginning of the next line on '\n' */
    uart_vfs_dev_port_set_tx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM, ESP_LINE_ENDINGS_CRLF);

    ESP_LOGI(TAG, "Input SSID:");

    if (fgets(stdin_ssid, 32, stdin) == NULL) {
        ESP_LOGE(TAG, "Failed to get SSID");
    } else {
        stdin_ssid[strcspn(stdin_ssid, "\n")] = '\0';
    }

    ESP_LOGI(TAG, "Input Password:");
    if (fgets(stdin_password, 64, stdin) == NULL) {
        ESP_LOGE(TAG, "Failed to get password");
    } else {
        stdin_password[strcspn(stdin_password, "\n")] = '\0';
    }

    /* Back to use non-blocking vfs console*/
    uart_vfs_dev_use_nonblocking(CONFIG_ESP_CONSOLE_UART_NUM);
    uart_driver_delete(CONFIG_ESP_CONSOLE_UART_NUM);
}
#endif

char *get_ap_ssid(void)
{
#if CONFIG_EXAMPLE_GET_AP_INFO_FROM_STDIN
    return stdin_ssid;
#else
    return CONFIG_EXAMPLE_WIFI_SSID;
#endif
}

char *get_ap_password(void)
{
#if CONFIG_EXAMPLE_GET_AP_INFO_FROM_STDIN
    return stdin_password;
#else
    return CONFIG_EXAMPLE_WIFI_PASSWORD;
#endif
}
