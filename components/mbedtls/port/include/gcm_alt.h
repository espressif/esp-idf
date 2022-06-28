/*
 * gcm_alt.h: AES block cipher
 *
 * SPDX-FileCopyrightText: The Mbed TLS Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2016-2022 Espressif Systems (Shanghai) CO LTD
 */
#ifndef GCM_ALT_H
#define GCM_ALT_H

#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MBEDTLS_GCM_ALT)


#include "aes/esp_aes_gcm.h"


typedef esp_gcm_context mbedtls_gcm_context;

#define mbedtls_gcm_init            esp_aes_gcm_init
#define mbedtls_gcm_free            esp_aes_gcm_free
#define mbedtls_gcm_setkey          esp_aes_gcm_setkey
#define mbedtls_gcm_starts          esp_aes_gcm_starts
#define mbedtls_gcm_update_ad       esp_aes_gcm_update_ad
#define mbedtls_gcm_update          esp_aes_gcm_update
#define mbedtls_gcm_finish          esp_aes_gcm_finish
#define mbedtls_gcm_auth_decrypt    esp_aes_gcm_auth_decrypt
#define mbedtls_gcm_crypt_and_tag   esp_aes_gcm_crypt_and_tag

#endif /* MBEDTLS_GCM_ALT */

#ifdef __cplusplus
}
#endif

#endif
