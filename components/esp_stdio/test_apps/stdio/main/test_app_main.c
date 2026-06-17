/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include "sdkconfig.h"

#include "esp_rom_serial_output.h"
#include "esp_rom_sys.h"

#include "hal/uart_ll.h"

#include "driver/uart.h"
#include "soc/uart_pins.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>

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
#define ESP_STDIO_RUN_OPEN_CLOSE_CHECK 1

#if ESP_STDIO_RUN_OPEN_CLOSE_CHECK
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
#endif

static void stdio_fd_mode_behavior_check(void)
{
    printf("STDIO_TEST:MODE=FORWARDING\n");

    int fd0 = open("/dev/console", O_RDWR);
    assert(fd0 >= 0);

    int fd1 = open("/dev/console", O_RDWR);
    assert(fd1 >= 0);
    assert(fd0 != fd1);
    printf("STDIO_TEST:UNIQUE_FDS_OK\n");

    const char *msg = "STDIO_TEST:WRITE_OK\n";
    ssize_t wr = write(fd0, msg, strlen(msg));
    assert(wr == (ssize_t) strlen(msg));

    assert(fsync(fd0) == 0);
    printf("STDIO_TEST:FSYNC_OK\n");

    assert(close(fd0) == 0);
    assert(close(fd1) == 0);
    printf("STDIO_TEST:CLOSE_OK\n");
}

static void stdio_fd_init_check(void)
{
    int in_fd = fileno(stdin);
    int out_fd = fileno(stdout);
    int err_fd = fileno(stderr);

    assert(in_fd == 0);
    assert(out_fd == 1);
    assert(err_fd == 2);

    printf("STDIO_TEST:FD_INIT:IN=%d,OUT=%d,ERR=%d\n", in_fd, out_fd, err_fd);
    fprintf(stdout, "STDIO_TEST:STREAM:STDOUT\n");
    fflush(stdout);
    fprintf(stderr, "STDIO_TEST:STREAM:STDERR\n");
    fflush(stderr);
}

#if CONFIG_VFS_SUPPORT_SELECT && CONFIG_ESP_CONSOLE_UART
static void stdio_select_fd_mapping_check(void)
{
    bool driver_installed_here = false;
    esp_err_t err = uart_driver_install(CONFIG_ESP_CONSOLE_UART_NUM, 256, 0, 0, NULL, 0);
    if (err == ESP_OK) {
        driver_installed_here = true;
    } else {
        assert(err == ESP_ERR_INVALID_STATE);
    }

    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(fileno(stdout), &writefds);
    FD_SET(fileno(stderr), &writefds);

    struct timeval tv = {
        .tv_sec = 0,
        .tv_usec = 0,
    };

    errno = 0;
    int ret = select(fileno(stderr) + 1, NULL, &writefds, NULL, &tv);
    assert(ret >= 0);
    if (driver_installed_here) {
        assert(uart_driver_delete(CONFIG_ESP_CONSOLE_UART_NUM) == ESP_OK);
    }
    printf("STDIO_TEST:SELECT:LOGICAL_FD_MAP_OK\n");
}
#endif
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

    stdio_fd_init_check();
#if CONFIG_VFS_SUPPORT_SELECT && CONFIG_ESP_CONSOLE_UART
    stdio_select_fd_mapping_check();
#endif
#endif // CONFIG_VFS_SUPPORT_IO

#if CONFIG_ESP_CONSOLE_NONE
    console_none_print();
#endif // CONFIG_ESP_CONSOLE_NONE

#if CONFIG_VFS_SUPPORT_IO
#if ESP_STDIO_RUN_OPEN_CLOSE_CHECK
    console_open_close_check();
#else
    printf("STDIO_TEST:SKIP:OPEN_CLOSE_CHECK\n");
#endif
    stdio_fd_mode_behavior_check();
#endif // CONFIG_VFS_SUPPORT_IO

}
