/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_openthread.h"

#include <stdio.h>

#include "esp_log.h"
#include "esp_openthread_common_macro.h"
#include "openthread/platform/logging.h"

/**
 * The default platform logging tag.
 *
 */
#if (OPENTHREAD_CONFIG_LOG_OUTPUT == OPENTHREAD_CONFIG_LOG_OUTPUT_PLATFORM_DEFINED) || \
    (OPENTHREAD_CONFIG_LOG_OUTPUT == OPENTHREAD_CONFIG_LOG_OUTPUT_NCP_SPINEL)
OT_TOOL_WEAK void otPlatLog(otLogLevel log_level, otLogRegion log_region, const char *format, ...)
{
    va_list args;

    va_start(args, format);

    switch (log_level) {
    case OT_LOG_LEVEL_CRIT:
        if (LOG_LOCAL_LEVEL >= ESP_LOG_ERROR) {
            esp_log_write(ESP_LOG_ERROR, OT_PLAT_LOG_TAG, LOG_COLOR_E "E(%lu) %s:", esp_log_timestamp(),
                          OT_PLAT_LOG_TAG);
            esp_log_writev(ESP_LOG_ERROR, OT_PLAT_LOG_TAG, format, args);
            esp_log_write(ESP_LOG_ERROR, OT_PLAT_LOG_TAG, LOG_RESET_COLOR "\n");
        }
        break;
    case OT_LOG_LEVEL_WARN:
        if (LOG_LOCAL_LEVEL >= ESP_LOG_WARN) {
            esp_log_write(ESP_LOG_WARN, OT_PLAT_LOG_TAG, LOG_COLOR_W "W(%lu) %s:", esp_log_timestamp(),
                          OT_PLAT_LOG_TAG);
            esp_log_writev(ESP_LOG_WARN, OT_PLAT_LOG_TAG, format, args);
            esp_log_write(ESP_LOG_WARN, OT_PLAT_LOG_TAG, LOG_RESET_COLOR "\n");
        }
        break;
    case OT_LOG_LEVEL_NOTE:
    case OT_LOG_LEVEL_INFO:
        if (LOG_LOCAL_LEVEL >= ESP_LOG_INFO) {
            esp_log_write(ESP_LOG_INFO, OT_PLAT_LOG_TAG, LOG_COLOR_I "I(%lu) %s:", esp_log_timestamp(),
                          OT_PLAT_LOG_TAG);
            esp_log_writev(ESP_LOG_INFO, OT_PLAT_LOG_TAG, format, args);
            esp_log_write(ESP_LOG_INFO, OT_PLAT_LOG_TAG, LOG_RESET_COLOR "\n");
        }
        break;
    default:
        if (LOG_LOCAL_LEVEL >= ESP_LOG_DEBUG) {
            esp_log_write(ESP_LOG_DEBUG, OT_PLAT_LOG_TAG, LOG_COLOR_D "D(%lu) %s:", esp_log_timestamp(),
                          OT_PLAT_LOG_TAG);
            esp_log_writev(ESP_LOG_DEBUG, OT_PLAT_LOG_TAG, format, args);
            esp_log_write(ESP_LOG_DEBUG, OT_PLAT_LOG_TAG, LOG_RESET_COLOR "\n");
        }
        break;
    }
    va_end(args);
}
#endif
