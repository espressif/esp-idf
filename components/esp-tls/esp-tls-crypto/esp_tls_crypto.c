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
#include "mbedtls/sha1.h"
#include "mbedtls/base64.h"
#include "mbedtls/error.h"
#define _esp_crypto_sha1 esp_crypto_sha1_mbedtls
#define _esp_crypto_base64_encode esp_crypto_bas64_encode_mbedtls
#elif  CONFIG_ESP_TLS_USING_WOLFSSL
    #include "wolfssl/wolfcrypt/settings.h"
    #include "wolfssl/ssl.h" /* some SHA functions are listed in wolfssl/ssl.h */
    #if defined(CONFIG_ESP_WOLFSSL_OPENSSL_EXTRA) || defined(OPENSSL_EXTRA)
        #include "wolfssl/openssl/sha.h" /* old SHA functions only available with OpenSSL */
    #endif
    #include "wolfssl/wolfcrypt/coding.h"
    #define _esp_crypto_sha1 esp_crypto_sha1_wolfSSL
    #define _esp_crypto_base64_encode esp_crypto_base64_encode_woflSSL
#endif

#ifdef CONFIG_ESP_TLS_USING_MBEDTLS
static int esp_crypto_sha1_mbedtls( const unsigned char *input,
                                    size_t ilen,
                                    unsigned char output[20])
{
#if CONFIG_MBEDTLS_SHA1_C || CONFIG_MBEDTLS_HARDWARE_SHA
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    mbedtls_sha1_context ctx;

    mbedtls_sha1_init(&ctx);

    if ((ret = mbedtls_sha1_starts(&ctx)) != 0) {
        goto exit;
    }

    if ((ret = mbedtls_sha1_update(&ctx, input, ilen)) != 0) {
        goto exit;
    }

    if ((ret = mbedtls_sha1_finish(&ctx, output)) != 0) {
        goto exit;
    }

exit:
    mbedtls_sha1_free(&ctx);
    if (ret != 0) {
        ESP_LOGE(TAG, "Error in calculating sha1 sum , Returned 0x%02X", ret);
    }
    return ret;
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
    unsigned char *ret = wolfSSL_SHA1(input, ilen, output);
    if (ret == NULL) {
        ESP_LOGE(TAG, "Error in calculating sha1 sum");
        return -1;
    }
    return 0;
}

static int esp_crypto_base64_encode_woflSSL(unsigned char *dst, size_t dlen, size_t *olen,
        const unsigned char *src, size_t slen)
{
    *olen = dlen;
#if defined(CONFIG_ESP_TLS_USING_WOLFSSL) && (defined(WOLFSSL_BASE64_ENCODE) || !defined(OPENSSL_EXTRA))
    #if defined(WOLFSSL_BASE64_ENCODE_LINE_LEN)
        #error "WOLFSSL_BASE64_ENCODE_LINE_LEN is defined - Base64_Encode() will insert newlines. Rebuild without it."
    #endif
    #ifndef WOLFSSL_BASE64_ENCODE
        #warning "WOLFSSL_BASE64_ENCODE is missing - Base64_Encode() is unavailable."
    #endif
    return Base64_Encode((const byte *) src, (word32) slen, (byte *) dst, (word32 *) olen);
#else
    return Base64_Encode_NoNl((const byte *) src, (word32) slen, (byte *) dst, (word32 *) olen);
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
