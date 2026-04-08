/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Maximum key component length in bytes.
 * Supports up to P-384 (48 bytes). Increase to 66 for P-521 if needed.
 */
#define SECURE_ELEMENT_MAX_KEY_BYTES   48

/**
 * @brief Opaque key structure for secure element import
 *
 * This struct is passed as the "key material" to psa_import_key().
 * It identifies which SE slot the key refers to.
 */
typedef struct {
    uint8_t slot_id;                  /**< Slot index on the secure element */
} secure_element_opaque_key_t;

/**
 * @brief Operation context for transparent verify_hash
 *
 * Used by the interruptible verify API to store intermediate state
 * between start / complete / abort calls.
 */
typedef struct {
    uint8_t sha[SECURE_ELEMENT_MAX_KEY_BYTES];
    uint8_t r[SECURE_ELEMENT_MAX_KEY_BYTES];
    uint8_t s[SECURE_ELEMENT_MAX_KEY_BYTES];
    uint8_t qx[SECURE_ELEMENT_MAX_KEY_BYTES];
    uint8_t qy[SECURE_ELEMENT_MAX_KEY_BYTES];
    size_t key_len;
    size_t sha_len;
} secure_element_transparent_verify_hash_operation_t;

/**
 * @brief Operation context for opaque sign_hash
 *
 * Used by the interruptible sign API to store intermediate state
 * between start / complete / abort calls.
 */
typedef struct {
    uint8_t sha[SECURE_ELEMENT_MAX_KEY_BYTES];
    size_t key_len;
    size_t sha_len;
    secure_element_opaque_key_t *opaque_key;
    unsigned int alg;
} secure_element_opaque_sign_hash_operation_t;

#ifdef __cplusplus
}
#endif
