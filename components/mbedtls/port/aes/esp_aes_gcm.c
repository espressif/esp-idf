/*
 * GCM block cipher, ESP DMA hardware accelerated version
 * Based on mbedTLS FIPS-197 compliant version.
 *
 * SPDX-FileCopyrightText: The Mbed TLS Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2016-2024 Espressif Systems (Shanghai) CO LTD
 */
/*
 *  The AES block cipher was designed by Vincent Rijmen and Joan Daemen.
 *
 *  http://csrc.nist.gov/encryption/aes/rijndael/Rijndael.pdf
 *  http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
 */
#include <string.h>

#include "aes/esp_aes.h"
#include "aes/esp_aes_gcm.h"
#include "esp_aes_internal.h"
#include "hal/aes_hal.h"

#include "mbedtls/aes.h"
#include "mbedtls/error.h"
#include "mbedtls/gcm.h"

#include "esp_heap_caps.h"
#include "esp_log.h"
#include "soc/soc_caps.h"
#include "soc/soc_memory_layout.h"

#include "sdkconfig.h"

#if SOC_AES_SUPPORT_DMA
#include "esp_aes_dma_priv.h"
#endif

#define ESP_PUT_BE64(a, val)                                    \
    do {                                                        \
        *(uint64_t*)(a) = __builtin_bswap64( (uint64_t)(val) ); \
    } while (0)

/* For simplicity limit the maximum amount of aad bytes to a single DMA descriptor
   This should cover all normal, e.g. mbedtls, use cases */
#define ESP_AES_GCM_AAD_MAX_BYTES 4080

static const char *TAG = "esp-aes-gcm";

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

/* Based on MbedTLS's implementation
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

    for ( i = 4; i > 0; i >>= 1 ) {
        uint32_t T = ( vl & 1 ) * 0xe1000000U;
        vl  = ( vh << 63 ) | ( vl >> 1 );
        vh  = ( vh >> 1 ) ^ ( (uint64_t) T << 32);

        ctx->HL[i] = vl;
        ctx->HH[i] = vh;
    }

    for ( i = 2; i <= 8; i *= 2 ) {
        uint64_t *HiL = ctx->HL + i, *HiH = ctx->HH + i;
        vh = *HiH;
        vl = *HiL;
        for ( j = 1; j < i; j++ ) {
            HiH[j] = vh ^ ctx->HH[j];
            HiL[j] = vl ^ ctx->HL[j];
        }
    }

    return ( 0 );
}
/*
 * Shoup's method for multiplication use this table with
 *      last4[x] = x times P^128
 * where x and last4[x] are seen as elements of GF(2^128) as in [MGV]
 */
static const uint32_t last4[16] = {
    0x00000000, 0x1c200000, 0x38400000, 0x24600000,
    0x70800000, 0x6ca00000, 0x48c00000, 0x54e00000,
    0xe1000000, 0xfd200000, 0xd9400000, 0xc5600000,
    0x91800000, 0x8da00000, 0xa9c00000, 0xb5e00000
};
/* Based on MbedTLS's implementation
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
    hi = x[15] >> 4;

    zh = ctx->HH[lo];
    zl = ctx->HL[lo];

    rem = (unsigned char) zl & 0xf;
    zl = ( zh << 60 ) | ( zl >> 4 );
    zh = ( zh >> 4 );
    zh ^= (uint64_t) last4[rem] << 32;
    zh ^= ctx->HH[hi];
    zl ^= ctx->HL[hi];

    for ( i = 14; i >= 0; i-- ) {
        lo = x[i] & 0xf;
        hi = x[i] >> 4;

        rem = (unsigned char) zl & 0xf;
        zl = ( zh << 60 ) | ( zl >> 4 );
        zh = ( zh >> 4 );
        zh ^= (uint64_t) last4[rem] << 32;
        zh ^= ctx->HH[lo];
        zl ^= ctx->HL[lo];

        rem = (unsigned char) zl & 0xf;
        zl = ( zh << 60 ) | ( zl >> 4 );
        zh = ( zh >> 4 );
        zh ^= (uint64_t) last4[rem] << 32;
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
    /* Fallback to software implementation of GCM operation when a non-AES
     * cipher is selected, as we support hardware acceleration only for a
     * GCM operation using AES cipher.
     */
