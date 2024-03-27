/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "soc/soc_caps.h"

#include "esp_log.h"
#include "esp_memory_utils.h"
#include "esp_heap_caps.h"
#include "sys/param.h"
#include "soc/lldesc.h"

#if SOC_SHA_SUPPORTED
#if SOC_SHA_SUPPORT_DMA

#include "soc/periph_defs.h"
#include "esp_private/periph_ctrl.h"
#include "hal/sha_hal.h"
#include "hal/clk_gate_ll.h"
#include "sha_dma.h"

#if CONFIG_SOC_SHA_GDMA
#include "esp_crypto_shared_gdma.h"
#else
#include "soc/crypto_dma_reg.h"
#include "hal/crypto_dma_ll.h"
#endif /* CONFIG_SOC_SHA_GDMA */

#ifndef SOC_SHA_DMA_MAX_BUFFER_SIZE
#define SOC_SHA_DMA_MAX_BUFFER_SIZE (3968)
#endif

const static char* TAG = "sha_dma";
static bool s_check_dma_capable(const void *p);

/* These are static due to:
 *  * Must be in DMA capable memory, so stack is not a safe place to put them
 *  * To avoid having to malloc/free them for every DMA operation
 */
static DRAM_ATTR lldesc_t s_dma_descr_input;
static DRAM_ATTR lldesc_t s_dma_descr_buf;

#if CONFIG_SOC_SHA_GDMA

static esp_err_t esp_sha_dma_start(const lldesc_t *input)
{
    return esp_crypto_shared_gdma_start(input, NULL, GDMA_TRIG_PERIPH_SHA);
}

#else

static esp_err_t esp_sha_dma_start(const lldesc_t *input)
{
    crypto_dma_ll_set_mode(CRYPTO_DMA_SHA);
    crypto_dma_ll_reset();

    crypto_dma_ll_outlink_set((intptr_t)input);
    crypto_dma_ll_outlink_start();

    return ESP_OK;
}

#endif

static void acquire_hardware(void)
{
#if SOC_AES_CRYPTO_DMA
    periph_ll_enable_clk_clear_rst(PERIPH_SHA_DMA_MODULE);
#elif SOC_AES_GDMA
    periph_ll_enable_clk_clear_rst(PERIPH_SHA_MODULE);
#endif
}

static void release_hardware(void)
{
#if SOC_AES_CRYPTO_DMA
    periph_ll_disable_clk_set_rst(PERIPH_SHA_DMA_MODULE);
#elif SOC_AES_GDMA
    periph_ll_disable_clk_set_rst(PERIPH_SHA_MODULE);
#endif
}

static int esp_sha_dma_process(esp_sha_type sha_type, const void *input, uint32_t ilen,
                               const void *buf, uint32_t buf_len, bool is_first_block);

/* Performs SHA on multiple blocks at a time using DMA
   splits up into smaller operations for inputs that exceed a single DMA list
 */
static int esp_sha_dma(esp_sha_type sha_type, const void *input, uint32_t ilen,
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
    lldesc_t *dma_descr_head = NULL;
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

#if defined(SOC_SHA_SUPPORT_SHA1)

static void esp_internal_sha1_update_state(sha1_ctx *ctx, esp_sha_type sha_type)
{
    if (ctx->sha_state == ESP_SHA_STATE_INIT) {
        ctx->first_block = true;
        ctx->sha_state = ESP_SHA_STATE_IN_PROCESS;
    } else if (ctx->sha_state == ESP_SHA_STATE_IN_PROCESS) {
        ctx->first_block = false;
        sha_hal_write_digest(sha_type, ctx->state);
    }
}

static void sha1_update_dma(sha1_ctx* ctx, esp_sha_type sha_type, const unsigned char *input, size_t ilen)
{
    size_t fill;
    uint32_t left, len, local_len = 0;

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += (uint32_t) ilen;
    ctx->total[0] &= 0xFFFFFFFF;

    if ( ctx->total[0] < (uint32_t) ilen ) {
        ctx->total[1]++;
    }

    if ( left && ilen >= fill ) {
        memcpy( (void *) (ctx->buffer + left), input, fill );
        input += fill;
        ilen  -= fill;
        left = 0;
        local_len = 64;
    }

    len = (ilen / 64) * 64;
    if ( len || local_len) {
        /* Enable peripheral module */
        acquire_hardware();

        esp_internal_sha1_update_state(ctx, sha_type);

        int ret = esp_sha_dma(sha_type, input, len, ctx->buffer, local_len, ctx->first_block);
        if (ret != 0) {
            release_hardware();
            return ;
        }

        /* Reads the current message digest from the SHA engine */
        sha_hal_read_digest(sha_type, ctx->state);

        /* Disable peripheral module */
        release_hardware();
    }

    if ( ilen > 0 ) {
        memcpy( (void *) (ctx->buffer + left), input + len, ilen - len );
    }
}

void sha1_dma(esp_sha_type sha_type, const unsigned char *input, size_t ilen, unsigned char *output)
{

    sha1_ctx ctx;
    ctx.total[0] = 0;
    ctx.total[1] = 0;

    memset(&ctx, 0, sizeof( sha1_ctx ) );
    ctx.mode = SHA1;

    sha1_update_dma(&ctx, sha_type, input, ilen);

    uint32_t last, padn;
    uint32_t high, low;
    unsigned char msglen[8];

    high = ( ctx.total[0] >> 29 )
           | ( ctx.total[1] <<  3 );
    low  = ( ctx.total[0] <<  3 );

    PUT_UINT32_BE( high, msglen, 0 );
    PUT_UINT32_BE( low,  msglen, 4 );

    last = ctx.total[0] & 0x3F;
    padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );

    sha1_update_dma(&ctx, sha_type, sha1_padding, padn);

    sha1_update_dma(&ctx, sha_type, msglen, 8);

    memcpy(output, ctx.state, 20);
}

