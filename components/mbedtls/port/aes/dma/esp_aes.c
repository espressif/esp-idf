/**
 * \brief AES block cipher, ESP DMA hardware accelerated version
 * Based on mbedTLS FIPS-197 compliant version.
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
 *  The AES block cipher was designed by Vincent Rijmen and Joan Daemen.
 *
 *  http://csrc.nist.gov/encryption/aes/rijndael/Rijndael.pdf
 *  http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
 */

#include <string.h>
#include "mbedtls/aes.h"
#include "esp_intr_alloc.h"
#include "esp_private/periph_ctrl.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "soc/lldesc.h"
#include "esp_heap_caps.h"
#include "esp_memory_utils.h"
#include "sys/param.h"
#if CONFIG_PM_ENABLE
#include "esp_pm.h"
#endif
#include "esp_crypto_lock.h"
#include "hal/aes_hal.h"
#include "aes/esp_aes_internal.h"
#include "esp_aes_dma_priv.h"

#if CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/cache.h"
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#if SOC_AES_GDMA
#define AES_LOCK() esp_crypto_sha_aes_lock_acquire()
#define AES_RELEASE() esp_crypto_sha_aes_lock_release()
#elif SOC_AES_CRYPTO_DMA
#define AES_LOCK() esp_crypto_dma_lock_acquire()
#define AES_RELEASE() esp_crypto_dma_lock_release()
#endif

/* Max size of each chunk to process when output buffer is in unaligned external ram
   must be a multiple of block size
*/
#define AES_MAX_CHUNK_WRITE_SIZE 1600

/* Input over this length will yield and wait for interrupt instead of
   busy-waiting, 30000 bytes is approx 0.5 ms */
#define AES_DMA_INTR_TRIG_LEN 2000

/* With buffers in PSRAM (worst condition) we still achieve a speed of 4 MB/s
   thus a 2 second timeout value should be suffient for even very large buffers.
 */
#define AES_WAIT_INTR_TIMEOUT_MS 2000

#if defined(CONFIG_MBEDTLS_AES_USE_INTERRUPT)
static SemaphoreHandle_t op_complete_sem;
#if defined(CONFIG_PM_ENABLE)
static esp_pm_lock_handle_t s_pm_cpu_lock;
static esp_pm_lock_handle_t s_pm_sleep_lock;
#endif
#endif

#if SOC_PSRAM_DMA_CAPABLE

#if (CONFIG_ESP32S2_DATA_CACHE_LINE_16B || CONFIG_ESP32S3_DATA_CACHE_LINE_16B)
#define DCACHE_LINE_SIZE 16
#elif (CONFIG_ESP32S2_DATA_CACHE_LINE_32B || CONFIG_ESP32S3_DATA_CACHE_LINE_32B)
#define DCACHE_LINE_SIZE 32
#elif CONFIG_ESP32S3_DATA_CACHE_LINE_64B
#define DCACHE_LINE_SIZE 64
#endif //(CONFIG_ESP32S2_DATA_CACHE_LINE_16B || CONFIG_ESP32S3_DATA_CACHE_LINE_16B)

#endif //SOC_PSRAM_DMA_CAPABLE

static const char *TAG = "esp-aes";
static bool s_check_dma_capable(const void *p);

/* These are static due to:
 *  * Must be in DMA capable memory, so stack is not a safe place to put them
 *  * To avoid having to malloc/free them for every DMA operation
 */
static DRAM_ATTR lldesc_t s_stream_in_desc;
static DRAM_ATTR lldesc_t s_stream_out_desc;
static DRAM_ATTR uint8_t s_stream_in[AES_BLOCK_BYTES];
static DRAM_ATTR uint8_t s_stream_out[AES_BLOCK_BYTES];

static inline void esp_aes_wait_dma_done(lldesc_t *output)
{
    /* Wait for DMA write operation to complete */
    while (1) {
        if ( esp_aes_dma_done(output) ) {
            break;
        }
    }
}

/* Append a descriptor to the chain, set head if chain empty */
static inline void lldesc_append(lldesc_t **head, lldesc_t *item)
{
    lldesc_t *it;
    if (*head == NULL) {
        *head = item;
        return;
    }

    it = *head;

    while (it->empty != 0) {
        it = (lldesc_t *)it->empty;
    }
    it->eof = 0;
    it->empty = (uint32_t)item;
}

