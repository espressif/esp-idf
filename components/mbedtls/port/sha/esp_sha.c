/*
 * SPDX-FileCopyrightText: 2018-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "hal/sha_hal.h"
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
#if SOC_SHA_SUPPORT_SHA224 || SOC_SHA_SUPPORT_SHA256
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

#if SOC_SHA_SUPPORT_SHA224
    if (sha_type == SHA2_224) {
        mbedtls_sha256_init(&ctx.sha256);
        mbedtls_sha256_starts(&ctx.sha256, 1);
        ret = mbedtls_sha256_update(&ctx.sha256, input, ilen);
        assert(ret == 0);
        ret = mbedtls_sha256_finish(&ctx.sha256, output);
        assert(ret == 0);
        mbedtls_sha256_free(&ctx.sha256);
        return;
    }
#endif //SOC_SHA_SUPPORT_SHA224

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


#if SOC_SHA_SUPPORT_SHA512_T

/* The initial hash value for SHA512/t is generated according to the
   algorithm described in the TRM, chapter SHA-Accelerator
*/
int esp_sha_512_t_init_hash(uint16_t t)
{
    uint32_t t_string = 0;
    uint8_t t0, t1, t2, t_len;

    if (t == 384) {
        ESP_LOGE(TAG, "Invalid t for SHA512/t, t = %u,cannot be 384", t);
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
        ESP_LOGE(TAG, "Invalid t for SHA512/t, t = %u, must equal or less than 512", t);
        return -1;
    }

    sha_hal_sha512_init_hash(t_string, t_len);

    return 0;
}

#endif //SOC_SHA_SUPPORT_SHA512_T
