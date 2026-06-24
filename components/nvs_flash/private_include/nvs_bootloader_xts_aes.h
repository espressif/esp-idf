/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "nvs_bootloader_aes.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief The XTS-AES context-type definition.
 */
typedef struct
{
    uint8_t crypt_key[32]; /*!< The AES context to use for AES block
                                        encryption or decryption. */
    uint8_t tweak_key[32]; /*!< The AES context used for tweak
                                        computation. */
} nvs_bootloader_xts_aes_context;

/** Invalid key length. */
#define NVS_BOOTLOADER_ERR_AES_INVALID_KEY_LENGTH                -0x0020
/** Invalid data input length. */
#define NVS_BOOTLOADER_ERR_AES_INVALID_INPUT_LENGTH              -0x0022

/**
 * @brief Initializes the specified XTS-AES context.
 *
 * @param ctx The XTS-AES context to be initialized. This must not be \c NULL.
 */
void nvs_bootloader_xts_aes_init(nvs_bootloader_xts_aes_context *ctx);

/**
 * @brief Clears the specified XTS-AES context.
 *
 * @param ctx The XTS-AES context to clear.
 *            If this is \c NULL, this function does nothing.
 *            Otherwise, the context must have been at least initialized.
 */
void nvs_bootloader_xts_aes_free(nvs_bootloader_xts_aes_context *ctx);

/**
 * @brief Sets the XTS-AES encryption-decryption key
 *
 * @param ctx The XTS-AES context to which the key should be bound.
 *            It must be initialized.
 * @param key The encryption key.
 *            This must be a readable buffer of size \p key_bytes bytes.
 * @param key_bytes The size of data passed in bits. Valid options are:
 *                 <ul><li>128 bits</li>
 *                 <li>192 bits</li>
 *                 <li>256 bits</li></ul>
 * @return \c 0 indicating success.
 */
int nvs_bootloader_xts_aes_setkey(nvs_bootloader_xts_aes_context *ctx,
                                const unsigned char *key,
                                unsigned int key_bytes);

/**
 * @brief Performs an XTS-AES encryption or decryption operation for an entire XTS data unit.
 *
 * @param ctx The XTS-AES context to use for XTS-AES operation
 *            It must be initialized and bound to a key.
 * @param mode The AES operation: AES_ENC or AES_DEC.
 * @param length The length of a data unit in bytes.
 * @param data_unit The address of the data unit encoded as an array of 16
 *                  bytes in little-endian format. For disk encryption, this
 *                  is typically the index of the block device sector that
 *                  contains the data.
 * @param input The buffer holding the input data (which is an entire
 *              data unit). This function reads \p length Bytes from \p
 *                     input.
 * @param output The buffer holding the output data (which is an entire
 *               data unit). This function writes \p length Bytes to \p
 *               output.
 * @return \c 0 on success.
 *            1 on failure.
 */
int nvs_bootloader_aes_crypt_xts(nvs_bootloader_xts_aes_context *ctx,
                                enum AES_TYPE mode,
                                size_t length,
                                const unsigned char data_unit[16],
                                const unsigned char *input,
                                unsigned char *output);

#ifdef __cplusplus
}
#endif
