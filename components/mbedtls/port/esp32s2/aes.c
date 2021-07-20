/**
 * \brief AES block cipher, ESP32-S2 hardware accelerated version
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

#include <stdio.h>
#include <string.h>
#include <sys/lock.h>
#include "mbedtls/aes.h"
#include "esp32s2/aes.h"
#include "esp32s2/gcm.h"
#include "soc/cpu.h"
#include "soc/dport_reg.h"
#include "soc/hwcrypto_reg.h"
#include "soc/crypto_dma_reg.h"
#include "soc/periph_defs.h"
#include "esp32s2/rom/lldesc.h"
#include "esp32s2/rom/cache.h"
#include "esp_intr_alloc.h"
#include "driver/periph_ctrl.h"
#include "esp_log.h"
#include "soc/lldesc.h"
#include "esp_heap_caps.h"
#include "sys/param.h"
#include "esp_pm.h"
#include "esp_crypto_lock.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define AES_BLOCK_BYTES 16
#define IV_WORDS        4

/* Max size of each chunk to process when output buffer is in unaligned external ram
   must be a multiple of block size
*/
#define AES_MAX_CHUNK_WRITE_SIZE 1600

/* Input over this length will yield and wait for interrupt instead of
   busy-waiting, 30000 bytes is approx 0.5 ms */
#define AES_DMA_INTR_TRIG_LEN 2000

#define ESP_PUT_BE64(a, val)                                    \
    do {                                                        \
        *(uint64_t*)(a) = __builtin_bswap64( (uint64_t)(val) ); \
    } while (0)


/* DMA AES working modes*/
typedef enum {
    ESP_AES_BLOCK_MODE_ECB = 0,
    ESP_AES_BLOCK_MODE_CBC,
    ESP_AES_BLOCK_MODE_OFB,
    ESP_AES_BLOCK_MODE_CTR,
    ESP_AES_BLOCK_MODE_CFB8,
    ESP_AES_BLOCK_MODE_CFB128,
    ESP_AES_BLOCK_MODE_GCM,
} esp_aes_mode_t;


#if defined(CONFIG_MBEDTLS_AES_USE_INTERRUPT)
static SemaphoreHandle_t op_complete_sem;
#if defined(CONFIG_PM_ENABLE)
static esp_pm_lock_handle_t s_pm_cpu_lock;
static esp_pm_lock_handle_t s_pm_sleep_lock;
#endif
#endif

static const char *TAG = "esp-aes";

static inline bool valid_key_length(const esp_aes_context *ctx)
{
    return ctx->key_bytes == 128 / 8 || ctx->key_bytes == 192 / 8 || ctx->key_bytes == 256 / 8;
}


void esp_aes_acquire_hardware( void )
{
    /* Need to lock DMA since it is shared with SHA block */
    esp_crypto_dma_lock_acquire();

    /* Enable AES hardware */
    periph_module_enable(PERIPH_AES_DMA_MODULE);
}

/* Function to disable AES and Crypto DMA clocks and release locks */
void esp_aes_release_hardware( void )
{
    /* Disable AES hardware */
    periph_module_disable(PERIPH_AES_DMA_MODULE);

    esp_crypto_dma_lock_release();
}


/* Function to init AES context to zero */
void esp_aes_init( esp_aes_context *ctx )
{
    if ( ctx == NULL ) {
        return;
    }

    bzero( ctx, sizeof( esp_aes_context ) );
}

/* Function to clear AES context */
void esp_aes_free( esp_aes_context *ctx )
{
    if ( ctx == NULL ) {
        return;
    }

    bzero( ctx, sizeof( esp_aes_context ) );
}

/*
 * AES key schedule (same for encryption or decryption, as hardware handles schedule)
 *
 */
int esp_aes_setkey( esp_aes_context *ctx, const unsigned char *key,
                    unsigned int keybits )
{
    if (keybits != 128 && keybits != 192 && keybits != 256) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }
    ctx->key_bytes = keybits / 8;
    memcpy(ctx->key, key, ctx->key_bytes);
    ctx->key_in_hardware = 0;
    return 0;
}

/*
 * Helper function to copy key from esp_aes_context buffer
 * to hardware key registers.
 *
 * Call only while holding esp_aes_acquire_hardware().
 */
static void esp_aes_setkey_hardware( esp_aes_context *ctx, int crypt_mode)
{
    const uint32_t MODE_DECRYPT_BIT = 4;
    uint32_t key_word;
    unsigned mode_reg_base = (crypt_mode == ESP_AES_ENCRYPT) ? 0 : MODE_DECRYPT_BIT;

    ctx->key_in_hardware = 0;
    /* Memcpy to avoid potential unaligned access */
    for (int i = 0; i < ctx->key_bytes / 4; ++i) {
        memcpy(&key_word, ctx->key + 4 * i, 4);
        REG_WRITE(AES_KEY_BASE + i * 4,  key_word);
        ctx->key_in_hardware += 4;
    }

    REG_WRITE(AES_MODE_REG, mode_reg_base + ((ctx->key_bytes / 8) - 2));

    /* Fault injection check: all words of key data should have been written to hardware */
    if (ctx->key_in_hardware < 16
            || ctx->key_in_hardware != ctx->key_bytes) {
        abort();
    }
}

