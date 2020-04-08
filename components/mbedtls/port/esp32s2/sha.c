/*
 *  ESP32 hardware accelerated SHA1/256/512 implementation
 *  based on mbedTLS FIPS-197 compliant version.
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  Additions Copyright (C) 2016-2020, Espressif Systems (Shanghai) PTE Ltd
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */
/*
 *  The SHA-1 standard was published by NIST in 1993.
 *
 *  http://www.itl.nist.gov/fipspubs/fip180-1.htm
 */

#include <string.h>
#include <stdio.h>
#include <sys/lock.h>

#include "esp_log.h"
#include "esp_crypto_lock.h"
#include "esp32s2/rom/cache.h"
#include "esp32s2/rom/lldesc.h"
#include "esp32s2/rom/ets_sys.h"
#include "soc/crypto_dma_reg.h"
#include "soc/dport_reg.h"
#include "soc/hwcrypto_reg.h"
#include "soc/cache_memory.h"
#include "soc/periph_defs.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "driver/periph_ctrl.h"
#include "sys/param.h"

#include "esp32s2/sha.h"

/* Max amount of bytes in a single DMA operation is 4095,
   for SHA this means that the biggest safe amount of bytes is
   31 blocks of 128 bytes = 3968
*/
#define SHA_DMA_MAX_BYTES 3968

/* The longest length of a single block is for SHA512 = 128 byte */
#define SHA_MAX_BLK_LEN 128

const static char *TAG = "esp-sha";

/* Return block size (in bytes) for a given SHA type */
inline static size_t block_length(esp_sha_type type)
{
    switch (type) {
    case SHA1:
    case SHA2_224:
    case SHA2_256:
        return 64;
    case SHA2_384:
    case SHA2_512:
    case SHA2_512224:
    case SHA2_512256:
    case SHA2_512T:
        return 128;
    default:
        return 0;
    }
}

/* Return state size (in bytes) for a given SHA type */
inline static size_t state_length(esp_sha_type type)
{
    switch (type) {
    case SHA1:
        return 160 / 8;
    case SHA2_224:
    case SHA2_256:
        return 256 / 8;
    case SHA2_384:
    case SHA2_512:
    case SHA2_512224:
    case SHA2_512256:
    case SHA2_512T:
        return 512 / 8;
    default:
        return 0;
    }
}

/* Enable SHA peripheral and then lock it */
void esp_sha_acquire_hardware()
{
    esp_crypto_dma_lock_acquire();

    /* Enable SHA and DMA hardware */
    periph_module_enable(PERIPH_SHA_DMA_MODULE);

    /* DMA for SHA */
    REG_WRITE(CRYPTO_DMA_AES_SHA_SELECT_REG, 1);
}

/* Disable SHA peripheral block and then release it */
void esp_sha_release_hardware()
{
    /* Disable SHA and DMA hardware */
    periph_module_disable(PERIPH_SHA_DMA_MODULE);

    esp_crypto_dma_lock_release();
}

/* Busy wait until SHA is idle */
static void esp_sha_wait_idle(void)
{
    while (DPORT_REG_READ(SHA_BUSY_REG) != 0) {
    }
}

void esp_sha_write_digest_state(esp_sha_type sha_type, void *digest_state)
{
    uint32_t *digest_state_words = (uint32_t *)digest_state;
    uint32_t *reg_addr_buf = (uint32_t *)(SHA_H_BASE);

    for (int i = 0; i < state_length(sha_type) / 4; i++) {
        REG_WRITE(&reg_addr_buf[i], digest_state_words[i]);
    }
}

/* Read the SHA digest from hardware */
void esp_sha_read_digest_state(esp_sha_type sha_type, void *digest_state)
{
    uint32_t *digest_state_words = (uint32_t *)digest_state;
    int word_len = state_length(sha_type) / 4;

    esp_dport_access_read_buffer(digest_state_words, SHA_H_BASE, word_len);

    /* Fault injection check: verify SHA engine actually ran,
       state is not all zeroes.
    */
    for (int i = 0; i < word_len; i++) {
        if (digest_state_words[i] != 0) {
            return;
        }
    }
    abort(); // SHA peripheral returned all zero state, probably due to fault injection
}

