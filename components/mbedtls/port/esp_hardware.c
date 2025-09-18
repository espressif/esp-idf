/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <mbedtls/build_info.h>

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include "esp_random.h"
#include "mbedtls/esp_mbedtls_random.h"

#include <entropy_poll.h>

#ifndef MBEDTLS_ENTROPY_HARDWARE_ALT
#error "MBEDTLS_ENTROPY_HARDWARE_ALT should always be set in ESP-IDF"
#endif

int mbedtls_hardware_poll( void *data,
                           unsigned char *output, size_t len, size_t *olen )
{
    esp_fill_random(output, len);
    *olen = len;
    return 0;
}

int mbedtls_esp_random(void *ctx, unsigned char *buf, size_t len)
{
    (void) ctx; // unused
    esp_fill_random(buf, len);
    return 0;
}
