/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/errno.h>
#include "unity.h"
#include "esp_private/usb_console.h"
#include "esp_vfs_cdcacm.h"

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
            read_count = read(fileno(stream), buf + total_read, 1);
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

void app_main(void)
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
    // ESP_ERROR_CHECK(esp_usb_console_init());
    // esp_vfs_dev_cdcacm_register();

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

    vTaskDelay(10); // wait for the string to send
}
