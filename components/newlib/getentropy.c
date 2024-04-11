/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/random.h>
#include <errno.h>

int getentropy(void *buffer, size_t length)
{
    ssize_t ret;

    if (buffer == NULL) {
        errno = EFAULT;
        return -1;
    }

    if (length > 256) {
        errno = EIO;
        return -1;
    }

    ret = getrandom(buffer, length, 0);
    if (ret == -1) {
        return -1;
    }

    return 0;
}

void newlib_include_getentropy_impl(void)
{
    // Linker hook, exists for no other purpose
}
