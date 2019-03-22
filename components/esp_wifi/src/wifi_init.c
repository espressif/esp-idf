// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_event.h>
#include <esp_wifi.h>
#include "esp_log.h"
#include "esp_private/wifi.h"
#include "esp_pm.h"
#include "soc/rtc.h"
#include "esp_mesh.h"

/* mesh event callback handler */
mesh_event_cb_t g_mesh_event_cb = NULL;

#ifdef CONFIG_PM_ENABLE
static esp_pm_lock_handle_t s_wifi_modem_sleep_lock;
#endif

/* Callback function to update WiFi MAC time */
wifi_mac_time_update_cb_t s_wifi_mac_time_update_cb = NULL;

static void __attribute__((constructor)) s_set_default_wifi_log_level()
{
    /* WiFi libraries aren't compiled to know CONFIG_LOG_DEFAULT_LEVEL,
       so set it at runtime startup. Done here not in esp_wifi_init() to allow
       the user to set the level again before esp_wifi_init() is called.
    */
    esp_log_level_set("wifi", CONFIG_LOG_DEFAULT_LEVEL);
    esp_log_level_set("mesh", CONFIG_LOG_DEFAULT_LEVEL);
}

static void esp_wifi_set_debug_log()
{
    /* set WiFi log level and module */
#if CONFIG_ESP32_WIFI_DEBUG_LOG_ENABLE
    uint32_t g_wifi_log_level = WIFI_LOG_INFO;
    uint32_t g_wifi_log_module = 0;
    uint32_t g_wifi_log_submodule = 0;
#if CONFIG_ESP32_WIFI_DEBUG_LOG_DEBUG
    g_wifi_log_level = WIFI_LOG_DEBUG;
#endif
#if CONFIG_ESP32_WIFI_DEBUG_LOG_VERBOSE
    g_wifi_log_level = WIFI_LOG_VERBOSE;
#endif
#if CONFIG_ESP32_WIFI_DEBUG_LOG_MODULE_ALL
    g_wifi_log_module = WIFI_LOG_MODULE_ALL;
#endif
#if CONFIG_ESP32_WIFI_DEBUG_LOG_MODULE_WIFI
    g_wifi_log_module = WIFI_LOG_MODULE_WIFI;
#endif
#if CONFIG_ESP32_WIFI_DEBUG_LOG_MODULE_COEX
    g_wifi_log_module = WIFI_LOG_MODULE_COEX;
#endif
#if CONFIG_ESP32_WIFI_DEBUG_LOG_MODULE_MESH
    g_wifi_log_module = WIFI_LOG_MODULE_MESH;
#endif
#if CONFIG_ESP32_WIFI_DEBUG_LOG_SUBMODULE_ALL
    g_wifi_log_submodule |= WIFI_LOG_SUBMODULE_ALL;
#endif
#if CONFIG_ESP32_WIFI_DEBUG_LOG_SUBMODULE_INIT
    g_wifi_log_submodule |= WIFI_LOG_SUBMODULE_INIT;
#endif
#if CONFIG_ESP32_WIFI_DEBUG_LOG_SUBMODULE_IOCTL
    g_wifi_log_submodule |= WIFI_LOG_SUBMODULE_IOCTL;
#endif
#if CONFIG_ESP32_WIFI_DEBUG_LOG_SUBMODULE_CONN
    g_wifi_log_submodule |= WIFI_LOG_SUBMODULE_CONN;
#endif
#if CONFIG_ESP32_WIFI_DEBUG_LOG_SUBMODULE_SCAN
    g_wifi_log_submodule |= WIFI_LOG_SUBMODULE_SCAN;
#endif
    esp_wifi_internal_set_log_level(g_wifi_log_level);
    esp_wifi_internal_set_log_mod(g_wifi_log_module, g_wifi_log_submodule, true);

#endif /* CONFIG_ESP32_WIFI_DEBUG_LOG_ENABLE*/

}

esp_err_t esp_wifi_init(const wifi_init_config_t *config)
{
#ifdef CONFIG_PM_ENABLE
    if (s_wifi_modem_sleep_lock == NULL) {
        esp_err_t err = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "wifi",
                &s_wifi_modem_sleep_lock);
        if (err != ESP_OK) {
            return err;
        }
    }
#endif
    esp_event_set_default_wifi_handlers();
    esp_err_t result = esp_wifi_init_internal(config);
    if (result == ESP_OK) {
        esp_wifi_set_debug_log();
        s_wifi_mac_time_update_cb = esp_wifi_internal_update_mac_time;
    }

    return result;
}

#ifdef CONFIG_PM_ENABLE
void wifi_apb80m_request(void)
{
    assert(s_wifi_modem_sleep_lock);
    esp_pm_lock_acquire(s_wifi_modem_sleep_lock);
    if (rtc_clk_apb_freq_get() != APB_CLK_FREQ) {
        ESP_LOGE(__func__, "WiFi needs 80MHz APB frequency to work, but got %dHz", rtc_clk_apb_freq_get());
    }
}

void wifi_apb80m_release(void)
{
    assert(s_wifi_modem_sleep_lock);
    esp_pm_lock_release(s_wifi_modem_sleep_lock);
}
#endif //CONFIG_PM_ENABLE
