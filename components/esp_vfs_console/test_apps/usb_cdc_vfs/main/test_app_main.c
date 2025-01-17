/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
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

static void flush_write(void)
{
    fflush(stdout);
    fsync(fileno(stdout));
}

static void test_setup(const char* func_name, const size_t func_name_size)
{
    /* write the name of the test back to the pytest environment to start the test */
    write(fileno(stdout), func_name, func_name_size);
    write(fileno(stdout), "\n", 1);
    flush_write();
}

static int read_bytes_with_select(FILE *stream, void *buf, size_t buf_len, struct timeval* tv)
{
    int fd = fileno(stream);
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);
    /* call select to wait for either a read ready or an except to happen */
    int nread = select(fd + 1, &read_fds, NULL, NULL, tv);
    if (nread < 0) {
        return -1;
    } else if (FD_ISSET(fd, &read_fds)) {
        int read_count = 0;
        int total_read = 0;

        do {
            read_count = read(fd, buf + total_read, buf_len - total_read);
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
        /* select timed out */
        return -2;
    }
    return nread;
}

static bool wait_for_read_ready(FILE *stream)
{
    int fd = fileno(stream);
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);
    /* call select to wait for either a read ready or an except to happen */
    int nread = select(fd + 1, &read_fds, NULL, NULL, NULL);
    if ((nread >= 0) && (FD_ISSET(fd, &read_fds))) {
        return true;
    } else {
        return false;
    }
}

static void test_usb_cdc_select(void)
{
    test_setup(__func__, sizeof(__func__));

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
        int nread = read_bytes_with_select(stdin, out_buffer + char_read, out_buffer_len - char_read, &tv);
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
            flush_write();
        } else {
            break;
        }
    }

    // write the received message back to the test environment. The test
    // environment will check that the message received matches the one sent
    write(fileno(stdout), out_buffer, char_read);
    flush_write();

    vTaskDelay(10); // wait for the string to send
}

static void test_usb_cdc_read_non_blocking(void)
{
    test_setup(__func__, sizeof(__func__));

    const size_t max_read_bytes = 16;
    char read_data_buffer[max_read_bytes];

    memset(read_data_buffer, 0x00, max_read_bytes);

    /* reset errno to 0 for the purpose of the test to make sure it is no
     * longer set to EWOULDBLOCK later in the test. */
    errno = 0;

    /* make sure to read in non blocking mode */
    int stdin_fd = fileno(stdin);
    int flags = fcntl(stdin_fd, F_GETFL);
    flags |= O_NONBLOCK;
    int res = fcntl(stdin_fd, F_SETFL, flags);
    TEST_ASSERT(res == 0);

    /* call read when no data is available for reading.
     * expected result:
     * - read returns -1 and errno has been set to EWOULDBLOCK */
    int nread = read(stdin_fd, read_data_buffer, max_read_bytes);
    TEST_ASSERT(nread == -1);
    TEST_ASSERT(errno == EWOULDBLOCK);

    /* reset errno to 0 for the purpose of the test to make sure it is no
     * longer set to EWOULDBLOCK later in the test. */
    errno = 0;

    /* call read X number of bytes when less than X number of
     * bytes are available for reading (Y bytes).
     * expected result:
     * - read returns at least 1 bytes errno is not set to EWOULDBLOCK */
    char message_8[] = "send_bytes\n";
    write(fileno(stdout), message_8, sizeof(message_8));
    flush_write();

    const bool is_ready = wait_for_read_ready(stdin);
    TEST_ASSERT(is_ready);
    nread = read(stdin_fd, read_data_buffer, max_read_bytes);
    TEST_ASSERT(nread >= 1);
    TEST_ASSERT(errno != EWOULDBLOCK);
}

void app_main(void)
{
    test_usb_cdc_select();
    test_usb_cdc_read_non_blocking();
}