/*
 * Sets the AES DMA block mode (ECB, CBC, CFB, OFB, GCM, CTR)
 * and intializes the required registers for that working mode
 */
static inline void esp_aes_mode_init(esp_aes_mode_t mode)
{
    /* Set the algorithm mode CBC, CFB ... */
    REG_WRITE(AES_BLOCK_MODE_REG, mode);

    /* Presently hard-coding the INC function to 32 bit */
    if (mode == ESP_AES_BLOCK_MODE_CTR) {
        REG_WRITE(AES_INC_SEL_REG, 0);
    }
}

/*
 * Write IV to hardware iv registers
 */
static inline void esp_aes_set_iv(uint8_t *iv)
{
    uint32_t *reg_addr_buf = (uint32_t *)(AES_IV_BASE);
    uint32_t iv_word;

    for (int i = 0; i<IV_WORDS; i++ ) {
        /* Memcpy to avoid potential unaligned access */
        memcpy(&iv_word, iv + 4 * i, sizeof(iv_word));
        REG_WRITE(&reg_addr_buf[i], iv_word);
    }
}

/*
 * Read IV from hardware iv registers
 */
static inline void esp_aes_get_iv(uint8_t *iv)
{
    esp_dport_access_read_buffer((uint32_t*)iv, AES_IV_BASE, IV_WORDS);
}


#if defined (CONFIG_MBEDTLS_AES_USE_INTERRUPT)
static IRAM_ATTR void esp_aes_complete_isr(void *arg)
{
    BaseType_t higher_woken;
    REG_WRITE(AES_INT_CLR_REG, 1);
    xSemaphoreGiveFromISR(op_complete_sem, &higher_woken);
    if (higher_woken) {
        portYIELD_FROM_ISR();
    }
}

