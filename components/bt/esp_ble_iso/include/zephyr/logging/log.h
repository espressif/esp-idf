/*
 * SPDX-FileCopyrightText: 2018 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_LOGGING_LOG_H_
#define ZEPHYR_INCLUDE_LOGGING_LOG_H_

#include <stdio.h>
#include <assert.h>

#include "sdkconfig.h"

#include "esp_log.h"

/* esp_ble_iso, esp_ble_audio and any future ISO consumer are compiled
 * under the unified BLE_ISO compression channel and share one
 * iso_log_index.h with one monotonic log-id counter — no per-consumer
 * index header, no per-TU sentinel needed. */
#if CONFIG_BLE_ISO_COMPRESSED_LOG_ENABLE
#include "iso_log_index.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Define a per-translation-unit log tag.
 * The first argument is the module name (an identifier); it is stringified to
 * produce the tag passed to esp_log_write(). LOG_MODULE_DECLARE has the same
 * effect and is provided for Zephyr API parity (in our port the two expand to
 * identical code). */
#define LOG_MODULE_REGISTER(name, ...) \
    static const char *const __iso_log_tag __attribute__((unused)) = #name

#define LOG_MODULE_DECLARE(name, ...) \
    static const char *const __iso_log_tag __attribute__((unused)) = #name

#define BT_ISO_LOG_COLOR_BLACK      "30"
#define BT_ISO_LOG_COLOR_RED        "31"
#define BT_ISO_LOG_COLOR_GREEN      "32"
#define BT_ISO_LOG_COLOR_YELLOW     "33"
#define BT_ISO_LOG_COLOR_BLUE       "34"
#define BT_ISO_LOG_COLOR_PURPLE     "35"
#define BT_ISO_LOG_COLOR_CYAN       "36"
#define BT_ISO_LOG_COLOR_WHITE      "37"
#define BT_ISO_LOG_COLOR(COLOR)     "\033[0;" COLOR "m"
#define BT_ISO_LOG_BOLD(COLOR)      "\033[1;" COLOR "m"
#define BT_ISO_LOG_RESET_COLOR      "\033[0m"
#define BT_ISO_LOG_COLOR_E          BT_ISO_LOG_COLOR(BT_ISO_LOG_COLOR_RED)
#define BT_ISO_LOG_COLOR_W          BT_ISO_LOG_COLOR(BT_ISO_LOG_COLOR_YELLOW)
#define BT_ISO_LOG_COLOR_I          BT_ISO_LOG_COLOR(BT_ISO_LOG_COLOR_GREEN)
#define BT_ISO_LOG_COLOR_D          BT_ISO_LOG_COLOR(BT_ISO_LOG_COLOR_WHITE)
#define BT_ISO_LOG_COLOR_V          BT_ISO_LOG_COLOR(BT_ISO_LOG_COLOR_WHITE)

#define BT_ISO_LOG_ERROR            1
#define BT_ISO_LOG_WARN             2
#define BT_ISO_LOG_INFO             3
#define BT_ISO_LOG_DEBUG            4
#define BT_ISO_LOG_VERBOSE          5

#define BT_ISO_LOGE(tag, format, ...) \
        esp_log_write(ESP_LOG_ERROR, tag, LOG_FORMAT(E, format), \
                      esp_log_timestamp(), tag, ##__VA_ARGS__);

#define BT_ISO_LOGW(tag, format, ...) \
        esp_log_write(ESP_LOG_WARN, tag, LOG_FORMAT(W, format), \
                      esp_log_timestamp(), tag, ##__VA_ARGS__);

#define BT_ISO_LOGI(tag, format, ...) \
        esp_log_write(ESP_LOG_INFO, tag, LOG_FORMAT(I, format), \
                      esp_log_timestamp(), tag, ##__VA_ARGS__);

#define BT_ISO_LOGD(tag, format, ...) \
        esp_log_write(ESP_LOG_INFO, tag, LOG_FORMAT(D, format), \
                      esp_log_timestamp(), tag, ##__VA_ARGS__);

#if CONFIG_BT_ISO_NO_LOG
#define LOG_ERR(fmt, args...)
#define LOG_WRN(fmt, args...)
#define LOG_INF(fmt, args...)
#define LOG_DBG(fmt, args...)
#else /* CONFIG_BT_ISO_NO_LOG */
#if (CONFIG_BT_ISO_LOG_LEVEL >= BT_ISO_LOG_ERROR)
#define LOG_ERR(fmt, args...)   BT_ISO_LOGE(__iso_log_tag, fmt, ## args)
#else /* (CONFIG_BT_ISO_LOG_LEVEL >= BT_ISO_LOG_ERROR) */
#define LOG_ERR(fmt, args...)
#endif /* (CONFIG_BT_ISO_LOG_LEVEL >= BT_ISO_LOG_ERROR) */

#if (CONFIG_BT_ISO_LOG_LEVEL >= BT_ISO_LOG_WARN)
#define LOG_WRN(fmt, args...)   BT_ISO_LOGW(__iso_log_tag, fmt, ## args)
#else /* (CONFIG_BT_ISO_LOG_LEVEL >= BT_ISO_LOG_WARN) */
#define LOG_WRN(fmt, args...)
#endif /* (CONFIG_BT_ISO_LOG_LEVEL >= BT_ISO_LOG_WARN) */

#if (CONFIG_BT_ISO_LOG_LEVEL >= BT_ISO_LOG_INFO)
#define LOG_INF(fmt, args...)   BT_ISO_LOGI(__iso_log_tag, fmt, ## args)
#else /* (CONFIG_BT_ISO_LOG_LEVEL >= BT_ISO_LOG_INFO) */
#define LOG_INF(fmt, args...)
#endif /* (CONFIG_BT_ISO_LOG_LEVEL >= BT_ISO_LOG_INFO) */

#if (CONFIG_BT_ISO_LOG_LEVEL >= BT_ISO_LOG_DEBUG)
#define LOG_DBG(fmt, args...)   BT_ISO_LOGD(__iso_log_tag, fmt, ## args)
#else /* (CONFIG_BT_ISO_LOG_LEVEL >= BT_ISO_LOG_DEBUG) */
#define LOG_DBG(fmt, args...)
#endif /* (CONFIG_BT_ISO_LOG_LEVEL >= BT_ISO_LOG_DEBUG) */
#endif /* CONFIG_BT_ISO_NO_LOG */

#define NET_BUF_ERR(fmt, args...)           /* TBD */
#define NET_BUF_WARN(fmt, args...)          /* TBD */
#define NET_BUF_INFO(fmt, args...)          /* TBD */
#define NET_BUF_DBG(fmt, args...)           /* TBD */
#define NET_BUF_ASSERT                      assert

#define NET_BUF_SIMPLE_ERR(fmt, args...)    /* TBD */
#define NET_BUF_SIMPLE_WARN(fmt, args...)   /* TBD */
#define NET_BUF_SIMPLE_INFO(fmt, args...)   /* TBD */
#define NET_BUF_SIMPLE_DBG(fmt, args...)    /* TBD */
#define NET_BUF_SIMPLE_ASSERT               assert

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_LOGGING_LOG_H_ */
