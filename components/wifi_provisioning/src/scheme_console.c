// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#include <string.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_wifi.h>

#include <protocomm.h>
#include <protocomm_console.h>

#include "wifi_provisioning/scheme_console.h"
#include "wifi_provisioning_priv.h"

static const char *TAG = "wifi_prov_scheme_console";

extern const wifi_prov_scheme_t wifi_prov_scheme_console;

static esp_err_t prov_start(protocomm_t *pc, void *config)
{
    if (!pc) {
        ESP_LOGE(TAG, "Protocomm handle cannot be null");
        return ESP_ERR_INVALID_ARG;
    }

    if (!config) {
        ESP_LOGE(TAG, "Cannot start with null configuration");
        return ESP_ERR_INVALID_ARG;
    }

    protocomm_console_config_t *console_config = (protocomm_console_config_t *) config;

    /* Start protocomm console */
    esp_err_t err = protocomm_console_start(pc, console_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start protocomm HTTP server");
        return ESP_FAIL;
    }
    return ESP_OK;
}

static void *new_config(void)
{
    protocomm_console_config_t *console_config = malloc(sizeof(protocomm_console_config_t));
    if (!console_config) {
        ESP_LOGE(TAG, "Error allocating memory for new configuration");
        return NULL;
    }
    protocomm_console_config_t default_config = PROTOCOMM_CONSOLE_DEFAULT_CONFIG();
    memcpy(console_config, &default_config, sizeof(default_config));
    return console_config;
}

static void delete_config(void *config)
{
    if (!config) {
        ESP_LOGE(TAG, "Cannot delete null configuration");
        return;
    }
    free(config);
}

static esp_err_t set_config_service(void *config, const char *service_name, const char *service_key)
{
    return ESP_OK;
}

static esp_err_t set_config_endpoint(void *config, const char *endpoint_name, uint16_t uuid)
{
    return ESP_OK;
}

const wifi_prov_scheme_t wifi_prov_scheme_console = {
    .prov_start          = prov_start,
    .prov_stop           = protocomm_console_stop,
    .new_config          = new_config,
    .delete_config       = delete_config,
    .set_config_service  = set_config_service,
    .set_config_endpoint = set_config_endpoint,
    .wifi_mode           = WIFI_MODE_STA
};
