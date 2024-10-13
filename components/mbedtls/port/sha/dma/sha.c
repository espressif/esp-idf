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

#include "esp_private/esp_crypto_lock_internal.h"
#include "esp_private/esp_cache_private.h"
#include "esp_log.h"
#include "esp_memory_utils.h"
#include "esp_crypto_lock.h"
#include "esp_attr.h"
#include "esp_crypto_dma.h"
#include "esp_cache.h"
#include "hal/dma_types.h"
#include "soc/ext_mem_defs.h"
#include "soc/periph_defs.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_private/periph_ctrl.h"
#include "sys/param.h"

#include "sha/sha_dma.h"
#include "hal/sha_hal.h"
#include "hal/sha_ll.h"
#include "soc/soc_caps.h"
#include "esp_sha_dma_priv.h"
#include "sdkconfig.h"

#ifdef SOC_AXI_DMA_EXT_MEM_ENC_ALIGNMENT
#include "esp_flash_encrypt.h"
#endif /* SOC_AXI_DMA_EXT_MEM_ENC_ALIGNMENT */

#if SOC_SHA_GDMA
#define SHA_LOCK() esp_crypto_sha_aes_lock_acquire()
#define SHA_RELEASE() esp_crypto_sha_aes_lock_release()
#elif SOC_SHA_CRYPTO_DMA
#define SHA_LOCK() esp_crypto_dma_lock_acquire()
#define SHA_RELEASE() esp_crypto_dma_lock_release()
#include "hal/crypto_dma_ll.h"
#endif

const static char *TAG = "esp-sha";

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

    SHA_RCC_ATOMIC() {
        sha_ll_enable_bus_clock(true);
#if SOC_AES_CRYPTO_DMA
        crypto_dma_ll_enable_bus_clock(true);
#endif
        sha_ll_reset_register();
#if SOC_AES_CRYPTO_DMA
        crypto_dma_ll_reset_register();
#endif
    }
}

