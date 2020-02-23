/**
 * \brief AES block cipher, ESP32 hardware accelerated version
 * Based on mbedTLS FIPS-197 compliant version.
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  Additions Copyright (C) 2016-2017, Espressif Systems (Shanghai) PTE Ltd
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
#include "soc/soc.h"
#include "soc/cpu.h"
#include "soc/dport_reg.h"
#include "soc/hwcrypto_reg.h"
#include "soc/crypto_dma_reg.h"
#include "esp32s2/crypto_dma.h"
#include "esp32s2/rom/lldesc.h"
#include "esp32s2/rom/cache.h"
#include "soc/periph_defs.h"
#include "esp_intr_alloc.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"


#define AES_BLOCK_BYTES 16

#define LE_TO_BE(x)    (((x) >> 24) & 0x000000ff) | \
                       (((x) << 24) & 0xff000000) | \
                       (((x) >> 8) & 0x0000ff00) | \
                       (((x) << 8) & 0x00ff0000)

static inline uint32_t WPA_GET_BE32(const uint8_t *a)
{
    return ((uint32_t) a[0] << 24) | (a[1] << 16) | (a[2] << 8) | a[3];
}

static inline void WPA_PUT_BE32(uint8_t *a, uint32_t val)
{
    a[0] = (val >> 24) & 0xff;
    a[1] = (val >> 16) & 0xff;
    a[2] = (val >> 8) & 0xff;
    a[3] = val & 0xff;
}


/* Enable this if want to use AES interrupt */
//#define CONFIG_MBEDTLS_AES_USE_INTERRUPT

portMUX_TYPE crypto_dma_spinlock = portMUX_INITIALIZER_UNLOCKED;

#if defined(CONFIG_MBEDTLS_AES_USE_INTERRUPT)
static SemaphoreHandle_t op_complete_sem;
#endif

/* AES uses a spinlock mux not a lock as the underlying block operation
   only takes a small number of cycles, much less than using
   a mutex for this.

   For CBC, CFB, etc. this may mean that interrupts are disabled for a longer
   period of time for bigger data lengths.
*/
portMUX_TYPE aes_spinlock = portMUX_INITIALIZER_UNLOCKED;

/* The function will pad 0 if the length is not multiple of
 * 16 bytes for the incoming data buffer
 */
static uint8_t *textpad_zero(const unsigned char *buf, unsigned char *len)
{
    uint8_t offset;
    uint8_t *data = NULL;

    offset = *len % AES_BLOCK_BYTES;

    if (offset) {
        data = (uint8_t *)calloc(1, (*len + (AES_BLOCK_BYTES - offset)));
        if (data) {
            memcpy(data, buf, *len);
            *len += (AES_BLOCK_BYTES - offset);
        }
    }

    return data;
}

static inline bool valid_key_length(const esp_aes_context *ctx)
{
    return ctx->key_bytes == 128/8 || ctx->key_bytes == 192/8 || ctx->key_bytes == 256/8;
}

void esp_aes_acquire_hardware( void )
{
    /* newlib locks lazy initialize on ESP-IDF */
    portENTER_CRITICAL(&aes_spinlock);

    /* Need to lock DMA since it is shared with SHA block */
    portENTER_CRITICAL(&crypto_dma_spinlock);

    /* Enable AES hardware */
    REG_SET_BIT(DPORT_PERIP_CLK_EN1_REG, DPORT_CRYPTO_AES_CLK_EN | DPORT_CRYPTO_DMA_CLK_EN);
    /* Clear reset on digital signature unit,
       otherwise AES unit is held in reset also. */
    REG_CLR_BIT(DPORT_PERIP_RST_EN1_REG,
                DPORT_CRYPTO_AES_RST | DPORT_CRYPTO_DMA_RST | DPORT_CRYPTO_DS_RST);
}

