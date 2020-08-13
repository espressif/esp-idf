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
#pragma once

#include <stdbool.h>
#include "hal/sha_types.h"
#include "soc/hwcrypto_reg.h"
#include "soc/dport_access.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SHA_LL_TYPE_OFFSET 0x10

/**
 * @brief Returns the LOAD_REG register address for the given sha type
 *
 * @param sha_type The SHA algorithm type
 * @return uint32_t the LOAD_REG register address
 */
inline static uint32_t SHA_LOAD_REG(esp_sha_type sha_type)
{
    return SHA_1_LOAD_REG + sha_type * SHA_LL_TYPE_OFFSET;
}

/**
 * @brief Returns the BUSY register address for the given sha type
 *
 * @param sha_type The SHA algorithm type
 * @return uint32_t the BUSY register address
 */
inline static uint32_t SHA_BUSY_REG(esp_sha_type sha_type)
{
    return SHA_1_BUSY_REG + sha_type * SHA_LL_TYPE_OFFSET;
}

/**
 * @brief Returns the START register address for the given sha type
 *
 * @param sha_type The SHA algorithm type
 * @return uint32_t the START register address
 */
inline static uint32_t SHA_START_REG(esp_sha_type sha_type)
{
    return SHA_1_START_REG + sha_type * SHA_LL_TYPE_OFFSET;
}

/**
 * @brief Returns the CONTINUE register address for the given sha type
 *
 * @param sha_type The SHA algorithm type
 * @return uint32_t the CONTINUE register address
 */
inline static uint32_t SHA_CONTINUE_REG(esp_sha_type sha_type)
{
    return SHA_1_CONTINUE_REG + sha_type * SHA_LL_TYPE_OFFSET;
}

/**
 * @brief Start a new SHA block conversion (no initial hash in HW)
 *
 * @param sha_type The SHA algorithm type
 */
static inline void sha_ll_start_block(esp_sha_type sha_type)
{
    DPORT_REG_WRITE(SHA_START_REG(sha_type), 1);
}

/**
 * @brief Continue a SHA block conversion (initial hash in HW)
 *
 * @param sha_type The SHA algorithm type
 */
static inline void sha_ll_continue_block(esp_sha_type sha_type)
{
    DPORT_REG_WRITE(SHA_CONTINUE_REG(sha_type), 1);
}

/**
 * @brief Load the current hash digest to digest register
 *
 * @param sha_type The SHA algorithm type
 */
static inline void sha_ll_load(esp_sha_type sha_type)
{
    DPORT_REG_WRITE(SHA_LOAD_REG(sha_type), 1);
}

/**
 * @brief Checks if the SHA engine is currently busy hashing a block
 *
 * @return true SHA engine busy
 * @return false SHA engine idle
 */
static inline bool sha_ll_busy(void)
{
    return (DPORT_REG_READ(SHA_1_BUSY_REG) || DPORT_REG_READ(SHA_256_BUSY_REG)
            || DPORT_REG_READ(SHA_384_BUSY_REG) || DPORT_REG_READ(SHA_512_BUSY_REG));
}

/**
 * @brief Write a text (message) block to the SHA engine
 *
 * @param input_text Input buffer to be written to the SHA engine
 * @param block_word_len Number of words in block
 */
static inline void sha_ll_fill_text_block(const void *input_text, size_t block_word_len)
{
    uint32_t *reg_addr_buf = NULL;
    uint32_t *data_words = NULL;
    reg_addr_buf = (uint32_t *)(SHA_TEXT_BASE);
    data_words = (uint32_t *)input_text;
    for (int i = 0; i < block_word_len; i++) {
        reg_addr_buf[i] = __builtin_bswap32(data_words[i]);
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
    uint32_t *reg_addr_buf  = (uint32_t *)(SHA_TEXT_BASE);
    if (sha_type == SHA2_384 || sha_type == SHA2_512) {
        /* for these ciphers using 64-bit states, swap each pair of words */
        DPORT_INTERRUPT_DISABLE(); // Disable interrupt only on current CPU.
        for (int i = 0; i < digest_word_len; i += 2) {
            digest_state_words[i + 1] = DPORT_SEQUENCE_REG_READ((uint32_t)&reg_addr_buf[i]);
            digest_state_words[i]   = DPORT_SEQUENCE_REG_READ((uint32_t)&reg_addr_buf[i + 1]);
        }
        DPORT_INTERRUPT_RESTORE(); // restore the previous interrupt level
    } else {
        esp_dport_access_read_buffer(digest_state_words, (uint32_t)&reg_addr_buf[0], digest_word_len);
    }
}

#ifdef __cplusplus
}
#endif
