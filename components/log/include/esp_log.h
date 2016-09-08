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

#ifndef __ESP_LOG_H__
#define __ESP_LOG_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Logging library
 *
 * In each C file which uses logging functionality, define TAG variable like this:
 *
 *      static const char* TAG = "MyModule";
 *
 * then use one of logging macros to produce output, e.g:
 *
 *      ESP_LOGW(TAG, "Baud rate error %.1f%%. Requested: %d baud, actual: %d baud", error * 100, baud_req, baud_real);
 *
 * Log filtering happens both at compile time and at runtime.
 *
 * At compile time, filtering is done using CONFIG_ESP_LOG_LEVEL macro, set via menuconfig.
 * All logging statments for levels higher than CONFIG_ESP_LOG_LEVEL will be removed by the preprocessor.
 *
 * At run time, all logs below CONFIG_ESP_LOG_LEVEL are enabled by default.
 * esp_log_set function may be used to set logging level per tag.
 *
 *      esp_log_set("*", ESP_LOG_ERROR);        // set all components to ERROR level
 *      esp_log_set("wifi", ESP_LOG_WARN);      // enable WARN logs from WiFi stack
 *      esp_log_set("dhcpc", ESP_LOG_INFO);     // enable INFO logs from DHCP client
 *
 *
 */


typedef enum {
    ESP_LOG_NONE,
    ESP_LOG_ERROR,
    ESP_LOG_WARN,
    ESP_LOG_INFO,
    ESP_LOG_DEBUG,
    ESP_LOG_VERBOSE
} esp_log_level_t;


/**
 * @brief Set log level for given tag
 *
 * If logging for given component has already been enabled, changes previous setting.
 *
 * @param tag Tag of the log entries to enable. Must be a non-NULL zero terminated string.
 *            Value "*" means that all tags are affected.
 *
 * @param level  Selects log level to enable. Only logs at this and lower levels will be shown.
 */
void esp_log_set(const char* tag, esp_log_level_t level);

/**
 * @brief Write message into the log
 *
 * This function is not intended to be used directly. Instead, use one of
 * ESP_LOGE, ESP_LOGW, ESP_LOGI, ESP_LOGD, ESP_LOGV macros.
 */
void esp_log_write(esp_log_level_t level, const char* tag, const char* format, ...) __attribute__ ((format (printf, 3, 4)));
#ifndef CONFIG_ESP_LOG_LEVEL
#define CONFIG_ESP_LOG_LEVEL ESP_LOG_NONE
#endif

#if (CONFIG_ESP_LOG_LEVEL < ESP_LOG_ERROR)
#define ESP_LOGE( tag, format, ... )  esp_log_write(ESP_LOG_ERROR, tag, format, ##__VA_ARGS__)
#else
#define ESP_LOGE( tag, format, ... )
#endif

#if (CONFIG_ESP_LOG_LEVEL < ESP_LOG_WARN)
#define ESP_LOGW( tag, format, ... )  esp_log_write(ESP_LOG_WARN, tag, format, ##__VA_ARGS__)
#else
#define ESP_LOGW( tag, format, ... )
#endif

#if (CONFIG_ESP_LOG_LEVEL < ESP_LOG_INFO)
#define ESP_LOGI( tag, format, ... )  esp_log_write(ESP_LOG_INFO, tag, format, ##__VA_ARGS__)
#else
#define ESP_LOGI( tag, format, ... )
#endif


#if (CONFIG_ESP_LOG_LEVEL < ESP_LOG_DEBUG)
#define ESP_LOGD( tag, format, ... )  esp_log_write(ESP_LOG_DEBUG, tag, format, ##__VA_ARGS__)
#else
#define ESP_LOGD( tag, format, ... )
#endif

#if (CONFIG_ESP_LOG_VERBOSE < ESP_LOG_ERROR)
#define ESP_LOGV( tag, format, ... )  esp_log_write(ESP_LOG_VERBOSE, tag, format, ##__VA_ARGS__)
#else
#define ESP_LOGV( tag, format, ... )
#endif

#ifdef __cplusplus
}
#endif


#endif /* __ESP_LOG_H__ */
