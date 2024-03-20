/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include "sdkconfig.h"

#include "esp_rom_uart.h"
#include "esp_rom_sys.h"

#include "hal/uart_ll.h"
#include "esp_private/uart_share_hw_ctrl.h"

#include "driver/uart.h"
#include "soc/uart_channel.h"

#if CONFIG_ESP_CONSOLE_NONE
/* Set up UART on UART_0 (console) to be able to
   notify pytest test case that app booted successfully
*/
#define CONSOLE_UART_NUM 0

static void console_none_print(void)
{
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = CONFIG_ESPTOOLPY_MONITOR_BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_driver_install(CONSOLE_UART_NUM, 256, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(CONSOLE_UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(CONSOLE_UART_NUM, UART_NUM_0_TXD_DIRECT_GPIO_NUM, UART_NUM_0_RXD_DIRECT_GPIO_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // Configure a temporary buffer for the incoming data
    uint8_t data[] = "This message will be printed even with CONFIG_ESP_CONSOLE_NONE\r\n";

    uart_write_bytes(CONSOLE_UART_NUM, data, sizeof(data));
}
#endif

void app_main(void)
{
    printf("Hello World\n");

#if CONFIG_ESP_CONSOLE_NONE
    console_none_print();
#endif // CONFIG_ESP_CONSOLE_NONE
}
