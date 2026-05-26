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
#if CONFIG_ESP_STDIO_BASIC_MODE
#define ESP_STDIO_RUN_OPEN_CLOSE_CHECK 1
#elif CONFIG_ESP_STDIO_MAX_FDS > 3
#define ESP_STDIO_RUN_OPEN_CLOSE_CHECK 1
#else
#define ESP_STDIO_RUN_OPEN_CLOSE_CHECK 0
#endif

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
#if CONFIG_ESP_STDIO_BASIC_MODE
    printf("STDIO_TEST:MODE=BASIC\n");

    int fd0 = open("/dev/console", O_RDWR);
    int fd1 = open("/dev/console", O_RDWR);
    int fd2 = open("/dev/console", O_RDWR);
    assert(fd0 >= 0);
    assert(fd1 >= 0);
    assert(fd2 >= 0);
    printf("STDIO_TEST:BASIC:OPEN_OK\n");

    const char *msg = "STDIO_TEST:BASIC:WRITE_OK\n";
    ssize_t wr = write(fd0, msg, strlen(msg));
    assert(wr == (ssize_t) strlen(msg));

    /* Verify write return value matches actual bytes (primary sink propagation) */
    const char *short_msg = "ab";
    wr = write(fd1, short_msg, 2);
    assert(wr == 2);

    assert(close(fd0) == 0);
    assert(close(fd1) == 0);
    assert(close(fd2) == 0);
    printf("STDIO_TEST:BASIC:CLOSE_OK\n");

    errno = 0;
    assert(close(-1) < 0);
    assert(errno == EBADF);

    errno = 0;
    assert(write(fd0, msg, strlen(msg)) < 0);
    assert(errno == EBADF);
    printf("STDIO_TEST:BASIC:EBADF_OK\n");
#else
    printf("STDIO_TEST:MODE=NON_BASIC\n");

    errno = 0;
    int fd0 = open("/dev/console", O_RDWR);
    if (fd0 < 0) {
        /* Small fd limits can be fully consumed by stdin/stdout/stderr. */
        assert(errno == ENFILE);
        printf("STDIO_TEST:NON_BASIC:SATURATED_OK\n");
        return;
    }

    int fd1 = open("/dev/console", O_RDWR);
    int fd2 = open("/dev/console", O_RDWR);
    assert(fd0 >= 0);
    assert(fd1 >= 0);
    assert(fd2 >= 0);
    assert(fd0 != fd1 && fd1 != fd2 && fd0 != fd2);
    printf("STDIO_TEST:NON_BASIC:UNIQUE_OK\n");

    assert(close(fd1) == 0);
    int fd_reused = open("/dev/console", O_RDWR);
    assert(fd_reused == fd1);
    printf("STDIO_TEST:NON_BASIC:REUSE_OK\n");
    assert(close(fd0) == 0);
    assert(close(fd2) == 0);
    assert(close(fd_reused) == 0);

    int fd_ro = open("/dev/console", O_RDONLY);
    assert(fd_ro >= 0);
    errno = 0;
    assert(write(fd_ro, "x", 1) < 0);
    assert(errno == EBADF);
    assert(close(fd_ro) == 0);

    int fd_wo = open("/dev/console", O_WRONLY);
    assert(fd_wo >= 0);
    errno = 0;
    char tmp = 0;
    assert(read(fd_wo, &tmp, 1) < 0);
    assert(errno == EBADF);
    assert(close(fd_wo) == 0);

    int fd_rw = open("/dev/console", O_RDWR);
    assert(fd_rw >= 0);
    const char *wmsg = "z";
    ssize_t wret = write(fd_rw, wmsg, 1);
    assert(wret == 1);
    assert(close(fd_rw) == 0);
    printf("STDIO_TEST:NON_BASIC:FLAGS_OK\n");

    int fd_fsync = open("/dev/console", O_RDWR);
    assert(fd_fsync >= 0);
    assert(fsync(fd_fsync) == 0);
    assert(close(fd_fsync) == 0);
    printf("STDIO_TEST:NON_BASIC:FSYNC_OK\n");

    int fds[CONFIG_ESP_STDIO_MAX_FDS + 2];
    int opened = 0;
    while (opened < (int)(sizeof(fds) / sizeof(fds[0]))) {
        int fd = open("/dev/console", O_RDWR);
        if (fd < 0) {
            break;
        }
        fds[opened++] = fd;
    }
    assert(opened >= 0);
    assert(opened <= CONFIG_ESP_STDIO_MAX_FDS);
    errno = 0;
    assert(open("/dev/console", O_RDWR) < 0);
    assert(errno == ENFILE);
    printf("STDIO_TEST:NON_BASIC:LIMIT_OK\n");

    for (int i = 0; i < opened; ++i) {
        assert(close(fds[i]) == 0);
    }

    const char *msg = "x";
    errno = 0;
    assert(write(-1, msg, 1) < 0);
    assert(errno == EBADF);
    printf("STDIO_TEST:NON_BASIC:EBADF_OK\n");
#endif
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