static esp_err_t esp_aes_isr_initialise( void )
{
    REG_WRITE(AES_INT_CLR_REG, 1);
    REG_WRITE(AES_INT_ENA_REG, 1);
    if (op_complete_sem == NULL) {
        op_complete_sem = xSemaphoreCreateBinary();

        if (op_complete_sem == NULL) {
            ESP_LOGE(TAG, "Failed to create intr semaphore");
            return ESP_FAIL;
        }

        esp_intr_alloc(ETS_AES_INTR_SOURCE, 0, esp_aes_complete_isr, NULL, NULL);
    }

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
static void esp_aes_dma_wait_complete(bool use_intr, lldesc_t *output_desc)
{
    volatile uint32_t dma_done;

#if defined (CONFIG_MBEDTLS_AES_USE_INTERRUPT)
    if (use_intr) {
        if (!xSemaphoreTake(op_complete_sem, 2000 / portTICK_PERIOD_MS)) {
            /* indicates a fundamental problem with driver */
            ESP_LOGE("AES", "Timed out waiting for completion of AES Interrupt");
            abort();
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
    while (REG_READ(AES_STATE_REG) != AES_STATE_DONE) {
    }


    /* Wait for DMA write operation to complete */
    while (1) {
        dma_done = REG_READ(CRYPTO_DMA_INT_RAW_REG);
        // Wait for ownership of buffer to be transferred back to CPU
        if ( ((dma_done & INT_RAW_IN_SUC_EOF) == INT_RAW_IN_SUC_EOF) && (output_desc->owner == 0) ) {
            break;
        }
    }
}

/* Init DMA related registers for AES operation */
static void esp_aes_dma_init(lldesc_t *input, lldesc_t *output)
{
    /* Enable DMA mode */
    REG_WRITE(AES_DMA_ENABLE_REG, 1);

    /* Reset DMA */
    SET_PERI_REG_MASK(CRYPTO_DMA_CONF0_REG, CONF0_REG_AHBM_RST | CONF0_REG_IN_RST | CONF0_REG_OUT_RST | CONF0_REG_AHBM_FIFO_RST);
    CLEAR_PERI_REG_MASK(CRYPTO_DMA_CONF0_REG, CONF0_REG_AHBM_RST | CONF0_REG_IN_RST | CONF0_REG_OUT_RST | CONF0_REG_AHBM_FIFO_RST);

    /* Set DMA for AES Use */
    REG_WRITE(CRYPTO_DMA_AES_SHA_SELECT_REG, 0);

    /* Set descriptors */
    CLEAR_PERI_REG_MASK(CRYPTO_DMA_OUT_LINK_REG, OUT_LINK_REG_OUTLINK_ADDR);
    SET_PERI_REG_MASK(CRYPTO_DMA_OUT_LINK_REG, ((uint32_t)(input))&OUT_LINK_REG_OUTLINK_ADDR);
    CLEAR_PERI_REG_MASK(CRYPTO_DMA_IN_LINK_REG, IN_LINK_REG_INLINK_ADDR);
    SET_PERI_REG_MASK(CRYPTO_DMA_IN_LINK_REG, ((uint32_t)(output))&IN_LINK_REG_INLINK_ADDR);

    /* Start transfer */
    SET_PERI_REG_MASK(CRYPTO_DMA_OUT_LINK_REG, OUT_LINK_REG_OUTLINK_START);
    SET_PERI_REG_MASK(CRYPTO_DMA_IN_LINK_REG, IN_LINK_REG_INLINK_START);
}

static int esp_aes_process_dma(esp_aes_context *ctx, const unsigned char *input, unsigned char *output, size_t len, uint8_t *stream_out);


/* Output buffers in external ram needs to be 16-byte aligned and DMA cant access input in the iCache mem range,
   reallocate them into internal memory and encrypt in chunks to avoid
   having to malloc too big of a buffer
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
            ESP_LOGE(TAG, "Failed to allocate memory");
            ret = -1;
            goto cleanup;
        }
    }

    if (realloc_output) {
        output_buf = heap_caps_malloc(chunk_len, MALLOC_CAP_DMA);

        if (output_buf == NULL) {
            ESP_LOGE(TAG, "Failed to allocate memory");
            ret = -1;
            goto cleanup;
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

/* Encrypt/decrypt the input using DMA */
static int esp_aes_process_dma(esp_aes_context *ctx, const unsigned char *input, unsigned char *output, size_t len, uint8_t *stream_out)
{
    lldesc_t stream_in_desc, stream_out_desc;
    lldesc_t *in_desc_head, *out_desc_head;
    lldesc_t *out_desc_tail = NULL; /* pointer to the final output descriptor */
    lldesc_t *block_desc = NULL, *block_in_desc = NULL, *block_out_desc = NULL;
    size_t lldesc_num;
    uint8_t stream_in[16] = {};
    unsigned stream_bytes = len % AES_BLOCK_BYTES; // bytes which aren't in a full block
    unsigned block_bytes = len - stream_bytes;     // bytes which are in a full block
    unsigned char *non_icache_input = NULL;
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
        bzero(output, len);
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }

    if (block_bytes > 0) {
        /* Flush cache if input in external ram */
#if (CONFIG_SPIRAM_USE_CAPS_ALLOC || CONFIG_SPIRAM_USE_MALLOC)
        if (esp_ptr_external_ram(input)) {
            Cache_WriteBack_All();
        }
        if (esp_ptr_external_ram(output)) {
            if (((intptr_t)(output) & 0xF) != 0) {
                // Non aligned ext-mem buffer
                output_needs_realloc = true;
            }
        }
#endif
        /* DMA cannot access memory in the iCache range, copy input to internal ram */
        if (!esp_ptr_dma_ext_capable(input) && !esp_ptr_dma_capable(input)) {
            input_needs_realloc = true;
        }

        if (!esp_ptr_dma_ext_capable(output) && !esp_ptr_dma_capable(output)) {
            output_needs_realloc = true;
        }

        /* If either input or output is unaccessible to the DMA then they need to be reallocated */
        if (input_needs_realloc || output_needs_realloc) {
            return esp_aes_process_dma_ext_ram(ctx, input, output, len, stream_out, input_needs_realloc, output_needs_realloc);
        }


        /* Set up dma descriptors for input and output */
        lldesc_num = lldesc_get_required_num(block_bytes);

        /* Allocate both in and out descriptors to save a malloc/free per function call */
        block_desc = heap_caps_malloc(sizeof(lldesc_t) * lldesc_num * 2, MALLOC_CAP_DMA);
        if (block_desc == NULL) {
            ESP_LOGE(TAG, "Failed to allocate memory");
            ret = -1;
            goto cleanup;
        }

        block_in_desc = block_desc;
        block_out_desc = block_desc + lldesc_num;

        lldesc_setup_link(block_in_desc, input, block_bytes, 0);
        lldesc_setup_link(block_out_desc, output, block_bytes, 0);

        out_desc_tail = &block_out_desc[lldesc_num - 1];
    }

    /* Any leftover bytes which are appended as an additional DMA list */
    if (stream_bytes > 0) {
        memcpy(stream_in, input + block_bytes, stream_bytes);

        lldesc_setup_link(&stream_in_desc, stream_in, AES_BLOCK_BYTES, 0);
        lldesc_setup_link(&stream_out_desc, stream_out, AES_BLOCK_BYTES, 0);

        if (block_bytes > 0) {
            /* Link with block descriptors*/
            block_in_desc[lldesc_num - 1].empty = (uint32_t)&stream_in_desc;
            block_out_desc[lldesc_num - 1].empty = (uint32_t)&stream_out_desc;
        }

        out_desc_tail = &stream_out_desc;
    }

    // block buffers are sent to DMA first, unless there aren't any
    in_desc_head =  (block_bytes > 0) ? block_in_desc : &stream_in_desc;
    out_desc_head = (block_bytes > 0) ? block_out_desc : &stream_out_desc;

    esp_aes_dma_init(in_desc_head, out_desc_head);

    /* Write the number of blocks */
    REG_WRITE(AES_BLOCK_NUM_REG, blocks);


#if defined (CONFIG_MBEDTLS_AES_USE_INTERRUPT)
    /* Only use interrupt for long AES operations */
    if (len > AES_DMA_INTR_TRIG_LEN) {
        use_intr = true;
        if (esp_aes_isr_initialise() == ESP_FAIL) {
            ret = -1;
            goto cleanup;
        }
    } else
#endif
    {
        REG_WRITE(AES_INT_ENA_REG, 0);
    }

    /* Start AES operation */
    REG_WRITE(AES_TRIGGER_REG, 1);
    esp_aes_dma_wait_complete(use_intr, out_desc_tail);



#if (CONFIG_SPIRAM_USE_CAPS_ALLOC || CONFIG_SPIRAM_USE_MALLOC)
    if (block_bytes > 0) {
        if (esp_ptr_external_ram(output)) {
            Cache_Invalidate_DCache_All();
        }
    }
#endif

    REG_WRITE(AES_DMA_EXIT_REG, 0);
    /* Disable DMA mode */
    REG_WRITE(AES_DMA_ENABLE_REG, 0);

    if (stream_bytes > 0) {
        memcpy(output + block_bytes, stream_out, stream_bytes);
    }

cleanup:
    free(non_icache_input);
    free(block_desc);
    return ret;
}


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
int esp_internal_aes_encrypt( esp_aes_context *ctx,
                              const unsigned char input[16],
                              unsigned char output[16] )
{
    int r;

    if (esp_aes_validate_input(ctx, input, output)) {
        return -1;
    }

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    esp_aes_acquire_hardware();
    ctx->key_in_hardware = 0;
    esp_aes_setkey_hardware(ctx, ESP_AES_ENCRYPT);
    esp_aes_mode_init(ESP_AES_BLOCK_MODE_ECB);
    r = esp_aes_process_dma(ctx, input, output, AES_BLOCK_BYTES, NULL);
    esp_aes_release_hardware();

    return r;
}

void esp_aes_encrypt( esp_aes_context *ctx,
                      const unsigned char input[16],
                      unsigned char output[16] )
{
    esp_internal_aes_encrypt(ctx, input, output);
}

/*
 * AES-ECB single block decryption
 */
int esp_internal_aes_decrypt( esp_aes_context *ctx,
                              const unsigned char input[16],
                              unsigned char output[16] )
{
    int r;

    if (esp_aes_validate_input(ctx, input, output)) {
        return -1;
    }

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    esp_aes_acquire_hardware();
    ctx->key_in_hardware = 0;
    esp_aes_setkey_hardware(ctx, ESP_AES_DECRYPT);
    esp_aes_mode_init(ESP_AES_BLOCK_MODE_ECB);
    r = esp_aes_process_dma(ctx, input, output, AES_BLOCK_BYTES, NULL);
    esp_aes_release_hardware();

    return r;
}

void esp_aes_decrypt( esp_aes_context *ctx,
                      const unsigned char input[16],
                      unsigned char output[16] )
{
    esp_internal_aes_decrypt(ctx, input, output);
}


/*
 * AES-ECB block encryption/decryption
 */
int esp_aes_crypt_ecb( esp_aes_context *ctx,
                       int mode,
                       const unsigned char input[16],
                       unsigned char output[16] )
{
    int r;

    if (esp_aes_validate_input(ctx, input, output)) {
        return -1;
    }

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    esp_aes_acquire_hardware();
    ctx->key_in_hardware = 0;
    esp_aes_setkey_hardware(ctx, mode);
    esp_aes_mode_init(ESP_AES_BLOCK_MODE_ECB);
    r = esp_aes_process_dma(ctx, input, output, AES_BLOCK_BYTES, NULL);
    esp_aes_release_hardware();

    return r;
}

/*
 * AES-CBC buffer encryption/decryption
 */
int esp_aes_crypt_cbc( esp_aes_context *ctx,
                       int mode,
                       size_t length,
                       unsigned char iv[16],
                       const unsigned char *input,
                       unsigned char *output )
{
    int r = 0;
    if (esp_aes_validate_input(ctx, input, output)) {
        return -1;
    }

    if (!iv) {
        ESP_LOGE(TAG, "No IV supplied");
        return -1;
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
    esp_aes_setkey_hardware(ctx, mode);
    esp_aes_mode_init(ESP_AES_BLOCK_MODE_CBC);
    esp_aes_set_iv(iv);

    r = esp_aes_process_dma(ctx, input, output, length, NULL);
    if (r != 0) {
        esp_aes_release_hardware();
        return r;
    }

    esp_aes_get_iv(iv);
    esp_aes_release_hardware();

    return r;
}

/*
 * AES-CFB8 buffer encryption/decryption
 */
int esp_aes_crypt_cfb8( esp_aes_context *ctx,
                        int mode,
                        size_t length,
                        unsigned char iv[16],
                        const unsigned char *input,
                        unsigned char *output )
{
    unsigned char c;
    unsigned char ov[17];
    int r = 0;
    size_t block_bytes = length - (length % AES_BLOCK_BYTES);

    if (esp_aes_validate_input(ctx, input, output)) {
        return -1;
    }

    if (!iv) {
        ESP_LOGE(TAG, "No IV supplied");
        return -1;
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
        esp_aes_setkey_hardware(ctx, mode);
        esp_aes_mode_init(ESP_AES_BLOCK_MODE_CFB8);
        esp_aes_set_iv(iv);
        r = esp_aes_process_dma(ctx, input, output, block_bytes, NULL);
        esp_aes_get_iv(iv);

        if (r != 0) {
            esp_aes_release_hardware();
            return r;
        }

        length -= block_bytes;
        input += block_bytes;
        output += block_bytes;
    }

    // Process remaining bytes block-at-a-time in ECB mode
    if (length > 0) {
        ctx->key_in_hardware = 0;
        esp_aes_setkey_hardware(ctx, MBEDTLS_AES_ENCRYPT);
        esp_aes_mode_init(ESP_AES_BLOCK_MODE_ECB);

        while ( length-- ) {
            memcpy( ov, iv, 16 );

            r = esp_aes_process_dma(ctx, iv, iv, AES_BLOCK_BYTES, NULL);
            if (r != 0) {
                esp_aes_release_hardware();
                return r;
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
    esp_aes_release_hardware();

    return r;
}

/*
 * AES-CFB128 buffer encryption/decryption
 */
int esp_aes_crypt_cfb128( esp_aes_context *ctx,
                          int mode,
                          size_t length,
                          size_t *iv_off,
                          unsigned char iv[16],
                          const unsigned char *input,
                          unsigned char *output )

{
    uint8_t c;
    int r = 0;
    size_t stream_bytes = 0;
    size_t n;

    if (esp_aes_validate_input(ctx, input, output)) {
        return -1;
    }

    if (!iv) {
        ESP_LOGE(TAG, "No IV supplied");
        return -1;
    }

    if (!iv_off) {
        ESP_LOGE(TAG, "No IV offset supplied");
        return -1;
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
        esp_aes_setkey_hardware(ctx, mode);
        esp_aes_mode_init(ESP_AES_BLOCK_MODE_CFB128);
        esp_aes_set_iv(iv);

        r = esp_aes_process_dma(ctx, input, output, length, iv);
        if (r != 0) {
            esp_aes_release_hardware();
            return r;
        }

        if (stream_bytes == 0) {
            // if we didn't need the partial 'stream block' then the new IV is in the IV register
            esp_aes_get_iv(iv);
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
    return r;
}

/*
 * AES-OFB (Output Feedback Mode) buffer encryption/decryption
 */

int esp_aes_crypt_ofb( esp_aes_context *ctx,
                       size_t length,
                       size_t *iv_off,
                       unsigned char iv[16],
                       const unsigned char *input,
                       unsigned char *output )
{
    int r = 0;
    size_t n;
    size_t stream_bytes = 0;

    if (esp_aes_validate_input(ctx, input, output)) {
        return -1;
    }

    if (!iv) {
        ESP_LOGE(TAG, "No IV supplied");
        return -1;
    }

    if (!iv_off) {
        ESP_LOGE(TAG, "No IV offset supplied");
        return -1;
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
        esp_aes_setkey_hardware(ctx, ESP_AES_DECRYPT);
        esp_aes_mode_init(ESP_AES_BLOCK_MODE_OFB);
        esp_aes_set_iv(iv);

        r = esp_aes_process_dma(ctx, input, output, length, iv);
        if (r != 0) {
            esp_aes_release_hardware();
            return r;
        }

        esp_aes_get_iv(iv);
        esp_aes_release_hardware();
    }

    *iv_off = n + stream_bytes;

    return r;
}

/*
 * AES-CTR buffer encryption/decryption
 */
int esp_aes_crypt_ctr( esp_aes_context *ctx,
                       size_t length,
                       size_t *nc_off,
                       unsigned char nonce_counter[16],
                       unsigned char stream_block[16],
                       const unsigned char *input,
                       unsigned char *output )
{
    int r = 0;
    size_t n;

    if (esp_aes_validate_input(ctx, input, output)) {
        return -1;
    }

    if (!nonce_counter) {
        ESP_LOGE(TAG, "No nonce supplied");
        return -1;
    }

    if (!nc_off) {
        ESP_LOGE(TAG, "No nonce offset supplied");
        return -1;
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
        esp_aes_setkey_hardware(ctx, ESP_AES_DECRYPT);

        esp_aes_mode_init(ESP_AES_BLOCK_MODE_CTR);
        esp_aes_set_iv(nonce_counter);

        r = esp_aes_process_dma(ctx, input, output, length, stream_block);

        if (r != 0) {
            esp_aes_release_hardware();
            return r;
        }

        esp_aes_get_iv(nonce_counter);

        esp_aes_release_hardware();

    }
    *nc_off = n + (length % AES_BLOCK_BYTES);

    return r;
}

static void esp_gcm_ghash(esp_gcm_context *ctx, const unsigned char *x, size_t x_len, uint8_t *z);

/*
 * Calculates the Initial Counter Block, J0
 * and copies to to the esp_gcm_context
 */
static void esp_gcm_derive_J0(esp_gcm_context *ctx)
{
    uint8_t len_buf[16];

    memset(ctx->J0, 0, AES_BLOCK_BYTES);
    memset(len_buf, 0, AES_BLOCK_BYTES);

    /* If IV is 96 bits J0 = ( IV || 0^31 || 1 ) */
    if (ctx->iv_len == 12) {
        memcpy(ctx->J0, ctx->iv, ctx->iv_len);
        ctx->J0[AES_BLOCK_BYTES - 1] |= 1;
    } else {
   /* For IV != 96 bit, J0 = GHASH(IV || 0[s+64] || [len(IV)]64) */
        /* First calculate GHASH on IV */
        esp_gcm_ghash(ctx, ctx->iv, ctx->iv_len, ctx->J0);
        /* Next create 128 bit block which is equal to
        64 bit 0 + iv length truncated to 64 bits */
        ESP_PUT_BE64(len_buf + 8, ctx->iv_len * 8);
        /*   Calculate GHASH on last block */
        esp_gcm_ghash(ctx, len_buf, 16, ctx->J0);


    }
}


/*
 * Increment J0 as per GCM spec, by applying the Standard Incrementing
   Function INC_32 to it.
 * j is the counter which needs to be incremented which is
 * copied to ctx->J0 after incrementing
 */
static void increment32_j0(esp_gcm_context *ctx, uint8_t *j)
{
    uint8_t j_len = AES_BLOCK_BYTES;
    memcpy(j, ctx->J0, AES_BLOCK_BYTES);
    if (j) {
        for (uint32_t i = j_len; i > (j_len - 4); i--) {
            if (++j[i - 1] != 0) {
                break;
            }
        }
        memcpy(ctx->J0, j, AES_BLOCK_BYTES);
    }
}

/* Function to xor two data blocks */
static void xor_data(uint8_t *d, const uint8_t *s)
{
    for (int i = 0; i < AES_BLOCK_BYTES; i++) {
        d[i] ^= s[i];
    }
}


/*
 * 32-bit integer manipulation macros (big endian)
 */
#ifndef GET_UINT32_BE
#define GET_UINT32_BE(n,b,i)                            \
{                                                       \
    (n) = ( (uint32_t) (b)[(i)    ] << 24 )             \
        | ( (uint32_t) (b)[(i) + 1] << 16 )             \
        | ( (uint32_t) (b)[(i) + 2] <<  8 )             \
        | ( (uint32_t) (b)[(i) + 3]       );            \
}
#endif

#ifndef PUT_UINT32_BE
#define PUT_UINT32_BE(n,b,i)                            \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
}
#endif

/* Based on MbedTLS's implemenation
 *
 * Precompute small multiples of H, that is set
 *      HH[i] || HL[i] = H times i,
 * where i is seen as a field element as in [MGV], ie high-order bits
 * correspond to low powers of P. The result is stored in the same way, that
 * is the high-order bit of HH corresponds to P^0 and the low-order bit of HL
 * corresponds to P^127.
 */
static int gcm_gen_table( esp_gcm_context *ctx )
{
    int i, j;
    uint64_t hi, lo;
    uint64_t vl, vh;
    unsigned char *h;

    h = ctx->H;

    /* pack h as two 64-bits ints, big-endian */
    GET_UINT32_BE( hi, h,  0  );
    GET_UINT32_BE( lo, h,  4  );
    vh = (uint64_t) hi << 32 | lo;

    GET_UINT32_BE( hi, h,  8  );
    GET_UINT32_BE( lo, h,  12 );
    vl = (uint64_t) hi << 32 | lo;

    /* 8 = 1000 corresponds to 1 in GF(2^128) */
    ctx->HL[8] = vl;
    ctx->HH[8] = vh;

    /* 0 corresponds to 0 in GF(2^128) */
    ctx->HH[0] = 0;
    ctx->HL[0] = 0;

    for( i = 4; i > 0; i >>= 1 )
    {
        uint32_t T = ( vl & 1 ) * 0xe1000000U;
        vl  = ( vh << 63 ) | ( vl >> 1 );
        vh  = ( vh >> 1 ) ^ ( (uint64_t) T << 32);

        ctx->HL[i] = vl;
        ctx->HH[i] = vh;
    }

    for( i = 2; i <= 8; i *= 2 )
    {
        uint64_t *HiL = ctx->HL + i, *HiH = ctx->HH + i;
        vh = *HiH;
        vl = *HiL;
        for( j = 1; j < i; j++ )
        {
            HiH[j] = vh ^ ctx->HH[j];
            HiL[j] = vl ^ ctx->HL[j];
        }
    }

    return( 0 );
}
/*
 * Shoup's method for multiplication use this table with
 *      last4[x] = x times P^128
 * where x and last4[x] are seen as elements of GF(2^128) as in [MGV]
 */
static const uint64_t last4[16] =
{
    0x0000, 0x1c20, 0x3840, 0x2460,
    0x7080, 0x6ca0, 0x48c0, 0x54e0,
    0xe100, 0xfd20, 0xd940, 0xc560,
    0x9180, 0x8da0, 0xa9c0, 0xb5e0
};
/* Based on MbedTLS's implemenation
 *
 * Sets output to x times H using the precomputed tables.
 * x and output are seen as elements of GF(2^128) as in [MGV].
 */
static void gcm_mult( esp_gcm_context *ctx, const unsigned char x[16],
                      unsigned char output[16] )
{
    int i = 0;
    unsigned char lo, hi, rem;
    uint64_t zh, zl;

    lo = x[15] & 0xf;

    zh = ctx->HH[lo];
    zl = ctx->HL[lo];

    for( i = 15; i >= 0; i-- )
    {
        lo = x[i] & 0xf;
        hi = x[i] >> 4;

        if( i != 15 )
        {
            rem = (unsigned char) zl & 0xf;
            zl = ( zh << 60 ) | ( zl >> 4 );
            zh = ( zh >> 4 );
            zh ^= (uint64_t) last4[rem] << 48;
            zh ^= ctx->HH[lo];
            zl ^= ctx->HL[lo];

        }

        rem = (unsigned char) zl & 0xf;
        zl = ( zh << 60 ) | ( zl >> 4 );
        zh = ( zh >> 4 );
        zh ^= (uint64_t) last4[rem] << 48;
        zh ^= ctx->HH[hi];
        zl ^= ctx->HL[hi];
    }

    PUT_UINT32_BE( zh >> 32, output, 0 );
    PUT_UINT32_BE( zh, output, 4 );
    PUT_UINT32_BE( zl >> 32, output, 8 );
    PUT_UINT32_BE( zl, output, 12 );
}



/* Update the key value in gcm context */
int esp_aes_gcm_setkey( esp_gcm_context *ctx,
                        mbedtls_cipher_id_t cipher,
                        const unsigned char *key,
                        unsigned int keybits )
{
    if (keybits != 128 && keybits != 192 && keybits != 256) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    ctx->aes_ctx.key_bytes = keybits / 8;

    memcpy(ctx->aes_ctx.key, key, ctx->aes_ctx.key_bytes);

    return ( 0 );
}


/* AES-GCM GHASH calculation z = GHASH(x) using h0 hash key
*/
static void esp_gcm_ghash(esp_gcm_context *ctx, const unsigned char *x, size_t x_len, uint8_t *z)
{

    uint8_t tmp[AES_BLOCK_BYTES];

    memset(tmp, 0, AES_BLOCK_BYTES);
    /* GHASH(X) is calculated on input string which is multiple of 128 bits
     * If input string bit length is not multiple of 128 bits it needs to
     * be padded by 0
     *
     * Steps:
     * 1. Let X1, X2, ... , Xm-1, Xm denote the unique sequence of blocks such
     * that X = X1 || X2 || ... || Xm-1 || Xm.
     * 2. Let Y0 be the “zero block,” 0128.
     * 3. Fori=1,...,m,letYi =(Yi-1 ^ Xi)•H.
     * 4. Return Ym
     */

    /* If input bit string is >= 128 bits, process full 128 bit blocks */
    while (x_len >= AES_BLOCK_BYTES) {

        xor_data(z, x);
        gcm_mult(ctx, z, z);

        x += AES_BLOCK_BYTES;
        x_len -= AES_BLOCK_BYTES;
    }

    /* If input bit string is not multiple of 128 create last 128 bit
     * block by padding necessary 0s
     */
    if (x_len) {
        memcpy(tmp, x, x_len);
        xor_data(z, tmp);
        gcm_mult(ctx, z, z);
    }
}


/* Function to init AES GCM context to zero */
void esp_aes_gcm_init( esp_gcm_context *ctx)
{
    if (ctx == NULL) {
        return;
    }

    bzero(ctx, sizeof(esp_gcm_context));

    ctx->gcm_state = ESP_AES_GCM_STATE_INIT;
}

/* Function to clear AES-GCM context */
void esp_aes_gcm_free( esp_gcm_context *ctx)
{
    if (ctx == NULL) {
        return;
    }
    bzero(ctx, sizeof(esp_gcm_context));
}

/* Setup AES-GCM */
int esp_aes_gcm_starts( esp_gcm_context *ctx,
                        int mode,
                        const unsigned char *iv,
                        size_t iv_len,
                        const unsigned char *aad,
                        size_t aad_len )
{
    /* IV and AD are limited to 2^64 bits, so 2^61 bytes */
    /* IV is not allowed to be zero length */
    if ( iv_len == 0 ||
            ( (uint64_t) iv_len  ) >> 61 != 0 ||
            ( (uint64_t) aad_len ) >> 61 != 0 ) {
        return ( MBEDTLS_ERR_GCM_BAD_INPUT );
    }

    if (!ctx) {
        ESP_LOGE(TAG, "No AES context supplied");
        return -1;
    }

    if (!iv) {
        ESP_LOGE(TAG, "No IV supplied");
        return -1;
    }

    if ( (aad_len > 0) && !aad) {
        ESP_LOGE(TAG, "No aad supplied");
        return -1;
    }

    /* Initialize AES-GCM context */
    memset(ctx->ghash, 0, sizeof(ctx->ghash));
    ctx->data_len = 0;

    ctx->iv = iv;
    ctx->iv_len = iv_len;
    ctx->aad = aad;
    ctx->aad_len = aad_len;
    ctx->mode = mode;

    /* H and the lookup table are only generated once per ctx */
    if (ctx->gcm_state == ESP_AES_GCM_STATE_INIT) {
        /* Lock the AES engine to calculate ghash key H in hardware */
        esp_aes_acquire_hardware();
        esp_aes_setkey_hardware( &ctx->aes_ctx, mode);
        esp_aes_mode_init(ESP_AES_BLOCK_MODE_GCM);
        /* Enable DMA mode */
        REG_WRITE(AES_DMA_ENABLE_REG, 1);
        REG_WRITE(AES_TRIGGER_REG, 1);
        while (REG_READ(AES_STATE_REG) != AES_STATE_IDLE);

        memcpy(ctx->H, (uint8_t *)AES_H_BASE, AES_BLOCK_BYTES);

        esp_aes_release_hardware();

        gcm_gen_table(ctx);
    }

    ctx->gcm_state = ESP_AES_GCM_STATE_START;

    /* Once H is obtained we need to derive J0 (Initial Counter Block) */
    esp_gcm_derive_J0(ctx);

    /* The initial counter block keeps updating during the esp_gcm_update call
     * however to calculate final authentication tag T we need original J0
     * so we make a copy here
     */
    memcpy(ctx->ori_j0, ctx->J0, 16);

    esp_gcm_ghash(ctx, ctx->aad, ctx->aad_len, ctx->ghash);

    return ( 0 );
}

/* Perform AES-GCM operation */
int esp_aes_gcm_update( esp_gcm_context *ctx,
                        size_t length,
                        const unsigned char *input,
                        unsigned char *output )
{
    size_t nc_off = 0;
    uint8_t nonce_counter[AES_BLOCK_BYTES] = {0};
    uint8_t stream[AES_BLOCK_BYTES] = {0};

    if (!ctx) {
        ESP_LOGE(TAG, "No GCM context supplied");
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

    if ( output > input && (size_t) ( output - input ) < length ) {
        return ( MBEDTLS_ERR_GCM_BAD_INPUT );
    }
    /* If this is the first time esp_gcm_update is getting called
     * calculate GHASH on aad and preincrement the ICB
     */
    if (ctx->gcm_state == ESP_AES_GCM_STATE_START) {
        /* Jo needs to be incremented first time, later the CTR
         * operation will auto update it
         */
        increment32_j0(ctx, nonce_counter);
        ctx->gcm_state = ESP_AES_GCM_STATE_UPDATE;
    } else if (ctx->gcm_state == ESP_AES_GCM_STATE_UPDATE) {
        memcpy(nonce_counter, ctx->J0, AES_BLOCK_BYTES);
    }

    /* Perform intermediate GHASH on "encrypted" data during decryption */
    if (ctx->mode == ESP_AES_DECRYPT) {
        esp_gcm_ghash(ctx, input, length, ctx->ghash);
    }

    /* Output = GCTR(J0, Input): Encrypt/Decrypt the input */
    esp_aes_crypt_ctr(&ctx->aes_ctx, length, &nc_off, nonce_counter, stream, input, output);

    /* ICB gets auto incremented after GCTR operation here so update the context */
    memcpy(ctx->J0, nonce_counter, AES_BLOCK_BYTES);

    /* Keep updating the length counter for final tag calculation */
    ctx->data_len += length;

    /* Perform intermediate GHASH on "encrypted" data during encryption*/
    if (ctx->mode == ESP_AES_ENCRYPT) {
        esp_gcm_ghash(ctx, output, length, ctx->ghash);
    }

    return 0;
}

/* Function to read the tag value */
int esp_aes_gcm_finish( esp_gcm_context *ctx,
                        unsigned char *tag,
                        size_t tag_len )
{
    size_t nc_off = 0;
    uint8_t len_block[AES_BLOCK_BYTES] = {0};

    if ( tag_len > 16 || tag_len < 4 ) {
        return ( MBEDTLS_ERR_GCM_BAD_INPUT );
    }

    /* Calculate final GHASH on aad_len, data length */
    ESP_PUT_BE64(len_block, ctx->aad_len * 8);
    ESP_PUT_BE64(len_block + 8, ctx->data_len * 8);
    esp_gcm_ghash(ctx, len_block, AES_BLOCK_BYTES, ctx->ghash);

    /* Tag T = GCTR(J0, ) where T is truncated to tag_len */
    esp_aes_crypt_ctr(&ctx->aes_ctx, tag_len, &nc_off, ctx->ori_j0, 0, ctx->ghash, tag);

    return 0;
}

int esp_aes_gcm_crypt_and_tag( esp_gcm_context *ctx,
                               int mode,
                               size_t length,
                               const unsigned char *iv,
                               size_t iv_len,
                               const unsigned char *add,
                               size_t add_len,
                               const unsigned char *input,
                               unsigned char *output,
                               size_t tag_len,
                               unsigned char *tag )
{
    int ret;


    if ( ( ret = esp_aes_gcm_starts( ctx, mode, iv, iv_len, add, add_len ) ) != 0 ) {
        return ( ret );
    }

    if ( ( ret = esp_aes_gcm_update( ctx, length, input, output ) ) != 0 ) {
        return ( ret );
    }

    if ( ( ret = esp_aes_gcm_finish( ctx, tag, tag_len ) ) != 0 ) {
        return ( ret );
    }

    return ( 0 );
}

int esp_aes_gcm_auth_decrypt( esp_gcm_context *ctx,
                              size_t length,
                              const unsigned char *iv,
                              size_t iv_len,
                              const unsigned char *add,
                              size_t add_len,
                              const unsigned char *tag,
                              size_t tag_len,
                              const unsigned char *input,
                              unsigned char *output )
{
    int ret;
    unsigned char check_tag[16];
    size_t i;
    int diff;

    if ( ( ret = esp_aes_gcm_crypt_and_tag( ctx, ESP_AES_DECRYPT, length,
                                            iv, iv_len, add, add_len,
                                            input, output, tag_len, check_tag ) ) != 0 ) {
        return ( ret );
    }

    /* Check tag in "constant-time" */
    for ( diff = 0, i = 0; i < tag_len; i++ ) {
        diff |= tag[i] ^ check_tag[i];
    }

    if ( diff != 0 ) {
        bzero( output, length );
        return ( MBEDTLS_ERR_GCM_AUTH_FAILED );
    }

    return ( 0 );
}
