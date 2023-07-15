/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The result when checking whether the key to decrypt the RSA parameters is ready.
 */
typedef enum {
    DS_KEY_INPUT_OK = 0, /**< The decryption key is ready. */
    DS_NO_KEY_INPUT,     /**< Dependent peripheral providing key hasn't been activated. */
    DS_OTHER_WRONG,      /**< Dependent peripheral running but problem receiving the key. */
} ds_key_check_t;

typedef enum {
    DS_SIGNATURE_OK = 0,                    /**< Signature is valid and can be read. */
    DS_SIGNATURE_PADDING_FAIL = 1,          /**< Padding invalid, signature can be read if user wants it. */
    DS_SIGNATURE_MD_FAIL = 2,               /**< Message digest check failed, signature invalid. */
    DS_SIGNATURE_PADDING_AND_MD_FAIL = 3,   /**< Both padding and MD check failed. */
} ds_signature_check_t;

#ifdef __cplusplus
}
#endif
