/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stddef.h>

static inline void mbedtls_platform_zeroize(void *buf, size_t len)
{
    bzero(buf, len);
}
