/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
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
    int ret = mbedtls_sha256_starts(ctx, false);
    if (ret != 0) {
        return NULL;
    }
    return ctx;
}

void bootloader_sha256_data(bootloader_sha256_handle_t handle, const void *data, size_t data_len)
{
    assert(handle != NULL);
    mbedtls_sha256_context *ctx = (mbedtls_sha256_context *)handle;
    int ret = mbedtls_sha256_update(ctx, data, data_len);
    assert(ret == 0);
    (void)ret;
}

void bootloader_sha256_finish(bootloader_sha256_handle_t handle, uint8_t *digest)
{
    assert(handle != NULL);
    mbedtls_sha256_context *ctx = (mbedtls_sha256_context *)handle;
    if (digest != NULL) {
        int ret = mbedtls_sha256_finish(ctx, digest);
        assert(ret == 0);
        (void)ret;
    }
    mbedtls_sha256_free(ctx);
    free(handle);
    handle = NULL;
}
