/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <string.h>
#include "soc/hwcrypto_reg.h"
#include "soc/pcr_struct.h"
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
 * @brief Enable the bus clock for AES peripheral module
 *
 * @param enable true to enable the module, false to disable the module
 */
static inline void aes_ll_enable_bus_clock(bool enable)
{
    PCR.aes_conf.aes_clk_en = enable;
}

/**
 * @brief Reset the AES peripheral module
 */
static inline void aes_ll_reset_register(void)
{
    PCR.aes_conf.aes_rst_en = 1;
    PCR.aes_conf.aes_rst_en = 0;

    // Clear reset on digital signature also, otherwise AES is held in reset
    PCR.ds_conf.ds_rst_en = 0;
}

/**
 * @brief Write the encryption/decryption key to hardware
 *
 * @param key Key to be written to the AES hardware
 * @param key_word_len Number of words in the key
 *
 * @return Number of bytes written to hardware, used for fault injection check
 */
static inline uint8_t aes_ll_write_key(const uint8_t *key, size_t key_word_len)
{
    /* This variable is used for fault injection checks, so marked volatile to avoid optimisation */
    volatile uint8_t key_in_hardware = 0;
    /* Memcpy to avoid potential unaligned access */
    uint32_t key_word;
    for (int i = 0; i < key_word_len; i++) {
        memcpy(&key_word, key + 4 * i, 4);
        REG_WRITE(AES_KEY_0_REG + i * 4,  key_word);
        key_in_hardware += 4;
    }
    return key_in_hardware;
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
    REG_WRITE(AES_MODE_REG, mode_reg_base + ((key_bytes / 8) - 2));
}

/**
 * @brief Writes message block to AES hardware
 *
 * @param input Block to be written
 */
static inline void aes_ll_write_block(const void *input)
{
    uint32_t input_word;

    for (int i = 0; i < AES_BLOCK_WORDS; i++) {
        memcpy(&input_word, (uint8_t*)input + 4 * i, 4);
        REG_WRITE(AES_TEXT_IN_0_REG + i * 4, input_word);
    }
}

/**
 * @brief Read the AES block
 *
 * @param output the output of the transform, length = AES_BLOCK_BYTES
 */
static inline void aes_ll_read_block(void *output)
{
    uint32_t output_word;
    const size_t REG_WIDTH = sizeof(uint32_t);

    for (size_t i = 0; i < AES_BLOCK_WORDS; i++) {
        output_word = REG_READ(AES_TEXT_OUT_0_REG + (i * REG_WIDTH));
        /* Memcpy to avoid potential unaligned access */
        memcpy( (uint8_t*)output + i * 4, &output_word, sizeof(output_word));
    }
}

/**
 * @brief Starts block transform
 *
 */
static inline void aes_ll_start_transform(void)
{
    REG_WRITE(AES_TRIGGER_REG, 1);
}


/**
 * @brief Read state of AES accelerator
 *
 * @return esp_aes_state_t
 */
static inline esp_aes_state_t aes_ll_get_state(void)
{
    return (esp_aes_state_t)REG_READ(AES_STATE_REG);
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
    REG_WRITE(AES_BLOCK_MODE_REG, mode);
}

/**
 * @brief Set AES-CTR counter to INC32
 *
 * @note Only affects AES-CTR mode
 *
 */
static inline void aes_ll_set_inc(void)
{
    REG_WRITE(AES_INC_SEL_REG, 0);
}

/**
 * @brief Release the DMA
 *
 */
static inline void aes_ll_dma_exit(void)
{
    REG_WRITE(AES_DMA_EXIT_REG, 0);
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
    REG_WRITE(AES_BLOCK_NUM_REG, num_blocks);
}

/*
 * Write IV to hardware iv registers
 */
static inline void aes_ll_set_iv(const uint8_t *iv)
{
    uint32_t *reg_addr_buf = (uint32_t *)(AES_IV_MEM);
    uint32_t iv_word;

    for (int i = 0; i < IV_WORDS; i++ ) {
        /* Memcpy to avoid potential unaligned access */
        memcpy(&iv_word, iv + 4 * i, sizeof(iv_word));
        REG_WRITE(&reg_addr_buf[i], iv_word);
    }
}

/*
 * Read IV from hardware iv registers
 */
static inline void aes_ll_read_iv(uint8_t *iv)
{
    uint32_t iv_word;
    const size_t REG_WIDTH = sizeof(uint32_t);

    for (size_t i = 0; i < IV_WORDS; i++) {
        iv_word = REG_READ(AES_IV_MEM + (i * REG_WIDTH));
        /* Memcpy to avoid potential unaligned access */
        memcpy(iv + i * 4, &iv_word, sizeof(iv_word));
    }
}

/**
 * @brief Enable or disable DMA mode
 *
 * @param enable true to enable, false to disable.
 */
static inline void aes_ll_dma_enable(bool enable)
{
    REG_WRITE(AES_DMA_ENABLE_REG, enable);
}

/**
 * @brief Enable or disable transform completed interrupt
 *
 * @param enable true to enable, false to disable.
 */
static inline void aes_ll_interrupt_enable(bool enable)
{
    REG_WRITE(AES_INT_ENA_REG, enable);
}

/**
 * @brief Clears the interrupt
 *
 */
static inline void aes_ll_interrupt_clear(void)
{
    REG_WRITE(AES_INT_CLEAR_REG, 1);
}


#ifdef __cplusplus
}
#endif
