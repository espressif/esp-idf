// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "esp_tls_crypto.h"
#include "esp_log.h"
#include "esp_err.h"
static const char *TAG = "esp_crypto";
#ifdef CONFIG_ESP_TLS_USING_MBEDTLS
#include "mbedtls/sha1.h"
#include "mbedtls/base64.h"
#define _esp_crypto_sha1 esp_crypto_sha1_mbedtls
#define _esp_crypto_base64_encode esp_crypto_bas64_encode_mbedtls
#elif  CONFIG_ESP_TLS_USING_WOLFSSL
#include "wolfssl/ssl.h" /* SHA functions are listed in wolfssl/ssl.h */
#include "wolfssl/wolfcrypt/coding.h"
#define _esp_crypto_sha1 esp_crypto_sha1_wolfSSL
#define _esp_crypto_base64_encode esp_crypto_base64_encode_woflSSL
#endif

#ifdef CONFIG_ESP_TLS_USING_MBEDTLS
static int esp_crypto_sha1_mbedtls( const unsigned char *input,
                                    size_t ilen,
                                    unsigned char output[20])
{
    int ret = mbedtls_sha1_ret(input, ilen, output);
    if (ret != 0) {
        ESP_LOGE(TAG, "Error in calculating sha1 sum , Returned 0x%02X", ret);
    }
    return ret;
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
    return Base64_Encode((const byte *) src, (word32) slen, (byte *) dst, (word32 *) olen);
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
