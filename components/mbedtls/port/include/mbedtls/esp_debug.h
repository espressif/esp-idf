// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _ESP_DEBUG_H_
#define _ESP_DEBUG_H_

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
 * @param mbedTLS debug threshold, 0-4. Messages are filtered at runtime.
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
