// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use it in application code.
 * See readme.md in soc/include/hal/readme.md
 ******************************************************************************/

#pragma once

#if CONFIG_IDF_TARGET_ESP32
    #error "ESP32 doesn't have a DS peripheral"
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

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

/**
 * @brief Start the whole signing process after the input key is ready.
 *
 * Call this before using any of the functions below. The input key is ready must be ready at this point.
 */
void ds_hal_start(void);

/**
 * @brief Finish the whole signing process. Call this after the signature is read or in case of an error.
 */
void ds_hal_finish(void);

/**
 * @brief Check whether the key input (HMAC on ESP32-C3) is correct.
 */
ds_key_check_t ds_hal_check_decryption_key(void);

/**
 * @brief Write the initialization vector.
 */
void ds_hal_configure_iv(const uint32_t *iv);

/**
 * @brief Write the message which should be signed.
 *
 * @param msg Pointer to the message.
 * @param size Length of signature result in bytes. It is the RSA signature length in bytes.
 */
void ds_hal_write_message(const uint8_t *msg, size_t size);

/**
 * @brief Write the encrypted private key parameters.
 */
void ds_hal_write_private_key_params(const uint8_t *block);

/**
 * @brief Begin signing procedure.
 */
void ds_hal_start_sign(void);

/**
 * @brief Check whether the hardware is busy with an operation.
 *
 * @return True if the hardware has finished the signing procedure, otherwise false.
 */
bool ds_hal_busy(void);

/**
 * @brief Check and read the signature from the hardware.
 *
 * @return
 * - DS_SIGNATURE_OK if no issue is detected with the signature.
 * - DS_SIGNATURE_PADDING_FAIL if the padding of the private key parameters is wrong.
 * - DS_SIGNATURE_MD_FAIL if the message digest check failed. This means that the message digest calculated using
 *      the private key parameters fails, i.e., the integrity of the private key parameters is not protected.
 * - DS_SIGNATURE_PADDING_AND_MD_FAIL if both padding and message digest check fail.
 */
ds_signature_check_t ds_hal_read_result(uint8_t *result, size_t size);

#ifdef __cplusplus
}
#endif
