/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _ESP_DEBUG_H_
#define _ESP_DEBUG_H_

#include "mbedtls/ssl.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "sdkconfig.h"
#ifdef CONFIG_MBEDTLS_DEBUG

/** @brief Enable mbedTLS debug logging via the esp_log mechanism.
 *
 * mbedTLS internal debugging is filtered from a specified mbedTLS
 * threshold level to esp_log level at runtime:
 *
 * - 1 - Warning
 * - 2 - Info
 * - 3 - Debug
 * - 4 - Verbose
 *
 * (Note that mbedTLS debug thresholds are not always consistently used.)
 *
 * This function will set the esp log level for "mbedtls" to the specified mbedTLS
 * threshold level that matches. However, the overall max ESP log level must be set high
 * enough in menuconfig, or some messages may be filtered at compile time.
 *
 * @param conf mbedtls_ssl_config structure
 * @param threshold mbedTLS debug threshold, 0-4. Messages are filtered at runtime.
 */
void mbedtls_esp_enable_debug_log(mbedtls_ssl_config *conf, int threshold);

/** @brief Disable mbedTLS debug logging via the esp_log mechanism.
 *
 */
void mbedtls_esp_disable_debug_log(mbedtls_ssl_config *conf);


#endif

#ifdef __cplusplus
}
#endif

#endif /* __ESP_DEBUG_H__ */
