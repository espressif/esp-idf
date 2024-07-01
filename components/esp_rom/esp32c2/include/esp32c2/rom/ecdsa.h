/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ECDSA_CURVE_P192 = 1,
    ECDSA_CURVE_P256 = 2
} ECDSA_CURVE;

int ets_ecdsa_verify(const uint8_t *key, const uint8_t *sig, ECDSA_CURVE curve_id, const uint8_t *digest, uint8_t *verified_digest);

#ifdef __cplusplus
}
#endif
