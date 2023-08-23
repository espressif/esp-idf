/*
 *  ESP hardware accelerated SHA1/256/512 implementation
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
#include "esp_memory_utils.h"
#include "esp_crypto_lock.h"
#include "esp_attr.h"
#include "soc/lldesc.h"
#include "soc/ext_mem_defs.h"
#include "soc/periph_defs.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_private/periph_ctrl.h"
#include "sys/param.h"

#include "sha/sha_dma.h"
#include "hal/sha_hal.h"
#include "soc/soc_caps.h"
#include "esp_sha_dma_priv.h"

#if CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32s3/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32C2
#include "esp32c2/rom/cache.h"
#endif

#if SOC_SHA_GDMA
#define SHA_LOCK() esp_crypto_sha_aes_lock_acquire()
#define SHA_RELEASE() esp_crypto_sha_aes_lock_release()
#elif SOC_SHA_CRYPTO_DMA
#define SHA_LOCK() esp_crypto_dma_lock_acquire()
#define SHA_RELEASE() esp_crypto_dma_lock_release()
#endif

const static char *TAG = "esp-sha";
static bool s_check_dma_capable(const void *p);

/* These are static due to:
 *  * Must be in DMA capable memory, so stack is not a safe place to put them
 *  * To avoid having to malloc/free them for every DMA operation
 */
static DRAM_ATTR lldesc_t s_dma_descr_input;
static DRAM_ATTR lldesc_t s_dma_descr_buf;

void esp_sha_write_digest_state(esp_sha_type sha_type, void *digest_state)
{
    sha_hal_write_digest(sha_type, digest_state);
}

void esp_sha_read_digest_state(esp_sha_type sha_type, void *digest_state)
{
    sha_hal_read_digest(sha_type, digest_state);
}

/* Return block size (in bytes) for a given SHA type */
inline static size_t block_length(esp_sha_type type)
{
    switch (type) {
    case SHA1:
    case SHA2_224:
    case SHA2_256:
        return 64;
#if SOC_SHA_SUPPORT_SHA384
    case SHA2_384:
#endif
#if SOC_SHA_SUPPORT_SHA512
    case SHA2_512:
#endif
#if SOC_SHA_SUPPORT_SHA512_T
    case SHA2_512224:
    case SHA2_512256:
    case SHA2_512T:
#endif
        return 128;
    default:
        return 0;
    }
}


/* Enable SHA peripheral and then lock it */
void esp_sha_acquire_hardware()
{
    SHA_LOCK(); /* Released when releasing hw with esp_sha_release_hardware() */

    /* Enable SHA and DMA hardware */
#if SOC_SHA_CRYPTO_DMA
    periph_module_enable(PERIPH_SHA_DMA_MODULE);
#elif SOC_SHA_GDMA
    periph_module_enable(PERIPH_SHA_MODULE);
#endif
}

/* Disable SHA peripheral block and then release it */
void esp_sha_release_hardware()
{
    /* Disable SHA and DMA hardware */
#if SOC_SHA_CRYPTO_DMA
    periph_module_disable(PERIPH_SHA_DMA_MODULE);
#elif SOC_SHA_GDMA
    periph_module_disable(PERIPH_SHA_MODULE);
#endif

    SHA_RELEASE();
}

#if SOC_SHA_SUPPORT_SHA512_T
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

    sha_hal_sha512_init_hash(t_string, t_len);

    return 0;
}

#endif //SOC_SHA_SUPPORT_SHA512_T


