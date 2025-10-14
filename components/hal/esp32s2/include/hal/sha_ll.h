/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <string.h>
#include "soc/hwcrypto_reg.h"
#include "hal/sha_types.h"
#include "soc/dport_reg.h"
#include "hal/mmu_ll.h"

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
    if (enable) {
        SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN1_REG, DPORT_CRYPTO_SHA_CLK_EN);
    } else {
        CLEAR_PERI_REG_MASK(DPORT_PERIP_CLK_EN1_REG, DPORT_CRYPTO_SHA_CLK_EN);
    }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define sha_ll_enable_bus_clock(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        sha_ll_enable_bus_clock(__VA_ARGS__); \
    } while(0)

/**
 * @brief Reset the SHA peripheral module
 */
static inline void sha_ll_reset_register(void)
{
    SET_PERI_REG_MASK(DPORT_PERIP_RST_EN1_REG, DPORT_CRYPTO_SHA_RST);
    CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN1_REG, DPORT_CRYPTO_SHA_RST);

    // Clear reset on digital signature and hmac also, otherwise SHA is held in reset
    CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN1_REG, DPORT_CRYPTO_DS_RST);
    CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN1_REG, DPORT_CRYPTO_HMAC_RST);
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define sha_ll_reset_register(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        sha_ll_reset_register(__VA_ARGS__); \
    } while(0)

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
 * @note Happens automatically on ESP32S2
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
    REG_WRITE(SHA_BLOCK_NUM_REG, num_blocks);
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
    uint32_t input_word;
    uint8_t *data_bytes = (uint8_t *)input_text;
    uint32_t *reg_addr_buf = (uint32_t *)(SHA_TEXT_BASE);

    bool force_word_aligned_access = false;

    /* In case of ESP32-S2, the DPORT bus region is word-aligned memory
     * and does not support 8-bit accesses.
     * Thus, when accessing data from these addresses we need to ensure
     * the operations are word-aligned.
     */
    if (mmu_ll_vaddr_in_dport_bus_region((uint32_t)input_text)) {
        force_word_aligned_access = true;
    }

    for (size_t i = 0; i < block_word_len; i++) {
        if (force_word_aligned_access) {
            memcpy(&input_word, data_bytes + 4 * i, 4);
            REG_WRITE(&reg_addr_buf[i], input_word);
        } else {
            REG_WRITE(&reg_addr_buf[i], *((uint32_t *)data_bytes + i));
        }
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

    esp_dport_access_read_buffer(digest_state_words, SHA_H_BASE, digest_word_len);
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
    uint32_t *reg_addr_buf = (uint32_t *)(SHA_H_BASE);

    for (size_t i = 0; i < digest_word_len; i++) {
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
