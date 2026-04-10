/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ETS_DIGEST_SHA256_LEN 32 /* SHA-256, bytes */
#define ETS_DIGEST_SHA384_LEN 48 /* SHA-384, bytes */

typedef enum {
    ECDSA_CURVE_P192 = 1,
    ECDSA_CURVE_P256 = 2,
    ECDSA_CURVE_P384 = 3
} ECDSA_CURVE;

int ets_ecdsa_verify(const uint8_t *key, const uint8_t *sig, ECDSA_CURVE curve_id, const uint8_t *digest, uint8_t *verified_digest);

#ifdef __cplusplus
}
#endif