#if defined(MBEDTLS_GCM_NON_AES_CIPHER_SOFT_FALLBACK)
    if (ctx->ctx_soft != NULL) {
        mbedtls_gcm_free_soft(ctx->ctx_soft);
        free(ctx->ctx_soft);
        ctx->ctx_soft = NULL;
    }

    if (cipher != MBEDTLS_CIPHER_ID_AES) {
        ctx->ctx_soft = (mbedtls_gcm_context_soft*) malloc(sizeof(mbedtls_gcm_context_soft));
        if (ctx->ctx_soft == NULL) {
            return MBEDTLS_ERR_CIPHER_ALLOC_FAILED;
        }
        mbedtls_gcm_init_soft(ctx->ctx_soft);
        return mbedtls_gcm_setkey_soft(ctx->ctx_soft, cipher, key, keybits);
    }
#endif

#if !SOC_AES_SUPPORT_AES_192
    if (keybits == 192) {
        return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
    }
#endif
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

#if SOC_AES_SUPPORT_DMA && CONFIG_MBEDTLS_AES_USE_INTERRUPT
    esp_aes_intr_alloc();
#endif

    ctx->gcm_state = ESP_AES_GCM_STATE_INIT;
}

/* Function to clear AES-GCM context */
void esp_aes_gcm_free( esp_gcm_context *ctx)
{
    if (ctx == NULL) {
        return;
    }
#if defined(MBEDTLS_GCM_NON_AES_CIPHER_SOFT_FALLBACK)
    if (ctx->ctx_soft != NULL) {
        mbedtls_gcm_free_soft(ctx->ctx_soft);
        free(ctx->ctx_soft);
        /* Note that the value of ctx->ctx_soft should be NULL'ed out
        and here it is taken care by the bzero call below */
    }
#endif
    bzero(ctx, sizeof(esp_gcm_context));
}

/* Setup AES-GCM */
int esp_aes_gcm_starts( esp_gcm_context *ctx,
                        int mode,
                        const unsigned char *iv,
                        size_t iv_len )
{
    if (!ctx) {
        ESP_LOGE(TAG, "No AES context supplied");
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }

#if defined(MBEDTLS_GCM_NON_AES_CIPHER_SOFT_FALLBACK)
    if (ctx->ctx_soft != NULL) {
        return mbedtls_gcm_starts_soft(ctx->ctx_soft, mode, iv, iv_len);
    }
#endif

    /* IV is limited to 2^32 bits, so 2^29 bytes */
    /* IV is not allowed to be zero length */
    if ( iv_len == 0 ||
            ( (uint32_t) iv_len  ) >> 29 != 0 ) {
        return ( MBEDTLS_ERR_GCM_BAD_INPUT );
    }

    if (!iv) {
        ESP_LOGE(TAG, "No IV supplied");
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }

    /* Initialize AES-GCM context */
    memset(ctx->ghash, 0, sizeof(ctx->ghash));
    ctx->data_len = 0;
    ctx->aad = NULL;
    ctx->aad_len = 0;

    ctx->iv = iv;
    ctx->iv_len = iv_len;
    ctx->mode = mode;

    /* H and the lookup table are only generated once per ctx */
    if (ctx->gcm_state == ESP_AES_GCM_STATE_INIT) {
        /* Lock the AES engine to calculate ghash key H in hardware */
#if CONFIG_MBEDTLS_HARDWARE_GCM
        esp_aes_acquire_hardware();
        ctx->aes_ctx.key_in_hardware = aes_hal_setkey(ctx->aes_ctx.key, ctx->aes_ctx.key_bytes, mode);
        aes_hal_mode_init(ESP_AES_BLOCK_MODE_GCM);

        aes_hal_gcm_calc_hash(ctx->H);

        esp_aes_release_hardware();
#else
        memset(ctx->H, 0, sizeof(ctx->H));
        int ret = esp_aes_crypt_ecb(&ctx->aes_ctx, MBEDTLS_AES_ENCRYPT, ctx->H, ctx->H);
        if (ret != 0) {
            return ret;
        }
#endif
        gcm_gen_table(ctx);
    }

    /* Once H is obtained we need to derive J0 (Initial Counter Block) */
    esp_gcm_derive_J0(ctx);

    /* The initial counter block keeps updating during the esp_gcm_update call
     * however to calculate final authentication tag T we need original J0
     * so we make a copy here
     */
    memcpy(ctx->ori_j0, ctx->J0, 16);

    ctx->gcm_state = ESP_AES_GCM_STATE_START;

    return ( 0 );
}

