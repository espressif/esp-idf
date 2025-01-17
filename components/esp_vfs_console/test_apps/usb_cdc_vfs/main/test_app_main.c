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
    test_usb_cdc_read_non_blocking();
}
