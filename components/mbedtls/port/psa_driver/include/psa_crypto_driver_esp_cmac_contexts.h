/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once


#include "esp_types.h"
#include "psa/crypto_driver_common.h"
#include "psa_crypto_driver_esp_aes_contexts.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ESP_CMAC_DRIVER_ENABLED)

#define PSA_AES_BLOCK_SIZE PSA_BLOCK_CIPHER_BLOCK_LENGTH(PSA_KEY_TYPE_AES)
#define PSA_CMAC_MAX_BLOCK_SIZE PSA_AES_BLOCK_SIZE

typedef struct {
    /** The CMAC key identifier for cipher operations */
    psa_key_id_t key_id;

    /** The internal state of the CMAC algorithm.  */
    unsigned char state[PSA_CMAC_MAX_BLOCK_SIZE];

    /** Unprocessed data - either data that was not block aligned and is still
     *  pending processing, or the final block. */
    unsigned char unprocessed_block[PSA_CMAC_MAX_BLOCK_SIZE];

    /** The length of data pending processing. */
    size_t unprocessed_len;

    uint8_t cipher_block_length;

    esp_aes_operation_t esp_aes_ctx;

    psa_algorithm_t alg;
} esp_cmac_operation_t;

#endif /* ESP_CMAC_DRIVER_ENABLED */

#ifdef __cplusplus
}
#endif
