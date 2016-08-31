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

#include <string.h>
#include "sha.h"
#include "esp_crypto.h"

/* Implementation that should never be optimized out by the compiler */
//static void bzero( void *v, size_t n ) {
//    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
//}

void esp_sha1_init( SHA1_CTX *ctx )
{
    memset( ctx, 0, sizeof( SHA1_CTX ) );

    SHA_LOCK();
    SHA_TAKE();
	ets_sha_enable();	
	SHA_UNLOCK();
}

void esp_sha1_free( SHA1_CTX *ctx )
{
    if( ctx == NULL )
        return;

    bzero( ctx, sizeof( SHA1_CTX ) );

    SHA_LOCK();
    SHA_GIVE();
    if (false == SHA_IS_USED())
	    ets_sha_disable();
	SHA_UNLOCK();
}

void esp_sha1_clone( SHA1_CTX *dst, const SHA1_CTX *src )
{
    *dst = *src;
}

void esp_sha1_process(SHA1_CTX *ctx, const unsigned char data[64])
{

}

/*
 * SHA-1 context setup
 */
void esp_sha1_start( SHA1_CTX *ctx )
{
	SHA_LOCK();
	ets_sha_init(&ctx->context);

	ctx->context_type = SHA1;
}

/*
 * SHA-1 process buffer
 */
void esp_sha1_update( SHA1_CTX *ctx, const unsigned char *input, size_t ilen )
{
	ets_sha_update(&ctx->context, ctx->context_type, input, ilen * 8);
}

/*
 * SHA-1 final digest
 */
void esp_sha1_finish( SHA1_CTX *ctx, unsigned char output[20] )
{
	ets_sha_finish(&ctx->context, ctx->context_type, output);
	SHA_UNLOCK();
}

/*
 * output = SHA-1( input buffer )
 */
void esp_sha1_output( const unsigned char *input, size_t ilen, unsigned char output[20] )
{
    SHA1_CTX ctx;

    esp_sha1_init( &ctx );
    esp_sha1_start( &ctx );
    esp_sha1_update( &ctx, input, ilen );
    esp_sha1_finish( &ctx, output );
    esp_sha1_free( &ctx );
}

/////
/* Implementation that should never be optimized out by the compiler */
void esp_sha256_init( SHA256_CTX *ctx )
{
    memset( ctx, 0, sizeof( SHA256_CTX ) );

    SHA_LOCK();
    SHA_TAKE();
	ets_sha_enable();
	SHA_UNLOCK();
}

void esp_sha256_process(SHA256_CTX *ctx, const unsigned char data[64])
{

}

void esp_sha256_free( SHA256_CTX *ctx )
{
    if( ctx == NULL )
        return;

    bzero( ctx, sizeof( SHA256_CTX ) );

    SHA_LOCK();
    SHA_GIVE();
    if (false == SHA_IS_USED())
	    ets_sha_disable();
	SHA_UNLOCK();
}

void esp_sha256_clone( SHA256_CTX *dst, const SHA256_CTX *src )
{
    *dst = *src;
}

/*
 * SHA-256 context setup
 */
void esp_sha256_start( SHA256_CTX *ctx, int is224 )
{
	SHA_LOCK();
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
void esp_sha256_update( SHA256_CTX *ctx, const unsigned char *input, size_t ilen )
{
	ets_sha_update(&ctx->context, ctx->context_type, input, ilen * 8);
}

/*
 * SHA-256 final digest
 */
void esp_sha256_finish( SHA256_CTX *ctx, unsigned char output[32] )
{
	ets_sha_finish(&ctx->context, ctx->context_type, output);
	SHA_UNLOCK();
}

/*
 * output = SHA-256( input buffer )
 */
void esp_sha256_output( const unsigned char *input, size_t ilen, unsigned char output[32], int is224 )
{
    SHA256_CTX ctx;

    esp_sha256_init( &ctx );
    esp_sha256_start( &ctx, is224 );
    esp_sha256_update( &ctx, input, ilen );
    esp_sha256_finish( &ctx, output );
    esp_sha256_free( &ctx );
}


/////
void esp_sha512_init( SHA512_CTX *ctx )
{
    memset( ctx, 0, sizeof( SHA512_CTX ) );

    SHA_LOCK();
    SHA_TAKE();
	ets_sha_enable();
	SHA_UNLOCK();
}

void esp_sha512_process( SHA512_CTX *ctx, const unsigned char data[128] )
{

}

void esp_sha512_free( SHA512_CTX *ctx )
{
    if( ctx == NULL )
        return;

    bzero( ctx, sizeof( SHA512_CTX ) );

    SHA_LOCK();
    SHA_GIVE();
    if (false == SHA_IS_USED())
	    ets_sha_disable();
	SHA_UNLOCK();
}

void esp_sha512_clone( SHA512_CTX *dst, const SHA512_CTX *src )
{
    *dst = *src;
}

/*
 * SHA-512 context setup
 */
void esp_sha512_start( SHA512_CTX *ctx, int is384 )
{
	SHA_LOCK();
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
void esp_sha512_update( SHA512_CTX *ctx, const unsigned char *input,size_t ilen )
{
    ets_sha_update(&ctx->context, ctx->context_type, input, ilen * 8);
}

/*
 * SHA-512 final digest
 */
void esp_sha512_finish( SHA512_CTX *ctx, unsigned char output[64] )
{
    ets_sha_finish(&ctx->context, ctx->context_type, output);
    SHA_UNLOCK();
}

/*
 * output = SHA-512( input buffer )
 */
void esp_sha512_output( const unsigned char *input, size_t ilen,unsigned char output[64], int is384 )
{
    SHA512_CTX ctx;

    esp_sha512_init( &ctx );
    esp_sha512_start( &ctx, is384 );
    esp_sha512_update( &ctx, input, ilen );
    esp_sha512_finish( &ctx, output );
    esp_sha512_free( &ctx );
}

////

