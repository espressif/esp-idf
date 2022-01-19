/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


/***************************************************************
 *
 * This file needs to be removed. Left here just for passing build
 * TODO // TODO: IDF-3844
 ***************************************************************/
#pragma once

#include <stdbool.h>
#include "soc/hwcrypto_reg.h"
#include "hal/aes_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief State of AES accelerator, busy, idle or done
 *
 */
typedef enum {
    ESP_AES_STATE_IDLE = 0, /* AES accelerator is idle */
    ESP_AES_STATE_BUSY,     /* Transform in progress */
    ESP_AES_STATE_DONE,     /* Transform completed */
} esp_aes_state_t;


/**
 * @brief Write the encryption/decryption key to hardware
 *
 * @param key Key to be written to the AES hardware
 * @param key_word_len Number of words in the key
 *
 * @return volatile number of bytes written to hardware, used for fault injection check
 */
static inline uint8_t aes_ll_write_key(const uint8_t *key, size_t key_word_len)
{
    abort(); // TODO: IDF-3844
}

/**
 * @brief Sets the mode
 *
 * @param mode ESP_AES_ENCRYPT = 1, or ESP_AES_DECRYPT = 0
 * @param key_bytes Number of bytes in the key
 */
static inline void aes_ll_set_mode(int mode, uint8_t key_bytes)
{
    abort(); // TODO: IDF-3844
}

/**
 * @brief Writes message block to AES hardware
 *
 * @param input Block to be written
 */
static inline void aes_ll_write_block(const void *input)
{
    abort(); // TODO: IDF-3844
}

/**
 * @brief Read the AES block
 *
 * @param output the output of the transform, length = AES_BLOCK_BYTES
 */
static inline void aes_ll_read_block(void *output)
{
    abort(); // TODO: IDF-3844
}

/**
 * @brief Starts block transform
 *
 */
static inline void aes_ll_start_transform(void)
{
    abort(); // TODO: IDF-3844
}


/**
 * @brief Read state of AES accelerator
 *
 * @return esp_aes_state_t
 */
static inline esp_aes_state_t aes_ll_get_state(void)
{
    abort(); // TODO: IDF-3844
}


/**
 * @brief Set mode of operation
 *
 * @note Only used for DMA transforms
 *
 * @param mode
 */
static inline void aes_ll_set_block_mode(esp_aes_mode_t mode)
{
    abort(); // TODO: IDF-3844
}

/**
 * @brief Set AES-CTR counter to INC32
 *
 * @note Only affects AES-CTR mode
 *
 */
static inline void aes_ll_set_inc(void)
{
    abort(); // TODO: IDF-3844
}

/**
 * @brief Release the DMA
 *
 */
static inline void aes_ll_dma_exit(void)
{
    abort(); // TODO: IDF-3844
}

/**
 * @brief Sets the number of blocks to be transformed
 *
 * @note Only used for DMA transforms
 *
 * @param num_blocks Number of blocks to transform
 */
static inline void aes_ll_set_num_blocks(size_t num_blocks)
{
    abort(); // TODO: IDF-3844
}

/*
 * Write IV to hardware iv registers
 */
static inline void aes_ll_set_iv(const uint8_t *iv)
{
    abort(); // TODO: IDF-3844
}

/*
 * Read IV from hardware iv registers
 */
static inline void aes_ll_read_iv(uint8_t *iv)
{
    abort(); // TODO: IDF-3844
}

/**
 * @brief Enable or disable DMA mode
 *
 * @param enable true to enable, false to disable.
 */
static inline void aes_ll_dma_enable(bool enable)
{
    abort(); // TODO: IDF-3844
}

/**
 * @brief Enable or disable transform completed interrupt
 *
 * @param enable true to enable, false to disable.
 */
static inline void aes_ll_interrupt_enable(bool enable)
{
    abort(); // TODO: IDF-3844
}

/**
 * @brief Clears the interrupt
 *
 */
static inline void aes_ll_interrupt_clear(void)
{
    abort(); // TODO: IDF-3844
}


#ifdef __cplusplus
}
#endif
