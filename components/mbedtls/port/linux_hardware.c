/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <sys/random.h>
#include <stdint.h>
#include <stdlib.h>
#include "psa/crypto.h"

// psa_status_t mbedtls_psa_external_get_random(
//     mbedtls_psa_external_random_context_t *context,
//     uint8_t *output, size_t output_size, size_t *output_length)
// {
//     (void) context; // Unused parameter

//     if (output == NULL || output_length == NULL) {
//         return PSA_ERROR_INVALID_ARGUMENT;
//     }

//     if (output_size == 0) {
//         *output_length = 0;
//         return PSA_SUCCESS;
//     }

//     // Try to use getrandom() first (more secure)
//     ssize_t result = getrandom(output, output_size, 0);
//     if (result == (ssize_t)output_size) {
//         *output_length = output_size;
//         return PSA_SUCCESS;
//     }

//     *output_length = output_size;

//     return PSA_SUCCESS;
// }
