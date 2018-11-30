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

#include <strings.h>

#include "esp_log.h"
#include "mbedtls/platform.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/esp_debug.h"

#ifdef CONFIG_MBEDTLS_DEBUG
static const char *TAG = "mbedtls";

static void mbedtls_esp_debug(void *ctx, int level,
                              const char *file, int line,
                              const char *str);

void mbedtls_esp_enable_debug_log(mbedtls_ssl_config *conf, int threshold)
{
    esp_log_level_t level = ESP_LOG_NONE;
    mbedtls_debug_set_threshold(threshold);
    mbedtls_ssl_conf_dbg(conf, mbedtls_esp_debug, NULL);
    switch(threshold) {
    case 1:
        level = ESP_LOG_WARN;
        break;
    case 2:
        level = ESP_LOG_INFO;
        break;
    case 3:
        level = ESP_LOG_DEBUG;
        break;
    case 4:
        level = ESP_LOG_VERBOSE;
        break;
    }
    esp_log_level_set(TAG, level);
}

void mbedtls_esp_disable_debug_log(mbedtls_ssl_config *conf)
{
    mbedtls_ssl_conf_dbg(conf, NULL, NULL);
}


/* Default mbedtls debug function that translates mbedTLS debug output
   to ESP_LOGx debug output.
*/
static void mbedtls_esp_debug(void *ctx, int level,
                     const char *file, int line,
                     const char *str)
{
    char *file_sep;

    /* Shorten 'file' from the whole file path to just the filename

       This is a bit wasteful because the macros are compiled in with
       the full _FILE_ path in each case.
    */
    file_sep = rindex(file, '/');
    if(file_sep)
        file = file_sep+1;

    switch(level) {
    case 1:
        ESP_LOGW(TAG, "%s:%d %s", file, line, str);
        break;
    case 2:
        ESP_LOGI(TAG, "%s:%d %s", file, line, str);
        break;
    case 3:
        ESP_LOGD(TAG, "%s:%d %s", file, line, str);
        break;
    case 4:
        ESP_LOGV(TAG, "%s:%d %s", file, line, str);
        break;
    default:
        ESP_LOGE(TAG, "Unexpected log level %d: %s", level, str);
        break;
    }
}
#endif