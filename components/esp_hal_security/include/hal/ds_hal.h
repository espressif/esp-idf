/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use it in application code.
 * See readme.md in soc/include/hal/readme.md
 ******************************************************************************/

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "soc/soc_caps.h"
#include "hal/ds_types.h"

#ifdef __cplusplus
extern "C" {
#endif

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
 * @brief Write the initialization vector.
 */
void ds_hal_configure_iv(const uint32_t *iv);

#if SOC_KEY_MANAGER_DS_KEY_DEPLOY
/**
 * @brief Set the DS key source.
 */
void ds_hal_set_key_source(ds_key_source_t key_source);
#endif

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
