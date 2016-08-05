/* 
 * copyright (c) 2010 - 2012 Espressif System 
 * 
 * esf Link List Descriptor
 */
#ifndef _SHA1_H_
#define _SHA1_H_

#include "c_types.h"
#include "rom/ets_sys.h"
#include "rom/sha.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_SHA1_C

#define SHA1 0

/**
 * \brief          SHA-1 context structure
 */
typedef struct{
	SHA_CTX context;
	int context_type;
} sha1_context;

typedef sha1_context SHA1_CTX;

/**
 * \brief          Initialize SHA-1 context
 *
 * \param ctx      SHA-1 context to be initialized
 */
void sha1_init( SHA1_CTX *ctx );

/**
 * \brief          Clear SHA-1 context
 *
 * \param ctx      SHA-1 context to be cleared
 */
void sha1_free( SHA1_CTX *ctx );

/**
 * \brief          Clone (the state of) a SHA-1 context
 *
 * \param dst      The destination context
 * \param src      The context to be cloned
 */
void sha1_clone( SHA1_CTX *dst, const SHA1_CTX *src );

void sha1_process(SHA1_CTX *ctx, const unsigned char data[64]);

/**
 * \brief          SHA-1 context setup
 *
 * \param ctx      context to be initialized
 */
void sha1_starts( SHA1_CTX *ctx );

/**
 * \brief          SHA-1 process buffer
 *
 * \param ctx      SHA-1 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void sha1_update( SHA1_CTX *ctx, const unsigned char *input, size_t ilen );

/**
 * \brief          SHA-1 final digest
 *
 * \param ctx      SHA-1 context
 * \param output   SHA-1 checksum result
 */
void sha1_finish( SHA1_CTX *ctx, unsigned char output[20] );

/**
 * \brief          Output = SHA-1( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   SHA-1 checksum result
 */
void sha1_output( const unsigned char *input, size_t ilen, unsigned char output[20] );

#ifdef __cplusplus
}
#endif

#endif

