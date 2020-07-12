// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
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
#include "bootloader_sha.h"
#include "bootloader_flash_priv.h"
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <sys/param.h>
#include <mbedtls/sha256.h>

bootloader_sha256_handle_t bootloader_sha256_start(void)
{
    mbedtls_sha256_context *ctx = (mbedtls_sha256_context *)malloc(sizeof(mbedtls_sha256_context));
    if (!ctx) {
        return NULL;
    }
    mbedtls_sha256_init(ctx);
    int ret = mbedtls_sha256_starts_ret(ctx, false);
    if (ret != 0) {
        return NULL;
    }
    return ctx;
}

void bootloader_sha256_data(bootloader_sha256_handle_t handle, const void *data, size_t data_len)
{
    assert(handle != NULL);
    mbedtls_sha256_context *ctx = (mbedtls_sha256_context *)handle;
    int ret = mbedtls_sha256_update_ret(ctx, data, data_len);
    assert(ret == 0);
}

void bootloader_sha256_finish(bootloader_sha256_handle_t handle, uint8_t *digest)
{
    assert(handle != NULL);
    mbedtls_sha256_context *ctx = (mbedtls_sha256_context *)handle;
    if (digest != NULL) {
        int ret = mbedtls_sha256_finish_ret(ctx, digest);
        assert(ret == 0);
    }
    mbedtls_sha256_free(ctx);
    free(handle);
    handle = NULL;
}
