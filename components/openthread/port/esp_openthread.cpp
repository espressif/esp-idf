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

#include "esp_check.h"
#include "esp_openthread.h"
#include "esp_openthread_common_macro.h"
#include "esp_openthread_lock.h"
#include "esp_openthread_netif_glue_priv.h"
#include "esp_openthread_platform.h"
#include "esp_openthread_types.h"
#include "freertos/FreeRTOS.h"
#include "openthread/instance.h"
#include "openthread/tasklet.h"

static void esp_openthread_state_callback(otChangedFlags changed_flags, void *ctx)
{
    esp_openthread_netif_glue_state_callback(changed_flags);
}

static esp_err_t register_esp_openthread_state_callbacks(void)
{
    otInstance *instance = esp_openthread_get_instance();
    ESP_RETURN_ON_FALSE(otSetStateChangedCallback(instance, esp_openthread_state_callback, NULL) == OT_ERROR_NONE,
                        ESP_FAIL, OT_PLAT_LOG_TAG, "Failed to install OpenThread state callback");
    return ESP_OK;
}

esp_err_t esp_openthread_init(const esp_openthread_platform_config_t *config)
{
    ESP_RETURN_ON_ERROR(esp_openthread_platform_init(config), OT_PLAT_LOG_TAG,
                        "Failed to initialize OpenThread platform driver");
    ESP_RETURN_ON_FALSE(otInstanceInitSingle() != NULL, ESP_FAIL, OT_PLAT_LOG_TAG,
                        "Failed to initialize OpenThread instance");

    return register_esp_openthread_state_callbacks();
}

esp_err_t esp_openthread_launch_mainloop(void)
{
    esp_openthread_mainloop_context_t mainloop;
    otInstance *instance = esp_openthread_get_instance();
    esp_err_t error = ESP_OK;

    while (true) {
        FD_ZERO(&mainloop.read_fds);
        FD_ZERO(&mainloop.write_fds);
        FD_ZERO(&mainloop.error_fds);

        mainloop.max_fd = -1;
        mainloop.timeout.tv_sec = 10;
        mainloop.timeout.tv_usec = 0;

        esp_openthread_lock_acquire(portMAX_DELAY);
        esp_openthread_platform_update(&mainloop);
        if (otTaskletsArePending(instance)) {
            mainloop.timeout.tv_sec = 0;
            mainloop.timeout.tv_usec = 0;
        }
        esp_openthread_lock_release();

        if (select(mainloop.max_fd + 1, &mainloop.read_fds, &mainloop.write_fds, &mainloop.error_fds,
                   &mainloop.timeout) >= 0) {
            esp_openthread_lock_acquire(portMAX_DELAY);
            otTaskletsProcess(instance);
            error = esp_openthread_platform_process(instance, &mainloop);
            esp_openthread_lock_release();
            if (error != ESP_OK) {
                ESP_LOGE(OT_PLAT_LOG_TAG, "esp_openthread_platform_process failed");
                break;
            }
        } else {
            error = ESP_FAIL;
            ESP_LOGE(OT_PLAT_LOG_TAG, "OpenThread system polling failed");
            break;
        }
    }
    return error;
}

esp_err_t esp_openthread_deinit(void)
{
    otInstanceFinalize(esp_openthread_get_instance());
    return esp_openthread_platform_deinit();
}
