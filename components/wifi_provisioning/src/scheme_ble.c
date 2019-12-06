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
#include <esp_bt.h>

#include <protocomm.h>
#include <protocomm_ble.h>

#include "wifi_provisioning/scheme_ble.h"
#include "wifi_provisioning_priv.h"

static const char *TAG = "wifi_prov_scheme_ble";

extern const wifi_prov_scheme_t wifi_prov_scheme_ble;

static uint8_t *custom_service_uuid;

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

    protocomm_ble_config_t *ble_config = (protocomm_ble_config_t *) config;

    /* Start protocomm as BLE service */
    if (protocomm_ble_start(pc, ble_config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start protocomm BLE service");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t wifi_prov_scheme_ble_set_service_uuid(uint8_t *uuid128)
{
    if (!uuid128) {
        return ESP_ERR_INVALID_ARG;
    }
    custom_service_uuid = uuid128;
    return ESP_OK;
}

static void *new_config(void)
{
    protocomm_ble_config_t *ble_config = calloc(1, sizeof(protocomm_ble_config_t));
    if (!ble_config) {
        ESP_LOGE(TAG, "Error allocating memory for new configuration");
        return NULL;
    }

    /* The default provisioning service UUID */
    const uint8_t service_uuid[16] = {
        /* LSB <---------------------------------------
         * ---------------------------------------> MSB */
        0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80,
        0x00, 0x10, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00,
    };

    memcpy(ble_config->service_uuid, service_uuid, sizeof(ble_config->service_uuid));
    return ble_config;
}

static void delete_config(void *config)
{
    if (!config) {
        ESP_LOGE(TAG, "Cannot delete null configuration");
        return;
    }

    protocomm_ble_config_t *ble_config = (protocomm_ble_config_t *) config;
    for (unsigned int i = 0; i < ble_config->nu_lookup_count; i++) {
        free((void *)ble_config->nu_lookup[i].name);
    }
    free(ble_config->nu_lookup);
    free(ble_config);
}

static esp_err_t set_config_service(void *config, const char *service_name, const char *service_key)
{
    if (!config) {
        ESP_LOGE(TAG, "Cannot set null configuration");
        return ESP_ERR_INVALID_ARG;
    }

    if (!service_name) {
        ESP_LOGE(TAG, "Service name cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }

    protocomm_ble_config_t *ble_config = (protocomm_ble_config_t *) config;
    strlcpy(ble_config->device_name,  service_name, sizeof(ble_config->device_name));

    /* If a custom service UUID has been provided, override the default one */
    if (custom_service_uuid) {
        memcpy(ble_config->service_uuid, custom_service_uuid, sizeof(ble_config->service_uuid));
    }
    return ESP_OK;
}

static esp_err_t set_config_endpoint(void *config, const char *endpoint_name, uint16_t uuid)
{
    if (!config) {
        ESP_LOGE(TAG, "Cannot set null configuration");
        return ESP_ERR_INVALID_ARG;
    }

    if (!endpoint_name) {
        ESP_LOGE(TAG, "EP name cannot be null");
        return ESP_ERR_INVALID_ARG;
    }

    protocomm_ble_config_t *ble_config = (protocomm_ble_config_t *) config;

    char *copy_ep_name = strdup(endpoint_name);
    if (!copy_ep_name) {
        ESP_LOGE(TAG, "Error allocating memory for EP name");
        return ESP_ERR_NO_MEM;
    }

    protocomm_ble_name_uuid_t *lookup_table = (
                realloc(ble_config->nu_lookup, (ble_config->nu_lookup_count + 1) * sizeof(protocomm_ble_name_uuid_t)));
    if (!lookup_table) {
        ESP_LOGE(TAG, "Error allocating memory for EP-UUID lookup table");
        return ESP_ERR_NO_MEM;
    }

    lookup_table[ble_config->nu_lookup_count].name = copy_ep_name;
    lookup_table[ble_config->nu_lookup_count].uuid = uuid;
    ble_config->nu_lookup = lookup_table;
    ble_config->nu_lookup_count += 1;
    return ESP_OK;
}

/* Used when both BT and BLE are not needed by application */
void wifi_prov_scheme_ble_event_cb_free_btdm(void *user_data, wifi_prov_cb_event_t event, void *event_data)
{
    esp_err_t err;
    switch (event) {
        case WIFI_PROV_INIT:
            /* Release BT memory, as we need only BLE */
            err = esp_bt_mem_release(ESP_BT_MODE_CLASSIC_BT);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "bt_mem_release of classic BT failed %d", err);
            } else {
                ESP_LOGI(TAG, "BT memory released");
            }
            break;

        case WIFI_PROV_DEINIT:
            /* Release memory used by BLE and Bluedroid host stack */
            err = esp_bt_mem_release(ESP_BT_MODE_BTDM);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "bt_mem_release of BTDM failed %d", err);
            } else {
                ESP_LOGI(TAG, "BTDM memory released");
            }
            break;

        default:
            break;
    }
}

/* Used when BT is not needed by application */
void wifi_prov_scheme_ble_event_cb_free_bt(void *user_data, wifi_prov_cb_event_t event, void *event_data)
{
    esp_err_t err;
    switch (event) {
        case WIFI_PROV_INIT:
            /* Release BT memory, as we need only BLE */
            err = esp_bt_mem_release(ESP_BT_MODE_CLASSIC_BT);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "bt_mem_release of classic BT failed %d", err);
            } else {
                ESP_LOGI(TAG, "BT memory released");
            }
            break;

        default:
            break;
    }
}

/* Used when BLE is not needed by application */
void wifi_prov_scheme_ble_event_cb_free_ble(void *user_data, wifi_prov_cb_event_t event, void *event_data)
{
    esp_err_t err;
    switch (event) {
        case WIFI_PROV_DEINIT:
            /* Release memory used by BLE stack */
            err = esp_bt_mem_release(ESP_BT_MODE_BLE);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "bt_mem_release of BLE failed %d", err);
            } else {
                ESP_LOGI(TAG, "BLE memory released");
            }
            break;

        default:
            break;
    }
}

const wifi_prov_scheme_t wifi_prov_scheme_ble = {
    .prov_start          = prov_start,
    .prov_stop           = protocomm_ble_stop,
    .new_config          = new_config,
    .delete_config       = delete_config,
    .set_config_service  = set_config_service,
    .set_config_endpoint = set_config_endpoint,
    .wifi_mode           = WIFI_MODE_STA
};