int esp_aes_gcm_update_ad( esp_gcm_context *ctx,
                           const unsigned char *aad,
                           size_t aad_len )
{
    if (!ctx) {
        ESP_LOGE(TAG, "No AES context supplied");
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }

#if defined(MBEDTLS_GCM_NON_AES_CIPHER_SOFT_FALLBACK)
    if (ctx->ctx_soft != NULL) {
        return mbedtls_gcm_update_ad_soft(ctx->ctx_soft, aad, aad_len);
    }
#endif

    /* AD are limited to 2^32 bits, so 2^29 bytes */
    if ( ( (uint32_t) aad_len ) >> 29 != 0 ) {
        return ( MBEDTLS_ERR_GCM_BAD_INPUT );
    }

    if ( (aad_len > 0) && !aad) {
        ESP_LOGE(TAG, "No aad supplied");
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }

    if (ctx->gcm_state != ESP_AES_GCM_STATE_START) {
        ESP_LOGE(TAG, "AES context in invalid state!");
        return -1;
    }

    /* Initialise associated data */
    ctx->aad = aad;
    ctx->aad_len = aad_len;

    esp_gcm_ghash(ctx, ctx->aad, ctx->aad_len, ctx->ghash);

    return ( 0 );
}

/* Perform AES-GCM operation */
int esp_aes_gcm_update( esp_gcm_context *ctx,
                        const unsigned char *input, size_t input_length,
                        unsigned char *output, size_t output_size,
                        size_t *output_length )
{
    if (!ctx) {
        ESP_LOGE(TAG, "No GCM context supplied");
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }

#if defined(MBEDTLS_GCM_NON_AES_CIPHER_SOFT_FALLBACK)
    if (ctx->ctx_soft != NULL) {
        return mbedtls_gcm_update_soft(ctx->ctx_soft, input, input_length, output, output_size, output_length);
    }
#endif

    size_t nc_off = 0;
    uint8_t nonce_counter[AES_BLOCK_BYTES] = {0};
    uint8_t stream[AES_BLOCK_BYTES] = {0};

    if (!output_length) {
        ESP_LOGE(TAG, "No output length supplied");
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }
    *output_length = input_length;

    if (!input) {
        ESP_LOGE(TAG, "No input supplied");
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }
    if (!output) {
        ESP_LOGE(TAG, "No output supplied");
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }

    if ( output > input && (size_t) ( output - input ) < input_length ) {
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
        esp_gcm_ghash(ctx, input, input_length, ctx->ghash);
    }

    /* Output = GCTR(J0, Input): Encrypt/Decrypt the input */
    int ret = esp_aes_crypt_ctr(&ctx->aes_ctx, input_length, &nc_off, nonce_counter, stream, input, output);
    if (ret != 0) {
        return ret;
    }

    /* ICB gets auto incremented after GCTR operation here so update the context */
    memcpy(ctx->J0, nonce_counter, AES_BLOCK_BYTES);

    /* Keep updating the length counter for final tag calculation */
    ctx->data_len += input_length;

    /* Perform intermediate GHASH on "encrypted" data during encryption*/
    if (ctx->mode == ESP_AES_ENCRYPT) {
        esp_gcm_ghash(ctx, output, input_length, ctx->ghash);
    }

    return 0;
}

