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

typedef int _lock_t;

#include <string.h>
#include <stdio.h>
#include <sys/lock.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp32s3/rom/ets_sys.h"
#include "soc/dport_reg.h"
#include "soc/hwcrypto_reg.h"
#include "soc/soc_memory_layout.h"

#include "esp32s3/rom/cache.h"

#include "soc/cache_memory.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp32s3/sha.h"
#include "esp32s3/rom/lldesc.h"
#include "soc/periph_defs.h"
#include "driver/periph_ctrl.h"
#include "sys/param.h"
#include "soc/gdma_struct.h"
#include "soc/extmem_reg.h"

#define DMA_PERIPH_AES 6 /* DMA peripheral indexes */
#define DMA_PERIPH_SHA 7
#define DMA_CHANNEL 1 /* note: hard-coded */

/* Max amount of bytes in a single DMA operation is 4095,
   for SHA this means that the biggest safe amount of bytes is
   31 blocks of 128 bytes = 3968
*/
#define SHA_DMA_MAX_BYTES 3968

/* Lock for SHA engine */
static _lock_t s_sha_lock;

const static char *TAG = "esp-sha";

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
    _lock_acquire(&s_sha_lock);

    /* Enable SHA and DMA hardware */
    //periph_module_enable(PERIPH_SHA_DMA_MODULE);
    REG_SET_BIT(SYSTEM_PERIP_CLK_EN1_REG, SYSTEM_CRYPTO_SHA_CLK_EN | SYSTEM_DMA_CLK_EN);
    REG_CLR_BIT(SYSTEM_PERIP_RST_EN1_REG, SYSTEM_CRYPTO_SHA_RST | SYSTEM_CRYPTO_HMAC_RST |
                SYSTEM_DMA_RST | SYSTEM_CRYPTO_DS_RST);


}

/* Disable SHA peripheral block and then release it */
void esp_sha_release_hardware()
{
    /* Disable SHA and DMA hardware */
    //periph_module_disable(PERIPH_SHA_MODULE);
    REG_SET_BIT(SYSTEM_PERIP_RST_EN1_REG, SYSTEM_CRYPTO_SHA_RST | SYSTEM_DMA_RST |
                SYSTEM_CRYPTO_DS_RST);
    REG_CLR_BIT(SYSTEM_PERIP_CLK_EN1_REG, SYSTEM_CRYPTO_SHA_CLK_EN | SYSTEM_DMA_CLK_EN);

    _lock_release(&s_sha_lock);

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


static int esp_sha_dma_process(esp_sha_type sha_type, const void *input, uint32_t ilen,
                               const void *buf, uint32_t buf_len, bool is_first_block);

/* Performs SHA on multiple blocks at a time using DMA
   splits up into smaller operations for inputs that exceed a single DMA list
 */
int esp_sha_dma(esp_sha_type sha_type, const void *input, uint32_t ilen,
                const void *buf, uint32_t buf_len, bool is_first_block)
{
    int ret = 0;
    const void *dma_input;
    unsigned char *non_icache_input = NULL;
    unsigned char *non_icache_buf = NULL;
    int dma_op_num = ( ilen / (SHA_DMA_MAX_BYTES + 1) ) + 1;

    if (buf_len > 128) {
        ESP_LOGE(TAG, "SHA DMA buf_len cannot exceed max size for a single block");
        return -1;
    }

    /* DMA cannot access memory in the iCache range, copy data to temporary buffers before transfer */
    if (!esp_ptr_dma_capable(input) && ilen) {
        non_icache_input = malloc(sizeof(unsigned char) * MIN(ilen, SHA_DMA_MAX_BYTES));
        if (non_icache_input == NULL) {
            ESP_LOGE(TAG, "Failed to allocate memory");
            ret = ESP_ERR_NO_MEM;
            goto cleanup;
        }
    }

    if (!esp_ptr_dma_capable(buf) && buf_len) {
        non_icache_buf = malloc(sizeof(unsigned char) * buf_len);
        if (non_icache_buf == NULL) {
            ESP_LOGE(TAG, "Failed to allocate memory");
            ret = ESP_ERR_NO_MEM;
            goto cleanup;
        }
        memcpy(non_icache_buf, buf, buf_len);
        buf = non_icache_buf;
    }

    /* The max amount of blocks in a single hardware operation is 2^6 - 1 = 63
       Thus we only do a single DMA input list + dma buf list,
       which is max 3968/64 + 64/64 = 63 blocks */
    for (int i = 0; i < dma_op_num; i++) {
        int dma_chunk_len = MIN(ilen, SHA_DMA_MAX_BYTES);


        /* Input depends on if it's a temp alloc buffer or supplied by user */
        if (non_icache_input != NULL) {
            memcpy(non_icache_input, input, dma_chunk_len);
            dma_input = non_icache_input;
        } else {
            dma_input = input;
        }

        ret = esp_sha_dma_process(sha_type, dma_input, dma_chunk_len, buf, buf_len, is_first_block);


        if (ret != 0) {
            return ret;
        }

        is_first_block = false;


        ilen -= dma_chunk_len;
        input += dma_chunk_len;

        // Only append buf to the first operation
        buf_len = 0;
    }

cleanup:
    free(non_icache_input);
    free(non_icache_buf);
    return ret;
}

static void esp_sha_dma_init(lldesc_t *input)
{
    /* Reset DMA */
    REG_CLR_BIT(SYSTEM_PERIP_CLK_EN1_REG, SYSTEM_DMA_CLK_EN);
    REG_SET_BIT(SYSTEM_PERIP_CLK_EN1_REG, SYSTEM_DMA_CLK_EN);
    REG_SET_BIT(SYSTEM_PERIP_RST_EN1_REG, SYSTEM_DMA_RST);
    REG_CLR_BIT(SYSTEM_PERIP_RST_EN1_REG, SYSTEM_DMA_RST);

    /* NOTE: all hardcoded to DMA channel 1 */
    /* Note: burst mode has alignment requirements that we have not checked here */
    GDMA.conf0[0].outdscr_burst_en = 0; /* was 1*/
    GDMA.conf0[0].out_data_burst_en = 0; /* was 1*/
    GDMA.conf0[0].out_auto_wrback = 0;

    GDMA.peri_sel[0].peri_out_sel = DMA_PERIPH_SHA;

    GDMA.sram_size[0].in_size = 3; /* 40 bytes, also minimum size for EDMA */
    GDMA.sram_size[0].out_size = 3;
    GDMA.conf1[0].in_ext_mem_bk_size = 0; // 16 bytes
    GDMA.conf1[0].out_ext_mem_bk_size = 0; // 16 bytes

    /* Set descriptors */
    GDMA.out_link[0].addr = (uint32_t)input;

    GDMA.conf0[0].in_rst = 1;
    GDMA.conf0[0].in_rst = 0;
    GDMA.conf0[0].out_rst = 1;
    GDMA.conf0[0].out_rst = 0;

    /* Start transfer */
    GDMA.out_link[0].start = 1;
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

/* Performs SHA on multiple blocks at a time */
static int esp_sha_dma_process(esp_sha_type sha_type, const void *input, uint32_t ilen,
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
        dma_descr_input.buf = (void *)input;
        dma_descr_head = &dma_descr_input;
    }
    /* Check after input to overide head if there is any buf*/
    if (buf_len) {
        dma_descr_buf.length = buf_len;
        dma_descr_buf.size = buf_len;
        dma_descr_buf.owner = 1;
        dma_descr_buf.eof = 1;
        dma_descr_buf.buf = (void *)buf;
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

