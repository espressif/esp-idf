// Copyright 2018-2019 Espressif Systems (Shanghai) PTE LTD
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

    int ret;
    assert(input != NULL && output != NULL);

#if SOC_SHA_SUPPORT_SHA1
    if (sha_type == SHA1) {
        mbedtls_sha1_init(&ctx.sha1);
        mbedtls_sha1_starts_ret(&ctx.sha1);
        ret = mbedtls_sha1_update_ret(&ctx.sha1, input, ilen);
        assert(ret == 0);
        ret = mbedtls_sha1_finish_ret(&ctx.sha1, output);
        assert(ret == 0);
        mbedtls_sha1_free(&ctx.sha1);
        return;
    }
#endif //SOC_SHA_SUPPORT_SHA1

#if SOC_SHA_SUPPORT_SHA256
    if (sha_type == SHA2_256) {
        mbedtls_sha256_init(&ctx.sha256);
        mbedtls_sha256_starts_ret(&ctx.sha256, 0);
        ret = mbedtls_sha256_update_ret(&ctx.sha256, input, ilen);
        assert(ret == 0);
        ret = mbedtls_sha256_finish_ret(&ctx.sha256, output);
        assert(ret == 0);
        mbedtls_sha256_free(&ctx.sha256);
        return;
    }
#endif //SOC_SHA_SUPPORT_SHA256

#if SOC_SHA_SUPPORT_SHA384
    if (sha_type == SHA2_384) {
        mbedtls_sha512_init(&ctx.sha512);
        mbedtls_sha512_starts_ret(&ctx.sha512, 1);
        ret = mbedtls_sha512_update_ret(&ctx.sha512, input, ilen);
        assert(ret == 0);
        ret = mbedtls_sha512_finish_ret(&ctx.sha512, output);
        assert(ret == 0);
        mbedtls_sha512_free(&ctx.sha512);
        return;
    }
#endif //SOC_SHA_SUPPORT_SHA384

#if SOC_SHA_SUPPORT_SHA512
    if (sha_type == SHA2_512) {
        mbedtls_sha512_init(&ctx.sha512);
        mbedtls_sha512_starts_ret(&ctx.sha512, 0);
        ret = mbedtls_sha512_update_ret(&ctx.sha512, input, ilen);
        assert(ret == 0);
        ret = mbedtls_sha512_finish_ret(&ctx.sha512, output);
        assert(ret == 0);
        mbedtls_sha512_free(&ctx.sha512);
        return;
    }
#endif //SOC_SHA_SUPPORT_SHA512

    ESP_LOGE(TAG, "SHA type %d not supported", sha_type);
    abort();
}
