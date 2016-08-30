/* 
 * copyright (c) 2010 - 2012 Espressif System 
 * 
 * esf Link List Descriptor
 */

#ifndef _SHA512_ALT_H_
#define _SHA512_ALT_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MBEDTLS_SHA512_ALT)
#include "sha.h"

typedef SHA512_CTX	mbedtls_sha512_context;

#define mbedtls_sha512_init   esp_sha512_init
#define mbedtls_sha512_process	esp_sha512_process
#define mbedtls_sha512_clone  esp_sha512_clone
#define mbedtls_sha512_starts esp_sha512_starts
#define mbedtls_sha512_update esp_sha512_update
#define mbedtls_sha512_finish esp_sha512_finish
#define mbedtls_sha512_free   esp_sha512_free

#endif

#ifdef __cplusplus
}
#endif

#endif /* sha512.h */
