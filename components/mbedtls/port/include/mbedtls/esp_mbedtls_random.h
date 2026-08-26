/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
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
 * @note Suitable for passing as f_rng to various MbedTLS APIs that require it.
 *       It uses esp_fill_random internally, and the caller must ensure that the
 *       entropy sources of the RNG peripheral are enabled correctly. See the RNG
 *       chapter in the TRM for more details.
 *
 * @param ctx User-supplied context
 * @param buf Pointer to a buffer to fill with random numbers
 * @param len Length of the buffer in bytes
 *
 * @return 0 on success
 */
int mbedtls_esp_random(void *ctx, unsigned char *buf, size_t len);

#ifdef __cplusplus
}
#endif