void esp_aes_acquire_hardware( void )
{
    /* Released by esp_aes_release_hardware()*/
    AES_LOCK();

    /* Enable AES and DMA hardware */
#if SOC_AES_CRYPTO_DMA
    periph_module_enable(PERIPH_AES_DMA_MODULE);
#elif SOC_AES_GDMA
    periph_module_enable(PERIPH_AES_MODULE);
#endif
}

/* Function to disable AES and Crypto DMA clocks and release locks */
void esp_aes_release_hardware( void )
{
    /* Disable AES and DMA hardware */
#if SOC_AES_CRYPTO_DMA
    periph_module_disable(PERIPH_AES_DMA_MODULE);
#elif SOC_AES_GDMA
    periph_module_disable(PERIPH_AES_MODULE);
#endif

    AES_RELEASE();
}


#if defined (CONFIG_MBEDTLS_AES_USE_INTERRUPT)
static IRAM_ATTR void esp_aes_complete_isr(void *arg)
{
    BaseType_t higher_woken;
    aes_hal_interrupt_clear();
    xSemaphoreGiveFromISR(op_complete_sem, &higher_woken);
    if (higher_woken) {
        portYIELD_FROM_ISR();
    }
}

void esp_aes_intr_alloc(void)
{
    if (op_complete_sem == NULL) {
        const int isr_flags = esp_intr_level_to_flags(CONFIG_MBEDTLS_AES_INTERRUPT_LEVEL);

        esp_err_t ret = esp_intr_alloc(ETS_AES_INTR_SOURCE, isr_flags, esp_aes_complete_isr, NULL, NULL);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to allocate AES interrupt %d", ret);
            // This should be treated as fatal error as this API would mostly
            // be invoked within mbedTLS interface. There is no way for the system
            // to proceed if the AES interrupt allocation fails here.
            abort();
        }

        static StaticSemaphore_t op_sem_buf;
        op_complete_sem = xSemaphoreCreateBinaryStatic(&op_sem_buf);
        // Static semaphore creation is unlikley to fail but still basic sanity
        assert(op_complete_sem != NULL);
    }
}

static esp_err_t esp_aes_isr_initialise( void )
{
    aes_hal_interrupt_clear();
    aes_hal_interrupt_enable(true);

    /* AES is clocked proportionally to CPU clock, take power management lock */
#ifdef CONFIG_PM_ENABLE
    if (s_pm_cpu_lock == NULL) {
        if (esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "aes_sleep", &s_pm_sleep_lock) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to create PM sleep lock");
            return ESP_FAIL;
        }
        if (esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "aes_cpu", &s_pm_cpu_lock) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to create PM CPU lock");
            return ESP_FAIL;
        }
    }
    esp_pm_lock_acquire(s_pm_cpu_lock);
    esp_pm_lock_acquire(s_pm_sleep_lock);
#endif

    return ESP_OK;
}
#endif // CONFIG_MBEDTLS_AES_USE_INTERRUPT

/* Wait for AES hardware block operation to complete */
static int esp_aes_dma_wait_complete(bool use_intr, lldesc_t *output_desc)
{
#if defined (CONFIG_MBEDTLS_AES_USE_INTERRUPT)
    if (use_intr) {
        if (!xSemaphoreTake(op_complete_sem, AES_WAIT_INTR_TIMEOUT_MS / portTICK_PERIOD_MS)) {
            /* indicates a fundamental problem with driver */
            ESP_LOGE(TAG, "Timed out waiting for completion of AES Interrupt");
            return -1;
        }
#ifdef CONFIG_PM_ENABLE
        esp_pm_lock_release(s_pm_cpu_lock);
        esp_pm_lock_release(s_pm_sleep_lock);
#endif  // CONFIG_PM_ENABLE
    }
#endif
    /* Checking this if interrupt is used also, to avoid
       issues with AES fault injection
    */
    aes_hal_wait_done();

    esp_aes_wait_dma_done(output_desc);
    return 0;
}


static int esp_aes_process_dma(esp_aes_context *ctx, const unsigned char *input, unsigned char *output, size_t len, uint8_t *stream_out);


/* Output buffers in external ram needs to be 16-byte aligned and DMA cant access input in the iCache mem range,
   reallocate them into internal memory and encrypt in chunks to avoid
   having to malloc too big of a buffer

  The function esp_aes_process_dma_ext_ram zeroises the output buffer in the case of memory allocation failure.
*/

