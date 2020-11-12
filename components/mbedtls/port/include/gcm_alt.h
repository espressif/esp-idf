/**
 * \file gcm_alt.h
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
#ifndef GCM_ALT_H
#define GCM_ALT_H

#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MBEDTLS_GCM_ALT)

#if SOC_AES_SUPPORT_GCM
#include "aes/esp_aes_gcm.h"


typedef esp_gcm_context mbedtls_gcm_context;

#define mbedtls_gcm_init            esp_aes_gcm_init
#define mbedtls_gcm_free            esp_aes_gcm_free
#define mbedtls_gcm_setkey          esp_aes_gcm_setkey
#define mbedtls_gcm_starts          esp_aes_gcm_starts
#define mbedtls_gcm_update          esp_aes_gcm_update
#define mbedtls_gcm_finish          esp_aes_gcm_finish
#define mbedtls_gcm_auth_decrypt    esp_aes_gcm_auth_decrypt
#define mbedtls_gcm_crypt_and_tag   esp_aes_gcm_crypt_and_tag

#endif // SOC_AES_SUPPORT_GCM

#endif /* MBEDTLS_GCM_ALT */

#ifdef __cplusplus
}
#endif

#endif