#endif /* defined(SOC_SHA_SUPPORT_SHA1) */

#if defined(SOC_SHA_SUPPORT_SHA224) || defined(SOC_SHA_SUPPORT_SHA256)

static void esp_internal_sha256_update_state(sha256_ctx *ctx)
{
    if (ctx->sha_state == ESP_SHA_STATE_INIT) {
        ctx->first_block = true;
        ctx->sha_state = ESP_SHA_STATE_IN_PROCESS;
    } else if (ctx->sha_state == ESP_SHA_STATE_IN_PROCESS) {
        ctx->first_block = false;
        sha_hal_write_digest(ctx->mode, ctx->state);
    }
}

static void sha256_update_dma(sha256_ctx* ctx, esp_sha_type sha_type, const unsigned char *input, size_t ilen)
{
    size_t fill;
    uint32_t left, len, local_len = 0;

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += (uint32_t) ilen;
    ctx->total[0] &= 0xFFFFFFFF;

    if ( ctx->total[0] < (uint32_t) ilen ) {
        ctx->total[1]++;
    }

    if ( left && ilen >= fill ) {
        memcpy( (void *) (ctx->buffer + left), input, fill );
        input += fill;
        ilen  -= fill;
        left = 0;
        local_len = 64;
    }

    len = (ilen / 64) * 64;
    if ( len || local_len) {
        /* Enable peripheral module */
        acquire_hardware();

        esp_internal_sha256_update_state(ctx);

        int ret = esp_sha_dma(ctx->mode, input, len, ctx->buffer, local_len, ctx->first_block);

        if (ret != 0) {
            /* Disable peripheral module */
            release_hardware();
            return;
        }

        /* Reads the current message digest from the SHA engine */
        sha_hal_read_digest(sha_type, ctx->state);

        /* Disable peripheral module */
        release_hardware();
    }

    if ( ilen > 0 ) {
        memcpy( (void *) (ctx->buffer + left), input + len, ilen - len );
    }
}

void sha256_dma(esp_sha_type sha_type, const unsigned char *input, size_t ilen, unsigned char *output)
{

    sha256_ctx ctx;
    memset(&ctx, 0, sizeof(sha256_ctx));
    ctx.mode = sha_type;

    sha256_update_dma(&ctx, sha_type, input, ilen);

    uint32_t last, padn;
    uint32_t high, low;
    unsigned char msglen[8];

    high = ( ctx.total[0] >> 29 )
           | ( ctx.total[1] <<  3 );
    low  = ( ctx.total[0] <<  3 );

    PUT_UINT32_BE( high, msglen, 0 );
    PUT_UINT32_BE( low,  msglen, 4 );

    last = ctx.total[0] & 0x3F;
    padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );

    sha256_update_dma(&ctx, sha_type, sha256_padding, padn);

    sha256_update_dma(&ctx, sha_type, msglen, 8);

    if (sha_type == SHA2_256) {
        memcpy(output, ctx.state, 32);
    } else if (sha_type == SHA2_224) {
        memcpy(output, ctx.state, 28);
    }
}

#endif /* defined(SOC_SHA_SUPPORT_SHA224) || defined(SOC_SHA_SUPPORT_SHA256) */

#if defined(SOC_SHA_SUPPORT_SHA384) || defined(SOC_SHA_SUPPORT_SHA512)

#if SOC_SHA_SUPPORT_SHA512_T

int sha_512_t_init_hash_dma(uint16_t t)
{
    uint32_t t_string = 0;
    uint8_t t0, t1, t2, t_len;

    if (t == 384) {
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
        return -1;
    }

    /* Calculates and sets the initial digiest for SHA512_t */
    sha_hal_sha512_init_hash(t_string, t_len);

    return 0;
}

