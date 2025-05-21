/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: The Mbed TLS Contributors
 */

#include "sdkconfig.h"
#include "esp_ds.h"
#include "rsa_sign_alt.h"
#include "esp_memory_utils.h"

#ifdef SOC_DIG_SIGN_SUPPORTED
#include "rom/digital_signature.h"
#else
#error   "Selected target does not support esp_rsa_sign_alt (for DS)"
#endif

#include "esp_log.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <mbedtls/build_info.h>
static const char *TAG = "ESP_RSA_SIGN_ALT";
#define SWAP_INT32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24))

#include "mbedtls/rsa.h"
#include "mbedtls/oid.h"
#include "mbedtls/platform_util.h"
#include <string.h>

static hmac_key_id_t s_esp_ds_hmac_key_id;
static esp_ds_data_t *s_ds_data;
static SemaphoreHandle_t s_ds_lock;
static int s_timeout_ms = 0;

/* key length in bytes = (esp_digital_signature_length_t key + 1 ) * FACTOR_KEYLEN_IN_BYTES */
#define FACTOR_KEYLEN_IN_BYTES 4

/* Lock for the DS session, other TLS connections trying to use the DS peripheral will be blocked
 * till this DS session is completed (i.e. TLS handshake for this connection is completed) */
static void __attribute__((constructor)) esp_ds_conn_lock (void)
{
    if ((s_ds_lock = xSemaphoreCreateMutex()) == NULL) {
        ESP_EARLY_LOGE(TAG, "mutex for the DS session lock could not be created");
    }
}

void esp_ds_set_session_timeout(int timeout)
{
    /* add additional offset of 1000 ms to have enough time for deleting the TLS connection and free the previous ds context after exceeding timeout value (this offset also helps when timeout is set to 0) */
    if (timeout > s_timeout_ms) {
        s_timeout_ms = timeout + 1000;
    }
}

