/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
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
#include <stdbool.h>
#include "hal/hmac_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Make the peripheral ready for use.
 *
 * This triggers any further steps necessary after enabling the device
 */
void hmac_hal_start(void);

/**
 * @brief Configure which hardware key slot should be used and configure the target of the HMAC output.
 *
 * @note Writing out-of-range values is undefined behavior. The user has to ensure that the parameters are in range.
 *
 * @param config The target of the HMAC. Possible targets are described in \c hmac_hal_output_t.
 * See the TRM of your target chip for more details.
 * @param key_id The ID of the hardware key slot to be used.
 *
 * @return 0 if the configuration was successful, non-zero if not.
 * An unsuccessful configuration means that the purpose value in the eFuse of the corresponding key slot
 * doesn't match to supplied value of \c config.
 */
uint32_t hmac_hal_configure(hmac_hal_output_t config, uint32_t key_id);

/**
 * @brief Write a padded single-block message of 512 bits to the HMAC peripheral.
 *
 * The message must not be longer than one block (512 bits) and the padding has to be applied by software before
 * writing. The padding has to be able to fit into the block after the message.
 * For more information on HMAC padding, see the TRM of your target chip.
 */
void hmac_hal_write_one_block_512(const void *block);

/**
 * @brief Write a message block of 512 bits to the HMAC peripheral.
 *
 * This function must be used incombination with \c hmac_hal_next_block_normal() or \c hmac_hal_next_block_padding().
 * The first message block is written without any prerequisite.
 * All message blocks which are not the last one, need a call to \c hmac_hal_next_block_normal() before, indicating
 * to the hardware that a "normal", i.e. non-padded block will follow. This is even the case for a block which begins
 * padding already but where the padding doesn't fit in (remaining message size > (block size - padding size)).
 * Before writing the last block which contains the padding, a call to \c hmac_hal_next_block_padding() is necessary
 * to indicate to the hardware that a block with padding will be written.
 *
 * For more information on HMAC padding, see the TRM of your target chip for more details.
 */
void hmac_hal_write_block_512(const void *block);

/**
 * @brief Indicate to the hardware that a normal block will be written.
 */
void hmac_hal_next_block_normal(void);

/**
 * @brief Indicate to the hardware that a block with padding will be written.
 */
void hmac_hal_next_block_padding(void);

/**
 * @brief Read the 256 bit HMAC result from the hardware.
 */
void hmac_hal_read_result_256(void *result);

/**
 * @brief Clear (invalidate) the HMAC result provided to other hardware.
 */
void hmac_hal_clean(void);

#ifdef __cplusplus
}
#endif
