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

#include <stdio.h>
#include <string.h>
#include "mbedtls/md5.h"
#include "mbedtls/platform_util.h"

#if defined(MBEDTLS_MD5_ALT)
#include "md/esp_md.h"

int esp_md5_finish_ret( mbedtls_md5_context *ctx, unsigned char output[16] )
{
    esp_rom_md5_final(output, ctx);

    return 0;
}

int esp_md5_update_ret( mbedtls_md5_context *ctx, const unsigned char *input, size_t ilen )
{
    esp_rom_md5_update(ctx, input, ilen);

    return 0;
}

int esp_md5_init_ret( mbedtls_md5_context *ctx )
{
    esp_rom_md5_init(ctx);

    return 0;
}

void esp_md5_finish( mbedtls_md5_context *ctx, unsigned char output[16] )
{
    esp_md5_finish_ret(ctx, output);
}

void esp_md5_update( mbedtls_md5_context *ctx, const unsigned char *input, size_t ilen )
{
    esp_md5_update_ret(ctx, input, ilen);
}

void esp_md5_init( mbedtls_md5_context *ctx )
{
    esp_md5_init_ret(ctx);
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
    esp_md5_update_ret(ctx, data, 64);

    return 0;
}

void esp_md5_clone( mbedtls_md5_context *dst, const mbedtls_md5_context *src )
{
    *dst = *src;
}
#endif
