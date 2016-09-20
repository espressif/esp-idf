/*
 * copyright (c) 2010 - 2012 Espressif System
 *
 */

#ifndef _SHA256_ALT_H_
#define _SHA256_ALT_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MBEDTLS_SHA256_ALT)

#include "hwcrypto/sha.h"

typedef esp_sha_context mbedtls_sha256_context;

#define mbedtls_sha256_init     esp_sha256_init
#define mbedtls_sha256_clone    esp_sha256_clone
#define mbedtls_sha256_starts   esp_sha256_start
#define mbedtls_sha256_update   esp_sha256_update
#define mbedtls_sha256_finish   esp_sha256_finish
#define mbedtls_sha256_free     esp_sha256_free
#define mbedtls_sha256_process(...)

#endif

#ifdef __cplusplus
}
#endif

#endif /* sha256.h */
