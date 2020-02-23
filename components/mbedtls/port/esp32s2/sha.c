/*
 *  ESP32 hardware accelerated SHA1/256/512 implementation
 *  based on mbedTLS FIPS-197 compliant version.
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  Additions Copyright (C) 2016, Espressif Systems (Shanghai) PTE Ltd
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
#include <assert.h>
#include "soc/soc.h"
#include "esp32s2/crypto_dma.h"
#include "esp32s2/sha.h"
#include "soc/crypto_dma_reg.h"
#include "esp32s2/rom/ets_sys.h"
#include "soc/dport_reg.h"
#include "soc/hwcrypto_reg.h"
#include "esp32s2/rom/lldesc.h"
#include "esp32s2/rom/cache.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "soc/periph_defs.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

/* Single lock for SHA engine
*/
static _lock_t s_sha_lock;

/* Enable if want to use SHA interrupt */
//#define CONFIG_MBEDTLS_SHA_USE_INTERRUPT

#if defined(CONFIG_MBEDTLS_SHA_USE_INTERRUPT)
static SemaphoreHandle_t op_complete_sem;
#endif

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

/* This API was designed for ESP32, which has seperate
   engines for SHA1,256,512. ESP32C has a single engine.
*/
static void esp_sha_lock_engine_inner(void);

bool esp_sha_try_lock_engine(esp_sha_type sha_type)
{
    if (_lock_try_acquire(&s_sha_lock) != 0) {
        /* SHA engine is already in use */
        return false;
    } else {
        esp_sha_lock_engine_inner();
        return true;
    }
}

void esp_sha_lock_engine(esp_sha_type sha_type)
{
    _lock_acquire(&s_sha_lock);
    esp_sha_lock_engine_inner();
}

/* Enable SHA block and then lock it */
static void esp_sha_lock_engine_inner(void)
{
    /* Need to lock DMA since it is shared with AES block */
    portENTER_CRITICAL(&crypto_dma_spinlock);

    REG_SET_BIT(DPORT_PERIP_CLK_EN1_REG, DPORT_CRYPTO_SHA_CLK_EN | DPORT_CRYPTO_DMA_CLK_EN);
    REG_CLR_BIT(DPORT_PERIP_RST_EN1_REG, DPORT_CRYPTO_SHA_RST | DPORT_CRYPTO_HMAC_RST |
                DPORT_CRYPTO_DMA_RST | DPORT_CRYPTO_DS_RST);

    /* DMA for SHA */
    REG_WRITE(CRYPTO_DMA_AES_SHA_SELECT_REG, 1);
}

/* Disable SHA block and then unlock it */
void esp_sha_unlock_engine(esp_sha_type sha_type)
{
    REG_WRITE(CRYPTO_DMA_AES_SHA_SELECT_REG, 0);

    REG_SET_BIT(DPORT_PERIP_RST_EN1_REG, DPORT_CRYPTO_SHA_RST | DPORT_CRYPTO_DMA_RST |
                DPORT_CRYPTO_DS_RST);
    REG_CLR_BIT(DPORT_PERIP_CLK_EN1_REG, DPORT_CRYPTO_SHA_CLK_EN | DPORT_CRYPTO_DMA_CLK_EN);

    portEXIT_CRITICAL(&crypto_dma_spinlock);

    _lock_release(&s_sha_lock);
}

#if defined (CONFIG_MBEDTLS_SHA_USE_INTERRUPT)
static IRAM_ATTR void esp_sha_dma_isr(void *arg)
{
    BaseType_t higher_woken;
    REG_WRITE(SHA_CLEAR_IRQ_REG, 1);
    xSemaphoreGiveFromISR(op_complete_sem, &higher_woken);
    if (higher_woken) {
        portYIELD_FROM_ISR();
    }
}
#endif

/* Check if SHA operation completed */
static int esp_sha_dma_complete(void)
{
#if defined (CONFIG_MBEDTLS_SHA_USE_INTERRUPT)
    if (!xSemaphoreTake(op_complete_sem, 2000 / portTICK_PERIOD_MS)) {
        ESP_LOGE("SHA", "Timed out waiting for completion of SHA Interrupt");
        return -1;
    }
#else
    esp_sha_wait_idle();
#endif
    return 0;
}

/* Wait until SHA is busy */
void esp_sha_wait_idle(void)
{
    while (DPORT_REG_READ(SHA_BUSY_REG) != 0) { }
}

/* Read the SHA digest from hardware */
void esp_sha_read_digest_state(esp_sha_type sha_type, void *digest_state)
{
    esp_sha_wait_idle();
    memcpy(digest_state, (void *)SHA_H_BASE, state_length(sha_type));
}

