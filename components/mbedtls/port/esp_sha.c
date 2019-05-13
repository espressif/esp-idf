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

#include "esp32/sha.h"
#include <mbedtls/sha1.h>
#include <mbedtls/sha256.h>
#include <mbedtls/sha512.h>

void esp_sha(esp_sha_type sha_type, const unsigned char *input, size_t ilen, unsigned char *output)
{
    int ret;
    assert(input != NULL && output != NULL);

    if (sha_type == SHA1) {

        mbedtls_sha1_context *ctx1 = (mbedtls_sha1_context *)malloc(sizeof(mbedtls_sha1_context));
        assert(ctx1 != NULL);
        mbedtls_sha1_starts_ret(ctx1);
        ret = mbedtls_sha1_update_ret(ctx1, input, ilen);
        assert(ret == 0);
        ret = mbedtls_sha1_finish_ret(ctx1, output);
        assert(ret == 0);
        mbedtls_sha1_free(ctx1);
        free(ctx1);

    } else if (sha_type == SHA2_256) {

        mbedtls_sha256_context *ctx256 = (mbedtls_sha256_context *)malloc(sizeof(mbedtls_sha256_context));
        assert(ctx256 != NULL);
        mbedtls_sha256_starts_ret(ctx256, 0);
        ret = mbedtls_sha256_update_ret(ctx256, input, ilen);
        assert(ret == 0);
        ret = mbedtls_sha256_finish_ret(ctx256, output);
        assert(ret == 0);
        mbedtls_sha256_free(ctx256);
        free(ctx256);

    } else if (sha_type == SHA2_384) {

        mbedtls_sha512_context *ctx384 = (mbedtls_sha512_context *)malloc(sizeof(mbedtls_sha512_context));
        assert(ctx384 != NULL);
        mbedtls_sha512_starts_ret(ctx384, 1);
        ret = mbedtls_sha512_update_ret(ctx384, input, ilen);
        assert(ret == 0);
        ret = mbedtls_sha512_finish_ret(ctx384, output);
        assert(ret == 0);
        mbedtls_sha512_free(ctx384);
        free(ctx384);

    } else if (sha_type == SHA2_512) {

        mbedtls_sha512_context *ctx512 = (mbedtls_sha512_context *)malloc(sizeof(mbedtls_sha512_context));
        assert(ctx512 != NULL);
        mbedtls_sha512_starts_ret(ctx512, 0);
        ret = mbedtls_sha512_update_ret(ctx512, input, ilen);
        assert(ret == 0);
        ret = mbedtls_sha512_finish_ret(ctx512, output);
        assert(ret == 0);
        mbedtls_sha512_free(ctx512);
        free(ctx512);

    }

}
