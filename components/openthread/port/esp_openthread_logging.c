// Copyright 2021 Espressif Systems (Shanghai) CO LTD
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
// limitations under the License

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
            esp_log_write(ESP_LOG_ERROR, OT_PLAT_LOG_TAG, LOG_COLOR_E "E(%u) %s:", esp_log_timestamp(), OT_PLAT_LOG_TAG);
            esp_log_writev(ESP_LOG_ERROR, OT_PLAT_LOG_TAG, format, args);
            esp_log_write(ESP_LOG_ERROR, OT_PLAT_LOG_TAG, LOG_RESET_COLOR "\n");
        }
        break;
    case OT_LOG_LEVEL_WARN:
        if (LOG_LOCAL_LEVEL >= ESP_LOG_WARN) {
            esp_log_write(ESP_LOG_WARN, OT_PLAT_LOG_TAG, LOG_COLOR_W "W(%u) %s:", esp_log_timestamp(), OT_PLAT_LOG_TAG);
            esp_log_writev(ESP_LOG_WARN, OT_PLAT_LOG_TAG, format, args);
            esp_log_write(ESP_LOG_WARN, OT_PLAT_LOG_TAG, LOG_RESET_COLOR "\n");
        }
        break;
    case OT_LOG_LEVEL_NOTE:
    case OT_LOG_LEVEL_INFO:
        if (LOG_LOCAL_LEVEL >= ESP_LOG_INFO) {
            esp_log_write(ESP_LOG_INFO, OT_PLAT_LOG_TAG, LOG_COLOR_I "I(%u) %s:", esp_log_timestamp(), OT_PLAT_LOG_TAG);
            esp_log_writev(ESP_LOG_INFO, OT_PLAT_LOG_TAG, format, args);
            esp_log_write(ESP_LOG_INFO, OT_PLAT_LOG_TAG, LOG_RESET_COLOR "\n");
        }
        break;
    default:
        if (LOG_LOCAL_LEVEL >= ESP_LOG_DEBUG) {
            esp_log_write(ESP_LOG_DEBUG, OT_PLAT_LOG_TAG, LOG_COLOR_D "D(%u) %s:", esp_log_timestamp(), OT_PLAT_LOG_TAG);
            esp_log_writev(ESP_LOG_DEBUG, OT_PLAT_LOG_TAG, format, args);
            esp_log_write(ESP_LOG_DEBUG, OT_PLAT_LOG_TAG, LOG_RESET_COLOR "\n");
        }
        break;
    }
    va_end(args);
}
#endif
