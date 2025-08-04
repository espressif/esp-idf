/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <mbedtls/build_info.h>

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include "esp_random.h"
#include "mbedtls/esp_mbedtls_random.h"
#if defined(MBEDTLS_PLATFORM_GET_ENTROPY_ALT)
#include "psa/crypto.h"
#endif

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

#if defined(MBEDTLS_PLATFORM_GET_ENTROPY_ALT)
int mbedtls_platform_get_entropy(unsigned char *output, size_t output_size,
    size_t *output_len, size_t *entropy_content);

psa_status_t mbedtls_psa_external_get_random(
    mbedtls_psa_external_random_context_t *context,
    uint8_t *output, size_t output_size, size_t *output_length)
{
    if (context == NULL || output == NULL || output_length == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    esp_fill_random(output, output_size);
    *output_length = output_size;
    return PSA_SUCCESS;
}

int mbedtls_platform_get_entropy(unsigned char *output, size_t output_size,
    size_t *output_len, size_t *entropy_content)
{
    if (output == NULL || output_size == 0 || output_len == NULL || entropy_content == NULL) {
        return -1;
    }

    esp_fill_random(output, output_size);
    *output_len = output_size;
    *entropy_content = 8 * output_size;
    return 0;
}
#endif // MBEDTLS_PLATFORM_GET_ENTROPY_ALT
