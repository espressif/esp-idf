// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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
 * The hal is not public api, don't use in application code.
 * See readme.md in soc/include/hal/readme.md
 ******************************************************************************/

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "soc/soc_caps.h"
#include "soc/lldesc.h"
#include "hal/sha_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Hashes a single message block
 *
 * @param sha_type          SHA algorithm to hash with
 * @param data_block        Input message to be hashed
 * @param block_word_len    Length of the input message
 * @param first_block       Is this the first block in a message or a continuation?
 */
void sha_hal_hash_block(esp_sha_type sha_type, const void *data_block, size_t block_word_len, bool first_block);

/**
 * @brief Polls and waits until the SHA engine is idle
 *
 */
void sha_hal_wait_idle(void);

/**
 * @brief Reads the current message digest from the SHA engine
 *
 * @param sha_type SHA algorithm used
 * @param digest_state Output buffer to which to read message digest to
 */
void sha_hal_read_digest(esp_sha_type sha_type, void *digest_state);

#if SOC_SHA_SUPPORT_RESUME
/**
 * @brief Writes the message digest to the SHA engine
 *
 * @param sha_type The SHA algorithm type
 * @param digest_state Message digest to be written to SHA engine
 */
void sha_hal_write_digest(esp_sha_type sha_type, void *digest_state);
#endif

#if SOC_SHA_SUPPORT_DMA
/**
 * @brief Hashes a number of message blocks using DMA
 *
 * @param sha_type      SHA algorithm to hash with
 * @param num_blocks    Number of blocks to hash
 * @param first_block   Is this the first block in a message or a continuation?
 */
void sha_hal_hash_dma(esp_sha_type sha_type, size_t num_blocks, bool first_block);
#endif

#if SOC_SHA_SUPPORT_SHA512_T
/**
 * @brief Calculates and sets the initial digiest for SHA512_t
 *
 * @param t_string
 * @param t_len
 */
void sha_hal_sha512_init_hash(uint32_t t_string, uint8_t t_len);
#endif

#ifdef __cplusplus
}
#endif