static int esp_aes_process_dma_ext_ram(esp_aes_context *ctx, const unsigned char *input, unsigned char *output, size_t len, uint8_t *stream_out, bool realloc_input, bool realloc_output)
{
    size_t chunk_len;
    int ret = 0;
    int offset = 0;
    unsigned char *input_buf = NULL;
    unsigned char *output_buf = NULL;
    const unsigned char *dma_input;
    chunk_len = MIN(AES_MAX_CHUNK_WRITE_SIZE, len);

    if (realloc_input) {
        input_buf = heap_caps_malloc(chunk_len, MALLOC_CAP_DMA);

        if (input_buf == NULL) {
            mbedtls_platform_zeroize(output, len);
            ESP_LOGE(TAG, "Failed to allocate memory");
            return -1;
        }
    }

    if (realloc_output) {
        output_buf = heap_caps_malloc(chunk_len, MALLOC_CAP_DMA);

        if (output_buf == NULL) {
            mbedtls_platform_zeroize(output, len);
            ESP_LOGE(TAG, "Failed to allocate memory");
            return -1;
        }
    } else {
        output_buf = output;
    }

    while (len) {
        chunk_len = MIN(AES_MAX_CHUNK_WRITE_SIZE, len);

        /* If input needs realloc then copy it, else use the input with offset*/
        if (realloc_input) {
            memcpy(input_buf, input + offset, chunk_len);
            dma_input = input_buf;
        } else {
            dma_input = input + offset;
        }

        if (esp_aes_process_dma(ctx, dma_input, output_buf, chunk_len, stream_out) != 0) {
            ret = -1;
            goto cleanup;
        }

        if (realloc_output) {
            memcpy(output + offset, output_buf, chunk_len);
        } else {
            output_buf = output + offset + chunk_len;
        }

        len -= chunk_len;
        offset += chunk_len;
    }

cleanup:

    if (realloc_input) {
        free(input_buf);
    }
    if (realloc_output) {
        free(output_buf);
    }

    return ret;
}

/* Encrypt/decrypt the input using DMA
 * The function esp_aes_process_dma zeroises the output buffer in the case of following conditions:
 * 1. If key is not written in the hardware
 * 2. Memory allocation failures
 * 3. If AES interrupt is enabled and ISR initialisation fails
 * 4. Failure in any of the AES operations
 */
