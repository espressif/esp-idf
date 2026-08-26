/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sys/random.h"
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>

// getrandom() is not available on macOS, so we read from /dev/urandom instead.

int getrandom(void *buf, size_t buflen, unsigned int flags)
{
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        return -1;
    }
    ssize_t ret = read(fd, buf, buflen);
    close(fd);
    if (ret < 0) {
        return -1;
    }
    return 0;
}
