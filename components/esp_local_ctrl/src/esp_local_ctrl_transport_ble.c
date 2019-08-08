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

#include <stdio.h>
#include <string.h>
#include <esp_err.h>
#include <esp_log.h>

#include <protocomm_ble.h>
#include <esp_local_ctrl.h>

#include "esp_local_ctrl_priv.h"

#define LOCAL_CTRL_VERSION "v1.0"

static const char *TAG = "esp_local_ctrl_transport_ble";

static esp_err_t start_ble_transport(protocomm_t *pc, const esp_local_ctrl_transport_config_t *config)
{
    if (!config || !config->ble) {
        ESP_LOGE(TAG, "NULL configuration provided");
        return ESP_ERR_INVALID_ARG;
    }
    return protocomm_ble_start(pc, config->ble);
}

static void stop_ble_transport(protocomm_t *pc)
{
    protocomm_ble_stop(pc);
}

static esp_err_t copy_ble_config(esp_local_ctrl_transport_config_t *dest_config, const esp_local_ctrl_transport_config_t *src_config)
{
    if (!dest_config || !src_config || !src_config->ble) {
        ESP_LOGE(TAG, "NULL arguments provided");
        return ESP_ERR_INVALID_ARG;
    }

    dest_config->ble = calloc(1, sizeof(protocomm_ble_config_t));
    if (!dest_config->ble) {
        ESP_LOGE(TAG, "Failed to allocate memory for BLE transport config");
        return ESP_ERR_NO_MEM;
    }

    /* Copy BLE device name */
    memcpy(dest_config->ble->device_name,
           src_config->ble->device_name,
           sizeof(src_config->ble->device_name));

    /* Copy Service UUID */
    memcpy(dest_config->ble->service_uuid,
           src_config->ble->service_uuid,
           sizeof(src_config->ble->service_uuid));

    dest_config->ble->nu_lookup_count = 0;
    if (src_config->ble->nu_lookup_count) {
        /* Copy any provided name-uuid lookup table */
        dest_config->ble->nu_lookup = calloc(src_config->ble->nu_lookup_count,
                                             sizeof(protocomm_ble_name_uuid_t));
        if (!dest_config->ble->nu_lookup) {
            ESP_LOGE(TAG, "Failed to allocate memory for BLE characteristic names");
            free(dest_config->ble);
            return ESP_ERR_NO_MEM;
        }
        for (uint16_t i = 0; i < src_config->ble->nu_lookup_count; i++) {
            dest_config->ble->nu_lookup[i].uuid = src_config->ble->nu_lookup[i].uuid;
            if (!src_config->ble->nu_lookup[i].name) {
                ESP_LOGE(TAG, "Endpoint name cannot be null");
                return ESP_ERR_INVALID_ARG;
            }
            dest_config->ble->nu_lookup[i].name = strdup(src_config->ble->nu_lookup[i].name);
            if (!dest_config->ble->nu_lookup[i].name) {
                ESP_LOGE(TAG, "Failed to allocate memory for endpoint name");
                return ESP_ERR_NO_MEM;
            }
            dest_config->ble->nu_lookup_count++;
        }
    }
    return ESP_OK;
}

static esp_err_t declare_endpoint(esp_local_ctrl_transport_config_t *config, const char *ep_name, uint16_t ep_uuid)
{
    if (!config || !config->ble) {
        ESP_LOGE(TAG, "NULL configuration provided");
        return ESP_ERR_INVALID_ARG;
    }

    protocomm_ble_name_uuid_t *nu_lookup = realloc(config->ble->nu_lookup,
                                                   (config->ble->nu_lookup_count + 1)
                                                   * sizeof(protocomm_ble_name_uuid_t));
    if (!nu_lookup) {
        ESP_LOGE(TAG, "Failed to allocate memory for new endpoint entry");
        return ESP_ERR_NO_MEM;
    }
    config->ble->nu_lookup = nu_lookup;
    nu_lookup[config->ble->nu_lookup_count].uuid = ep_uuid;
    nu_lookup[config->ble->nu_lookup_count].name = strdup(ep_name);
    if (!nu_lookup[config->ble->nu_lookup_count].name) {
        ESP_LOGE(TAG, "Failed to allocate memory for new endpoint name");
        return ESP_ERR_NO_MEM;
    }
    config->ble->nu_lookup_count++;
    return ESP_OK;
}

static void free_config(esp_local_ctrl_transport_config_t *config)
{
    if (config && config->ble) {
        for (unsigned int i = 0; i < config->ble->nu_lookup_count; i++) {
            free((void*) config->ble->nu_lookup[i].name);
        }
        free(config->ble->nu_lookup);
        free(config->ble);
        config->ble = NULL;
    }
}

const esp_local_ctrl_transport_t *esp_local_ctrl_get_transport_ble(void)
{
    static const esp_local_ctrl_transport_t tp = {
        .start_service = start_ble_transport,
        .stop_service  = stop_ble_transport,
        .copy_config   = copy_ble_config,
        .declare_ep    = declare_endpoint,
        .free_config   = free_config
    };
    return &tp;
};
