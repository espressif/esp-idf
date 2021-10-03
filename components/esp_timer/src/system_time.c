// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Provides strong definition for system time functions relied upon
// by core components.
#include "sdkconfig.h"

#if CONFIG_ESP_TIME_FUNCS_USE_ESP_TIMER
#include "esp_timer.h"
#include "esp_timer_impl.h"
#include "esp_system.h"
#include "esp_newlib.h"
#include "esp_log.h"

#include "esp_private/startup_internal.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/rtc.h"
#endif

__attribute__((unused)) static const char* TAG = "system_time";

// Correction for underlying timer to keep definition
// of system time consistent.
static int64_t s_correction_us = 0;

void esp_timer_impl_init_system_time(void)
{
    s_correction_us = esp_rtc_get_time_us() - g_startup_time - esp_timer_impl_get_time();
#if defined(CONFIG_ESP_TIME_FUNCS_USE_ESP_TIMER) && defined(CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER)
    esp_err_t err = esp_register_shutdown_handler(esp_sync_counters_rtc_and_frc);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Register shutdown handler failed, err = 0x%x", err);
    }
#endif
}

int64_t IRAM_ATTR esp_system_get_time(void)
{
    return esp_timer_get_time() + s_correction_us;
}

uint32_t IRAM_ATTR esp_system_get_time_resolution(void)
{
    return 1000;
}
#endif