esp_err_t esp_ds_init_data_ctx(esp_ds_data_ctx_t *ds_data)
{
    if (ds_data == NULL || ds_data->esp_ds_data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    /* mutex is given back when the DS context is freed after the TLS handshake is completed or in case of failure (at cleanup) */
    if ((xSemaphoreTake(s_ds_lock, s_timeout_ms / portTICK_PERIOD_MS) != pdTRUE)) {
        ESP_LOGE(TAG, "ds_lock could not be obtained in specified time");
        return ESP_FAIL;
    }
    s_ds_data = ds_data->esp_ds_data;
    ESP_LOGD(TAG, "Using DS with key block %u, RSA length %u", ds_data->efuse_key_id, ds_data->rsa_length_bits);
    s_esp_ds_hmac_key_id = (hmac_key_id_t) ds_data->efuse_key_id;

    const unsigned rsa_length_int = (ds_data->rsa_length_bits / 32) - 1;
    if (esp_ptr_byte_accessible(s_ds_data)) {
        /* calculate the rsa_length in terms of esp_digital_signature_length_t which is required for the internal DS API */
        s_ds_data->rsa_length = rsa_length_int;
    } else if (s_ds_data->rsa_length != rsa_length_int) {
        /*
         * Configuration data is most likely from DROM segment and it
         * is not properly formatted for all parameters consideration.
         * Moreover, we can not modify as it is read-only and hence
         * the error.
         */
        ESP_LOGE(TAG, "RSA length mismatch %u, %u", s_ds_data->rsa_length, rsa_length_int);
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

void esp_ds_release_ds_lock(void)
{
    if (xSemaphoreGetMutexHolder(s_ds_lock) == xTaskGetCurrentTaskHandle()) {
        /* Give back the semaphore (DS lock) */
        xSemaphoreGive(s_ds_lock);
    }
}

size_t esp_ds_get_keylen(void *ctx)
{
    /* calculating the rsa_length in bytes */
    return ((s_ds_data->rsa_length + 1) * FACTOR_KEYLEN_IN_BYTES);
}

static int rsa_rsassa_pkcs1_v15_encode( mbedtls_md_type_t md_alg,
                                        unsigned int hashlen,
                                        const unsigned char *hash,
                                        size_t dst_len,
                                        unsigned char *dst )
{
    size_t oid_size  = 0;
    size_t nb_pad    = dst_len;
    unsigned char *p = dst;
    const char *oid  = NULL;

    /* Are we signing hashed or raw data? */
    if ( md_alg != MBEDTLS_MD_NONE ) {
        const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type( md_alg );
        if ( md_info == NULL ) {
            return ( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
        }

        if ( mbedtls_oid_get_oid_by_md( md_alg, &oid, &oid_size ) != 0 ) {
            return ( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
        }

        hashlen = mbedtls_md_get_size( md_info );

        /* Double-check that 8 + hashlen + oid_size can be used as a
         * 1-byte ASN.1 length encoding and that there's no overflow. */
        if ( 8 + hashlen + oid_size  >= 0x80         ||
                10 + hashlen            <  hashlen      ||
                10 + hashlen + oid_size <  10 + hashlen ) {
            return ( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
        }

        /*
         * Static bounds check:
         * - Need 10 bytes for five tag-length pairs.
         *   (Insist on 1-byte length encodings to protect against variants of
         *    Bleichenbacher's forgery attack against lax PKCS#1v1.5 verification)
         * - Need hashlen bytes for hash
         * - Need oid_size bytes for hash alg OID.
         */
        if ( nb_pad < 10 + hashlen + oid_size ) {
            return ( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
        }
        nb_pad -= 10 + hashlen + oid_size;
    } else {
        if ( nb_pad < hashlen ) {
            return ( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
        }

        nb_pad -= hashlen;
    }

    /* Need space for signature header and padding delimiter (3 bytes),
     * and 8 bytes for the minimal padding */
    if ( nb_pad < 3 + 8 ) {
        return ( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    }
    nb_pad -= 3;

    /* Now nb_pad is the amount of memory to be filled
     * with padding, and at least 8 bytes long. */

    /* Write signature header and padding */
    *p++ = 0;
    *p++ = MBEDTLS_RSA_SIGN;
    memset( p, 0xFF, nb_pad );
    p += nb_pad;
    *p++ = 0;

    /* Are we signing raw data? */
    if ( md_alg == MBEDTLS_MD_NONE ) {
        memcpy( p, hash, hashlen );
        return ( 0 );
    }

    /* Signing hashed data, add corresponding ASN.1 structure
     *
     * DigestInfo ::= SEQUENCE {
     *   digestAlgorithm DigestAlgorithmIdentifier,
     *   digest Digest }
     * DigestAlgorithmIdentifier ::= AlgorithmIdentifier
     * Digest ::= OCTET STRING
     *
     * Schematic:
     * TAG-SEQ + LEN [ TAG-SEQ + LEN [ TAG-OID  + LEN [ OID  ]
     *                                 TAG-NULL + LEN [ NULL ] ]
     *                 TAG-OCTET + LEN [ HASH ] ]
     */
    *p++ = MBEDTLS_ASN1_SEQUENCE | MBEDTLS_ASN1_CONSTRUCTED;
    *p++ = (unsigned char)( 0x08 + oid_size + hashlen );
    *p++ = MBEDTLS_ASN1_SEQUENCE | MBEDTLS_ASN1_CONSTRUCTED;
    *p++ = (unsigned char)( 0x04 + oid_size );
    *p++ = MBEDTLS_ASN1_OID;
    *p++ = (unsigned char) oid_size;
    memcpy( p, oid, oid_size );
    p += oid_size;
    *p++ = MBEDTLS_ASN1_NULL;
    *p++ = 0x00;
    *p++ = MBEDTLS_ASN1_OCTET_STRING;
    *p++ = (unsigned char) hashlen;
    memcpy( p, hash, hashlen );
    p += hashlen;

    /* Just a sanity-check, should be automatic
     * after the initial bounds check. */
    if ( p != dst + dst_len ) {
        mbedtls_platform_zeroize( dst, dst_len );
        return ( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    }

    return ( 0 );
}

#ifdef CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
static int mgf_mask(unsigned char *dst, size_t dlen, unsigned char *src,
                    size_t slen, mbedtls_md_type_t md_alg)
{
    unsigned char counter[4];
    unsigned char *p;
    unsigned int hlen;
    size_t i, use_len;
    unsigned char mask[MBEDTLS_MD_MAX_SIZE];
    int ret = 0;
    const mbedtls_md_info_t *md_info;
    mbedtls_md_context_t md_ctx;

    mbedtls_md_init(&md_ctx);
    md_info = mbedtls_md_info_from_type(md_alg);
    if (md_info == NULL) {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    mbedtls_md_init(&md_ctx);
    if ((ret = mbedtls_md_setup(&md_ctx, md_info, 0)) != 0) {
        goto exit;
    }

    hlen = mbedtls_md_get_size(md_info);

    memset(mask, 0, sizeof(mask));
    memset(counter, 0, 4);

    /* Generate and apply dbMask */
    p = dst;

    while (dlen > 0) {
        use_len = hlen;
        if (dlen < hlen) {
            use_len = dlen;
        }

        if ((ret = mbedtls_md_starts(&md_ctx)) != 0) {
            goto exit;
        }
        if ((ret = mbedtls_md_update(&md_ctx, src, slen)) != 0) {
            goto exit;
        }
        if ((ret = mbedtls_md_update(&md_ctx, counter, 4)) != 0) {
            goto exit;
        }
        if ((ret = mbedtls_md_finish(&md_ctx, mask)) != 0) {
            goto exit;
        }

        for (i = 0; i < use_len; ++i) {
            *p++ ^= mask[i];
        }

        counter[3]++;

        dlen -= use_len;
    }

exit:
    mbedtls_platform_zeroize(mask, sizeof(mask));
    mbedtls_md_free(&md_ctx);

    return ret;
}

static int hash_mprime(const unsigned char *hash, size_t hlen,
                    const unsigned char *salt, size_t slen,
                    unsigned char *out, mbedtls_md_type_t md_alg)
{
    const unsigned char zeros[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

    mbedtls_md_context_t md_ctx;
    int ret = MBEDTLS_ERR_RSA_BAD_INPUT_DATA;

    const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(md_alg);
    if (md_info == NULL) {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    mbedtls_md_init(&md_ctx);
    if ((ret = mbedtls_md_setup(&md_ctx, md_info, 0)) != 0) {
        goto exit;
    }
    if ((ret = mbedtls_md_starts(&md_ctx)) != 0) {
        goto exit;
    }
    if ((ret = mbedtls_md_update(&md_ctx, zeros, sizeof(zeros))) != 0) {
        goto exit;
    }
    if ((ret = mbedtls_md_update(&md_ctx, hash, hlen)) != 0) {
        goto exit;
    }
    if ((ret = mbedtls_md_update(&md_ctx, salt, slen)) != 0) {
        goto exit;
    }
    if ((ret = mbedtls_md_finish(&md_ctx, out)) != 0) {
        goto exit;
    }

exit:
    mbedtls_md_free(&md_ctx);

    return ret;
}

static int rsa_rsassa_pss_pkcs1_v21_encode( int (*f_rng)(void *, unsigned char *, size_t), void *p_rng,
                                            mbedtls_md_type_t md_alg,
                                            unsigned int hashlen,
                                            const unsigned char *hash,
                                            int saltlen,
                                            unsigned char *sig, size_t dst_len)
{
    size_t olen;
    unsigned char *p = sig;
    unsigned char *salt = NULL;
    size_t slen, min_slen, hlen, offset = 0;
    int ret = MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    size_t msb;

    if ((md_alg != MBEDTLS_MD_NONE || hashlen != 0) && hash == NULL) {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    if (f_rng == NULL) {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    olen = dst_len;

    if (md_alg != MBEDTLS_MD_NONE) {
        /* Gather length of hash to sign */
        size_t exp_hashlen = mbedtls_md_get_size_from_type(md_alg);
        if (exp_hashlen == 0) {
            return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
        }

        if (hashlen != exp_hashlen) {
            return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
        }
    }

    hlen = mbedtls_md_get_size_from_type(md_alg);
    if (hlen == 0) {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    if (saltlen == MBEDTLS_RSA_SALT_LEN_ANY) {
        /* Calculate the largest possible salt length, up to the hash size.
        * Normally this is the hash length, which is the maximum salt length
        * according to FIPS 185-4 �5.5 (e) and common practice. If there is not
        * enough room, use the maximum salt length that fits. The constraint is
        * that the hash length plus the salt length plus 2 bytes must be at most
        * the key length. This complies with FIPS 186-4 �5.5 (e) and RFC 8017
        * (PKCS#1 v2.2) �9.1.1 step 3. */
        min_slen = hlen - 2;
        if (olen < hlen + min_slen + 2) {
            return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
        } else if (olen >= hlen + hlen + 2) {
            slen = hlen;
        } else {
            slen = olen - hlen - 2;
        }
    } else if ((saltlen < 0) || (saltlen + hlen + 2 > olen)) {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    } else {
        slen = (size_t) saltlen;
    }

    memset(sig, 0, olen);

    /* Note: EMSA-PSS encoding is over the length of N - 1 bits */
    msb = dst_len * 8 - 1;
    p += olen - hlen - slen - 2;
    *p++ = 0x01;


    /* Generate salt of length slen in place in the encoded message */
    salt = p;
    if ((ret = f_rng(p_rng, salt, slen)) != 0) {
        return MBEDTLS_ERR_RSA_RNG_FAILED;
    }
    p += slen;

    /* Generate H = Hash( M' ) */
    ret = hash_mprime(hash, hashlen, salt, slen, p, md_alg);
    if (ret != 0) {
        return ret;
    }

    /* Compensate for boundary condition when applying mask */
    if (msb % 8 == 0) {
        offset = 1;
    }

    /* maskedDB: Apply dbMask to DB */
    ret = mgf_mask(sig + offset, olen - hlen - 1 - offset, p, hlen, md_alg);
    if (ret != 0) {
        return ret;
    }

    msb = dst_len * 8 - 1;
    sig[0] &= 0xFF >> (olen * 8 - msb);

    p += hlen;
    *p++ = 0xBC;
    return ret;
}

static int rsa_rsassa_pkcs1_v21_encode(int (*f_rng)(void *, unsigned char *, size_t), void *p_rng,
                                    mbedtls_md_type_t md_alg,
                                        unsigned int hashlen,
                                        const unsigned char *hash,
                                        size_t dst_len,
                                        unsigned char *dst )
{
    return rsa_rsassa_pss_pkcs1_v21_encode(f_rng, p_rng, md_alg, hashlen, hash, MBEDTLS_RSA_SALT_LEN_ANY, dst, dst_len);
}
#endif /* CONFIG_MBEDTLS_SSL_PROTO_TLS1_3 */

int esp_ds_rsa_sign( void *ctx,
                    int (*f_rng)(void *, unsigned char *, size_t), void *p_rng,
                    mbedtls_md_type_t md_alg, unsigned int hashlen,
                    const unsigned char *hash, unsigned char *sig )
{
    esp_ds_context_t *esp_ds_ctx = NULL;
    esp_err_t ds_r;
    int ret = -1;

    mbedtls_rsa_context *pk = (mbedtls_rsa_context *)ctx;

    const size_t data_len = s_ds_data->rsa_length + 1;
    const size_t sig_len = data_len * FACTOR_KEYLEN_IN_BYTES;

    if (pk->MBEDTLS_PRIVATE(padding) == MBEDTLS_RSA_PKCS_V21) {
#ifdef CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
        if ((ret = (rsa_rsassa_pkcs1_v21_encode(f_rng, p_rng ,md_alg, hashlen, hash, sig_len, sig ))) != 0) {
            ESP_LOGE(TAG, "Error in pkcs1_v21 encoding, returned %d", ret);
            return -1;
        }
#else /* CONFIG_MBEDTLS_SSL_PROTO_TLS1_3 */
        ESP_LOGE(TAG, "RSA PKCS#1 v2.1 padding is not supported. Please enable CONFIG_MBEDTLS_SSL_PROTO_TLS1_3");
        return -1;
#endif /* CONFIG_MBEDTLS_SSL_PROTO_TLS1_3 */
    } else {
        if ((ret = (rsa_rsassa_pkcs1_v15_encode(md_alg, hashlen, hash, sig_len, sig ))) != 0) {
            ESP_LOGE(TAG, "Error in pkcs1_v15 encoding, returned %d", ret);
            return -1;
        }
    }

    uint32_t *signature = heap_caps_malloc_prefer(sig_len, 2, MALLOC_CAP_32BIT | MALLOC_CAP_INTERNAL, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL);
    if (signature == NULL) {
        ESP_LOGE(TAG, "Could not allocate memory for internal DS operations");
        return -1;
    }

    for (unsigned int i = 0; i < (data_len); i++) {
        signature[i] = SWAP_INT32(((uint32_t *)sig)[(data_len) - (i + 1)]);
    }

    ds_r = esp_ds_start_sign((const void *)signature,
                            s_ds_data,
                            s_esp_ds_hmac_key_id,
                            &esp_ds_ctx);
    if (ds_r != ESP_OK) {
        ESP_LOGE(TAG, "Error in esp_ds_start_sign, returned %d ", ds_r);
        heap_caps_free(signature);
        return -1;
    }

    ds_r = esp_ds_finish_sign((void *)signature, esp_ds_ctx);
    if (ds_r != ESP_OK) {
        if (ds_r == ESP_ERR_HW_CRYPTO_DS_INVALID_DIGEST) {
            ESP_LOGE(TAG, "Invalid digest in DS data reported by esp_ds_finish_sign");
        } else {
            ESP_LOGE(TAG, "Error in esp_ds_finish_sign, returned %d ", ds_r);
        }
        heap_caps_free(signature);
        return -1;
    }

    for (unsigned int i = 0; i < (data_len); i++) {
        ((uint32_t *)sig)[i] = SWAP_INT32(((uint32_t *)signature)[(data_len) - (i + 1)]);
    }
    heap_caps_free(signature);
    return 0;
}