static int esp_aes_process_dma(esp_aes_context *ctx, const unsigned char *input, unsigned char *output, size_t len, uint8_t *stream_out)
{
    lldesc_t *in_desc_head = NULL, *out_desc_head = NULL;
    lldesc_t *out_desc_tail = NULL; /* pointer to the final output descriptor */
    lldesc_t *block_desc = NULL, *block_in_desc = NULL, *block_out_desc = NULL;
    size_t lldesc_num = 0;
    unsigned stream_bytes = len % AES_BLOCK_BYTES; // bytes which aren't in a full block
    unsigned block_bytes = len - stream_bytes;     // bytes which are in a full block
    unsigned blocks = (block_bytes / AES_BLOCK_BYTES) + ((stream_bytes > 0) ? 1 : 0);
    bool use_intr = false;
    bool input_needs_realloc = false;
    bool output_needs_realloc = false;
    int ret = 0;

    assert(len > 0); // caller shouldn't ever have len set to zero
    assert(stream_bytes == 0 || stream_out != NULL); // stream_out can be NULL if we're processing full block(s)

    /* If no key is written to hardware yet, either the user hasn't called
       mbedtls_aes_setkey_enc/mbedtls_aes_setkey_dec - meaning we also don't
       know which mode to use - or a fault skipped the
       key write to hardware. Treat this as a fatal error and zero the output block.
    */
    if (ctx->key_in_hardware != ctx->key_bytes) {
        mbedtls_platform_zeroize(output, len);
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }

    if (block_bytes > 0) {
        /* Flush cache if input in external ram */
#if (CONFIG_SPIRAM && SOC_PSRAM_DMA_CAPABLE)
        if (esp_ptr_external_ram(input)) {
            Cache_WriteBack_Addr((uint32_t)input, len);
        }
        if (esp_ptr_external_ram(output)) {
            if ((((intptr_t)(output) & (DCACHE_LINE_SIZE - 1)) != 0) || (block_bytes % DCACHE_LINE_SIZE != 0)) {
                // Non aligned ext-mem buffer
                output_needs_realloc = true;
            }
        }
#endif
        /* DMA cannot access memory in the iCache range, copy input to internal ram */
        if (!s_check_dma_capable(input)) {
            input_needs_realloc = true;
        }

        if (!s_check_dma_capable(output)) {
            output_needs_realloc = true;
        }

        /* If either input or output is unaccessible to the DMA then they need to be reallocated */
        if (input_needs_realloc || output_needs_realloc) {
            return esp_aes_process_dma_ext_ram(ctx, input, output, len, stream_out, input_needs_realloc, output_needs_realloc);
        }

        /* Set up dma descriptors for input and output considering the 16 byte alignment requirement for EDMA */
        lldesc_num = lldesc_get_required_num_constrained(block_bytes, LLDESC_MAX_NUM_PER_DESC_16B_ALIGNED);

        /* Allocate both in and out descriptors to save a malloc/free per function call */
        block_desc = heap_caps_calloc(lldesc_num * 2, sizeof(lldesc_t), MALLOC_CAP_DMA);
        if (block_desc == NULL) {
            mbedtls_platform_zeroize(output, len);
            ESP_LOGE(TAG, "Failed to allocate memory");
            return -1;
        }

        block_in_desc = block_desc;
        block_out_desc = block_desc + lldesc_num;

        lldesc_setup_link(block_in_desc, input, block_bytes, 0);
        //Limit max inlink descriptor length to be 16 byte aligned, require for EDMA
        lldesc_setup_link_constrained(block_out_desc, output, block_bytes, LLDESC_MAX_NUM_PER_DESC_16B_ALIGNED, 0);

        /* Setup in/out start descriptors */
        lldesc_append(&in_desc_head, block_in_desc);
        lldesc_append(&out_desc_head, block_out_desc);

        out_desc_tail = &block_out_desc[lldesc_num - 1];
    }

    /* Any leftover bytes which are appended as an additional DMA list */
    if (stream_bytes > 0) {

        memset(&s_stream_in_desc, 0, sizeof(lldesc_t));
        memset(&s_stream_out_desc, 0, sizeof(lldesc_t));

        memset(s_stream_in, 0, AES_BLOCK_BYTES);
        memset(s_stream_out, 0, AES_BLOCK_BYTES);

        memcpy(s_stream_in, input + block_bytes, stream_bytes);

        lldesc_setup_link(&s_stream_in_desc, s_stream_in, AES_BLOCK_BYTES, 0);
        lldesc_setup_link(&s_stream_out_desc, s_stream_out, AES_BLOCK_BYTES, 0);

        /* Link with block descriptors */
        lldesc_append(&in_desc_head, &s_stream_in_desc);
        lldesc_append(&out_desc_head, &s_stream_out_desc);

        out_desc_tail = &s_stream_out_desc;
    }

#if defined (CONFIG_MBEDTLS_AES_USE_INTERRUPT)
    /* Only use interrupt for long AES operations */
    if (len > AES_DMA_INTR_TRIG_LEN) {
        use_intr = true;
        if (esp_aes_isr_initialise() != ESP_OK) {
            ESP_LOGE(TAG, "ESP-AES ISR initialisation failed");
            ret = -1;
            goto cleanup;
        }
    } else
#endif
    {
        aes_hal_interrupt_enable(false);
    }

    if (esp_aes_dma_start(in_desc_head, out_desc_head) != ESP_OK) {
        ESP_LOGE(TAG, "esp_aes_dma_start failed, no DMA channel available");
        ret = -1;
        goto cleanup;
    }

    aes_hal_transform_dma_start(blocks);

    if (esp_aes_dma_wait_complete(use_intr, out_desc_tail) < 0) {
        ESP_LOGE(TAG, "esp_aes_dma_wait_complete failed");
        ret = -1;
        goto cleanup;
    }

#if (CONFIG_SPIRAM && SOC_PSRAM_DMA_CAPABLE)
    if (block_bytes > 0) {
        if (esp_ptr_external_ram(output)) {
            Cache_Invalidate_Addr((uint32_t)output, block_bytes);
        }
    }
#endif
    aes_hal_transform_dma_finish();

    if (stream_bytes > 0) {
        memcpy(output + block_bytes, s_stream_out, stream_bytes);
        memcpy(stream_out, s_stream_out, AES_BLOCK_BYTES);
    }

cleanup:
    if (ret != 0) {
        mbedtls_platform_zeroize(output, len);
    }
    free(block_desc);
    return ret;
}


