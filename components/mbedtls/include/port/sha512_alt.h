/* 
 * copyright (c) 2010 - 2012 Espressif System 
 * 
 * esf Link List Descriptor
 */

#ifndef _SHA512_H_
#define _SHA512_H_

#include "c_types.h"
#include "rom/ets_sys.h"
#include "rom/sha.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_SHA512_C

/**
 * \brief          SHA-512 context structure
 */
typedef struct{
	SHA_CTX context;
	int     context_type;
}sha512_context;

typedef sha512_context SHA512_CTX;

/**
 * \brief          Initialize SHA-512 context
 *
 * \param ctx      SHA-512 context to be initialized
 */
void sha512_init( SHA512_CTX *ctx );

/**
 * \brief          Clear SHA-512 context
 *
 * \param ctx      SHA-512 context to be cleared
 */
void sha512_free( SHA512_CTX *ctx );

/**
 * \brief          Clone (the state of) a SHA-512 context
 *
 * \param dst      The destination context
 * \param src      The context to be cloned
 */
void sha512_clone( SHA512_CTX *dst, const SHA512_CTX *src );

/**
 * \brief          SHA-512 context setup
 *
 * \param ctx      context to be initialized
 * \param is384    0 = use SHA512, 1 = use SHA384
 */
void sha512_starts( SHA512_CTX *ctx, int is384 );

/**
 * \brief          SHA-512 process buffer
 *
 * \param ctx      SHA-512 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void sha512_update( SHA512_CTX *ctx, const unsigned char *input, size_t ilen );

/**
 * \brief          SHA-512 final digest
 *
 * \param ctx      SHA-512 context
 * \param output   SHA-384/512 checksum result
 */
void sha512_finish( SHA512_CTX *ctx, unsigned char output[64] );

/**
 * \brief          Output = SHA-512( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   SHA-384/512 checksum result
 * \param is384    0 = use SHA512, 1 = use SHA384
 */
void sha512_output( const unsigned char *input, size_t ilen, unsigned char output[64], int is384 );


#ifdef __cplusplus
}
#endif

#endif /* sha512.h */
