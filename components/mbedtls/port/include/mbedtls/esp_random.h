/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MbedTLS-compatible RNG function
 *
 * @note Suitable for passing as f_rng to various Mbed-TLS APIs that require it.
 *
 * @param ctx User-supplied context
 * @param buf Pointer to buffer to fill with random numbers.
 * @param len Length of buffer in bytes
 *
 * @return 0 (success)
 */
int mbedtls_esp_random(void *ctx, unsigned char *buf, size_t len);

#ifdef __cplusplus
}
#endif
