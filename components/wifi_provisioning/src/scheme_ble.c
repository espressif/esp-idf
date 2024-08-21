/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <esp_log.h>
#include <esp_err.h>
#ifdef CONFIG_BT_CONTROLLER_ENABLED
#include "esp_bt.h"
#endif

#include <protocomm.h>
#include <protocomm_ble.h>

#include "wifi_provisioning/scheme_ble.h"
#include "wifi_provisioning_priv.h"

static const char *TAG = "wifi_prov_scheme_ble";

extern const wifi_prov_scheme_t wifi_prov_scheme_ble;

static uint8_t *custom_service_uuid;
static uint8_t *custom_ble_addr;
static uint8_t custom_keep_ble_on;

static uint8_t *custom_manufacturer_data;
static size_t custom_manufacturer_data_len;

esp_err_t wifi_prov_mgr_keep_ble_on(uint8_t is_on_after_ble_stop)
{
    if (!is_on_after_ble_stop) {
        return ESP_ERR_INVALID_ARG;
    }

    custom_keep_ble_on = is_on_after_ble_stop;
    return ESP_OK;
}

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

    ble_config->keep_ble_on = custom_keep_ble_on;

#if defined(CONFIG_WIFI_PROV_BLE_BONDING)
    ble_config->ble_bonding = 1;
#endif

#if defined(CONFIG_WIFI_PROV_BLE_SEC_CONN) || defined(CONFIG_BT_BLUEDROID_ENABLED)
    ble_config->ble_sm_sc = 1;
#endif

#if defined(CONFIG_WIFI_PROV_BLE_FORCE_ENCRYPTION)
    ble_config->ble_link_encryption = 1;
#endif
    /* Start protocomm as BLE service */
    if (protocomm_ble_start(pc, ble_config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start protocomm BLE service");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t wifi_prov_scheme_ble_set_random_addr(const uint8_t *addr)
{
    if (!addr) {
        return ESP_ERR_INVALID_ARG;
    }

    custom_ble_addr = (uint8_t *) malloc(BLE_ADDR_LEN);
    if (custom_ble_addr == NULL) {
         ESP_LOGE(TAG, "Error allocating memory for random address");
         return ESP_ERR_NO_MEM;
    }

    memcpy(custom_ble_addr, addr, BLE_ADDR_LEN);
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

esp_err_t wifi_prov_scheme_ble_set_mfg_data(uint8_t *mfg_data, ssize_t mfg_data_len)
{
    if (!mfg_data || !mfg_data_len) {
        return ESP_ERR_INVALID_ARG;
    }

    custom_manufacturer_data = (uint8_t *) malloc(mfg_data_len);
    if (custom_manufacturer_data == NULL) {
        ESP_LOGE(TAG, "Error allocating memory for mfg_data");
        return ESP_ERR_NO_MEM;
    }

    custom_manufacturer_data_len = mfg_data_len;
    memcpy(custom_manufacturer_data, mfg_data, mfg_data_len);
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
        0x07, 0xed, 0x9b, 0x2d, 0x0f, 0x06, 0x7c, 0x87,
        0x9b, 0x43, 0x43, 0x6b, 0x4d, 0x24, 0x75, 0x17,
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
    /* Set manufacturer data if it is provided by app */
    if (custom_manufacturer_data) {
        size_t mfg_data_len = custom_manufacturer_data_len;
        /* Manufacturer Data Length + 2 Byte header + BLE Device name + 2 Byte
         * header <= 31 Bytes */
        if (mfg_data_len > (MAX_BLE_MANUFACTURER_DATA_LEN - sizeof(ble_config->device_name) - 2)) {
            ESP_LOGE(TAG, "Manufacturer data length is more than the max allowed size; expect truncated mfg_data ");
            /* XXX Does it even make any sense to set truncated mfg_data ? The
             * only reason to not return failure from here is provisioning
             * should continue as it is with error prints for mfg_data length */
            mfg_data_len = MAX_BLE_MANUFACTURER_DATA_LEN - sizeof(ble_config->device_name) - 2;
        }

        ble_config->manufacturer_data = custom_manufacturer_data;
        ble_config->manufacturer_data_len = mfg_data_len;
    } else {
        ble_config->manufacturer_data = NULL;
        ble_config->manufacturer_data_len = 0;
    }

    if (custom_ble_addr){
        ble_config->ble_addr = custom_ble_addr;
    } else {
        ble_config->ble_addr = NULL;
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
    switch (event) {
    case WIFI_PROV_INIT: {
#ifdef CONFIG_BT_CONTROLLER_ENABLED
        /* Release BT memory, as we need only BLE */
        esp_err_t err = esp_bt_mem_release(ESP_BT_MODE_CLASSIC_BT);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "bt_mem_release of classic BT failed %d", err);
        } else {
            ESP_LOGI(TAG, "BT memory released");
        }
#endif
        break;
    }
    case WIFI_PROV_DEINIT: {
#ifndef CONFIG_WIFI_PROV_KEEP_BLE_ON_AFTER_PROV
#ifdef CONFIG_BT_CONTROLLER_ENABLED
        /* Release memory used by BLE and Bluedroid host stack */
        esp_err_t err = esp_bt_mem_release(ESP_BT_MODE_BTDM);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "bt_mem_release of BTDM failed %d", err);
        } else {
            ESP_LOGI(TAG, "BTDM memory released");
        }
#endif
#endif
        break;
    }
    default:
        break;
    }
}

/* Used when BT is not needed by application */
void wifi_prov_scheme_ble_event_cb_free_bt(void *user_data, wifi_prov_cb_event_t event, void *event_data)
{
    switch (event) {
    case WIFI_PROV_INIT: {
#ifdef CONFIG_BT_CONTROLLER_ENABLED
        esp_err_t err;
        /* Release BT memory, as we need only BLE */
        err = esp_bt_mem_release(ESP_BT_MODE_CLASSIC_BT);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "bt_mem_release of classic BT failed %d", err);
        } else {
            ESP_LOGI(TAG, "BT memory released");
        }
#endif
        break;
    }
    default:
        break;
    }
}

/* Used when BLE is not needed by application */
void wifi_prov_scheme_ble_event_cb_free_ble(void *user_data, wifi_prov_cb_event_t event, void *event_data)
{
    switch (event) {
    case WIFI_PROV_DEINIT: {
#ifndef CONFIG_WIFI_PROV_KEEP_BLE_ON_AFTER_PROV
#ifdef CONFIG_BT_CONTROLLER_ENABLED
        esp_err_t err;
        /* Release memory used by BLE stack */
        err = esp_bt_mem_release(ESP_BT_MODE_BLE);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "bt_mem_release of BLE failed %d", err);
        } else {
            ESP_LOGI(TAG, "BLE memory released");
        }
#endif
#endif
        break;
    }
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
