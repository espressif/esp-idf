/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SUPPLICANT_OPT_H
#define _SUPPLICANT_OPT_H

#include "sdkconfig.h"

#if CONFIG_WPA_MBEDTLS_CRYPTO
#define USE_MBEDTLS_CRYPTO 1
#else
#define CONFIG_TLS_INTERNAL_CLIENT
#define CONFIG_CRYPTO_INTERNAL
#define CONFIG_TLSV12
#endif

#if CONFIG_WPA_DEBUG_PRINT
#define DEBUG_PRINT
#endif

#if CONFIG_WPA_SCAN_CACHE
#define SCAN_CACHE_SUPPORTED
#endif

#endif /* _SUPPLICANT_OPT_H */