/* Function to disable AES and Crypto DMA clocks and release locks */
void esp_aes_release_hardware( void )
{
    /* Disable DMA mode */
    REG_WRITE(AES_DMA_ENABLE_REG, 0);
    /* Disable AES hardware */
    REG_SET_BIT(DPORT_PERIP_RST_EN1_REG, DPORT_CRYPTO_AES_RST | DPORT_CRYPTO_DMA_RST);
    /* Don't return other units to reset, as this pulls
       reset on RSA & SHA units, respectively. */
    REG_CLR_BIT(DPORT_PERIP_CLK_EN1_REG, DPORT_CRYPTO_AES_CLK_EN | DPORT_CRYPTO_DMA_CLK_EN);

    portEXIT_CRITICAL(&crypto_dma_spinlock);

    portEXIT_CRITICAL(&aes_spinlock);
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
 * to hardware key registers. Also, set the AES block mode
 * (ECb, CBC, CFB, OFB, GCM, CTR) and crypt mode (ENCRYPT/DECRYPT)
 * Enable the DMA mode of operation
 *
 * Call only while holding esp_aes_acquire_hardware().
 */
static inline void esp_aes_setkey_hardware( esp_aes_context *ctx, int crypt_mode, uint8_t block_mode)
{
    const uint32_t MODE_DECRYPT_BIT = 4;
    unsigned mode_reg_base = (crypt_mode == ESP_AES_ENCRYPT) ? 0 : MODE_DECRYPT_BIT;

    ctx->key_in_hardware = 0;

    for (int i = 0; i < ctx->key_bytes/4; ++i) {
        REG_WRITE(AES_KEY_BASE + i * 4, *(((uint32_t *)ctx->key) + i));
        ctx->key_in_hardware += 4;
    }

    REG_WRITE(AES_MODE_REG, mode_reg_base + ((ctx->key_bytes / 8) - 2));

    /* Set the algorithm mode CBC, CFB ... */
    REG_WRITE(AES_BLOCK_MODE_REG, block_mode);

    /* Set the ENDIAN reg */
    REG_WRITE(AES_ENDIAN_REG, 0x3F);

    /* Enable DMA mode */
    REG_WRITE(AES_DMA_ENABLE_REG, 1);

    /* Presently hard-coding the INC function to 32 bit */
    if (block_mode == AES_BLOCK_MODE_CTR) {
        REG_WRITE(AES_INC_SEL_REG, 0);
    }

    /* Fault injection check: all words of key data should have been written to hardware */
    if (ctx->key_in_hardware < 16
        || ctx->key_in_hardware != ctx->key_bytes) {
        abort();
    }
}

/*
 * Function to write IV to hardware iv registers
 */
static inline void esp_aes_set_iv(uint8_t *iv, uint8_t len)
{
    memcpy((uint8_t *)AES_IV_BASE, iv, len);
}

/*
 * Function to read IV from hardware iv registers
 */
static inline void esp_aes_get_iv(uint8_t *iv, uint8_t len)
{
    memcpy(iv, (uint8_t *)AES_IV_BASE, len);
}

/* Function to set block number & trigger bit for AES GCM operation
*/
static inline void esp_aes_gcm_set_block_num_and_trigger(uint16_t len)
{
    /* Write the number of blocks */
    REG_WRITE(AES_BLOCK_NUM_REG, (len / AES_BLOCK_BYTES));

    REG_WRITE(AES_TRIGGER_REG, 1);
    while (REG_READ(AES_STATE_REG) != AES_STATE_IDLE) {
    }
}

/* For AES-GCM mode once H has been calculated
 * continue the AES operation & wait for DMA
 * to finish if input data length is non-zero
 * */
static inline void esp_aes_gcm_continue(size_t len)
{
    volatile uint32_t dma_done = 0;

    REG_WRITE(AES_CONTINUE_REG, 1);
    while (REG_READ(AES_STATE_REG) != AES_STATE_DONE) {
    }

    if (len == 0) {
        REG_WRITE(AES_DMA_EXIT_REG, 1);
        return;
    }

    /* Wait for AES-GCM DMA operation to complete */
    while (1) {
        dma_done = REG_READ(CRYPTO_DMA_INT_RAW_REG);
        if ((dma_done & INT_RAW_IN_SUC_EOF) == INT_RAW_IN_SUC_EOF) {
            break;
        }
    }

    REG_WRITE(AES_DMA_EXIT_REG, 1);
}

#if defined (CONFIG_MBEDTLS_AES_USE_INTERRUPT)
static IRAM_ATTR void esp_aes_dma_isr(void *arg)
{
    BaseType_t higher_woken;
    REG_WRITE(AES_INT_CLR_REG, 1);
    xSemaphoreGiveFromISR(op_complete_sem, &higher_woken);
    if (higher_woken) {
        portYIELD_FROM_ISR();
    }
}
#endif

/* Wait for AES hardware block operation to complete */
static int esp_aes_dma_complete(void)
{
#if defined (CONFIG_MBEDTLS_AES_USE_INTERRUPT)
    if (!xSemaphoreTake(op_complete_sem, 2000 / portTICK_PERIOD_MS)) {
        ESP_LOGE("AES", "Timed out waiting for completion of AES Interrupt");
        abort();
    }
#endif
    /* Checking this if interrupt is used also, to avoid
       issues with AES fault injection
    */
    while (REG_READ(AES_STATE_REG) != AES_STATE_DONE) {
    }

    return 0;
}

/* Perform AES-DMA operation and wait until the DMA operation is over
 *
 * input = Input data buffer, length len
 * output = Output data buffer, length len
 * len = Length of data in bytes, may not be multiple of AES block size
 * stream_out = Pointer to 16 byte buffer to hold final stream block, if
 * len is not a multiple of AES block size (16)
 *
 * The DMA processing works in following way:
 *
 * - If len >= AES_BLOCK_BYTES, there are DMA input and output
 *   descriptors which point to input & output (in_block_desc, out_block_desc) directly,
 *   to process block_bytes bytes.
 *
 * - If len % AES_BLOCK_BYTES != 0 then unaligned bytes are copied to stream_in block which is
 *   padded with zeroes. DMA descriptors (stream_in_desc, stream_out_desc) process this block and output to
 *   stream_out buffer argument. Otherwise, stream_out argument is ignored (may be NULL).
 *
 * - If both above conditions are true, DMA has two linked list input buffers and two linked list output buffers,
 *   and processes first the blocks and then the partial stream block.
 *
 * After the DMA operation, if we only processed full bytes then the IV memory block will contain the next IV
 * for the configured AES mode.
 *
 * If a partial "stream block" was processed then IV memory block will contain a garbage IV value, because of
 * the partial stream block. The IV can be recovered from the stream_out block (depending on algorithm some
 * post-processing of the 'output' bytes also in stream_out may be needed, to translate them back to correct IV bytes).
 *
 */
static int esp_aes_process_dma(esp_aes_context *ctx, const unsigned char *input, unsigned char *output, uint16_t len, uint8_t *stream_out)
{
    volatile lldesc_t block_in_desc, block_out_desc, stream_in_desc, stream_out_desc;
    volatile lldesc_t *in_desc_head, *out_desc_head;
    volatile uint32_t dma_done = 0;
    uint8_t stream_in[16];
    unsigned stream_bytes = len % AES_BLOCK_BYTES; // bytes which aren't in a full block
    unsigned block_bytes = len - stream_bytes;     // bytes which are in a full block
    unsigned blocks = (block_bytes / AES_BLOCK_BYTES) + ((stream_bytes > 0) ? 1 : 0);

    assert(len > 0); // caller shouldn't ever have len set to zero
    assert(stream_bytes == 0 || stream_out != NULL); // stream_out can be NULL if we're processing full block(s)

    /* If no key is written to hardware yet, either the user hasn't called
       mbedtls_aes_setkey_enc/mbedtls_aes_setkey_dec - meaning we also don't
       know which mode to use - or a fault skipped the
       key write to hardware. Treat this as a fatal error and zero the output block.
    */
    if (ctx->key_in_hardware != ctx->key_bytes) {
        bzero(output, 16);
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }

    if (block_bytes > 0) {
        /* If the block length is less than 16 we use internal RAM so no
         * need to flush Cache
        */
#if (CONFIG_SPIRAM_USE_CAPS_ALLOC || CONFIG_SPIRAM_USE_MALLOC)
        if ((unsigned int)input >= SOC_EXTRAM_DATA_LOW && (unsigned int)input <= SOC_EXTRAM_DATA_HIGH) {
            assert((((unsigned int)(input) & 0xF) == 0));
            Cache_WriteBack_All();
        }
        if ((unsigned int)output >= SOC_EXTRAM_DATA_LOW && (unsigned int)output <= SOC_EXTRAM_DATA_HIGH) {
            assert((((unsigned int)(output) & 0xF) == 0));
        }
#endif
        block_in_desc = (lldesc_t) {
            .length = block_bytes,
            .size = block_bytes,
            .buf = (void *)input,
            .owner = 1,
            .empty = (stream_bytes > 0) ? (uint32_t)&stream_in_desc : 0,
            .eof = (stream_bytes == 0),
        };

        block_out_desc = (lldesc_t) {
            .length = block_bytes,
            .size = block_bytes,
            .buf = output,
            .owner = 1,
            .empty = (stream_bytes > 0) ? (uint32_t)&stream_out_desc : 0,
            .eof = (stream_bytes == 0),
        };

        in_desc_head = &block_in_desc;
        out_desc_head = &block_out_desc;
    }

    if (stream_bytes > 0) {
        // can't read past end of 'input', so make a zero padded input buffer in RAM
        memcpy(stream_in, input + block_bytes, stream_bytes);
        bzero(stream_in + stream_bytes, AES_BLOCK_BYTES - stream_bytes);

        stream_in_desc = (lldesc_t) {
            .length = AES_BLOCK_BYTES,
            .size = AES_BLOCK_BYTES,
            .buf = stream_in,
            .owner = 1,
            .eof = 1,
        };

        stream_out_desc = (lldesc_t) {
            .length = AES_BLOCK_BYTES,
            .size = AES_BLOCK_BYTES,
            .buf = stream_out,
            .owner = 1,
            .eof = 1,
        };
    }

    // block buffers are sent to DMA first, unless there aren't any
    in_desc_head =  (block_bytes > 0) ? &block_in_desc : &stream_in_desc;
    out_desc_head = (block_bytes > 0) ? &block_out_desc : &stream_out_desc;

    /* Enable the DMA clock - currently only for FPGA test */
#if CONFIG_IDF_ENV_FPGA
    SET_PERI_REG_MASK(CRYPTO_DMA_CONF0_REG, CONF0_REG_GEN_CLK_EN);
#endif

    /* Reset DMA */
    SET_PERI_REG_MASK(CRYPTO_DMA_CONF0_REG, CONF0_REG_AHBM_RST | CONF0_REG_IN_RST | CONF0_REG_OUT_RST | CONF0_REG_AHBM_FIFO_RST);
    CLEAR_PERI_REG_MASK(CRYPTO_DMA_CONF0_REG, CONF0_REG_AHBM_RST | CONF0_REG_IN_RST | CONF0_REG_OUT_RST | CONF0_REG_AHBM_FIFO_RST);

    /* Set DMA for AES Use */
    REG_WRITE(CRYPTO_DMA_AES_SHA_SELECT_REG, 0);

    /* Set descriptors */
    CLEAR_PERI_REG_MASK(CRYPTO_DMA_OUT_LINK_REG, OUT_LINK_REG_OUTLINK_ADDR);
    SET_PERI_REG_MASK(CRYPTO_DMA_OUT_LINK_REG, ((uint32_t)(in_desc_head))&OUT_LINK_REG_OUTLINK_ADDR);
    CLEAR_PERI_REG_MASK(CRYPTO_DMA_IN_LINK_REG, IN_LINK_REG_INLINK_ADDR);
    SET_PERI_REG_MASK(CRYPTO_DMA_IN_LINK_REG, ((uint32_t)(out_desc_head))&IN_LINK_REG_INLINK_ADDR);

    /* Start transfer */
    SET_PERI_REG_MASK(CRYPTO_DMA_OUT_LINK_REG, OUT_LINK_REG_OUTLINK_START);
    SET_PERI_REG_MASK(CRYPTO_DMA_IN_LINK_REG, IN_LINK_REG_INLINK_START);

    /* Write the number of blocks */
    REG_WRITE(AES_BLOCK_NUM_REG, blocks);

#if defined (CONFIG_MBEDTLS_AES_USE_INTERRUPT)
    REG_WRITE(AES_INT_CLR_REG, 1);
    if (op_complete_sem == NULL) {
        op_complete_sem = xSemaphoreCreateBinary();
        esp_intr_alloc(ETS_AES_INTR_SOURCE, 0, esp_aes_dma_isr, 0, 0);
    }
    REG_WRITE(AES_INT_ENA_REG, 1);
#endif

    /* Start AES operation */
    REG_WRITE(AES_TRIGGER_REG, 1);

    esp_aes_dma_complete();

    /* Wait for DMA operation to complete */
    while (1) {
        dma_done = REG_READ(CRYPTO_DMA_INT_RAW_REG);
        if ( (dma_done & INT_RAW_IN_SUC_EOF) == INT_RAW_IN_SUC_EOF) {
            break;
        }
    }

    REG_WRITE(AES_DMA_EXIT_REG, 1);

#if (CONFIG_SPIRAM_USE_CAPS_ALLOC || CONFIG_SPIRAM_USE_MALLOC)
    if (block_bytes > 0) {
        if ((unsigned int)input >= SOC_EXTRAM_DATA_LOW && (unsigned int)input <= SOC_EXTRAM_DATA_HIGH) {
            Cache_Invalidate_DCache_All();
        }
    }
#endif

    if (stream_bytes > 0) {
        memcpy(output + block_bytes, stream_out, stream_bytes);
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

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    esp_aes_acquire_hardware();
    esp_aes_setkey_hardware(ctx, ESP_AES_ENCRYPT, AES_BLOCK_MODE_ECB);
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

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    esp_aes_acquire_hardware();
    esp_aes_setkey_hardware(ctx, ESP_AES_DECRYPT, AES_BLOCK_MODE_ECB);
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

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    esp_aes_acquire_hardware();
    ctx->key_in_hardware = 0;
    esp_aes_setkey_hardware(ctx, mode, AES_BLOCK_MODE_ECB);
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
    /* For CBC input length should be multiple of
     * AES BLOCK BYTES
     * */
    if ( length % AES_BLOCK_BYTES ) {
        return ERR_ESP_AES_INVALID_INPUT_LENGTH;
    }
    if ( length == 0 ) {
        return 0;
    }
    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    esp_aes_acquire_hardware();
    ctx->key_in_hardware = 0;
    esp_aes_setkey_hardware(ctx, mode, AES_BLOCK_MODE_CBC);
    esp_aes_set_iv(iv, AES_BLOCK_BYTES);
    esp_aes_process_dma(ctx, input, output, length, NULL);
    esp_aes_get_iv(iv, AES_BLOCK_BYTES);
    esp_aes_release_hardware();

    return 0;
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
    size_t block_bytes = length - (length % AES_BLOCK_BYTES);

    /* The DMA engine will only output correct IV if it runs
       full blocks of input in CFB8 mode
    */
    if (block_bytes > 0) {
        esp_aes_acquire_hardware();
        esp_aes_setkey_hardware(ctx, mode, AES_BLOCK_MODE_CFB8);
        esp_aes_set_iv(iv, AES_BLOCK_BYTES);
        esp_aes_process_dma(ctx, input, output, block_bytes, NULL);
        esp_aes_get_iv(iv, AES_BLOCK_BYTES);
        esp_aes_release_hardware();

        length -= block_bytes;
        input += block_bytes;
        output += block_bytes;
    }

    // Process remaining bytes block-at-a-time in ECB mode
    if (length > 0) {
        esp_aes_acquire_hardware();
        esp_aes_setkey_hardware(ctx, MBEDTLS_AES_ENCRYPT, AES_BLOCK_MODE_ECB);

        while ( length-- ) {
            memcpy( ov, iv, 16 );
            esp_aes_process_dma(ctx, iv, iv, AES_BLOCK_BYTES, NULL);

            if ( mode == MBEDTLS_AES_DECRYPT ) {
                ov[16] = *input;
            }

            c = *output++ = ( iv[0] ^ *input++ );

            if ( mode == MBEDTLS_AES_ENCRYPT ) {
                ov[16] = c;
            }
            memcpy( iv, ov + 1, 16 );
        }
        esp_aes_release_hardware();
    }

    return 0;
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
    size_t stream_bytes = 0;
    size_t n = *iv_off;

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    esp_aes_acquire_hardware();
    ctx->key_in_hardware = 0;

    /* Lets process the *iv_off bytes first
     * which are pending from the previous call to this API
     */
    while (n > 0 && length > 0) {
        if (mode == MBEDTLS_AES_ENCRYPT) {
            iv[n] = *output++ = (unsigned char)(*input++ ^ iv[n]);
        } else {
            c = *input++;
            *output++ = (c ^ iv[n]);
            iv[n] = c;
        }
        n = (n + 1) % AES_BLOCK_BYTES;
        length--;
    }


    if (length > 0) {
        stream_bytes = length % AES_BLOCK_BYTES;

        esp_aes_acquire_hardware();
        esp_aes_setkey_hardware(ctx, mode, AES_BLOCK_MODE_CFB128);
        esp_aes_set_iv(iv, AES_BLOCK_BYTES);

        esp_aes_process_dma(ctx, input, output, length, iv);

        if (stream_bytes == 0) {
            // if we didn't need the partial 'stream block' then the new IV is in the IV register
            esp_aes_get_iv(iv, AES_BLOCK_BYTES);
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
    size_t n = *nc_off;

    while (n > 0 && length > 0) {
        *output++ = (unsigned char)(*input++ ^ stream_block[n]);
        n = (n + 1) & 0xF;
        length--;
    }

    if (length > 0) {
        esp_aes_acquire_hardware();
        esp_aes_setkey_hardware(ctx, ESP_AES_DECRYPT, AES_BLOCK_MODE_CTR);
        esp_aes_set_iv(nonce_counter, AES_BLOCK_BYTES);
        esp_aes_process_dma(ctx, input, output, length, stream_block);
        esp_aes_get_iv(nonce_counter, AES_BLOCK_BYTES);
        esp_aes_release_hardware();
    }

    *nc_off = n + (length % AES_BLOCK_BYTES);

    return 0;
}

/* XOR two 32 bit words */
static void xor_block(uint8_t *dst, const uint8_t *src)
{
    uint32_t *d = (uint32_t *) dst;
    uint32_t *s = (uint32_t *) src;

    *d++ ^= *s++;
    *d++ ^= *s++;
    *d++ ^= *s++;
    *d++ ^= *s++;
}

static void right_shift(uint8_t *v)
{
    uint32_t val;

    val = WPA_GET_BE32(v + 12);
    val >>= 1;

    if (v[11] & 0x01) {
        val |= 0x80000000;
    }
    WPA_PUT_BE32(v + 12, val);
    val = WPA_GET_BE32(v + 8);
    val >>= 1;

    if (v[7] & 0x01) {
        val |= 0x80000000;
    }
    WPA_PUT_BE32(v + 8, val);
    val = WPA_GET_BE32(v + 4);
    val >>= 1;

    if (v[3] & 0x01) {
        val |= 0x80000000;
    }
    WPA_PUT_BE32(v + 4, val);
    val = WPA_GET_BE32(v);
    val >>= 1;
    WPA_PUT_BE32(v, val);
}

/* AES-GCM multiplication z = y * h */
static int gcm_mult(uint8_t *y, uint8_t *h, uint8_t *z)
{
    uint8_t v0[AES_BLOCK_BYTES];
    int i, j;

    if (!y || !h || !z) {
        return -1;
    }

    memset(z, 0, AES_BLOCK_BYTES);
    memcpy(v0, y, AES_BLOCK_BYTES);

    for (i = 0; i < AES_BLOCK_BYTES; i++) {
        for (j = 0; j < 8; j++) {
            if (y[i] & BIT(7 - j)) {
                xor_block(z, v0);
            }
            if (v0[15] & 0x01) {
                right_shift(v0);
                v0[0] ^= 0xE1;
            } else {
                right_shift(v0);
            }
        }
    }

    return 0;
}

/* Update the key value in gcm context */
int esp_aes_gcm_setkey( esp_aes_gcm_context *ctx,
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

/* AES-GCM GHASH calculation for IV != 12 bytes */
static void esp_aes_gcm_ghash(uint8_t *h0, uint8_t *iv, uint8_t iv_len, uint8_t *j0, uint16_t s)
{
    uint8_t *hash_buf;
    uint8_t y0[AES_BLOCK_BYTES], old_y0[AES_BLOCK_BYTES];
    uint16_t len, rem = 0;

    memset(old_y0, 0, AES_BLOCK_BYTES);

    /* We need to concatenate IV, s + 8 byte zeros & 8 byte IV length
     * J0 = GHASH( IV || 0 ^(s+64) || len(IV)^64 )
     */
    len = ( iv_len + (s + 8) + 8 );
    hash_buf = calloc( 1, len );
    if (hash_buf) {
        memcpy(hash_buf, iv, iv_len);

        // ToDo: iv_len is 1 byte in size, how to copy 8 bytes to other memory?
#if 0
        memcpy((hash_buf + iv_len + s + 8), &iv_len, 8);
#endif

        /* GHASH(x) calculation
         * Let X1, X2, ... , Xm-1, Xm denote the unique sequence of blocks
         * such that X = X1 || X2 || ... || Xm-1 || Xm.
         * Let Y0 be the “zero block,” 0 ^ 128.
         * Fori=1,...,m, let Yi =(Yi-1 xor Xi) • H.
         * Return Ym
         */
        while (len) {
            rem = ( len > AES_BLOCK_BYTES ) ? AES_BLOCK_BYTES : len;
            /* Yi-1 xor hash_buf */
            for (uint8_t j = 0; j < rem; j++) {
                y0[j] = old_y0[j] ^ hash_buf[j];
            }

            /* gcm multiplication : y0 x h0 */
            gcm_mult(y0, h0, j0);
            memcpy(old_y0, y0, rem);
            hash_buf += rem;
            len -= rem;
        }

        memcpy(j0, y0, rem);
        free(hash_buf);
    }
}

/* AES-GCM J0 calculation
 */
static void inline esp_aes_gcm_process_J0(uint8_t *data, uint8_t iv_len)
{
    uint8_t j_buf[AES_BLOCK_BYTES];
    uint8_t iv[32];
    uint16_t s;

    esp_aes_get_iv(iv, iv_len);

    /* If IV is 96 bits J0 = ( IV || 0^31 || 1 ) */
    if (iv_len == 12) {
        memset(j_buf, 0, AES_BLOCK_BYTES);
        memcpy(j_buf, iv, iv_len);
        j_buf[AES_BLOCK_BYTES - 1] |= 1;
    } else {
        /* If IV is != 96 bits then
         * J0 = GHASH( IV || 0 ^(s+64) || len(IV)^64 ) where
         * s = ( 128 * floor of ( len(IV) / 128 ) - len(IV) )
         * floor of (x) denotes to be the least integer no less than x
         * for example: floor of (1.5) = 2 since 2 is the least integer
         * which is no less than 1.5
         */
        s = ((iv_len / AES_BLOCK_BYTES) +
             ((iv_len % AES_BLOCK_BYTES) == 0) ? 0 : 1);
        s = (s * AES_BLOCK_BYTES) - iv_len;
        esp_aes_gcm_ghash(data, iv, iv_len, j_buf, s);
    }

    /* Write J0 to hardware registers */
    memcpy((uint8_t *)AES_J_BASE, j_buf, AES_BLOCK_BYTES);
}

/* Configure & start crypto DMA for AES GCM operation
 */
static void esp_aes_gcm_dma(unsigned char *aad, esp_aes_gcm_context *ctx,
                            unsigned char *input, size_t ilen,
                            unsigned char *len_buf, unsigned char *output)
{
    volatile lldesc_t dma_descr[4];
    int i = 0;

    bzero( (void *)dma_descr, sizeof( dma_descr ) );

#if (CONFIG_SPIRAM_USE_CAPS_ALLOC || CONFIG_SPIRAM_USE_MALLOC)
    if ((unsigned int)input >= SOC_EXTRAM_DATA_LOW && (unsigned int)input <= SOC_EXTRAM_DATA_HIGH) {
        assert((((unsigned int)(input) & 0xF) == 0));
        Cache_WriteBack_All();
    }
    if ((unsigned int)output >= SOC_EXTRAM_DATA_LOW && (unsigned int)output <= SOC_EXTRAM_DATA_HIGH) {
        assert((((unsigned int)(output) & 0xF) == 0));
    }
#endif

    dma_descr[0].length = ctx->aad_len;
    dma_descr[0].size = ctx->aad_len;
    dma_descr[0].buf = aad;
    dma_descr[0].owner = 1;
    dma_descr[0].eof = 0;
    dma_descr[0].empty = (uint32_t)&dma_descr[1];

    dma_descr[1].length = ilen;
    dma_descr[1].size = ilen;
    dma_descr[1].buf = input;
    dma_descr[1].owner = 1;
    dma_descr[1].eof = 0;
    dma_descr[1].empty = (uint32_t)&dma_descr[2];

    dma_descr[2].length = AES_BLOCK_BYTES;
    dma_descr[2].size = AES_BLOCK_BYTES;
    dma_descr[2].buf = len_buf;
    dma_descr[2].owner = 1;
    dma_descr[2].eof = 1;
    dma_descr[2].empty = 0;

    dma_descr[3].length = ctx->aad_len + ilen + AES_BLOCK_BYTES;
    dma_descr[3].size = ctx->aad_len + ilen + AES_BLOCK_BYTES;
    dma_descr[3].buf = output;
    dma_descr[3].owner = 1;
    dma_descr[3].eof = 1;
    dma_descr[3].empty = 0;

    /* If no additional authentication data */
    if (ctx->aad_len == 0) {
        i = 1;
    }
    /* If no input data */
    if (ilen == 0) {
        i = 2;
    }

    /* Enable the DMA clock - currently only for FPGA test */
#if CONFIG_IDF_ENV_FPGA
    SET_PERI_REG_MASK(CRYPTO_DMA_CONF0_REG, CONF0_REG_GEN_CLK_EN);
#endif

    /* Reset DMA */
    SET_PERI_REG_MASK(CRYPTO_DMA_CONF0_REG, CONF0_REG_AHBM_RST | CONF0_REG_IN_RST | CONF0_REG_OUT_RST | CONF0_REG_AHBM_FIFO_RST);
    CLEAR_PERI_REG_MASK(CRYPTO_DMA_CONF0_REG, CONF0_REG_AHBM_RST | CONF0_REG_IN_RST | CONF0_REG_OUT_RST | CONF0_REG_AHBM_FIFO_RST);

    /* Set DMA for AES Use */
    REG_WRITE(CRYPTO_DMA_AES_SHA_SELECT_REG, 0);

    /* Set descriptors */
    CLEAR_PERI_REG_MASK(CRYPTO_DMA_OUT_LINK_REG, OUT_LINK_REG_OUTLINK_ADDR);
    SET_PERI_REG_MASK(CRYPTO_DMA_OUT_LINK_REG, ((uint32_t)(&dma_descr[i]))&OUT_LINK_REG_OUTLINK_ADDR);
    CLEAR_PERI_REG_MASK(CRYPTO_DMA_IN_LINK_REG, IN_LINK_REG_INLINK_ADDR);
    SET_PERI_REG_MASK(CRYPTO_DMA_IN_LINK_REG, ((uint32_t)(&dma_descr[3]))&IN_LINK_REG_INLINK_ADDR);

    /* Start transfer */
    SET_PERI_REG_MASK(CRYPTO_DMA_OUT_LINK_REG, OUT_LINK_REG_OUTLINK_START);
    SET_PERI_REG_MASK(CRYPTO_DMA_IN_LINK_REG, IN_LINK_REG_INLINK_START);

    /* Trigger AES: Let hardware perform GCTR operation */
    esp_aes_gcm_set_block_num_and_trigger(ilen);

    /* While hardware is busy meanwhile software will calculate GHASH
     * Read H from hardware register
     */
    memcpy(ctx->H, (uint8_t *)AES_H_BASE, AES_BLOCK_BYTES);

    esp_aes_gcm_process_J0(ctx->H, ctx->iv_len);

    /* After following call the ciphertext is available in output buffer */
    esp_aes_gcm_continue(ilen);

#if (CONFIG_SPIRAM_USE_CAPS_ALLOC || CONFIG_SPIRAM_USE_MALLOC)
    if ((unsigned int)input >= SOC_EXTRAM_DATA_LOW && (unsigned int)input <= SOC_EXTRAM_DATA_HIGH) {
        Cache_Invalidate_DCache_All();
    }
#endif
}

/* Function to init AES GCM context to zero */
void esp_aes_gcm_init( esp_aes_gcm_context *ctx)
{
    if (ctx == NULL) {
        return;
    }

    bzero(ctx, sizeof(esp_aes_gcm_context));
}

/* Function to clear AES-GCM context */
void esp_aes_gcm_free( esp_aes_gcm_context *ctx)
{
    if (ctx == NULL) {
        return;
    }

    bzero(ctx, sizeof(esp_aes_gcm_context));
}

/* Setup AES-GCM */
int esp_aes_gcm_starts( esp_aes_gcm_context *ctx,
                        int mode,
                        const unsigned char *iv,
                        size_t iv_len,
                        const unsigned char *aad,
                        size_t aad_len )
{
    uint8_t temp[AES_BLOCK_BYTES] = {0};

    memcpy(temp, iv, iv_len);

    esp_aes_acquire_hardware();
    esp_aes_setkey_hardware( &ctx->aes_ctx, mode, AES_BLOCK_MODE_GCM);
    /* AES-GCM HW does not use IV but we program anyways so that
     * we can retrieve later for J0 calculation */
    esp_aes_set_iv(temp, iv_len);
    ctx->iv_len = iv_len;
    ctx->aad = aad;
    ctx->aad_len = aad_len;

    return ( 0 );
}

/* Perform AES-GCM operation */
int esp_aes_gcm_update( esp_aes_gcm_context *ctx,
                        size_t length,
                        const unsigned char *input,
                        unsigned char *output )
{
    const uint8_t *dbuf = input;
    uint8_t *abuf = (uint8_t *)ctx->aad;
    uint64_t ori_aad_len = ctx->aad_len, ori_p_len = length;
    uint32_t temp[4] = {0};
    bool abuf_alloc = false, dbuf_alloc = false;

    if ( output > input && (size_t) ( output - input ) < length ) {
        return ( MBEDTLS_ERR_GCM_BAD_INPUT );
    }

    /* Check length of AAD & pad if required */
    if (ctx->aad_len % AES_BLOCK_BYTES) {
        ori_aad_len = ctx->aad_len;
        abuf = textpad_zero(ctx->aad, (uint8_t *)&ctx->aad_len);
        if (!abuf) {
            return -1;
        } else {
            abuf_alloc = true;
        }
    }

    /* Check length of input & pad if required */
    if ( length % AES_BLOCK_BYTES ) {
        ori_p_len = length;
        REG_WRITE(AES_BIT_VALID_NUM_REG, (length % AES_BLOCK_BYTES) * 8);
        dbuf = textpad_zero(input, (uint8_t *)&length);
        if (!dbuf) {
            if (abuf_alloc) {
                free(abuf);
                return -1;
            }
        } else {
            dbuf_alloc = true;
        }
    }

    /* Update number of AAD blocks in hardware register */
    REG_WRITE(AES_AAD_BLOCK_NUM_REG, (ctx->aad_len / AES_BLOCK_BYTES));

    /* Input buffer is: length[textpad(input)] + length[textpad(aad)]
    *                   + [length]64 + [aad_len]64
    */
    ori_aad_len *= 8;
    ori_p_len *= 8;

    temp[0] = (uint32_t)LE_TO_BE(ori_aad_len >> 32);
    temp[1] = (uint32_t)LE_TO_BE(ori_aad_len);
    temp[2] = (uint32_t)LE_TO_BE(ori_p_len >> 32);
    temp[3] = (uint32_t)LE_TO_BE(ori_p_len);

    esp_aes_gcm_dma(abuf, ctx, (uint8_t *)dbuf, length, (uint8_t *)temp, output);

    if (abuf_alloc) {
        free((void *)abuf);
    }
    if (dbuf_alloc) {
        free((void *)dbuf);
    }

    return 0;
}

/* Function to read the tag value */
int esp_aes_gcm_finish( esp_aes_gcm_context *ctx,
                        unsigned char *tag,
                        size_t tag_len )
{
    memcpy(tag, (uint8_t *)AES_T_BASE, tag_len);

    return 0;
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
    size_t n = *iv_off;
    size_t stream_bytes = 0;

    while (n > 0 && length > 0) {
        *output++ = (unsigned char)(*input++ ^ iv[n]);
        n = (n + 1) & 0xF;
        length--;
    }

    if (length > 0) {
        stream_bytes = (length % AES_BLOCK_BYTES);

        esp_aes_acquire_hardware();
        esp_aes_setkey_hardware(ctx, ESP_AES_DECRYPT, AES_BLOCK_MODE_OFB);
        esp_aes_set_iv(iv, AES_BLOCK_BYTES);
        esp_aes_process_dma(ctx, input, output, length, iv);
        if (stream_bytes == 0) {
            // new IV is in the IV block
            esp_aes_get_iv(iv, AES_BLOCK_BYTES);
        } else {
            // IV is in the iv buffer (stream_out param), however 'stream_bytes'
            // of it are already XORed with input bytes so need to un-XOR them
            for (int i = 0; i < stream_bytes; i++) {
                iv[i] ^= input[length - stream_bytes + i];
            }
        }
        esp_aes_release_hardware();
    }

    *iv_off = n + stream_bytes;

    return 0;
}


/* Below XTS implementation is copied aes.c of mbedtls library.
 * When MBEDTLS_AES_ALT is defined mbedtls expects alternate
 * definition of XTS functions to be available. Even if this
 * could have been avoided, it is done for consistency reason.
 */

void esp_aes_xts_init( esp_aes_xts_context *ctx )
{
    esp_aes_init( &ctx->crypt );
    esp_aes_init( &ctx->tweak );
}

void esp_aes_xts_free( esp_aes_xts_context *ctx )
{
    esp_aes_free( &ctx->crypt );
    esp_aes_free( &ctx->tweak );
}

static int esp_aes_xts_decode_keys( const unsigned char *key,
                                        unsigned int keybits,
                                        const unsigned char **key1,
                                        unsigned int *key1bits,
                                        const unsigned char **key2,
                                        unsigned int *key2bits )
{
    const unsigned int half_keybits = keybits / 2;
    const unsigned int half_keybytes = half_keybits / 8;

    switch( keybits )
    {
        case 256: break;
        case 512: break;
        default : return( MBEDTLS_ERR_AES_INVALID_KEY_LENGTH );
    }

    *key1bits = half_keybits;
    *key2bits = half_keybits;
    *key1 = &key[0];
    *key2 = &key[half_keybytes];

    return 0;
}

int esp_aes_xts_setkey_enc( esp_aes_xts_context *ctx,
                                const unsigned char *key,
                                unsigned int keybits)
{
    int ret;
    const unsigned char *key1, *key2;
    unsigned int key1bits, key2bits;

    ret = esp_aes_xts_decode_keys( key, keybits, &key1, &key1bits,
                                       &key2, &key2bits );
    if( ret != 0 )
        return( ret );

    /* Set the tweak key. Always set tweak key for the encryption mode. */
    ret = esp_aes_setkey( &ctx->tweak, key2, key2bits );
    if( ret != 0 )
        return( ret );

    /* Set crypt key for encryption. */
    return esp_aes_setkey( &ctx->crypt, key1, key1bits );
}

int esp_aes_xts_setkey_dec( esp_aes_xts_context *ctx,
                                const unsigned char *key,
                                unsigned int keybits)
{
    int ret;
    const unsigned char *key1, *key2;
    unsigned int key1bits, key2bits;

    ret = esp_aes_xts_decode_keys( key, keybits, &key1, &key1bits,
                                       &key2, &key2bits );
    if( ret != 0 )
        return( ret );

    /* Set the tweak key. Always set tweak key for encryption. */
    ret = esp_aes_setkey( &ctx->tweak, key2, key2bits );
    if( ret != 0 )
        return( ret );

    /* Set crypt key for decryption. */
    return esp_aes_setkey( &ctx->crypt, key1, key1bits );
}

/* Endianess with 64 bits values */
#ifndef GET_UINT64_LE
#define GET_UINT64_LE(n,b,i)                            \
{                                                       \
    (n) = ( (uint64_t) (b)[(i) + 7] << 56 )             \
        | ( (uint64_t) (b)[(i) + 6] << 48 )             \
        | ( (uint64_t) (b)[(i) + 5] << 40 )             \
        | ( (uint64_t) (b)[(i) + 4] << 32 )             \
        | ( (uint64_t) (b)[(i) + 3] << 24 )             \
        | ( (uint64_t) (b)[(i) + 2] << 16 )             \
        | ( (uint64_t) (b)[(i) + 1] <<  8 )             \
        | ( (uint64_t) (b)[(i)    ]       );            \
}
#endif

#ifndef PUT_UINT64_LE
#define PUT_UINT64_LE(n,b,i)                            \
{                                                       \
    (b)[(i) + 7] = (unsigned char) ( (n) >> 56 );       \
    (b)[(i) + 6] = (unsigned char) ( (n) >> 48 );       \
    (b)[(i) + 5] = (unsigned char) ( (n) >> 40 );       \
    (b)[(i) + 4] = (unsigned char) ( (n) >> 32 );       \
    (b)[(i) + 3] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i)    ] = (unsigned char) ( (n)       );       \
}
#endif

typedef unsigned char esp_be128[16];

/*
 * GF(2^128) multiplication function
 *
 * This function multiplies a field element by x in the polynomial field
 * representation. It uses 64-bit word operations to gain speed but compensates
 * for machine endianess and hence works correctly on both big and little
 * endian machines.
 */
static void esp_gf128mul_x_ble( unsigned char r[16],
                                    const unsigned char x[16] )
{
    uint64_t a, b, ra, rb;

    GET_UINT64_LE( a, x, 0 );
    GET_UINT64_LE( b, x, 8 );

    ra = ( a << 1 )  ^ 0x0087 >> ( 8 - ( ( b >> 63 ) << 3 ) );
    rb = ( a >> 63 ) | ( b << 1 );

    PUT_UINT64_LE( ra, r, 0 );
    PUT_UINT64_LE( rb, r, 8 );
}

/*
 * AES-XTS buffer encryption/decryption
 */
int esp_aes_crypt_xts( esp_aes_xts_context *ctx,
                           int mode,
                           size_t length,
                           const unsigned char data_unit[16],
                           const unsigned char *input,
                           unsigned char *output )
{
    int ret;
    size_t blocks = length / 16;
    size_t leftover = length % 16;
    unsigned char tweak[16];
    unsigned char prev_tweak[16];
    unsigned char tmp[16];

    /* Sectors must be at least 16 bytes. */
    if( length < 16 )
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;

    /* NIST SP 80-38E disallows data units larger than 2**20 blocks. */
    if( length > ( 1 << 20 ) * 16 )
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;

    /* Compute the tweak. */
    ret = esp_aes_crypt_ecb( &ctx->tweak, MBEDTLS_AES_ENCRYPT,
                                 data_unit, tweak );
    if( ret != 0 )
        return( ret );

    while( blocks-- )
    {
        size_t i;

        if( leftover && ( mode == MBEDTLS_AES_DECRYPT ) && blocks == 0 )
        {
            /* We are on the last block in a decrypt operation that has
             * leftover bytes, so we need to use the next tweak for this block,
             * and this tweak for the lefover bytes. Save the current tweak for
             * the leftovers and then update the current tweak for use on this,
             * the last full block. */
            memcpy( prev_tweak, tweak, sizeof( tweak ) );
            esp_gf128mul_x_ble( tweak, tweak );
        }

        for( i = 0; i < 16; i++ )
            tmp[i] = input[i] ^ tweak[i];

        ret = esp_aes_crypt_ecb( &ctx->crypt, mode, tmp, tmp );
        if( ret != 0 )
            return( ret );

        for( i = 0; i < 16; i++ )
            output[i] = tmp[i] ^ tweak[i];

        /* Update the tweak for the next block. */
        esp_gf128mul_x_ble( tweak, tweak );

        output += 16;
        input += 16;
    }

    if( leftover )
    {
        /* If we are on the leftover bytes in a decrypt operation, we need to
         * use the previous tweak for these bytes (as saved in prev_tweak). */
        unsigned char *t = mode == MBEDTLS_AES_DECRYPT ? prev_tweak : tweak;

        /* We are now on the final part of the data unit, which doesn't divide
         * evenly by 16. It's time for ciphertext stealing. */
        size_t i;
        unsigned char *prev_output = output - 16;

        /* Copy ciphertext bytes from the previous block to our output for each
         * byte of cyphertext we won't steal. At the same time, copy the
         * remainder of the input for this final round (since the loop bounds
         * are the same). */
        for( i = 0; i < leftover; i++ )
        {
            output[i] = prev_output[i];
            tmp[i] = input[i] ^ t[i];
        }

        /* Copy ciphertext bytes from the previous block for input in this
         * round. */
        for( ; i < 16; i++ )
            tmp[i] = prev_output[i] ^ t[i];

        ret = esp_aes_crypt_ecb( &ctx->crypt, mode, tmp, tmp );
        if( ret != 0 )
            return ret;

        /* Write the result back to the previous block, overriding the previous
         * output we copied. */
        for( i = 0; i < 16; i++ )
            prev_output[i] = tmp[i] ^ t[i];
    }

    return( 0 );
}
int esp_aes_gcm_crypt_and_tag( esp_aes_gcm_context *ctx,
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

int esp_aes_gcm_auth_decrypt( esp_aes_gcm_context *ctx,
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


