/*
 * SPDX-FileCopyrightText: 2020-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_tls_crypto.h"
#include "esp_log.h"
#include "esp_err.h"
#include "sdkconfig.h"

#if CONFIG_ESP_TLS_USING_MBEDTLS
#include "mbedtls/base64.h"
#include "mbedtls/error.h"
#include "psa/crypto.h"
#endif

#if CONFIG_ESP_TLS_CUSTOM_STACK
#include "esp_tls_custom_stack.h"
#endif

#if CONFIG_ESP_TLS_USING_MBEDTLS
static int esp_crypto_sha1_mbedtls(const unsigned char *input,
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
    ESP_LOGE("esp_crypto", "Please enable CONFIG_MBEDTLS_SHA1_C or CONFIG_MBEDTLS_HARDWARE_SHA to support SHA1 operations");
    return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
#endif /*  CONFIG_MBEDTLS_SHA1_C || CONFIG_MBEDTLS_HARDWARE_SHA*/
}

static int esp_crypto_base64_encode_mbedtls(unsigned char *dst, size_t dlen,
                                            size_t *olen, const unsigned char *src,
                                            size_t slen)
{
    return mbedtls_base64_encode(dst, dlen, olen, src, slen);
}
#endif /* CONFIG_ESP_TLS_USING_MBEDTLS */

int esp_crypto_sha1(const unsigned char *input,
                    size_t ilen,
                    unsigned char output[20])
{
#if CONFIG_ESP_TLS_USING_MBEDTLS
    return esp_crypto_sha1_mbedtls(input, ilen, output);
#elif CONFIG_ESP_TLS_CUSTOM_STACK
    int ret = esp_tls_custom_stack_crypto_sha1(input, ilen, output);
    if (ret == ESP_ERR_NOT_SUPPORTED) {
        ESP_LOGE("esp_crypto", "Custom TLS stack must implement crypto_sha1 callback");
        return -1;
    }
    return ret;
#else
    ESP_LOGE("esp_crypto", "No TLS stack configured");
    return -1;
#endif
}

int esp_crypto_base64_encode(unsigned char *dst, size_t dlen, size_t *olen,
                             const unsigned char *src, size_t slen)
{
#if CONFIG_ESP_TLS_USING_MBEDTLS
    return esp_crypto_base64_encode_mbedtls(dst, dlen, olen, src, slen);
#elif CONFIG_ESP_TLS_CUSTOM_STACK
    int ret = esp_tls_custom_stack_crypto_base64_encode(dst, dlen, olen, src, slen);
    if (ret == ESP_ERR_NOT_SUPPORTED) {
        ESP_LOGE("esp_crypto", "Custom TLS stack must implement crypto_base64_encode callback");
        return -1;
    }
    return ret;
#else
    ESP_LOGE("esp_crypto", "No TLS stack configured");
    return -1;
#endif
}