#if CONFIG_MBEDTLS_HARDWARE_GCM

/* Encrypt/decrypt with AES-GCM the input using DMA
 * The function esp_aes_process_dma_gcm zeroises the output buffer in the case of following conditions:
 * 1. If key is not written in the hardware
 * 2. Memory allocation failures
 * 3. If AES interrupt is enabled and ISR initialisation fails
 * 4. Failure in any of the AES operations
 */
int esp_aes_process_dma_gcm(esp_aes_context *ctx, const unsigned char *input, unsigned char *output, size_t len, lldesc_t *aad_desc, size_t aad_len)
{
    lldesc_t *in_desc_head = NULL, *out_desc_head = NULL, *len_desc = NULL;
    lldesc_t *out_desc_tail = NULL; /* pointer to the final output descriptor */
    lldesc_t stream_in_desc, stream_out_desc;
    lldesc_t *block_desc = NULL, *block_in_desc = NULL, *block_out_desc = NULL;
    size_t lldesc_num;
    uint32_t len_buf[4] = {};
    uint8_t stream_in[16] = {};
    uint8_t stream_out[16] = {};
    unsigned stream_bytes = len % AES_BLOCK_BYTES; // bytes which aren't in a full block
    unsigned block_bytes = len - stream_bytes;     // bytes which are in a full block

    unsigned blocks = (block_bytes / AES_BLOCK_BYTES) + ((stream_bytes > 0) ? 1 : 0);

    bool use_intr = false;
    int ret = 0;

    /* If no key is written to hardware yet, either the user hasn't called
       mbedtls_aes_setkey_enc/mbedtls_aes_setkey_dec - meaning we also don't
       know which mode to use - or a fault skipped the
       key write to hardware. Treat this as a fatal error and zero the output block.
    */
    if (ctx->key_in_hardware != ctx->key_bytes) {
        mbedtls_platform_zeroize(output, len);
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }

    /* Set up dma descriptors for input and output */
    lldesc_num = lldesc_get_required_num(block_bytes);

    /* Allocate both in and out descriptors to save a malloc/free per function call, add 1 for length descriptor */
    block_desc = heap_caps_calloc( (lldesc_num * 2) + 1, sizeof(lldesc_t), MALLOC_CAP_DMA);
    if (block_desc == NULL) {
        mbedtls_platform_zeroize(output, len);
        ESP_LOGE(TAG, "Failed to allocate memory");
        return -1;
    }

    block_in_desc = block_desc;
    len_desc = block_desc + lldesc_num;
    block_out_desc = block_desc + lldesc_num + 1;

    if (aad_desc != NULL) {
        lldesc_append(&in_desc_head, aad_desc);
    }

    if (block_bytes > 0) {
        lldesc_setup_link(block_in_desc, input, block_bytes, 0);
        lldesc_setup_link(block_out_desc, output, block_bytes, 0);

        lldesc_append(&in_desc_head, block_in_desc);
        lldesc_append(&out_desc_head, block_out_desc);

        out_desc_tail = &block_out_desc[lldesc_num - 1];
    }

    /* Any leftover bytes which are appended as an additional DMA list */
    if (stream_bytes > 0) {
        memcpy(stream_in, input + block_bytes, stream_bytes);

        lldesc_setup_link(&stream_in_desc, stream_in, AES_BLOCK_BYTES, 0);
        lldesc_setup_link(&stream_out_desc, stream_out, AES_BLOCK_BYTES, 0);

        lldesc_append(&in_desc_head, &stream_in_desc);
        lldesc_append(&out_desc_head, &stream_out_desc);

        out_desc_tail = &stream_out_desc;
    }


    len_buf[1] = __builtin_bswap32(aad_len * 8);
    len_buf[3] = __builtin_bswap32(len * 8);

    len_desc->length = sizeof(len_buf);
    len_desc->size = sizeof(len_buf);
    len_desc->owner = 1;
    len_desc->eof = 1;
    len_desc->buf = (uint8_t *)len_buf;

    lldesc_append(&in_desc_head, len_desc);

#if defined (CONFIG_MBEDTLS_AES_USE_INTERRUPT)
    /* Only use interrupt for long AES operations */
    if (len > AES_DMA_INTR_TRIG_LEN) {
        use_intr = true;
        if (esp_aes_isr_initialise() != ESP_OK) {
            ESP_LOGE(TAG, "ESP-AES ISR initialisation failed");
            ret = -1;
            goto cleanup;
        }
    } else
#endif
    {
        aes_hal_interrupt_enable(false);
    }

    /* Start AES operation */
    if (esp_aes_dma_start(in_desc_head, out_desc_head) != ESP_OK) {
        ESP_LOGE(TAG, "esp_aes_dma_start failed, no DMA channel available");
        ret = -1;
        goto cleanup;
    }

    aes_hal_transform_dma_gcm_start(blocks);

    if (esp_aes_dma_wait_complete(use_intr, out_desc_tail) < 0) {
        ESP_LOGE(TAG, "esp_aes_dma_wait_complete failed");
        ret = -1;
        goto cleanup;
    }

    aes_hal_transform_dma_finish();

    if (stream_bytes > 0) {
        memcpy(output + block_bytes, stream_out, stream_bytes);
    }

cleanup:
    if (ret != 0) {
        mbedtls_platform_zeroize(output, len);
    }
    free(block_desc);
    return ret;
}

