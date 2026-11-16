/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The eventfd API of <sys/eventfd.h> on Linux, as far as ESP-IDF supports it.
 *
 * eventfd() is implemented by the vfs component: register it first with esp_vfs_eventfd_register(),
 * see esp_vfs_eventfd.h, which also defines the only supported flag, EFD_SUPPORT_ISR.
 */

typedef uint64_t eventfd_t;

int eventfd(unsigned int initval, int flags);

/* Reads the counter of an eventfd. Returns 0 on success, -1 on error with errno set. */
static inline int eventfd_read(int fd, eventfd_t *value)
{
    return read(fd, value, sizeof(*value)) == sizeof(*value) ? 0 : -1;
}

/* Adds value to the counter of an eventfd. Returns 0 on success, -1 on error with errno set. */
static inline int eventfd_write(int fd, eventfd_t value)
{
    return write(fd, &value, sizeof(value)) == sizeof(value) ? 0 : -1;
}

#ifdef __cplusplus
}
#endif
