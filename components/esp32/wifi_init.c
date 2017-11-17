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
#include "esp_wifi_internal.h"
#include "esp_pm.h"
#include "soc/rtc.h"

#ifdef CONFIG_PM_ENABLE
static esp_pm_lock_handle_t s_wifi_modem_sleep_lock;
#endif

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
    return esp_wifi_init_internal(config);
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