#endif //CONFIG_MBEDTLS_HARDWARE_GCM

static int esp_aes_validate_input(esp_aes_context *ctx, const unsigned char *input,
                                  unsigned char *output )
{
    if (!ctx) {
        ESP_LOGE(TAG, "No AES context supplied");
        return -1;
    }
    if (!input) {
        ESP_LOGE(TAG, "No input supplied");
        return -1;
    }
    if (!output) {
        ESP_LOGE(TAG, "No output supplied");
        return -1;
    }

    return 0;
}


/*
 * AES-ECB single block encryption
 */
int esp_internal_aes_encrypt(esp_aes_context *ctx,
                             const unsigned char input[16],
                             unsigned char output[16] )
{
    int r = -1;

    if (esp_aes_validate_input(ctx, input, output)) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    esp_aes_acquire_hardware();
    ctx->key_in_hardware = 0;
    ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, ESP_AES_ENCRYPT);
    aes_hal_mode_init(ESP_AES_BLOCK_MODE_ECB);
    r = esp_aes_process_dma(ctx, input, output, AES_BLOCK_BYTES, NULL);
    esp_aes_release_hardware();

    return r;
}

void esp_aes_encrypt(esp_aes_context *ctx,
                     const unsigned char input[16],
                     unsigned char output[16] )
{
    esp_internal_aes_encrypt(ctx, input, output);
}

/*
 * AES-ECB single block decryption
 */
int esp_internal_aes_decrypt(esp_aes_context *ctx,
                             const unsigned char input[16],
                             unsigned char output[16] )
{
    int r = -1;

    if (esp_aes_validate_input(ctx, input, output)) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    esp_aes_acquire_hardware();
    ctx->key_in_hardware = 0;
    ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, ESP_AES_DECRYPT);
    aes_hal_mode_init(ESP_AES_BLOCK_MODE_ECB);
    r = esp_aes_process_dma(ctx, input, output, AES_BLOCK_BYTES, NULL);
    esp_aes_release_hardware();

    return r;
}

void esp_aes_decrypt(esp_aes_context *ctx,
                     const unsigned char input[16],
                     unsigned char output[16] )
{
    esp_internal_aes_decrypt(ctx, input, output);
}


/*
 * AES-ECB block encryption/decryption
 */
int esp_aes_crypt_ecb(esp_aes_context *ctx,
                      int mode,
                      const unsigned char input[16],
                      unsigned char output[16] )
{
    int r = -1;

    if (esp_aes_validate_input(ctx, input, output)) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    esp_aes_acquire_hardware();
    ctx->key_in_hardware = 0;
    ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, mode);
    aes_hal_mode_init(ESP_AES_BLOCK_MODE_ECB);
    r = esp_aes_process_dma(ctx, input, output, AES_BLOCK_BYTES, NULL);
    esp_aes_release_hardware();

    return r;
}

