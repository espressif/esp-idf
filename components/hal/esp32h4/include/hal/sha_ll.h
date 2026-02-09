/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "soc/hwcrypto_reg.h"
#include "soc/pcr_struct.h"
#include "hal/sha_types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Enable the bus clock for SHA peripheral module
 *
 * @param enable true to enable the module, false to disable the module
 */
static inline void sha_ll_enable_bus_clock(bool enable)
{
    PCR.sha_conf.sha_clk_en = enable;
}

/**
 * @brief Reset the SHA peripheral module
 */
static inline void sha_ll_reset_register(void)
{
    PCR.sha_conf.sha_rst_en = 1;
    PCR.sha_conf.sha_rst_en = 0;

    PCR.hmac_conf.hmac_rst_en = 0;
    PCR.ecdsa_conf.ecdsa_rst_en = 0;
}

/**
 * @brief Load the mode for the SHA engine
 *
 * @param sha_type The SHA algorithm type
 */
static inline void sha_ll_set_mode(esp_sha_type sha_type)
{
    REG_WRITE(SHA_MODE_REG, sha_type);
}

/**
 * @brief Start a new SHA block conversions (no initial hash in HW)
 *
 * @param sha_type The SHA algorithm type
 */
static inline void sha_ll_start_block(esp_sha_type sha_type)
{
    (void) sha_type;
    REG_WRITE(SHA_START_REG, 1);
}

/**
 * @brief Continue a SHA block conversion (initial hash in HW)
 *
 * @param sha_type The SHA algorithm type
 */
static inline void sha_ll_continue_block(esp_sha_type sha_type)
{
    (void) sha_type;
    REG_WRITE(SHA_CONTINUE_REG, 1);
}

/**
 * @brief Start a new SHA message conversion using DMA (no initial hash in HW)
 */
static inline void sha_ll_start_dma(void)
{
    REG_WRITE(SHA_DMA_START_REG, 1);
}

/**
 * @brief Continue a SHA message conversion using DMA (initial hash in HW)
 */
static inline void sha_ll_continue_dma(void)
{
    REG_WRITE(SHA_DMA_CONTINUE_REG, 1);
}

/**
 * @brief Load the current hash digest to digest register
 *
 * @note Happens automatically on ESP32H4
 *
 * @param sha_type The SHA algorithm type
 */
static inline void sha_ll_load(esp_sha_type sha_type)
{
}

/**
 * @brief Sets the number of message blocks to be hashed
 *
 * @note DMA operation only
 *
 * @param num_blocks Number of message blocks to process
 */
static inline void sha_ll_set_block_num(size_t num_blocks)
{
    REG_WRITE(SHA_DMA_BLOCK_NUM_REG, num_blocks);
}

/**
 * @brief Checks if the SHA engine is currently busy hashing a block
 *
 * @return true SHA engine busy
 * @return false SHA engine idle
 */
static inline bool sha_ll_busy(void)
{
    return REG_READ(SHA_BUSY_REG);
}

/**
 * @brief Write a text (message) block to the SHA engine
 *
 * @param input_text Input buffer to be written to the SHA engine
 * @param block_word_len Number of words in block
 */
static inline void sha_ll_fill_text_block(const void *input_text, size_t block_word_len)
{
    uint32_t *data_words = (uint32_t *)input_text;
    uint32_t *reg_addr_buf = (uint32_t *)(SHA_2_SM_3_M_MEM);

    for (int i = 0; i < block_word_len; i++) {
        REG_WRITE(&reg_addr_buf[i], data_words[i]);
    }
}

/**
 * @brief Read the message digest from the SHA engine
 *
 * @param sha_type The SHA algorithm type
 * @param digest_state Buffer that message digest will be written to
 * @param digest_word_len Length of the message digest
 */
static inline void sha_ll_read_digest(esp_sha_type sha_type, void *digest_state, size_t digest_word_len)
{
    uint32_t *digest_state_words = (uint32_t *)digest_state;
    const size_t REG_WIDTH = sizeof(uint32_t);

    for (size_t i = 0; i < digest_word_len; i++) {
        digest_state_words[i] = REG_READ(SHA_2_SM_3_H_MEM + (i * REG_WIDTH));
    }

}

/**
 * @brief Write the message digest to the SHA engine
 *
 * @param sha_type The SHA algorithm type
 * @param digest_state Message digest to be written to SHA engine
 * @param digest_word_len Length of the message digest
 */
static inline void sha_ll_write_digest(esp_sha_type sha_type, void *digest_state, size_t digest_word_len)
{
    uint32_t *digest_state_words = (uint32_t *)digest_state;
    uint32_t *reg_addr_buf = (uint32_t *)(SHA_2_SM_3_H_MEM);

    for (int i = 0; i < digest_word_len; i++) {
        REG_WRITE(&reg_addr_buf[i], digest_state_words[i]);
    }
}

/**
 * @brief Sets SHA512_t T_string parameter
 *
 * @param t_string T_string parameter
 */
static inline void sha_ll_t_string_set(uint32_t t_string)
{
    REG_WRITE(SHA_T_STRING_REG, t_string);
}

/**
 * @brief Sets SHA512_t T_string parameter's length
 *
 * @param t_len T_string parameter length
 */
static inline void sha_ll_t_len_set(uint8_t t_len)
{
    REG_WRITE(SHA_T_LENGTH_REG, t_len);
}

#ifdef __cplusplus
}
#endif
