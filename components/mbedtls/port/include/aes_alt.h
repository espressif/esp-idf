/**
 * \file aes_alt.h
 *
 * \brief AES block cipher
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
 *
 */
#ifndef AES_ALT_H
#define AES_ALT_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MBEDTLS_AES_ALT)
#include "hwcrypto/aes.h"

typedef esp_aes_context mbedtls_aes_context;

#define mbedtls_aes_init            esp_aes_init
#define mbedtls_aes_free            esp_aes_free
#define mbedtls_aes_setkey_enc      esp_aes_setkey_enc
#define mbedtls_aes_setkey_dec      esp_aes_setkey_dec
#define mbedtls_aes_crypt_ecb       esp_aes_crypt_ecb
#if defined(MBEDTLS_CIPHER_MODE_CBC)
#define mbedtls_aes_crypt_cbc       esp_aes_crypt_cbc
#endif
#if defined(MBEDTLS_CIPHER_MODE_CFB)
#define mbedtls_aes_crypt_cfb128    esp_aes_crypt_cfb128
#define mbedtls_aes_crypt_cfb8      esp_aes_crypt_cfb8
#endif
#if defined(MBEDTLS_CIPHER_MODE_CTR)
#define mbedtls_aes_crypt_ctr       esp_aes_crypt_ctr
#endif
#define mbedtls_aes_encrypt         esp_aes_encrypt
#define mbedtls_aes_decrypt         esp_aes_decrypt
#endif /* MBEDTLS_AES_ALT */

#ifdef __cplusplus
}
#endif

#endif