/* The initial hash value for SHA512/t is generated according to the
   algorithm described in the TRM, chapter SHA-Accelerator
*/
int esp_sha_512_t_init_hash(uint16_t t)
{
    uint32_t t_string = 0;
    uint8_t t0, t1, t2, t_len;

    if (t == 384) {
        ESP_LOGE(TAG, "Invalid t for SHA512/t, t = %u,cannot be 384", t);
        return -1;
    }

    if (t <= 9) {
        t_string = (uint32_t)((1 << 23) | ((0x30 + t) << 24));
        t_len = 0x48;
    } else if (t <= 99) {
        t0 = t % 10;
        t1 = (t / 10) % 10;
        t_string = (uint32_t)((1 << 15) | ((0x30 + t0) << 16) |
                              (((0x30 + t1) << 24)));
        t_len = 0x50;
    } else if (t <= 512) {
        t0 = t % 10;
        t1 = (t / 10) % 10;
        t2 = t / 100;
        t_string = (uint32_t)((1 << 7) | ((0x30 + t0) << 8) |
                              (((0x30 + t1) << 16) + ((0x30 + t2) << 24)));
        t_len = 0x58;
    } else {
        ESP_LOGE(TAG, "Invalid t for SHA512/t, t = %u, must equal or less than 512", t);
        return -1;
    }

    REG_WRITE(SHA_T_LENGTH_REG, t_len);
    REG_WRITE(SHA_T_STRING_REG, t_string);
    REG_WRITE(SHA_MODE_REG, SHA2_512T);
    REG_WRITE(SHA_START_REG, 1);

    esp_sha_wait_idle();

    return 0;
}

static void esp_sha_fill_text_block(esp_sha_type sha_type, const void *input)
{
    uint32_t *reg_addr_buf = (uint32_t *)(SHA_TEXT_BASE);
    uint32_t *data_words = NULL;

    /* Fill the data block */
    data_words = (uint32_t *)(input);
    for (int i = 0; i < block_length(sha_type) / 4; i++) {
        reg_addr_buf[i] = (data_words[i]);
    }
    asm volatile ("memw");
}

/* Hash a single SHA block */
static void esp_sha_block(esp_sha_type sha_type, const void *input, bool is_first_block)
{
    esp_sha_fill_text_block(sha_type, input);

    esp_sha_wait_idle();
    /* Start hashing */
    if (is_first_block) {
        REG_WRITE(SHA_START_REG, 1);
    } else {
        REG_WRITE(SHA_CONTINUE_REG, 1);
    }
}

/* Hash the input block by block, using non-DMA mode */
static void esp_sha_block_mode(esp_sha_type sha_type, const uint8_t *input, uint32_t ilen,
                                const uint8_t *buf, uint32_t buf_len, bool is_first_block)
{
    size_t blk_len = 0;
    int num_block = 0;

    blk_len = block_length(sha_type);

    REG_WRITE(SHA_MODE_REG, sha_type);
    num_block = ilen / blk_len;

    if (buf_len != 0) {
        esp_sha_block(sha_type, buf, is_first_block);
        is_first_block = false;
    }

    for (int i = 0; i < num_block; i++) {
        esp_sha_block(sha_type, input + blk_len*i, is_first_block);
        is_first_block = false;
    }

    esp_sha_wait_idle();
}



static int esp_sha_dma_process(esp_sha_type sha_type, const void *input, uint32_t ilen,
                                const void *buf, uint32_t buf_len, bool is_first_block);

/* Performs SHA on multiple blocks at a time using DMA
   splits up into smaller operations for inputs that exceed a single DMA list
 */