/* Function to read the tag value */
int esp_aes_gcm_finish( esp_gcm_context *ctx,
                        unsigned char *output, size_t output_size,
                        size_t *output_length,
                        unsigned char *tag, size_t tag_len )
{
#if defined(MBEDTLS_GCM_NON_AES_CIPHER_SOFT_FALLBACK)
    if (ctx->ctx_soft != NULL) {
        return mbedtls_gcm_finish_soft(ctx->ctx_soft, output, output_size, output_length, tag, tag_len);
    }
#endif
    size_t nc_off = 0;
    uint8_t len_block[AES_BLOCK_BYTES] = {0};
    uint8_t stream[AES_BLOCK_BYTES] = {0};

    if ( tag_len > 16 || tag_len < 4 ) {
        return ( MBEDTLS_ERR_GCM_BAD_INPUT );
    }

    /* Calculate final GHASH on aad_len, data length */
    ESP_PUT_BE64(len_block, ctx->aad_len * 8);
    ESP_PUT_BE64(len_block + 8, ctx->data_len * 8);
    esp_gcm_ghash(ctx, len_block, AES_BLOCK_BYTES, ctx->ghash);

    /* Tag T = GCTR(J0, ) where T is truncated to tag_len */
    return esp_aes_crypt_ctr(&ctx->aes_ctx, tag_len, &nc_off, ctx->ori_j0, stream, ctx->ghash, tag);
}

#if CONFIG_MBEDTLS_HARDWARE_GCM
/* Due to restrictions in the hardware (e.g. need to do the whole conversion in one go),
   some combinations of inputs are not supported */
static bool esp_aes_gcm_input_support_hw_accel(size_t length, const unsigned char *aad, size_t aad_len,
                                               const unsigned char *input, unsigned char *output)
{
    bool support_hw_accel = true;

    if (aad_len > ESP_AES_GCM_AAD_MAX_BYTES) {
        support_hw_accel = false;
    } else if (!esp_ptr_dma_capable(aad) && aad_len > 0) {
        /* aad in non internal DMA memory */
        support_hw_accel = false;
    } else if (!esp_ptr_dma_capable(input) && length > 0) {
        /* input in non internal DMA memory */
        support_hw_accel = false;
    } else if (!esp_ptr_dma_capable(output) && length > 0) {
        /* output in non internal DMA memory */
        support_hw_accel = false;
    } else if (length == 0) {
        support_hw_accel = false;
    }


    return support_hw_accel;
}
#endif

static int esp_aes_gcm_crypt_and_tag_partial_hw( esp_gcm_context *ctx,
        int mode,
        size_t length,
        const unsigned char *iv,
        size_t iv_len,
        const unsigned char *aad,
        size_t aad_len,
        const unsigned char *input,
        unsigned char *output,
        size_t tag_len,
        unsigned char *tag )
{
    int ret = 0;
    size_t olen;

    if ( ( ret = esp_aes_gcm_starts( ctx, mode, iv, iv_len ) ) != 0 ) {
        return ( ret );
    }

    if ( ( ret =  esp_aes_gcm_update_ad( ctx, aad, aad_len ) ) != 0 ) {
        return ( ret );
    }

    if ( ( ret = esp_aes_gcm_update( ctx, input, length, output, 0, &olen ) ) != 0 ) {
        return ( ret );
    }

    if ( ( ret = esp_aes_gcm_finish( ctx, output, 0, &olen, tag, tag_len ) ) != 0 ) {
        return ( ret );
    }

    return ret;
}

