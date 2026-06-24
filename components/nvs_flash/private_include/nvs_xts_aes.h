/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"

/* NOTE: Using legacy mbedtls XTS API until PSA Crypto adds XTS support
* With TF-PSA-Crypto 1.0, AES headers moved to mbedtls/private/.
* Need MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS to access XTS functions.
*/

#if CONFIG_MBEDTLS_HARDWARE_AES
#include "aes/esp_aes.h"
#define XTS_FUNC(func) esp_aes_##func
#define XTS_MODE(mode) ESP_AES_##mode
#define XTS_CONTEXT esp_aes_xts_context
#else
#define MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS
#include "mbedtls/private/aes.h"
#define XTS_FUNC(func) mbedtls_aes_##func
#define XTS_MODE(mode) MBEDTLS_AES_##mode
#define XTS_CONTEXT mbedtls_aes_xts_context
#endif // CONFIG_MBEDTLS_HARDWARE_AES
