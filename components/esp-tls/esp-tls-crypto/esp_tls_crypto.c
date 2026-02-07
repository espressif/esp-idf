/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_tls_crypto.h"
#include "esp_log.h"
#include "esp_err.h"
#include "sdkconfig.h"
__attribute__((unused)) static const char *TAG = "esp_crypto";
#ifdef CONFIG_ESP_TLS_USING_MBEDTLS
#include "mbedtls/base64.h"
#include "mbedtls/error.h"
#include "psa/crypto.h"
#define _esp_crypto_sha1 esp_crypto_sha1_mbedtls
#define _esp_crypto_base64_encode esp_crypto_bas64_encode_mbedtls
#elif  CONFIG_ESP_TLS_USING_WOLFSSL
    #include "wolfssl/wolfcrypt/settings.h"
    #include "wolfssl/ssl.h" /* some SHA functions are listed in wolfssl/ssl.h */
    #include "wolfssl/wolfcrypt/coding.h"
    #define _esp_crypto_sha1 esp_crypto_sha1_wolfSSL
    #define _esp_crypto_base64_encode esp_crypto_base64_encode_wolfSSL
#endif

#ifdef CONFIG_ESP_TLS_USING_MBEDTLS
static int esp_crypto_sha1_mbedtls( const unsigned char *input,
                                    size_t ilen,
                                    unsigned char output[20])
{
#if CONFIG_MBEDTLS_SHA1_C || CONFIG_MBEDTLS_HARDWARE_SHA
    psa_hash_operation_t ctx = PSA_HASH_OPERATION_INIT;

    psa_status_t status = psa_hash_setup(&ctx, PSA_ALG_SHA_1);
    if (status != PSA_SUCCESS) {
        goto exit;
    }

    if ((status = psa_hash_update(&ctx, input, ilen)) != PSA_SUCCESS) {
        goto exit;
    }

    size_t hash_len;
    if ((status = psa_hash_finish(&ctx, output, 20, &hash_len)) != PSA_SUCCESS) {
        goto exit;
    }

exit:
    psa_hash_abort(&ctx);
    return status == PSA_SUCCESS ? 0 : -1;
#else
    ESP_LOGE(TAG, "Please enable CONFIG_MBEDTLS_SHA1_C or CONFIG_MBEDTLS_HARDWARE_SHA to support SHA1 operations");
    return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
#endif /*  CONFIG_MBEDTLS_SHA1_C || CONFIG_MBEDTLS_HARDWARE_SHA*/
}

static int esp_crypto_bas64_encode_mbedtls( unsigned char *dst, size_t dlen,
        size_t *olen, const unsigned char *src,
        size_t slen)
{
    return mbedtls_base64_encode(dst, dlen, olen, src, slen);
}

#elif CONFIG_ESP_TLS_USING_WOLFSSL
static int esp_crypto_sha1_wolfSSL( const unsigned char *input,
                                    size_t ilen,
                                    unsigned char output[20])
{
#ifndef NO_SHA
    int ret = 0;
    if ((ilen > 0 && input == NULL) || output == NULL)
        return BAD_FUNC_ARG;
    if (ilen > UINT32_MAX)
        return BAD_FUNC_ARG;

    ret = wc_ShaHash(input, (word32)ilen, output);
    return ret;
#else
    /* SHA-1 not compiled in this build */
    (void)input; (void)ilen; (void)output;
    return NOT_COMPILED_IN;
#endif
}

static int esp_crypto_base64_encode_wolfSSL(unsigned char *dst, size_t dlen, size_t *olen,
        const unsigned char *src, size_t slen)
{
    size_t req;
    if (olen == NULL) {
        return BAD_FUNC_ARG; /* wolfSSL error code. See error-crypt.h */
    }
    if ((slen > UINT32_MAX) || (dlen > UINT32_MAX)) {
        return BAD_FUNC_ARG; /* wolfSSL error code */
    }
    if ((slen > 0 && src == NULL) || (dlen > 0 && dst == NULL)) {
        return BAD_FUNC_ARG;
    }
    req = ((slen + 2u) / 3u) * 4u;
    if (dlen < req) {
        *olen = req;     /* tell caller capacity needed */
        return BUFFER_E; /* buffer too small */
    }
#if defined(WOLFSSL_BASE64_ENCODE)
    *olen = dlen;
    return Base64_Encode_NoNl((const byte *) src, (word32) slen, (byte *) dst, (word32 *) olen);
#else
    /* WOLFSSL_BASE64_ENCODE is typically enabled with OPENSSL_EXTRA,
     * or can be manually enabled in user_settings.h   */
    #error "WOLFSSL_BASE64_ENCODE is missing - Base64_Encode_NoNl() is unavailable."
#endif
}

#else
#error "No TLS/SSL Stack selected"
#endif

int esp_crypto_sha1( const unsigned char *input,
                     size_t ilen,
                     unsigned char output[20])
{
    return _esp_crypto_sha1(input, ilen, output);
}

int esp_crypto_base64_encode(unsigned char *dst, size_t dlen, size_t *olen,
                             const unsigned char *src, size_t slen )
{
    return _esp_crypto_base64_encode(dst, dlen, olen, src, slen);
}
