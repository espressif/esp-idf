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

#include <esp_wifi.h>
#include <esp_netif.h>

#include "wifi_provisioning/wifi_config.h"
#include "wifi_provisioning/wifi_scan.h"
#include "wifi_provisioning/manager.h"
#include "wifi_provisioning_priv.h"

static const char *TAG = "wifi_prov_handlers";

/* Provide definition of wifi_prov_ctx_t */
struct wifi_prov_ctx {
    wifi_config_t wifi_cfg;
};

static wifi_config_t *get_config(wifi_prov_ctx_t **ctx)
{
    return (*ctx ? & (*ctx)->wifi_cfg : NULL);
}

static wifi_config_t *new_config(wifi_prov_ctx_t **ctx)
{
    free(*ctx);
    (*ctx) = (wifi_prov_ctx_t *) calloc(1, sizeof(wifi_prov_ctx_t));
    return get_config(ctx);
}

static void free_config(wifi_prov_ctx_t **ctx)
{
    free(*ctx);
    *ctx = NULL;
}

static esp_err_t get_status_handler(wifi_prov_config_get_data_t *resp_data, wifi_prov_ctx_t **ctx)
{
    /* Initialize to zero */
    memset(resp_data, 0, sizeof(wifi_prov_config_get_data_t));

    if (wifi_prov_mgr_get_wifi_state(&resp_data->wifi_state) != ESP_OK) {
        ESP_LOGW(TAG, "Wi-Fi provisioning manager not running");
        return ESP_ERR_INVALID_STATE;
    }

    if (resp_data->wifi_state == WIFI_PROV_STA_CONNECTED) {
        ESP_LOGD(TAG, "Got state : connected");

        /* IP Addr assigned to STA */
        esp_netif_ip_info_t ip_info;
        esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), &ip_info);
        esp_ip4addr_ntoa(&ip_info.ip, resp_data->conn_info.ip_addr, sizeof(resp_data->conn_info.ip_addr));


        /* AP information to which STA is connected */
        wifi_ap_record_t ap_info;
        esp_wifi_sta_get_ap_info(&ap_info);
        memcpy(resp_data->conn_info.bssid, (char *)ap_info.bssid, sizeof(ap_info.bssid));
        memcpy(resp_data->conn_info.ssid,  (char *)ap_info.ssid,  sizeof(ap_info.ssid));
        resp_data->conn_info.channel   = ap_info.primary;
        resp_data->conn_info.auth_mode = ap_info.authmode;

        /* Tell manager to stop provisioning service */
        wifi_prov_mgr_done();
    } else if (resp_data->wifi_state == WIFI_PROV_STA_DISCONNECTED) {
        ESP_LOGD(TAG, "Got state : disconnected");

        /* If disconnected, convey reason */
        wifi_prov_mgr_get_wifi_disconnect_reason(&resp_data->fail_reason);
    } else {
        ESP_LOGD(TAG, "Got state : connecting");
    }
    return ESP_OK;
}

static esp_err_t set_config_handler(const wifi_prov_config_set_data_t *req_data, wifi_prov_ctx_t **ctx)
{
    wifi_config_t *wifi_cfg = get_config(ctx);
    if (wifi_cfg) {
        free_config(ctx);
    }

    wifi_cfg = new_config(ctx);
    if (!wifi_cfg) {
        ESP_LOGE(TAG, "Unable to allocate Wi-Fi config");
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGD(TAG, "Wi-Fi Credentials Received");

    /* Using strncpy allows the max SSID length to be 32 bytes (as per 802.11 standard).
     * But this doesn't guarantee that the saved SSID will be null terminated, because
     * wifi_cfg->sta.ssid is also 32 bytes long (without extra 1 byte for null character) */
    strncpy((char *) wifi_cfg->sta.ssid, req_data->ssid, sizeof(wifi_cfg->sta.ssid));

    /* Using strlcpy allows both max passphrase length (63 bytes) and ensures null termination
     * because size of wifi_cfg->sta.password is 64 bytes (1 extra byte for null character) */
    strlcpy((char *) wifi_cfg->sta.password, req_data->password, sizeof(wifi_cfg->sta.password));
    return ESP_OK;
}

static esp_err_t apply_config_handler(wifi_prov_ctx_t **ctx)
{
    wifi_config_t *wifi_cfg = get_config(ctx);
    if (!wifi_cfg) {
        ESP_LOGE(TAG, "Wi-Fi config not set");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret = wifi_prov_mgr_configure_sta(wifi_cfg);
    if (ret == ESP_OK) {
        ESP_LOGD(TAG, "Wi-Fi Credentials Applied");
    } else {
        ESP_LOGE(TAG, "Failed to apply Wi-Fi Credentials");
    }

    free_config(ctx);
    return ret;
}

esp_err_t get_wifi_prov_handlers(wifi_prov_config_handlers_t *ptr)
{
    if (!ptr) {
        return ESP_ERR_INVALID_ARG;
    }
    ptr->get_status_handler   = get_status_handler;
    ptr->set_config_handler   = set_config_handler;
    ptr->apply_config_handler = apply_config_handler;
    ptr->ctx = NULL;
    return ESP_OK;
}

/*************************************************************************/

static esp_err_t scan_start(bool blocking, bool passive,
                            uint8_t group_channels, uint32_t period_ms,
                            wifi_prov_scan_ctx_t **ctx)
{
    return wifi_prov_mgr_wifi_scan_start(blocking, passive, group_channels, period_ms);
}

static esp_err_t scan_status(bool *scan_finished,
                             uint16_t *result_count,
                             wifi_prov_scan_ctx_t **ctx)
{
    *scan_finished = wifi_prov_mgr_wifi_scan_finished();
    *result_count  = wifi_prov_mgr_wifi_scan_result_count();
    return ESP_OK;
}

static esp_err_t scan_result(uint16_t result_index,
                             wifi_prov_scan_result_t *result,
                             wifi_prov_scan_ctx_t **ctx)
{
    const wifi_ap_record_t *record = wifi_prov_mgr_wifi_scan_result(result_index);
    if (!record) {
        return ESP_FAIL;
    }

    /* Compile time check ensures memory safety in case SSID length in
     * record / result structure definition changes in future */
    _Static_assert(sizeof(result->ssid) == sizeof(record->ssid),
                   "source and destination should be of same size");
    memcpy(result->ssid, record->ssid, sizeof(record->ssid));
    memcpy(result->bssid, record->bssid, sizeof(record->bssid));
    result->channel = record->primary;
    result->rssi = record->rssi;
    result->auth = record->authmode;
    return ESP_OK;
}

esp_err_t get_wifi_scan_handlers(wifi_prov_scan_handlers_t *ptr)
{
    if (!ptr) {
        return ESP_ERR_INVALID_ARG;
    }
    ptr->scan_start  = scan_start;
    ptr->scan_status = scan_status;
    ptr->scan_result = scan_result;
    ptr->ctx = NULL;
    return ESP_OK;
}