int esp_sha_dma(esp_sha_type sha_type, const void *input, uint32_t ilen,
                const void *buf, uint32_t buf_len, bool is_first_block)
{
    int ret = 0;
    unsigned char *dma_cap_buf = NULL;
    int dma_op_num = ( ilen / (SHA_DMA_MAX_BYTES + 1) ) + 1;

    if (buf_len > block_length(sha_type)) {
        ESP_LOGE(TAG, "SHA DMA buf_len cannot exceed max size for a single block");
        return -1;
    }

    /* DMA cannot access memory in the iCache range, hash block by block instead of using DMA */
    if (!esp_ptr_dma_ext_capable(input) && !esp_ptr_dma_capable(input) && (ilen != 0)) {
        esp_sha_block_mode(sha_type, input, ilen, buf, buf_len, is_first_block);
        return 0;
    }

#if (CONFIG_ESP32S2_SPIRAM_SUPPORT)
    if (esp_ptr_external_ram(input)) {
        Cache_WriteBack_Addr((uint32_t)input, ilen);
    }
    if (esp_ptr_external_ram(buf)) {
        Cache_WriteBack_Addr((uint32_t)buf, buf_len);
    }
#endif

    /* Copy to internal buf if buf is in non DMA capable memory */
    if (!esp_ptr_dma_ext_capable(buf) && !esp_ptr_dma_capable(buf) && (buf_len != 0)) {
        dma_cap_buf = heap_caps_malloc(sizeof(unsigned char) * buf_len, MALLOC_CAP_DMA);
        if (dma_cap_buf == NULL) {
            ESP_LOGE(TAG, "Failed to allocate buf memory");
            ret = -1;
            goto cleanup;
        }
        memcpy(dma_cap_buf, buf, buf_len);
        buf = dma_cap_buf;
    }


    /* The max amount of blocks in a single hardware operation is 2^6 - 1 = 63
       Thus we only do a single DMA input list + dma buf list,
       which is max 3968/64 + 64/64 = 63 blocks */
    for (int i = 0; i < dma_op_num; i++) {

        int dma_chunk_len = MIN(ilen, SHA_DMA_MAX_BYTES);

        ret = esp_sha_dma_process(sha_type, input, dma_chunk_len, buf, buf_len, is_first_block);

        if (ret != 0) {
            goto cleanup;
        }

        ilen -= dma_chunk_len;
        input += dma_chunk_len;

        // Only append buf to the first operation
        buf_len = 0;
        is_first_block = false;
    }

cleanup:
    free(dma_cap_buf);
    return ret;
}

static void esp_sha_dma_init(lldesc_t *input)
{
    /* Reset DMA */
    SET_PERI_REG_MASK(CRYPTO_DMA_CONF0_REG, CONF0_REG_AHBM_RST | CONF0_REG_OUT_RST | CONF0_REG_AHBM_FIFO_RST);
    CLEAR_PERI_REG_MASK(CRYPTO_DMA_CONF0_REG, CONF0_REG_AHBM_RST | CONF0_REG_OUT_RST | CONF0_REG_AHBM_FIFO_RST);

    /* Set descriptors */
    CLEAR_PERI_REG_MASK(CRYPTO_DMA_OUT_LINK_REG, OUT_LINK_REG_OUTLINK_ADDR);
    SET_PERI_REG_MASK(CRYPTO_DMA_OUT_LINK_REG, ((uint32_t)(input))&OUT_LINK_REG_OUTLINK_ADDR);
    /* Start transfer */
    SET_PERI_REG_MASK(CRYPTO_DMA_OUT_LINK_REG, OUT_LINK_REG_OUTLINK_START);
}

/* Performs SHA on multiple blocks at a time */
static esp_err_t esp_sha_dma_process(esp_sha_type sha_type, const void *input, uint32_t ilen,
                                      const void *buf, uint32_t buf_len, bool is_first_block)
{
    size_t blk_len = 0;
    int ret = 0;
    lldesc_t dma_descr_input = {};
    lldesc_t dma_descr_buf = {};
    lldesc_t *dma_descr_head;

    blk_len = block_length(sha_type);

    REG_WRITE(SHA_MODE_REG, sha_type);
    REG_WRITE(SHA_BLOCK_NUM_REG, ((ilen + buf_len) / blk_len));


    /* DMA descriptor for Memory to DMA-SHA transfer */
    if (ilen) {
        dma_descr_input.length = ilen;
        dma_descr_input.size = ilen;
        dma_descr_input.owner = 1;
        dma_descr_input.eof = 1;
        dma_descr_input.buf = input;
        dma_descr_head = &dma_descr_input;
    }
    /* Check after input to overide head if there is any buf*/
    if (buf_len) {
        dma_descr_buf.length = buf_len;
        dma_descr_buf.size = buf_len;
        dma_descr_buf.owner = 1;
        dma_descr_buf.eof = 1;
        dma_descr_buf.buf = buf;
        dma_descr_head = &dma_descr_buf;
    }

    /* Link DMA lists */
    if (buf_len && ilen) {
        dma_descr_buf.eof = 0;
        dma_descr_buf.empty = (uint32_t)(&dma_descr_input);
    }

    esp_sha_dma_init(dma_descr_head);

    /* Start hashing */
    if (is_first_block) {
        REG_WRITE(SHA_DMA_START_REG, 1);
    } else {
        REG_WRITE(SHA_DMA_CONTINUE_REG, 1);
    }

    esp_sha_wait_idle();

    return ret;
}

