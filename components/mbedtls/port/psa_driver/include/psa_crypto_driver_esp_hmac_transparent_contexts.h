/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_types.h"
#include "soc/soc_caps.h"
#include "psa/crypto_driver_common.h"
#include "psa_crypto_driver_esp_sha_contexts.h"
#include "esp_rom_md5.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ESP_HMAC_TRANSPARENT_DRIVER_ENABLED)
typedef struct {
    psa_algorithm_t alg;
    uint8_t *opad;  // Dynamically allocated to save space
    union {
        md5_context_t md5_ctx;
        esp_sha_hash_operation_t esp_sha_ctx;
    };
} esp_hmac_transparent_operation_t;
#endif /* ESP_HMAC_TRANSPARENT_DRIVER_ENABLED */

#ifdef __cplusplus
}
#endif
