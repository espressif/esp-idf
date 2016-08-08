/*
 *  FIPS-180-1 compliant SHA-1 implementation
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
 *  The SHA-1 standard was published by NIST in 1993.
 *
 *  http://www.itl.nist.gov/fipspubs/fip180-1.htm
 */

#include "port/sha1_alt.h"

#if defined(ESP_SHA1_C)
#include <string.h>
#include "multi_thread.h"

/* Implementation that should never be optimized out by the compiler */
static void sha1_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
}

void sha1_init( SHA1_CTX *ctx )
{
    memset( ctx, 0, sizeof( SHA1_CTX ) );

    SHA1_LOCK();
    SHA1_TAKE();
	ets_sha_enable();	
	SHA1_UNLOCK();
}

void sha1_free( SHA1_CTX *ctx )
{
    if( ctx == NULL )
        return;

    sha1_zeroize( ctx, sizeof( SHA1_CTX ) );

    SHA1_LOCK();
    SHA1_GIVE();
    if (false == SHA1_IS_USED())
	    ets_sha_disable();
	SHA1_UNLOCK();
}

void sha1_clone( SHA1_CTX *dst, const SHA1_CTX *src )
{
    *dst = *src;
}

void sha1_process(SHA1_CTX *ctx, const unsigned char data[64])
{

}

/*
 * SHA-1 context setup
 */
void sha1_starts( SHA1_CTX *ctx )
{
	SHA1_LOCK();
	ets_sha_init(&ctx->context);
	SHA1_UNLOCK();

	ctx->context_type = SHA1;
}

/*
 * SHA-1 process buffer
 */
void sha1_update( SHA1_CTX *ctx, const unsigned char *input, size_t ilen )
{
	SHA1_LOCK();
	ets_sha_update(&ctx->context, ctx->context_type, input, ilen * 8);
}

/*
 * SHA-1 final digest
 */
void sha1_finish( SHA1_CTX *ctx, unsigned char output[20] )
{
	ets_sha_finish(&ctx->context, ctx->context_type, output);
	SHA1_UNLOCK();
}

/*
 * output = SHA-1( input buffer )
 */
void sha1_output( const unsigned char *input, size_t ilen, unsigned char output[20] )
{
    SHA1_CTX ctx;

    sha1_init( &ctx );
    sha1_starts( &ctx );
    sha1_update( &ctx, input, ilen );
    sha1_finish( &ctx, output );
    sha1_free( &ctx );
}

#endif /* _SHA1_C */