/*
 * AES-CBC buffer encryption/decryption
 */
int esp_aes_crypt_cbc(esp_aes_context *ctx,
                      int mode,
                      size_t length,
                      unsigned char iv[16],
                      const unsigned char *input,
                      unsigned char *output )
{
    int r = -1;
    if (esp_aes_validate_input(ctx, input, output)) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!iv) {
        ESP_LOGE(TAG, "No IV supplied");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    /* For CBC input length should be multiple of
     * AES BLOCK BYTES
     * */
    if ( (length % AES_BLOCK_BYTES) || (length == 0) ) {
        return ERR_ESP_AES_INVALID_INPUT_LENGTH;
    }

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    esp_aes_acquire_hardware();
    ctx->key_in_hardware = 0;
    ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, mode);
    aes_hal_mode_init(ESP_AES_BLOCK_MODE_CBC);
    aes_hal_set_iv(iv);

    r = esp_aes_process_dma(ctx, input, output, length, NULL);
    if (r != 0) {
        goto cleanup;
    }

    aes_hal_read_iv(iv);

cleanup:
    esp_aes_release_hardware();
    return r;
}

/*
 * AES-CFB8 buffer encryption/decryption
 */
int esp_aes_crypt_cfb8(esp_aes_context *ctx,
                       int mode,
                       size_t length,
                       unsigned char iv[16],
                       const unsigned char *input,
                       unsigned char *output )
{
    int r = -1;
    unsigned char c;
    unsigned char ov[17];
    size_t block_bytes = length - (length % AES_BLOCK_BYTES);

    if (esp_aes_validate_input(ctx, input, output)) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!iv) {
        ESP_LOGE(TAG, "No IV supplied");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }


    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    /* The DMA engine will only output correct IV if it runs
       full blocks of input in CFB8 mode
    */
    esp_aes_acquire_hardware();

    if (block_bytes > 0) {

        ctx->key_in_hardware = 0;
        ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, mode);
        aes_hal_mode_init(ESP_AES_BLOCK_MODE_CFB8);
        aes_hal_set_iv(iv);
        r = esp_aes_process_dma(ctx, input, output, block_bytes, NULL);
        if (r != 0) {
            goto cleanup;
        }

        aes_hal_read_iv(iv);

        length -= block_bytes;
        input += block_bytes;
        output += block_bytes;
    }

    // Process remaining bytes block-at-a-time in ECB mode
    if (length > 0) {
        ctx->key_in_hardware = 0;
        ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, MBEDTLS_AES_ENCRYPT);
        aes_hal_mode_init(ESP_AES_BLOCK_MODE_ECB);

        while ( length-- ) {
            memcpy( ov, iv, 16 );

            r = esp_aes_process_dma(ctx, iv, iv, AES_BLOCK_BYTES, NULL);
            if (r != 0) {
                goto cleanup;
            }

            if ( mode == MBEDTLS_AES_DECRYPT ) {
                ov[16] = *input;
            }

            c = *output++ = ( iv[0] ^ *input++ );

            if ( mode == MBEDTLS_AES_ENCRYPT ) {
                ov[16] = c;
            }
            memcpy( iv, ov + 1, 16 );
        }

    }
    r = 0;

cleanup:
    esp_aes_release_hardware();
    return r;
}

/*
 * AES-CFB128 buffer encryption/decryption
 */
int esp_aes_crypt_cfb128(esp_aes_context *ctx,
                         int mode,
                         size_t length,
                         size_t *iv_off,
                         unsigned char iv[16],
                         const unsigned char *input,
                         unsigned char *output )

