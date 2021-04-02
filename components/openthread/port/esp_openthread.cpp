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

#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_openthread_alarm.h"
#include "esp_openthread_common_macro.h"
#include "esp_openthread_lock.h"
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
#include "openthread/platform/alarm-milli.h"
#include "openthread/platform/time.h"
#include "openthread/tasklet.h"

static esp_openthread_platform_config_t s_platform_config;
static bool s_openthread_platform_initialized = false;

esp_err_t esp_openthread_platform_init(const esp_openthread_platform_config_t *config)
{
    if (config->radio_config.radio_mode != RADIO_MODE_UART_RCP) {
        otLogCritPlat("Radio mode not supported");
        return ESP_ERR_INVALID_ARG;
    }
    if (config->host_config.host_connection_mode != HOST_CONNECTION_MODE_NONE &&
            config->host_config.host_connection_mode != HOST_CONNECTION_MODE_UART) {
        otLogCritPlat("Host connection mode not supported");
        return ESP_ERR_INVALID_ARG;
    }
    if (s_openthread_platform_initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t error = ESP_OK;

    s_platform_config = *config;
    SuccessOrExit(error = esp_openthread_lock_init());
    if (config->host_config.host_connection_mode == HOST_CONNECTION_MODE_UART) {
        SuccessOrExit(error = esp_openthread_uart_init(config));
    }
    SuccessOrExit(error = esp_openthread_radio_init(config));
exit:
    if (error != ESP_OK) {
        esp_openthread_platform_deinit();
    }

    return error;
}

otInstance *esp_openthread_get_instance(void)
{
    return &ot::Instance::Get();
}

esp_err_t esp_openthread_platform_deinit(void)
{
    if (!s_openthread_platform_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
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
}

esp_err_t esp_openthread_platform_process(otInstance *instance, const esp_openthread_mainloop_context_t *mainloop)
{
    if (s_platform_config.host_config.host_connection_mode == HOST_CONNECTION_MODE_UART) {
        ESP_RETURN_ON_ERROR(esp_openthread_uart_process(), OT_PLAT_LOG_TAG, "esp_openthread_uart_process failed");
    }
    esp_openthread_radio_process(instance, mainloop);
    esp_openthread_alarm_process(instance);

    return ESP_OK;
}
