/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "sdkconfig.h"

#include "esp_rom_serial_output.h"
#include "esp_rom_sys.h"

#include "hal/uart_ll.h"

#include "driver/uart.h"
#include "soc/uart_pins.h"

#include <fcntl.h>
#include <unistd.h>

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
    ESP_ERROR_CHECK(uart_set_pin(CONSOLE_UART_NUM, U0TXD_GPIO_NUM, U0RXD_GPIO_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // Configure a temporary buffer for the incoming data
    uint8_t data[] = "This message will be printed even with CONFIG_ESP_CONSOLE_NONE\r\n";

    uart_write_bytes(CONSOLE_UART_NUM, data, sizeof(data));
}
#endif

#if CONFIG_VFS_SUPPORT_IO
static void console_open_close_check(void)
{
    printf("Opening /dev/console\n");
    int fd = open("/dev/console", O_RDWR);
    assert(fd >= 0 && "Could not open file");

    const char *msg = "This should be printed to stdout\n";

    write(fd, msg, strlen(msg));

    printf("Closing /dev/console\n");
    close(fd);

    printf("This should be printed to stdout\n");
}
#endif // CONFIG_VFS_SUPPORT_IO

void app_main(void)
{
    printf("Hello World\n");

#if CONFIG_VFS_SUPPORT_IO
    int fd = open("/dev/null", O_RDWR);
    assert(fd >= 0 && "Could not open file"); // Standard check

    // Check if correct file descriptor is returned
    // In this case it should be neither of 0, 1, 2 (== stdin, stdout, stderr)
    assert(fd > 2 && "Incorrect file descriptor returned, stdin, stdout, stderr were not correctly assigned");

    close(fd);
#endif // CONFIG_VFS_SUPPORT_IO

#if CONFIG_ESP_CONSOLE_NONE
    console_none_print();
#endif // CONFIG_ESP_CONSOLE_NONE

#if CONFIG_VFS_SUPPORT_IO
    console_open_close_check();
#endif // CONFIG_VFS_SUPPORT_IO

}
