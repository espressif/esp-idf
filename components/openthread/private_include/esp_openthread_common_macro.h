/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

#define OT_PLAT_LOG_TAG "OPENTHREAD"

#ifndef MS_PER_S
#define MS_PER_S 1000
#endif

#ifndef US_PER_MS
#define US_PER_MS 1000
#endif

#ifndef US_PER_S
#define US_PER_S (MS_PER_S * US_PER_MS)
#endif

#define ESP_OPENTHREAD_UART_BUFFER_SIZE CONFIG_OPENTHREAD_UART_BUFFER_SIZE

#if CONFIG_OPENTHREAD_DEBUG

#if CONFIG_OPENTHREAD_DUMP_MAC_ON_ASSERT && CONFIG_IEEE802154_RECORD
#include "esp_ieee802154.h"
#define IEEE802154_RECORD_PRINT() esp_ieee802154_record_print()
#else
#define IEEE802154_RECORD_PRINT()
#endif

#define ESP_OPENTHREAD_ASSERT(a) do { \
                                    if(unlikely(!(a))) { \
                                        IEEE802154_RECORD_PRINT(); \
                                        assert(a); \
                                    } \
                                } while (0)
#else
#define ESP_OPENTHREAD_ASSERT(a) assert(a)
#endif

#ifndef ESP_RETURN_VOID_ON_FALSE
#if defined(CONFIG_COMPILER_OPTIMIZATION_CHECKS_SILENT)
#define ESP_RETURN_VOID_ON_FALSE(a, log_tag, format, ...) \
    do {                                                  \
        (void)log_tag;                                    \
        if (unlikely(!(a))) {                             \
            return;                                       \
        }                                                 \
    } while (0)
#else
#define ESP_RETURN_VOID_ON_FALSE(a, log_tag, format, ...)                                \
    do {                                                                                 \
        if (unlikely(!(a))) {                                                            \
            ESP_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            return;                                                                      \
        }                                                                                \
    } while (0)
#endif
#endif