/* Disable SHA peripheral block and then release it */
void esp_sha_release_hardware()
{
    SHA_RCC_ATOMIC() {
        sha_ll_enable_bus_clock(false);
#if SOC_AES_CRYPTO_DMA
        crypto_dma_ll_enable_bus_clock(false);
#endif
    }

    SHA_RELEASE();
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

/* Hash the input block by block, using non-DMA mode */
static void esp_sha_block_mode(esp_sha_type sha_type, const uint8_t *input, uint32_t ilen,
                               const uint8_t *buf, uint32_t buf_len, bool is_first_block)
{
    size_t blk_len = 0;
    size_t blk_word_len = 0;
    int num_block = 0;

    blk_len = block_length(sha_type);
    assert(blk_len != 0);

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

static DRAM_ATTR crypto_dma_desc_t s_dma_descr_input;
static DRAM_ATTR crypto_dma_desc_t s_dma_descr_buf;

static esp_err_t esp_sha_dma_process(esp_sha_type sha_type, const void *input, uint32_t ilen,
                                     const void *buf, uint32_t buf_len, bool is_first_block);

#ifdef SOC_AXI_DMA_EXT_MEM_ENC_ALIGNMENT
static esp_err_t esp_sha_dma_process_ext(esp_sha_type sha_type, const void *input, uint32_t ilen,
                                        const void *buf, uint32_t buf_len, bool is_first_block,
                                        bool realloc_input, bool realloc_buf)
{
    int ret = ESP_FAIL;
    void *input_copy = NULL;
    void *buf_copy = NULL;

    const void *dma_input = NULL;
    const void *dma_buf = NULL;

    uint32_t heap_caps = 0;

    if (realloc_input) {
        heap_caps = MALLOC_CAP_8BIT | (esp_ptr_external_ram(input) ? MALLOC_CAP_SPIRAM : MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
        input_copy = heap_caps_aligned_alloc(SOC_AXI_DMA_EXT_MEM_ENC_ALIGNMENT, ilen, heap_caps);
        if (input_copy == NULL) {
            ESP_LOGE(TAG, "Failed to allocate aligned SPIRAM memory");
            return ret;
        }
        memcpy(input_copy, input, ilen);
        dma_input = input_copy;
    } else {
        dma_input = input;
    }

    if (realloc_buf) {
        heap_caps = MALLOC_CAP_8BIT | (esp_ptr_external_ram(buf) ? MALLOC_CAP_SPIRAM : MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
        buf_copy = heap_caps_aligned_alloc(SOC_AXI_DMA_EXT_MEM_ENC_ALIGNMENT, buf_len, heap_caps);
        if (buf_copy == NULL) {
            ESP_LOGE(TAG, "Failed to allocate aligned internal memory");
            return ret;
        }
        memcpy(buf_copy, buf, buf_len);
        dma_buf = buf_copy;
    } else {
        dma_buf = buf;
    }

    ret = esp_sha_dma_process(sha_type, dma_input, ilen, dma_buf, buf_len, is_first_block);

    if (realloc_input) {
        free(input_copy);
    }

    if (realloc_buf) {
        free(buf_copy);
    }

    return ret;
}
#endif /* SOC_AXI_DMA_EXT_MEM_ENC_ALIGNMENT */

/* Performs SHA on multiple blocks at a time */
static esp_err_t esp_sha_dma_process(esp_sha_type sha_type, const void *input, uint32_t ilen,
                                     const void *buf, uint32_t buf_len, bool is_first_block)
{
    int ret = 0;
    crypto_dma_desc_t *dma_descr_head = NULL;

    size_t blk_len = block_length(sha_type);
    if (blk_len == 0) {
        ESP_LOGE(TAG, "Unsupported SHA type");
        return ESP_FAIL;
    }
    size_t num_blks = (ilen + buf_len) / blk_len;

    memset(&s_dma_descr_input, 0, sizeof(crypto_dma_desc_t));
    memset(&s_dma_descr_buf, 0, sizeof(crypto_dma_desc_t));

/* When SHA-DMA operations are carried out using external memory with external memory encryption enabled,
   we need to make sure that the addresses and the sizes of the buffers on which the DMA operates are 16 byte-aligned. */
#ifdef SOC_AXI_DMA_EXT_MEM_ENC_ALIGNMENT
    if (esp_flash_encryption_enabled()) {
        if (esp_ptr_external_ram(input) || esp_ptr_external_ram(buf) || esp_ptr_in_drom(input) || esp_ptr_in_drom(buf)) {
            bool input_needs_realloc = false;
            bool buf_needs_realloc = false;

            if (ilen && ((intptr_t)(input) & (SOC_AXI_DMA_EXT_MEM_ENC_ALIGNMENT - 1)) != 0) {
                input_needs_realloc = true;
            }

            if (buf_len && ((intptr_t)(buf) & (SOC_AXI_DMA_EXT_MEM_ENC_ALIGNMENT - 1)) != 0) {
                buf_needs_realloc = true;
            }

            if (input_needs_realloc || buf_needs_realloc) {
                return esp_sha_dma_process_ext(sha_type, input, ilen, buf, buf_len, is_first_block, input_needs_realloc, buf_needs_realloc);
            }
        }
    }
#endif /* SOC_AXI_DMA_EXT_MEM_ENC_ALIGNMENT */

    /* DMA descriptor for Memory to DMA-SHA transfer */
    if (ilen) {
        s_dma_descr_input.dw0.length = ilen;
        s_dma_descr_input.dw0.size = ilen;
        s_dma_descr_input.dw0.owner = 1;
        s_dma_descr_input.dw0.suc_eof = 1;
        s_dma_descr_input.buffer = (void *) input;
        dma_descr_head = &s_dma_descr_input;
    }
    /* Check after input to override head if there is any buf*/
    if (buf_len) {
        s_dma_descr_buf.dw0.length = buf_len;
        s_dma_descr_buf.dw0.size = buf_len;
        s_dma_descr_buf.dw0.owner = 1;
        s_dma_descr_buf.dw0.suc_eof = 1;
        s_dma_descr_buf.buffer = (void *) buf;
        dma_descr_head = &s_dma_descr_buf;
    }

    /* Link DMA lists */
    if (buf_len && ilen) {
        s_dma_descr_buf.dw0.suc_eof = 0;
        s_dma_descr_buf.next = (&s_dma_descr_input);
    }

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    if (ilen) {
        ESP_ERROR_CHECK(esp_cache_msync(&s_dma_descr_input, sizeof(crypto_dma_desc_t), ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED));
        ESP_ERROR_CHECK(esp_cache_msync(s_dma_descr_input.buffer, s_dma_descr_input.dw0.length, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED));
    }

    if (buf_len) {
        ESP_ERROR_CHECK(esp_cache_msync(&s_dma_descr_buf, sizeof(crypto_dma_desc_t), ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED));
        ESP_ERROR_CHECK(esp_cache_msync(s_dma_descr_buf.buffer, s_dma_descr_buf.dw0.length, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED));
    }
#endif /* SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE */

    if (esp_sha_dma_start(dma_descr_head) != ESP_OK) {
        ESP_LOGE(TAG, "esp_sha_dma_start failed, no DMA channel available");
        return -1;
    }

    sha_hal_hash_dma(sha_type, num_blks, is_first_block);

    sha_hal_wait_idle();

    return ret;
}

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
        esp_cache_msync((void *)input, ilen, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);
    }
    if (esp_ptr_external_ram(buf)) {
        esp_cache_msync((void *)buf, buf_len, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);
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
