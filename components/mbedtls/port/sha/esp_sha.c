/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "hal/sha_types.h"
#include "soc/soc_caps.h"
#include "esp_log.h"

#include <mbedtls/sha1.h>
#include <mbedtls/sha256.h>
#include <mbedtls/sha512.h>

#if SOC_SHA_SUPPORT_PARALLEL_ENG
#include "sha/sha_parallel_engine.h"
#elif SOC_SHA_SUPPORT_DMA
#include "sha/sha_dma.h"
#else
#include "sha/sha_block.h"
#endif

static const char *TAG = "esp_sha";

void esp_sha(esp_sha_type sha_type, const unsigned char *input, size_t ilen, unsigned char *output)
{
    union {
#if SOC_SHA_SUPPORT_SHA1
        mbedtls_sha1_context sha1;
#endif
#if SOC_SHA_SUPPORT_SHA256
        mbedtls_sha256_context sha256;
#endif
#if SOC_SHA_SUPPORT_SHA384 || SOC_SHA_SUPPORT_SHA512
        mbedtls_sha512_context sha512;
#endif
    } ctx;

    int ret __attribute__((unused));
    assert(input != NULL && output != NULL);

#if SOC_SHA_SUPPORT_SHA1
    if (sha_type == SHA1) {
        mbedtls_sha1_init(&ctx.sha1);
        mbedtls_sha1_starts(&ctx.sha1);
        ret = mbedtls_sha1_update(&ctx.sha1, input, ilen);
        assert(ret == 0);
        ret = mbedtls_sha1_finish(&ctx.sha1, output);
        assert(ret == 0);
        mbedtls_sha1_free(&ctx.sha1);
        return;
    }
#endif //SOC_SHA_SUPPORT_SHA1

#if SOC_SHA_SUPPORT_SHA256
    if (sha_type == SHA2_256) {
        mbedtls_sha256_init(&ctx.sha256);
        mbedtls_sha256_starts(&ctx.sha256, 0);
        ret = mbedtls_sha256_update(&ctx.sha256, input, ilen);
        assert(ret == 0);
        ret = mbedtls_sha256_finish(&ctx.sha256, output);
        assert(ret == 0);
        mbedtls_sha256_free(&ctx.sha256);
        return;
    }
#endif //SOC_SHA_SUPPORT_SHA256

#if SOC_SHA_SUPPORT_SHA384
    if (sha_type == SHA2_384) {
        mbedtls_sha512_init(&ctx.sha512);
        mbedtls_sha512_starts(&ctx.sha512, 1);
        ret = mbedtls_sha512_update(&ctx.sha512, input, ilen);
        assert(ret == 0);
        ret = mbedtls_sha512_finish(&ctx.sha512, output);
        assert(ret == 0);
        mbedtls_sha512_free(&ctx.sha512);
        return;
    }
#endif //SOC_SHA_SUPPORT_SHA384

#if SOC_SHA_SUPPORT_SHA512
    if (sha_type == SHA2_512) {
        mbedtls_sha512_init(&ctx.sha512);
        mbedtls_sha512_starts(&ctx.sha512, 0);
        ret = mbedtls_sha512_update(&ctx.sha512, input, ilen);
        assert(ret == 0);
        ret = mbedtls_sha512_finish(&ctx.sha512, output);
        assert(ret == 0);
        mbedtls_sha512_free(&ctx.sha512);
        return;
    }
#endif //SOC_SHA_SUPPORT_SHA512

    ESP_LOGE(TAG, "SHA type %d not supported", (int)sha_type);
    abort();
}
