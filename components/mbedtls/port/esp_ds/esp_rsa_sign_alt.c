/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_ds.h"
#include "rsa_sign_alt.h"
#include "esp_memory_utils.h"

#ifdef CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/digital_signature.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/digital_signature.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/digital_signature.h"
#elif CONFIG_IDF_TARGET_ESP32C6
#include "esp32c6/rom/digital_signature.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/rom/digital_signature.h"
#elif CONFIG_IDF_TARGET_ESP32P4
#include "esp32p4/rom/digital_signature.h"
#elif CONFIG_IDF_TARGET_ESP32C5
#include "esp32c5/rom/digital_signature.h"
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


int esp_ds_rsa_sign( void *ctx,
                     int (*f_rng)(void *, unsigned char *, size_t), void *p_rng,
                     mbedtls_md_type_t md_alg, unsigned int hashlen,
                     const unsigned char *hash, unsigned char *sig )
{
    esp_ds_context_t *esp_ds_ctx;
    esp_err_t ds_r;
    int ret = -1;
    uint32_t *signature = heap_caps_malloc_prefer((s_ds_data->rsa_length + 1) * FACTOR_KEYLEN_IN_BYTES, 2, MALLOC_CAP_32BIT | MALLOC_CAP_INTERNAL, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL);
    if (signature == NULL) {
        ESP_LOGE(TAG, "Could not allocate memory for internal DS operations");
        return -1;
    }

    if ((ret = (rsa_rsassa_pkcs1_v15_encode( md_alg, hashlen, hash, ((s_ds_data->rsa_length + 1) * FACTOR_KEYLEN_IN_BYTES), sig ))) != 0) {
        ESP_LOGE(TAG, "Error in pkcs1_v15 encoding, returned %d", ret);
        heap_caps_free(signature);
        return -1;
    }

    for (unsigned int i = 0; i < (s_ds_data->rsa_length + 1); i++) {
        signature[i] = SWAP_INT32(((uint32_t *)sig)[(s_ds_data->rsa_length + 1) - (i + 1)]);
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

    for (unsigned int i = 0; i < (s_ds_data->rsa_length + 1); i++) {
        ((uint32_t *)sig)[i] = SWAP_INT32(((uint32_t *)signature)[(s_ds_data->rsa_length + 1) - (i + 1)]);
    }
    heap_caps_free(signature);
    return 0;
}
