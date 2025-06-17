/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/termios.h>
#include <sys/errno.h>
#include <sys/param.h>
#include <unistd.h>
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/usb_serial_jtag_vfs.h"
#include "driver/usb_serial_jtag.h"
#include "hal/usb_serial_jtag_ll.h"
#include "esp_clk_tree.h"
#include "test_utils.h"
#include "sdkconfig.h"

static int read_bytes_with_select(FILE *stream, void *buf, size_t buf_len, struct timeval tv)
{

    int fd = fileno(stream);
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);

    /* call select with to wait for either a read ready or timeout to happen */
    int nread = select(fd + 1, &read_fds, NULL, NULL, &tv);
    if (nread < 0) {
        return -1;
    } else if (FD_ISSET(fd, &read_fds)) {
        int read_count = 0;
        int total_read = 0;

        do {
            read_count = read(fileno(stream), buf + total_read, buf_len - total_read);
            if (read_count < 0 && errno != EWOULDBLOCK) {
                return -1;
            } else if (read_count > 0) {
                total_read += read_count;
                if (total_read > buf_len) {
                    fflush(stream);
                    break;
                }
            }
        } while (read_count > 0);

        return total_read;
    } else {
        return -2;
    }
}

/* In this test, the select returning on timeout, write and read ready functionalities
 * are under test. To achieve that, the testing environment:
 * 0) Starts the test
 * 1) Waits for the select to timeout (read_bytes_with_select returns timeout error which
 * triggers the sending of the timeout message back to the testing environment)
 * 2) Sends the test message (read_bytes_with_select returns with the received message that
 * is sent back to the testing environment to be checked)
 * 3) A parallel tasks calls select on stdout for writing and decrement a variable for every
 * write calls. At the end of the test, the variable is check to make sure the select returned
 * for each of the write calls.
 */
TEST_CASE("test select read, write and timeout", "[usb_serial_jtag vfs]")
{
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    char out_buffer[32];
    memset(out_buffer, 0, sizeof(out_buffer));
    size_t out_buffer_len = sizeof(out_buffer);

    // stdin needs to be non blocking to properly call read after select returns
    // with read ready on stdin.
    int fd = fileno(stdin);
    int flags = fcntl(fd, F_GETFL);
    flags |= O_NONBLOCK;
    int res = fcntl(fd, F_SETFL, flags);
    TEST_ASSERT(res == 0);

    // init driver
    usb_serial_jtag_driver_config_t usj_config = USB_SERIAL_JTAG_DRIVER_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(usb_serial_jtag_driver_install(&usj_config));
    usb_serial_jtag_vfs_use_driver();

    // send the message from pytest environment and make sure it can be read
    bool message_received = false;
    size_t char_read = 0;
    while (!message_received && out_buffer_len > char_read) {
        int nread = read_bytes_with_select(stdin, out_buffer + char_read, out_buffer_len - char_read, tv);
        if (nread > 0) {
            char_read += nread;
            if (out_buffer[char_read - 1] == '\n') {
                message_received = true;
            }
        } else if (nread == -2) {
            // time out occurred, send the expected message back to the testing
            // environment to trigger the testing environment into sending the
            // test message. don't update this message without updating the pytest
            // function since the string is expected as is by the test environment
            char timeout_msg[] = "select timed out\n";
            write(fileno(stdout), timeout_msg, sizeof(timeout_msg));
        } else {
            break;
        }
    }

    // write the received message back to the test environment. The test
    // environment will check that the message received matches the one sent
    write(fileno(stdout), out_buffer, char_read);

    // wait for the string to send
    vTaskDelay(10);

    usb_serial_jtag_vfs_use_nonblocking();
    usb_serial_jtag_driver_uninstall();
}

/* Test that the read function does not return prematurely when receiving a new line character
 */
TEST_CASE("read does not return on new line character", "[usb serial jtag vfs]")
{
    // Send a string with length less than the read requested length
    const char in_buffer[] = "!(@*#&(!*@&#((SDasdkjhad\nce"; // read should not early return on \n
    const size_t in_buffer_len = sizeof(in_buffer);

    const size_t out_buffer_len = in_buffer_len - 1; // don't compare the null character at the end of in_buffer string
    char out_buffer[out_buffer_len] = {};

    usb_serial_jtag_driver_config_t usj_config = USB_SERIAL_JTAG_DRIVER_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(usb_serial_jtag_driver_install(&usj_config));
    usb_serial_jtag_vfs_use_driver();

    usb_serial_jtag_vfs_set_rx_line_endings(ESP_LINE_ENDINGS_LF);
    usb_serial_jtag_vfs_set_tx_line_endings(ESP_LINE_ENDINGS_LF);

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    // trigger the test environment to send the test message
    char ready_msg[] = "ready to receive\n";
    write(fileno(stdout), ready_msg, sizeof(ready_msg));

    // wait for the string to be sent and buffered
    vTaskDelay(pdMS_TO_TICKS(500));

    char *out_buffer_ptr = out_buffer;
    size_t bytes_read = 0;
    do {
        int nread = read(STDIN_FILENO, out_buffer_ptr, out_buffer_len);
        printf("%d\n", nread);
        if (nread > 0) {
            out_buffer_ptr += nread;
            bytes_read += nread;
        }
    } while (bytes_read < in_buffer_len);

    // string compare
    for (size_t i = 0; i < out_buffer_len; i++) {
        TEST_ASSERT_EQUAL(in_buffer[i], out_buffer[i]);
    }

    usb_serial_jtag_vfs_use_nonblocking();
    fcntl(STDIN_FILENO, F_SETFL, flags);
    usb_serial_jtag_vfs_set_rx_line_endings(ESP_LINE_ENDINGS_CRLF);
    usb_serial_jtag_vfs_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);
    ESP_ERROR_CHECK(usb_serial_jtag_driver_uninstall());
    vTaskDelay(2);  // wait for tasks to exit
}

TEST_CASE("blocking read returns with available data", "[usb serial jtag vfs]")
{
    const size_t out_buffer_len = 32;
    char out_buffer[out_buffer_len] = {};

    usb_serial_jtag_driver_config_t usj_config = USB_SERIAL_JTAG_DRIVER_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(usb_serial_jtag_driver_install(&usj_config));
    usb_serial_jtag_vfs_use_driver();

    usb_serial_jtag_vfs_set_rx_line_endings(ESP_LINE_ENDINGS_LF);
    usb_serial_jtag_vfs_set_tx_line_endings(ESP_LINE_ENDINGS_LF);

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags & (~O_NONBLOCK));

    // trigger the test environment to send the test message
    char ready_msg[] = "ready to receive\n";
    write(fileno(stdout), ready_msg, sizeof(ready_msg));

    const int nread = read(STDIN_FILENO, out_buffer, out_buffer_len);
    TEST_ASSERT(nread > 0);
    TEST_ASSERT(nread < out_buffer_len);

    usb_serial_jtag_vfs_use_nonblocking();
    fcntl(STDIN_FILENO, F_SETFL, flags);
    usb_serial_jtag_vfs_set_rx_line_endings(ESP_LINE_ENDINGS_CRLF);
    usb_serial_jtag_vfs_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);
    ESP_ERROR_CHECK(usb_serial_jtag_driver_uninstall());
    vTaskDelay(2);  // wait for tasks to exit
}
