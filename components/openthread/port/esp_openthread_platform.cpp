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

#include "esp_openthread_platform.h"

#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_openthread_alarm.h"
#include "esp_openthread_common_macro.h"
#include "esp_openthread_lock.h"
#include "esp_openthread_netif_glue.h"
#include "esp_openthread_netif_glue_priv.h"
#include "esp_openthread_radio_uart.h"
#include "esp_openthread_types.h"
#include "esp_openthread_uart.h"
#include "common/code_utils.hpp"
#include "common/logging.hpp"
#include "core/common/instance.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "openthread/cli.h"
#include "openthread/instance.h"
#include "openthread/tasklet.h"

static esp_openthread_platform_config_t s_platform_config;
static bool s_openthread_platform_initialized = false;

esp_err_t esp_openthread_platform_init(const esp_openthread_platform_config_t *config)
{
    ESP_RETURN_ON_FALSE(config->radio_config.radio_mode == RADIO_MODE_UART_RCP, ESP_ERR_INVALID_ARG, OT_PLAT_LOG_TAG,
                        "Radio mode not supported");
    ESP_RETURN_ON_FALSE(config->host_config.host_connection_mode == HOST_CONNECTION_MODE_NONE ||
                        config->host_config.host_connection_mode == HOST_CONNECTION_MODE_UART,
                        ESP_ERR_INVALID_ARG, OT_PLAT_LOG_TAG, "Host connection mode not supported");
    ESP_RETURN_ON_FALSE(!s_openthread_platform_initialized, ESP_ERR_INVALID_STATE, OT_PLAT_LOG_TAG,
                        "OpenThread platform already initialized");

    esp_err_t ret = ESP_OK;

    s_platform_config = *config;
    ESP_GOTO_ON_ERROR(esp_openthread_lock_init(), exit, OT_PLAT_LOG_TAG, "esp_openthread_lock_init failed");
    if (config->host_config.host_connection_mode == HOST_CONNECTION_MODE_UART) {
        ESP_GOTO_ON_ERROR(esp_openthread_uart_init(config), exit, OT_PLAT_LOG_TAG, "esp_openthread_uart_init failed");
    }
    ESP_GOTO_ON_ERROR(esp_openthread_radio_init(config), exit, OT_PLAT_LOG_TAG, "esp_openthread_radio_init failed");

exit:
    if (ret != ESP_OK) {
        esp_openthread_platform_deinit();
    }

    return ret;
}

otInstance *esp_openthread_get_instance(void)
{
    return (otInstance *)&ot::Instance::Get();
}

esp_err_t esp_openthread_platform_deinit(void)
{
    ESP_RETURN_ON_FALSE(s_openthread_platform_initialized, ESP_ERR_INVALID_STATE, OT_PLAT_LOG_TAG,
                        "OpenThread platform not initialized");

    esp_openthread_radio_deinit();
    if (s_platform_config.host_config.host_connection_mode == HOST_CONNECTION_MODE_UART) {
        esp_openthread_uart_deinit();
    }
    esp_openthread_lock_deinit();

    return ESP_OK;
}

void esp_openthread_platform_update(esp_openthread_mainloop_context_t *mainloop)
{
    esp_openthread_alarm_update(mainloop);
    if (s_platform_config.host_config.host_connection_mode == HOST_CONNECTION_MODE_UART) {
        esp_openthread_uart_update(mainloop);
    }
    esp_openthread_radio_update(mainloop);
    esp_openthread_netif_glue_update(mainloop);
}

esp_err_t esp_openthread_platform_process(otInstance *instance, const esp_openthread_mainloop_context_t *mainloop)
{
    if (s_platform_config.host_config.host_connection_mode == HOST_CONNECTION_MODE_UART) {
        ESP_RETURN_ON_ERROR(esp_openthread_uart_process(), OT_PLAT_LOG_TAG, "esp_openthread_uart_process failed");
    }
    esp_openthread_radio_process(instance, mainloop);
    esp_openthread_alarm_process(instance);
    return esp_openthread_netif_glue_process(instance, mainloop);
}
