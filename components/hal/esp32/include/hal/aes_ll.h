/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/hwcrypto_reg.h"
#include "soc/dport_access.h"
#include "hal/aes_types.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief State of AES accelerator, busy or idle
 *
 */
typedef enum {
    ESP_AES_STATE_BUSY = 0, /* Transform in progress */
    ESP_AES_STATE_IDLE,     /* AES accelerator is idle */
} esp_aes_state_t;


/**
 * @brief Write the encryption/decryption key to hardware
 *
 * @param key Key to be written to the AES hardware
 * @param key_word_len Number of words in the key
 *
 * @return Number of bytes written to hardware, used for fault injection check,
 *         if a write was skipped then this sum is likely to be wrong
 */
static inline uint8_t aes_ll_write_key(const uint8_t *key, size_t key_word_len)
{
    /* This variable is used for fault injection checks, so marked volatile to avoid optimisation */
    volatile uint8_t key_bytes_in_hardware = 0;

    /* Memcpy to avoid potential unaligned access */
    uint32_t key_word;

    for (int i = 0; i < key_word_len; i++) {
        memcpy(&key_word, key + 4 * i, 4);
        DPORT_REG_WRITE(AES_KEY_BASE + i * 4, key_word);
        key_bytes_in_hardware += 4;
    }
    return key_bytes_in_hardware;
}

/**
 * @brief Sets the mode
 *
 * @param mode ESP_AES_ENCRYPT = 1, or ESP_AES_DECRYPT = 0
 * @param key_bytes Number of bytes in the key
 */
static inline void aes_ll_set_mode(int mode, uint8_t key_bytes)
{
    const uint32_t MODE_DECRYPT_BIT = 4;
    unsigned mode_reg_base = (mode == ESP_AES_ENCRYPT) ? 0 : MODE_DECRYPT_BIT;

    /* See TRM for the mapping between keylength and mode bit */
    DPORT_REG_WRITE(AES_MODE_REG, mode_reg_base + ((key_bytes / 8) - 2));
}

/**
 * @brief Writes message block to AES hardware
 *
 * @param input Block to be written
 */
static inline void aes_ll_write_block(const uint8_t *input)
{
    const uint32_t *input_words = (const uint32_t *)input;
    uint32_t i0;
    uint32_t i1;
    uint32_t i2;
    uint32_t i3;

    /* Storing i0,i1,i2,i3 in registers not an array
    helps a lot with optimisations at -Os level */
    i0 = input_words[0];
    DPORT_REG_WRITE(AES_TEXT_BASE, i0);

    i1 = input_words[1];
    DPORT_REG_WRITE(AES_TEXT_BASE + 4, i1);

    i2 = input_words[2];
    DPORT_REG_WRITE(AES_TEXT_BASE + 8, i2);

    i3 = input_words[3];
    DPORT_REG_WRITE(AES_TEXT_BASE + 12, i3);
}

/**
 * @brief Read the AES block
 *
 * @note If a transform was ran then this is the output
 *
 * @param output the output of the transform, length = AES_BLOCK_BYTES
 */
static inline void aes_ll_read_block(void *output)
{
    uint32_t *output_words = (uint32_t *)output;
    esp_dport_access_read_buffer(output_words, AES_TEXT_BASE, AES_BLOCK_WORDS);
}


/**
 * @brief Starts block transform
 *
 */
static inline void aes_ll_start_transform(void)
{
    DPORT_REG_WRITE(AES_START_REG, 1);
}


/**
 * @brief Read state of AES accelerator
 *
 * @return esp_aes_state_t
 */
static inline esp_aes_state_t aes_ll_get_state(void)
{
    return (esp_aes_state_t)DPORT_REG_READ(AES_IDLE_REG);
}

#ifdef __cplusplus
}
#endif
