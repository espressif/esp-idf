/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include "esp_random.h"
#include <entropy_poll.h>
#include "psa/crypto.h"

int mbedtls_hardware_poll( void *data,
                           unsigned char *output, size_t len, size_t *olen )
{
    esp_fill_random(output, len);
    *olen = len;
    return 0;
}

#if defined(MBEDTLS_PSA_DRIVER_GET_ENTROPY)
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
#endif // MBEDTLS_PSA_DRIVER_GET_ENTROPY