#endif //SOC_SHA_SUPPORT_SHA512_T

static void esp_internal_sha512_update_state(sha512_ctx *ctx)
{
    if (ctx->sha_state == ESP_SHA_STATE_INIT) {
        if (ctx->mode == SHA2_512T) {
            int ret = -1;
            if ((ret = sha_512_t_init_hash_dma(ctx->t_val)) != 0) {
                release_hardware();
                return;
            }
            ctx->first_block = false;
        } else {
            ctx->first_block = true;
        }
        ctx->sha_state = ESP_SHA_STATE_IN_PROCESS;
    } else if (ctx->sha_state == ESP_SHA_STATE_IN_PROCESS) {
        ctx->first_block = false;
        sha_hal_write_digest(ctx->mode, ctx->state);
    }
}

static void sha512_update_dma(sha512_ctx* ctx, esp_sha_type sha_type, const unsigned char *input, size_t ilen)
{

    size_t fill;
    unsigned int left, len, local_len = 0;

    left = (unsigned int) (ctx->total[0] & 0x7F);
    fill = 128 - left;

    ctx->total[0] += (uint64_t) ilen;

    if ( ctx->total[0] < (uint64_t) ilen ) {
        ctx->total[1]++;
    }

    if ( left && ilen >= fill ) {
        memcpy( (void *) (ctx->buffer + left), input, fill );
        input += fill;
        ilen  -= fill;
        left = 0;
        local_len = 128;
    }

    len = (ilen / 128) * 128;

    if ( len || local_len) {

        /* Enable peripheral module */
        acquire_hardware();

        esp_internal_sha512_update_state(ctx);

        int ret = esp_sha_dma(ctx->mode, input, len, ctx->buffer, local_len, ctx->first_block);

        if (ret != 0) {
            release_hardware();
            return;
        }

        /* Reads the current message digest from the SHA engine */
        sha_hal_read_digest(sha_type, ctx->state);

        /* Disable peripheral module */
        release_hardware();
    }

    if ( ilen > 0 ) {
        memcpy( (void *) (ctx->buffer + left), input + len, ilen - len );
    }
}

void sha512_dma(esp_sha_type sha_type, const unsigned char *input, size_t ilen, unsigned char *output)
{

    sha512_ctx ctx;
    memset(&ctx, 0, sizeof(sha512_ctx));
    ctx.mode = sha_type;

    sha512_update_dma(&ctx, sha_type, input, ilen);

    size_t last, padn;
    uint64_t high, low;
    unsigned char msglen[16];

    high = ( ctx.total[0] >> 61 )
           | ( ctx.total[1] <<  3 );
    low  = ( ctx.total[0] <<  3 );

    PUT_UINT64_BE( high, msglen, 0 );
    PUT_UINT64_BE( low,  msglen, 8 );

    last = (size_t)( ctx.total[0] & 0x7F );
    padn = ( last < 112 ) ? ( 112 - last ) : ( 240 - last );

    sha512_update_dma( &ctx, sha_type, sha512_padding, padn );

    sha512_update_dma( &ctx, sha_type, msglen, 16 );

    if (sha_type == SHA2_384) {
        memcpy(output, ctx.state, 48);
    } else {
        memcpy(output, ctx.state, 64);
    }

}

#endif /* defined(SOC_SHA_SUPPORT_SHA384) || defined(SOC_SHA_SUPPORT_SHA512) */

#if SOC_SHA_SUPPORT_SHA512_T

void sha512t_dma(esp_sha_type sha_type, const unsigned char *input, size_t ilen, unsigned char *output, uint32_t t_val)
{
    sha512_ctx ctx;
    memset(&ctx, 0, sizeof(sha512_ctx));
    ctx.t_val = t_val;
    ctx.mode = sha_type;

    sha512_update_dma(&ctx, sha_type, input, ilen);

    size_t last, padn;
    uint64_t high, low;
    unsigned char msglen[16];

    high = ( ctx.total[0] >> 61 )
           | ( ctx.total[1] <<  3 );
    low  = ( ctx.total[0] <<  3 );

    PUT_UINT64_BE( high, msglen, 0 );
    PUT_UINT64_BE( low,  msglen, 8 );

    last = (size_t)( ctx.total[0] & 0x7F );
    padn = ( last < 112 ) ? ( 112 - last ) : ( 240 - last );

    sha512_update_dma( &ctx, sha_type, sha512_padding, padn );

    sha512_update_dma( &ctx, sha_type, msglen, 16 );

    if (sha_type == SHA2_384) {
        memcpy(output, ctx.state, 48);
    } else {
        memcpy(output, ctx.state, 64);
    }
}

#endif /*SOC_SHA_SUPPORT_SHA512_T*/

#endif /* SOC_SHA_SUPPORT_DMA*/
#endif /*SOC_SHA_SUPPORTED*/
