/*
 *  FIPS-180-2 compliant SHA-256 implementation
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
 *  The SHA-256 Secure Hash Standard was published by NIST in 2002.
 *
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
 */

#include "port/sha256_alt.h"

#if defined(ESP_SHA256_C)
#include <string.h>

/* Implementation that should never be optimized out by the compiler */
static void sha256_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
}

void sha256_init( SHA256_CTX *ctx )
{
    memset( ctx, 0, sizeof( SHA256_CTX ) );
	ets_sha_enable();
}

void sha256_process(SHA256_CTX *ctx, const unsigned char data[64])
{

}

void sha256_free( SHA256_CTX *ctx )
{
    if( ctx == NULL )
        return;

    sha256_zeroize( ctx, sizeof( SHA256_CTX ) );
	ets_sha_disable();
}

void sha256_clone( SHA256_CTX *dst, const SHA256_CTX *src )
{
    *dst = *src;
}

/*
 * SHA-256 context setup
 */
void sha256_starts( SHA256_CTX *ctx, int is224 )
{
    ets_sha_init(&ctx->context);
	if( is224 == 0 )
    {
        /* SHA-256 */
		ctx->context_type = SHA256;
	}else{
		/* SHA-224 */
		ctx->context_type = SHA224;
	}	
}

/*
 * SHA-256 process buffer
 */
void sha256_update( SHA256_CTX *ctx, const unsigned char *input, size_t ilen )
{
	ets_sha_update(&ctx->context, ctx->context_type, input, ilen * 8);
}

/*
 * SHA-256 final digest
 */
void sha256_finish( SHA256_CTX *ctx, unsigned char output[32] )
{
	ets_sha_finish(&ctx->context, ctx->context_type, output);
}

/*
 * output = SHA-256( input buffer )
 */
void sha256_output( const unsigned char *input, size_t ilen, unsigned char output[32], int is224 )
{
    SHA256_CTX ctx;

    sha256_init( &ctx );
    sha256_starts( &ctx, is224 );
    sha256_update( &ctx, input, ilen );
    sha256_finish( &ctx, output );
    sha256_free( &ctx );
}

#endif /* SHA256_C */
