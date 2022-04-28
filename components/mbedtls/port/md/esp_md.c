/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include "mbedtls/md5.h"
#include "mbedtls/platform_util.h"

#if defined(MBEDTLS_MD5_ALT)
#include "md/esp_md.h"

int esp_md5_finish( mbedtls_md5_context *ctx, unsigned char output[16] )
{
    esp_rom_md5_final(output, ctx);


    return 0;
}

int esp_md5_update( mbedtls_md5_context *ctx, const unsigned char *input, size_t ilen )
{
    esp_rom_md5_update(ctx, input, ilen);


    return 0;
}

void esp_md5_init( mbedtls_md5_context *ctx )
{
    esp_rom_md5_init(ctx);
}

int esp_md5_starts( mbedtls_md5_context *ctx )
{
    esp_md5_init(ctx);
    return 0;
}

void esp_md5_free( mbedtls_md5_context *ctx )
{
    if (ctx == NULL) {
        return;
    }

    mbedtls_platform_zeroize( ctx, sizeof( mbedtls_md5_context ) );
}

int esp_md5_process( mbedtls_md5_context *ctx, const unsigned char data[64] )
{
    esp_md5_update(ctx, data, 64);

    return 0;
}

void esp_md5_clone( mbedtls_md5_context *dst, const mbedtls_md5_context *src )
{
    *dst = *src;
}
#endif