/* Internally calls DMA API for single block */
void esp_sha_block(esp_sha_type sha_type, const void *data_block, bool is_first_block)
{
    esp_sha_dma(sha_type, data_block, block_length(sha_type), is_first_block);
}

/* Performs SHA on multiple blocks at a time */
int esp_sha_dma(esp_sha_type sha_type, const void *data_block, uint32_t ilen, bool is_first_block)
{
    size_t blk_len = 0;
    const uint8_t *local_buf = data_block;
    int ret = 0;
    volatile lldesc_t dma_descr;

    if (ilen == 0) {
        return ret;
    }

    blk_len = block_length(sha_type);

    REG_WRITE(SHA_MODE_REG, sha_type);
    if ((sha_type == SHA2_512T) && (is_first_block == true)) {
        REG_WRITE(SHA_START_REG, 1);
    }

    REG_WRITE(SHA_BLOCK_NUM_REG, (ilen / blk_len));

    if ((sha_type == SHA2_512T) && (is_first_block == true)) {
        esp_sha_wait_idle();
        is_first_block = false;
    }

    bzero( (void *)&dma_descr, sizeof( dma_descr ) );

    /* DMA descriptor for Memory to DMA-AES transfer */
    dma_descr.length = ilen;
    dma_descr.size = ilen;
    dma_descr.owner = 1;
    dma_descr.eof = 1;
    dma_descr.buf = local_buf;
    dma_descr.sosf = 0;
    dma_descr.empty = 0;

#if (CONFIG_SPIRAM_USE_CAPS_ALLOC || CONFIG_SPIRAM_USE_MALLOC)
    if ((unsigned int)data_block >= SOC_EXTRAM_DATA_LOW && (unsigned int)data_block <= SOC_EXTRAM_DATA_HIGH) {
        Cache_WriteBack_All();
    }
#endif

    /* Reset DMA */
    SET_PERI_REG_MASK(CRYPTO_DMA_CONF0_REG, CONF0_REG_AHBM_RST | CONF0_REG_OUT_RST | CONF0_REG_AHBM_FIFO_RST);
    CLEAR_PERI_REG_MASK(CRYPTO_DMA_CONF0_REG, CONF0_REG_AHBM_RST | CONF0_REG_OUT_RST | CONF0_REG_AHBM_FIFO_RST);

    /* Set descriptors */
    CLEAR_PERI_REG_MASK(CRYPTO_DMA_OUT_LINK_REG, OUT_LINK_REG_OUTLINK_ADDR);
    SET_PERI_REG_MASK(CRYPTO_DMA_OUT_LINK_REG, ((uint32_t)(&dma_descr))&OUT_LINK_REG_OUTLINK_ADDR);
    /* Start transfer */
    SET_PERI_REG_MASK(CRYPTO_DMA_OUT_LINK_REG, OUT_LINK_REG_OUTLINK_START);

#if defined (CONFIG_MBEDTLS_SHA_USE_INTERRUPT)
    REG_WRITE(SHA_CLEAR_IRQ_REG, 1);
    if (op_complete_sem == NULL) {
        op_complete_sem = xSemaphoreCreateBinary();
        esp_intr_alloc(ETS_SHA_INTR_SOURCE, 0, esp_sha_dma_isr, 0, 0);
    }
    REG_WRITE(SHA_INT_ENA_REG, 1);
#endif

    if (is_first_block) {
        REG_WRITE(SHA_DMA_START_REG, 1);
    } else {
        REG_WRITE(SHA_DMA_CONTINUE_REG, 1);
    }

    ret = esp_sha_dma_complete();

#if (CONFIG_SPIRAM_USE_CAPS_ALLOC || CONFIG_SPIRAM_USE_MALLOC)
    if ((unsigned int)data_block >= SOC_EXTRAM_DATA_LOW && (unsigned int)data_block <= SOC_EXTRAM_DATA_HIGH) {
        Cache_Invalidate_DCache_All();
    }
#endif

    return ret;
}

void esp_sha(esp_sha_type sha_type, const unsigned char *input, size_t ilen, unsigned char *output)
{
    SHA_CTX ctx;

    esp_sha_lock_engine(sha_type);

    ets_sha_init(&ctx, sha_type);
    ets_sha_starts(&ctx, 0);
    ets_sha_update(&ctx, input, ilen, false);
    ets_sha_finish(&ctx, output);

    esp_sha_unlock_engine(sha_type);
}
