/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"


#define SHA256_LEN (32)
#define SHA256_HEX_LEN (65)

esp_err_t http_crypto_sha256(const uint8_t *data, size_t data_len, uint8_t *hash);

#ifdef __cplusplus
}
#endif
