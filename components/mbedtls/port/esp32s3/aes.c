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
#include "esp32s3/aes.h"
#include "soc/cpu.h"
#include "soc/dport_reg.h"
#include "soc/hwcrypto_reg.h"
#include "soc/periph_defs.h"
#include "esp32s3/rom/lldesc.h"
#include "esp32s3/rom/cache.h"
#include "esp_intr_alloc.h"
#include "driver/periph_ctrl.h"
#include "esp_log.h"
#include "soc/lldesc.h"
#include "esp_heap_caps.h"
#include "sys/param.h"
#include "esp_pm.h"
#include "soc/soc_memory_layout.h"
#include "soc/gdma_reg.h"
#include "soc/gdma_struct.h"
#include "soc/extmem_reg.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define AES_BLOCK_BYTES 16
#define IV_WORDS        4

#define DMA_PERIPH_AES 6 /* DMA peripheral indexes */
#define DMA_PERIPH_SHA 7
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
} esp_aes_mode_t;


#if defined(CONFIG_MBEDTLS_AES_USE_INTERRUPT)
static SemaphoreHandle_t op_complete_sem;
#if defined(CONFIG_PM_ENABLE)
static esp_pm_lock_handle_t s_pm_cpu_lock;
static esp_pm_lock_handle_t s_pm_sleep_lock;
#endif
#endif

static const char *TAG = "esp-aes";

static _lock_t s_aes_lock;

static inline bool valid_key_length(const esp_aes_context *ctx)
{
    return ctx->key_bytes == 128 / 8  || ctx->key_bytes == 256 / 8;
}


void esp_aes_acquire_hardware( void )
{
    _lock_acquire(&s_aes_lock);

    /* Enable AES hardware */
    //periph_module_enable(PERIPH_AES_DMA_MODULE);
    /* Enable AES hardware */
    REG_SET_BIT(SYSTEM_PERIP_CLK_EN1_REG, SYSTEM_CRYPTO_AES_CLK_EN | SYSTEM_DMA_CLK_EN);
    /* Clear reset on digital signature unit,
       otherwise AES unit is held in reset also. */
    REG_CLR_BIT(SYSTEM_PERIP_RST_EN1_REG,
                SYSTEM_CRYPTO_AES_RST | SYSTEM_DMA_RST | SYSTEM_CRYPTO_DS_RST);
}

/* Function to disable AES and Crypto DMA clocks and release locks */
void esp_aes_release_hardware( void )
{
    /* Disable AES hardware */
    //periph_module_disable(PERIPH_AES_DMA_MODULE);
    /* Disable AES hardware */
    REG_SET_BIT(SYSTEM_PERIP_RST_EN1_REG, SYSTEM_CRYPTO_AES_RST | SYSTEM_DMA_RST);
    /* Don't return other units to reset, as this pulls
       reset on RSA & SHA units, respectively. */
    REG_CLR_BIT(SYSTEM_PERIP_CLK_EN1_REG, SYSTEM_CRYPTO_AES_CLK_EN | SYSTEM_DMA_CLK_EN);

    _lock_release(&s_aes_lock);
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
    if (keybits == 192) {
        return MBEDTLS_ERR_AES_FEATURE_UNAVAILABLE;
    }
    if (keybits != 128  && keybits != 256) {
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
    unsigned mode_reg_base = (crypt_mode == ESP_AES_ENCRYPT) ? 0 : MODE_DECRYPT_BIT;

    ctx->key_in_hardware = 0;

    for (int i = 0; i < ctx->key_bytes / 4; ++i) {
        REG_WRITE(AES_KEY_BASE + i * 4, *(((uint32_t *)ctx->key) + i));
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
    uint32_t *iv_words = (uint32_t *)iv;
    uint32_t *reg_addr_buf = (uint32_t *)(AES_IV_BASE);

    for (int i = 0; i < IV_WORDS; i++ ) {
        REG_WRITE(&reg_addr_buf[i], iv_words[i]);
    }
}

/*
 * Read IV from hardware iv registers
 */
static inline void esp_aes_get_iv(uint8_t *iv)
{
    esp_dport_access_read_buffer((uint32_t *)iv, AES_IV_BASE, IV_WORDS);
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
    __attribute__((unused)) volatile uint32_t dma_done;

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
        if (  (output_desc->owner == 0) ) {
            break;
        }
    }
}

/* Init DMA related registers for AES operation */
static void esp_aes_dma_init(lldesc_t *input, lldesc_t *output)
{
    /* Enable DMA mode */
    REG_WRITE(AES_DMA_ENABLE_REG, 1);
    REG_CLR_BIT(SYSTEM_PERIP_CLK_EN1_REG, SYSTEM_DMA_CLK_EN);
    REG_SET_BIT(SYSTEM_PERIP_CLK_EN1_REG, SYSTEM_DMA_CLK_EN);
    REG_SET_BIT(SYSTEM_PERIP_RST_EN1_REG, SYSTEM_DMA_RST);
    REG_CLR_BIT(SYSTEM_PERIP_RST_EN1_REG, SYSTEM_DMA_RST);

    /* Initialize DMA registers - this is probably mostly one off initialization

         Note: hardcoded to DMA channel 0
      */
    /* Note: burst mode has alignment requirements that we have not checked here */
    GDMA.conf0[0].outdscr_burst_en = 0;
    GDMA.conf0[0].indscr_burst_en = 0;
    GDMA.conf0[0].out_data_burst_en = 0;
    GDMA.conf0[0].in_data_burst_en = 0;

    GDMA.peri_sel[0].peri_out_sel = DMA_PERIPH_AES;
    GDMA.peri_sel[0].peri_in_sel = DMA_PERIPH_AES;

    /* Set descriptor addresses: NOTE BACKWARDS AS DMA IN/OUT is reverse of AES in/out */
    GDMA.out_link[0].addr = (uint32_t)input;
    GDMA.in_link[0].addr = (uint32_t)output;

    GDMA.sram_size[0].in_size = 3; /* 40 bytes, also minimum size for EDMA */
    GDMA.sram_size[0].out_size = 3;
    GDMA.conf1[0].in_ext_mem_bk_size = 0; // 16 bytes
    GDMA.conf1[0].out_ext_mem_bk_size = 0; // 16 bytes

    /*
      printf("DESC HEAD pointers IN/outlink %p OUT/inlink / %p\n", in_desc_head, out_desc_head);

      printf("before starting in_desc_head owner %d out_desc_head owner %d INT_RAW 0x%08x\n",
      in_desc_head->owner,
      out_desc_head->owner,
      DMA.int_raw.val);

    */

    //REG_SET_BIT(EXTMEM_CACHE_MMU_OWNER_REG, 1<<23);  //mark PSRAM DCache as belonging to DMA

    GDMA.conf0[0].in_rst = 1;
    GDMA.conf0[0].in_rst = 0;
    GDMA.conf0[0].out_rst = 1;
    GDMA.conf0[0].out_rst = 0;

    /* Start transfer */
    GDMA.out_link[0].start = 1;
    GDMA.in_link[0].start = 1;
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
    lldesc_t *block_desc = NULL, *block_in_desc, *block_out_desc;
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

        lldesc_setup_link(block_desc, input, block_bytes, 0);
        lldesc_setup_link(block_desc + lldesc_num, output, block_bytes, 0);
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
    esp_aes_dma_wait_complete(use_intr, out_desc_head);



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
