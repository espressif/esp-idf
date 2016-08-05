/* 
 * copyright (c) 2010 - 2012 Espressif System 
 * 
 * esf Link List Descriptor
 */

#ifndef _SHA256_H_
#define _SHA256_H_

#include "c_types.h"
#include "rom/ets_sys.h"
#include "rom/sha.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_SHA256_C

#define SHA256	SHA2_256
#define SHA224	4

/**
 * \brief          SHA-256 context structure
 */
typedef struct{
	SHA_CTX context;
	int     context_type;
}sha256_context;

typedef sha256_context SHA256_CTX;

/**
 * \brief          Initialize SHA-256 context
 *
 * \param ctx      SHA-256 context to be initialized
 */
void sha256_init( SHA256_CTX *ctx );

/**
 * \brief          Clear SHA-256 context
 *
 * \param ctx      SHA-256 context to be cleared
 */
void sha256_free( SHA256_CTX *ctx );
void sha256_process(SHA256_CTX *ctx, const unsigned char data[64]);

/**
 * \brief          Clone (the state of) a SHA-256 context
 *
 * \param dst      The destination context
 * \param src      The context to be cloned
 */
void sha256_clone( SHA256_CTX *dst, const SHA256_CTX *src );

/**
 * \brief          SHA-256 context setup
 *
 * \param ctx      context to be initialized
 * \param is224    0 = use SHA256, 1 = use SHA224
 */
void sha256_starts( SHA256_CTX *ctx, int is224 );

/**
 * \brief          SHA-256 process buffer
 *
 * \param ctx      SHA-256 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void sha256_update( SHA256_CTX *ctx, const unsigned char *input, size_t ilen );

/**
 * \brief          SHA-256 final digest
 *
 * \param ctx      SHA-256 context
 * \param output   SHA-224/256 checksum result
 */
void sha256_finish( SHA256_CTX *ctx, unsigned char output[32] );

/**
 * \brief          Output = SHA-256( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   SHA-224/256 checksum result
 * \param is224    0 = use SHA256, 1 = use SHA224
 */
void sha256_output( const unsigned char *input, size_t ilen, unsigned char output[32], int is224 );

#ifdef __cplusplus
}
#endif

#endif /* sha256.h */
