/*
 *  FIPS-180-2 compliant SHA-384/512 implementation
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */
/*
 *  The SHA-512 Secure Hash Standard was published by NIST in 2002.
 *
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
 */
#include "port/sha512_alt.h"

#if defined(ESP_SHA512_C)
#include <string.h>

/* Implementation that should never be optimized out by the compiler */
static void sha512_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
}

void sha512_init( SHA512_CTX *ctx )
{
    memset( ctx, 0, sizeof( SHA512_CTX ) );
	ets_sha_enable();
}

void sha512_free( SHA512_CTX *ctx )
{
    if( ctx == NULL )
        return;

    sha512_zeroize( ctx, sizeof( SHA512_CTX ) );
	ets_sha_disable();
}

void sha512_clone( SHA512_CTX *dst, const SHA512_CTX *src )
{
    *dst = *src;
}

/*
 * SHA-512 context setup
 */
void sha512_starts( SHA512_CTX *ctx, int is384 )
{
	ets_sha_init(&ctx->context);
    if( is384 == 0 )
    {
        /* SHA-512 */
        ctx->context_type = SHA2_512;
    }
    else
    {
        /* SHA-384 */
        ctx->context_type = SHA2_384;
    }
}

/*
 * SHA-512 process buffer
 */
void sha512_update( SHA512_CTX *ctx, const unsigned char *input,size_t ilen )
{
    ets_sha_update(&ctx->context, ctx->context_type, input, ilen * 8);
}

/*
 * SHA-512 final digest
 */
void sha512_finish( SHA512_CTX *ctx, unsigned char output[64] )
{
    ets_sha_finish(&ctx->context, ctx->context_type, output);
}

/*
 * output = SHA-512( input buffer )
 */
void sha512_output( const unsigned char *input, size_t ilen,unsigned char output[64], int is384 )
{
    SHA512_CTX ctx;

    sha512_init( &ctx );
    sha512_starts( &ctx, is384 );
    sha512_update( &ctx, input, ilen );
    sha512_finish( &ctx, output );
    sha512_free( &ctx );
}

#endif /* SHA512_C */