/* Hash the input block by block, using non-DMA mode */
static void esp_sha_block_mode(esp_sha_type sha_type, const uint8_t *input, uint32_t ilen,
                               const uint8_t *buf, uint32_t buf_len, bool is_first_block)
{
    size_t blk_len = 0;
    size_t blk_word_len = 0;
    int num_block = 0;

    blk_len = block_length(sha_type);
    blk_word_len =  blk_len / 4;
    num_block = ilen / blk_len;

    if (buf_len != 0) {
        sha_hal_hash_block(sha_type, buf, blk_word_len, is_first_block);
        is_first_block = false;
    }

    for (int i = 0; i < num_block; i++) {
        sha_hal_hash_block(sha_type, input + blk_len * i, blk_word_len, is_first_block);
        is_first_block = false;
    }
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

    if (buf_len > block_length(sha_type)) {
        ESP_LOGE(TAG, "SHA DMA buf_len cannot exceed max size for a single block");
        return -1;
    }

    /* DMA cannot access memory in flash, hash block by block instead of using DMA */
    if (!s_check_dma_capable(input) && (ilen != 0)) {
        esp_sha_block_mode(sha_type, input, ilen, buf, buf_len, is_first_block);
        return 0;
    }

#if (CONFIG_SPIRAM && SOC_PSRAM_DMA_CAPABLE)
    if (esp_ptr_external_ram(input)) {
        Cache_WriteBack_Addr((uint32_t)input, ilen);
    }
    if (esp_ptr_external_ram(buf)) {
        Cache_WriteBack_Addr((uint32_t)buf, buf_len);
    }
#endif

    /* Copy to internal buf if buf is in non DMA capable memory */
    if (!s_check_dma_capable(buf) && (buf_len != 0)) {
        dma_cap_buf = heap_caps_malloc(sizeof(unsigned char) * buf_len, MALLOC_CAP_8BIT|MALLOC_CAP_DMA|MALLOC_CAP_INTERNAL);
        if (dma_cap_buf == NULL) {
            ESP_LOGE(TAG, "Failed to allocate buf memory");
            ret = -1;
            goto cleanup;
        }
        memcpy(dma_cap_buf, buf, buf_len);
        buf = dma_cap_buf;
    }

    uint32_t dma_op_num;

    if (ilen > 0) {
        /* Number of DMA operations based on maximum chunk size in single operation */
        dma_op_num = (ilen + SOC_SHA_DMA_MAX_BUFFER_SIZE - 1) / SOC_SHA_DMA_MAX_BUFFER_SIZE;
    } else {
        /* For zero input length, we must allow at-least 1 DMA operation to see
         * if there is any pending data that is yet to be copied out */
        dma_op_num = 1;
    }

    /* The max amount of blocks in a single hardware operation is 2^6 - 1 = 63
       Thus we only do a single DMA input list + dma buf list,
       which is max 3968/64 + 64/64 = 63 blocks */
    for (int i = 0; i < dma_op_num; i++) {

        int dma_chunk_len = MIN(ilen, SOC_SHA_DMA_MAX_BUFFER_SIZE);

        ret = esp_sha_dma_process(sha_type, input, dma_chunk_len, buf, buf_len, is_first_block);

        if (ret != 0) {
            goto cleanup;
        }

        ilen -= dma_chunk_len;
        input = (uint8_t *)input + dma_chunk_len;

        // Only append buf to the first operation
        buf_len = 0;
        is_first_block = false;
    }

cleanup:
    free(dma_cap_buf);
    return ret;
}


/* Performs SHA on multiple blocks at a time */
static esp_err_t esp_sha_dma_process(esp_sha_type sha_type, const void *input, uint32_t ilen,
                                     const void *buf, uint32_t buf_len, bool is_first_block)
{
    int ret = 0;
    lldesc_t *dma_descr_head;
    size_t num_blks = (ilen + buf_len) / block_length(sha_type);

    memset(&s_dma_descr_input, 0, sizeof(lldesc_t));
    memset(&s_dma_descr_buf, 0, sizeof(lldesc_t));

    /* DMA descriptor for Memory to DMA-SHA transfer */
    if (ilen) {
        s_dma_descr_input.length = ilen;
        s_dma_descr_input.size = ilen;
        s_dma_descr_input.owner = 1;
        s_dma_descr_input.eof = 1;
        s_dma_descr_input.buf = (uint8_t *)input;
        dma_descr_head = &s_dma_descr_input;
    }
    /* Check after input to overide head if there is any buf*/
    if (buf_len) {
        s_dma_descr_buf.length = buf_len;
        s_dma_descr_buf.size = buf_len;
        s_dma_descr_buf.owner = 1;
        s_dma_descr_buf.eof = 1;
        s_dma_descr_buf.buf = (uint8_t *)buf;
        dma_descr_head = &s_dma_descr_buf;
    }

    /* Link DMA lists */
    if (buf_len && ilen) {
        s_dma_descr_buf.eof = 0;
        s_dma_descr_buf.empty = (uint32_t)(&s_dma_descr_input);
    }

    if (esp_sha_dma_start(dma_descr_head) != ESP_OK) {
        ESP_LOGE(TAG, "esp_sha_dma_start failed, no DMA channel available");
        return -1;
    }

    sha_hal_hash_dma(sha_type, num_blks, is_first_block);

    sha_hal_wait_idle();

    return ret;
}

static bool s_check_dma_capable(const void *p)
{
    bool is_capable = false;
#if CONFIG_SPIRAM
    is_capable |= esp_ptr_dma_ext_capable(p);
#endif
    is_capable |= esp_ptr_dma_capable(p);

    return is_capable;
}
