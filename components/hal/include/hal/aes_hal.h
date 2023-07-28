/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in soc/include/hal/readme.md
 ******************************************************************************/

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "soc/soc_caps.h"
#include "hal/aes_types.h"
#if SOC_AES_SUPPORTED
#include "hal/aes_ll.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Sets the key used for AES encryption/decryption
 *
 * @param key pointer to the key
 * @param key_bytes number of bytes in key
 * @param mode key mode, 0 : decrypt, 1: encrypt
 *
 * @return uint8_t number of key bytes written to hardware, used for fault injection check
 */
uint8_t aes_hal_setkey(const uint8_t *key, size_t key_bytes, int mode);

/**
 * @brief encrypts/decrypts a single block
 *
 * @param input_block input block, size of AES_BLOCK_BYTES
 * @param output_block output block, size of AES_BLOCK_BYTES
 */
void aes_hal_transform_block(const void *input_block, void *output_block);

#if SOC_AES_SUPPORT_DMA
/**
 * @brief Inits the AES mode of operation
 *
 * @param mode mode of operation, e.g. CTR or CBC
 */
void aes_hal_mode_init(esp_aes_mode_t mode);

/**
 * @brief Sets the initialization vector for the transform
 *
 * @note The same IV must never be reused with the same key
 *
 * @param iv the initialization vector, length = IV_BYTES (16 bytes)
 */
void aes_hal_set_iv(const uint8_t *iv);

/**
 * @brief Reads the initialization vector
 *
 * @param iv initialization vector read from HW, length = IV_BYTES (16 bytes)
 */
void aes_hal_read_iv(uint8_t *iv);

/**
 * @brief Busy waits until the AES operation is done
 *
 * @param output pointer to inlink descriptor
 */
void aes_hal_wait_done(void);

/**
 * @brief Starts an already configured AES DMA transform
 *
 * @param num_blocks Number of blocks to transform
 */
void aes_hal_transform_dma_start(size_t num_blocks);

/**
 * @brief Finish up a AES DMA conversion, release DMA
 *
 */
void aes_hal_transform_dma_finish(void);

/**
 * @brief Enable or disable transform completed interrupt
 *
 * @param enable true to enable, false to disable.
 */
#define aes_hal_interrupt_enable(enable) aes_ll_interrupt_enable(enable)

/**
 * @brief Clears the interrupt
 *
 */
#define aes_hal_interrupt_clear() aes_ll_interrupt_clear()

#if SOC_AES_SUPPORT_GCM
/**
 * @brief Calculates the Hash sub-key H0 needed to start AES-GCM
 *
 * @param gcm_hash the Hash sub-key H0 output
 */
void aes_hal_gcm_calc_hash(uint8_t *gcm_hash);

/**
 * @brief Initializes the AES hardware for AES-GCM
 *
 * @param aad_num_blocks the number of Additional Authenticated Data (AAD) blocks
 * @param num_valid_bit the number of effective bits of incomplete blocks in plaintext/cipertext
 */
void aes_hal_gcm_init(size_t aad_num_blocks, size_t num_valid_bit);

/**
 * @brief Starts a AES-GCM transform
 *
 * @param num_blocks Number of blocks to transform
 */
void aes_hal_transform_dma_gcm_start(size_t num_blocks);

/**
 * @brief Sets the J0 value, for more information see the GCM subchapter in the TRM
 *
 * @note Only affects AES-GCM
 *
 * @param j0 J0 value
 */
#define aes_hal_gcm_set_j0(j0)  aes_ll_gcm_set_j0(j0)

/**
 * @brief Read the tag after a AES-GCM transform
 *
 * @param tag Pointer to where to store the result
 * @param tag_length number of bytes to read into tag
 */
void aes_hal_gcm_read_tag(uint8_t *tag, size_t tag_len);

#endif //SOC_AES_SUPPORT_GCM

#endif //SOC_AES_SUPPORT_DMA


#ifdef __cplusplus
}
#endif
