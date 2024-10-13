/*
 * SPDX-FileCopyrightText: 2010-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_check.h"
#include "soft_uart.h"

#define READ_COUNT  16

const char* EXAMPLE_TAG = "dedicated_gpio_example";

void app_main(void)
{
    esp_err_t ret = ESP_OK;
    uint8_t read_buffer[READ_COUNT] = { 0 };
    const uint8_t write_buffer[] = "Hello, world! This is a message.\r\n";
    uint8_t dummy = 0;
    soft_uart_port_t port = NULL;
    soft_uart_config_t config = {
        .tx_pin = CONFIG_EMULATE_UART_GPIO_TX,
        .rx_pin = CONFIG_EMULATE_UART_GPIO_RX,
        .baudrate = SOFT_UART_115200
    };

    /* Initialize and configure the software UART port */
    ret = soft_uart_new(&config, &port);
    ESP_GOTO_ON_ERROR(ret, error, EXAMPLE_TAG, "Error configuring software UART");

    /* Send a dummy byte as the software UART driver may corrupt the first byte */
    ret = soft_uart_send(port, &dummy, sizeof(uint8_t));
    ESP_GOTO_ON_ERROR(ret, error, EXAMPLE_TAG, "Error writing a dummy byte");

    /* Write few bytes to the UART */
    ret = soft_uart_send(port, write_buffer, sizeof(write_buffer));
    ESP_GOTO_ON_ERROR(ret, error, EXAMPLE_TAG, "Error writing to the software UART");

    /* Read from the UART */
    ret = soft_uart_receive(port, read_buffer, sizeof(read_buffer));
    ESP_GOTO_ON_ERROR(ret, error, EXAMPLE_TAG, "Error reading from the software UART");

    printf("UART transfers succeeded, received bytes: { ");
    for (int i = 0; i < sizeof(read_buffer); i++) {
        printf("0x%02x ", read_buffer[i]);
    }
    printf("}\n");
error:
    if (port != NULL) {
        soft_uart_del(port);
    }
    if (ret != ESP_OK) {
        ESP_LOGE(EXAMPLE_TAG, "An error occurred while communicating through the UART");
    }
}