int esp_aes_gcm_crypt_and_tag( esp_gcm_context *ctx,
                               int mode,
                               size_t length,
                               const unsigned char *iv,
                               size_t iv_len,
                               const unsigned char *aad,
                               size_t aad_len,
                               const unsigned char *input,
                               unsigned char *output,
                               size_t tag_len,
                               unsigned char *tag )
{
    if (!ctx) {
        ESP_LOGE(TAG, "No AES context supplied");
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }

#if defined(MBEDTLS_GCM_NON_AES_CIPHER_SOFT_FALLBACK)
    if (ctx->ctx_soft != NULL) {
        return mbedtls_gcm_crypt_and_tag_soft(ctx->ctx_soft, mode, length, iv, iv_len, aad, aad_len, input, output, tag_len, tag);
    }
#endif
#if CONFIG_MBEDTLS_HARDWARE_GCM
    int ret;
    size_t remainder_bit;

    /* Due to hardware limitation only certain cases are fully supported in HW */
    if (!esp_aes_gcm_input_support_hw_accel(length, aad, aad_len, input, output)) {
        return esp_aes_gcm_crypt_and_tag_partial_hw(ctx, mode, length, iv, iv_len, aad, aad_len, input, output, tag_len, tag);
    }

    /*  Limit aad len to a single DMA descriptor to simplify DMA handling
        In practice, e.g. with mbedtls the length of aad will always be short
        the size field has 12 bits, but 0 not for 4096.
        to avoid possible problem when the size is not word-aligned, we only use 4096-4 per desc.
        Maximum size of data in the buffer that a DMA descriptor can hold.
    */
    if (aad_len > DMA_DESCRIPTOR_BUFFER_MAX_SIZE_4B_ALIGNED) {
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }
    /* IV and AD are limited to 2^32 bits, so 2^29 bytes */
    /* IV is not allowed to be zero length */
    if ( iv_len == 0 ||
            ( (uint32_t) iv_len  ) >> 29 != 0 ||
            ( (uint32_t) aad_len ) >> 29 != 0 ) {
        return ( MBEDTLS_ERR_GCM_BAD_INPUT );
    }

    if (!iv) {
        ESP_LOGE(TAG, "No IV supplied");
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }

    if ( (aad_len > 0) && !aad) {
        ESP_LOGE(TAG, "No aad supplied");
        return MBEDTLS_ERR_GCM_BAD_INPUT;
    }

    /* Initialize AES-GCM context */
    memset(ctx->ghash, 0, sizeof(ctx->ghash));
    ctx->data_len = 0;

    ctx->iv = iv;
    ctx->iv_len = iv_len;
    ctx->aad = aad;
    ctx->aad_len = aad_len;
    ctx->mode = mode;

    esp_aes_acquire_hardware();
    ctx->aes_ctx.key_in_hardware = 0;
    ctx->aes_ctx.key_in_hardware = aes_hal_setkey(ctx->aes_ctx.key, ctx->aes_ctx.key_bytes, mode);

    aes_hal_mode_init(ESP_AES_BLOCK_MODE_GCM);

    /* See TRM GCM chapter for description of this calculation */
    remainder_bit = (8 * length) % 128;
    aes_hal_gcm_init( (aad_len + AES_BLOCK_BYTES - 1) / AES_BLOCK_BYTES, remainder_bit);
    aes_hal_gcm_calc_hash(ctx->H);

    gcm_gen_table(ctx);
    esp_gcm_derive_J0(ctx);

    aes_hal_gcm_set_j0(ctx->J0);

    ret = esp_aes_process_dma_gcm(&ctx->aes_ctx, input, output, length, aad, aad_len);
    if (ret != 0) {
        esp_aes_release_hardware();
        return ret;
    }

    aes_hal_gcm_read_tag(tag, tag_len);

    esp_aes_release_hardware();

    return ( ret );
#else
    return esp_aes_gcm_crypt_and_tag_partial_hw(ctx, mode, length, iv, iv_len, aad, aad_len, input, output, tag_len, tag);
#endif
}


int esp_aes_gcm_auth_decrypt( esp_gcm_context *ctx,
                              size_t length,
                              const unsigned char *iv,
                              size_t iv_len,
                              const unsigned char *aad,
                              size_t aad_len,
                              const unsigned char *tag,
                              size_t tag_len,
                              const unsigned char *input,
                              unsigned char *output )
{
#if defined(MBEDTLS_GCM_NON_AES_CIPHER_SOFT_FALLBACK)
    if (ctx->ctx_soft != NULL) {
        return mbedtls_gcm_auth_decrypt_soft(ctx->ctx_soft, length, iv, iv_len, aad, aad_len, tag, tag_len, input, output);
    }
#endif
    int ret;
    unsigned char check_tag[16];
    size_t i;
    int diff;

    if ( ( ret = esp_aes_gcm_crypt_and_tag( ctx, ESP_AES_DECRYPT, length,
                                            iv, iv_len, aad, aad_len,
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