{
    uint8_t c;
    size_t stream_bytes = 0;
    size_t n;

    if (esp_aes_validate_input(ctx, input, output)) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!iv) {
        ESP_LOGE(TAG, "No IV supplied");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!iv_off) {
        ESP_LOGE(TAG, "No IV offset supplied");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    n = *iv_off;

    /* First process the *iv_off bytes
     * which are pending from the previous call to this API
     */
    while (n > 0 && length > 0) {
        if (mode == MBEDTLS_AES_ENCRYPT) {
            iv[n] = *output++ = *input++ ^ iv[n];
        } else {
            c = *input++;
            *output++ = c ^ iv[n];
            iv[n] = c;
        }
        n = (n + 1) % AES_BLOCK_BYTES;
        length--;
    }


    if (length > 0) {
        stream_bytes = length % AES_BLOCK_BYTES;
        esp_aes_acquire_hardware();
        ctx->key_in_hardware = 0;
        ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, mode);
        aes_hal_mode_init(ESP_AES_BLOCK_MODE_CFB128);
        aes_hal_set_iv(iv);

        int r = esp_aes_process_dma(ctx, input, output, length, iv);
        if (r != 0) {
            esp_aes_release_hardware();
            return r;
        }

        if (stream_bytes == 0) {
            // if we didn't need the partial 'stream block' then the new IV is in the IV register
            aes_hal_read_iv(iv);
        } else {
            // if we did process a final partial block the new IV is already processed via DMA (and has some bytes of output in it),
            // In decrypt mode any partial bytes are output plaintext (iv ^ c) and need to be swapped back to ciphertext (as the next
            // block uses ciphertext as its IV input)
            //
            // Note: It may be more efficient to not process the partial block via DMA in this case.
            if (mode == MBEDTLS_AES_DECRYPT) {
                memcpy(iv, input + length - stream_bytes, stream_bytes);
            }
        }
        esp_aes_release_hardware();
    }

    *iv_off = n + stream_bytes;
    return 0;
}

/*
 * AES-OFB (Output Feedback Mode) buffer encryption/decryption
 */

int esp_aes_crypt_ofb(esp_aes_context *ctx,
                      size_t length,
                      size_t *iv_off,
                      unsigned char iv[16],
                      const unsigned char *input,
                      unsigned char *output )
{
    size_t n;
    size_t stream_bytes = 0;

    if (esp_aes_validate_input(ctx, input, output)) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!iv) {
        ESP_LOGE(TAG, "No IV supplied");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!iv_off) {
        ESP_LOGE(TAG, "No IV offset supplied");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    n = *iv_off;

    /* If there is an offset then use the output of the previous AES block
        (the updated IV) to calculate the new output */
    while (n > 0 && length > 0) {
        *output++ = (*input++ ^ iv[n]);
        n = (n + 1) & 0xF;
        length--;
    }
    if (length > 0) {
        stream_bytes = (length % AES_BLOCK_BYTES);

        esp_aes_acquire_hardware();
        ctx->key_in_hardware = 0;
        ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, ESP_AES_DECRYPT);
        aes_hal_mode_init(ESP_AES_BLOCK_MODE_OFB);
        aes_hal_set_iv(iv);

        int r = esp_aes_process_dma(ctx, input, output, length, iv);
        if (r != 0) {
            esp_aes_release_hardware();
            return r;
        }

        aes_hal_read_iv(iv);
        esp_aes_release_hardware();
    }

    *iv_off = n + stream_bytes;

    return 0;
}

/*
 * AES-CTR buffer encryption/decryption
 */
int esp_aes_crypt_ctr(esp_aes_context *ctx,
                      size_t length,
                      size_t *nc_off,
                      unsigned char nonce_counter[16],
                      unsigned char stream_block[16],
                      const unsigned char *input,
                      unsigned char *output )
{
    size_t n;

    if (esp_aes_validate_input(ctx, input, output)) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!stream_block) {
        ESP_LOGE(TAG, "No stream supplied");
        return -1;
    }

    if (!nonce_counter) {
        ESP_LOGE(TAG, "No nonce supplied");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!nc_off) {
        ESP_LOGE(TAG, "No nonce offset supplied");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    n = *nc_off;

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    /* Process any unprocessed bytes left in stream block from
       last operation */
    while (n > 0 && length > 0) {
        *output++ = (unsigned char)(*input++ ^ stream_block[n]);
        n = (n + 1) & 0xF;
        length--;
    }

    if (length > 0) {

        esp_aes_acquire_hardware();
        ctx->key_in_hardware = 0;
        ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, ESP_AES_DECRYPT);

        aes_hal_mode_init(ESP_AES_BLOCK_MODE_CTR);
        aes_hal_set_iv(nonce_counter);

        int r = esp_aes_process_dma(ctx, input, output, length, stream_block);

        if (r != 0) {
            esp_aes_release_hardware();
            return r;
        }

        aes_hal_read_iv(nonce_counter);

        esp_aes_release_hardware();

    }
    *nc_off = n + (length % AES_BLOCK_BYTES);

    return 0;
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
