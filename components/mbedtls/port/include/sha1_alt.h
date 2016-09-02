/*
 * copyright (c) 2010 - 2012 Espressif System
 *
 * esf Link List Descriptor
 */
#ifndef _SHA1_ALT_H_
#define _SHA1_ALT_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MBEDTLS_SHA1_ALT)

#include "sha.h"

typedef SHA1_CTX mbedtls_sha1_context;

#define mbedtls_sha1_init       esp_sha1_init
#define mbedtls_sha1_starts     esp_sha1_start
#define mbedtls_sha1_clone      esp_sha1_clone
#define mbedtls_sha1_update     esp_sha1_update
#define mbedtls_sha1_finish     esp_sha1_finish
#define mbedtls_sha1_free       esp_sha1_free
#define mbedtls_sha1_process    esp_sha1_process

#endif

#ifdef __cplusplus
}
#endif

#endif

