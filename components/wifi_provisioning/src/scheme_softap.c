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

#include <mdns.h>
#include <protocomm.h>
#include <protocomm_httpd.h>

#include "wifi_provisioning/scheme_softap.h"
#include "wifi_provisioning_priv.h"

typedef struct softap_config {
    protocomm_httpd_config_t httpd_config;
    char ssid[33];
    char password[65];
} wifi_prov_softap_config_t;

static const char *TAG = "wifi_prov_scheme_softap";

extern const wifi_prov_scheme_t wifi_prov_scheme_softap;
static void *scheme_softap_prov_httpd_handle;
static esp_err_t start_wifi_ap(const char *ssid, const char *pass)
{
    /* Build Wi-Fi configuration for AP mode */
    wifi_config_t wifi_config = {
        .ap = {
            .max_connection = 5,
        },
    };

    /* SSID can be a non NULL terminated string if `ap.ssid_len` is specified
     * Hence, memcpy is used to support 32 bytes long SSID per 802.11 standard */
    const size_t ssid_len = strnlen(ssid, sizeof(wifi_config.ap.ssid));
    memcpy(wifi_config.ap.ssid, ssid, ssid_len);
    wifi_config.ap.ssid_len = ssid_len;

    if (strlen(pass) == 0) {
        memset(wifi_config.ap.password, 0, sizeof(wifi_config.ap.password));
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    } else {
        strlcpy((char *) wifi_config.ap.password, pass, sizeof(wifi_config.ap.password));
        wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    }

    /* Run Wi-Fi in AP + STA mode with configuration built above */
    esp_err_t err = esp_wifi_set_mode(WIFI_MODE_APSTA);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set Wi-Fi mode : %d", err);
        return err;
    }
    err = esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set Wi-Fi config : %d", err);
        return err;
    }

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

    wifi_prov_softap_config_t *softap_config = (wifi_prov_softap_config_t *) config;

    protocomm_httpd_config_t *httpd_config = &softap_config->httpd_config;

    if (scheme_softap_prov_httpd_handle) {
        httpd_config->ext_handle_provided = true;
        httpd_config->data.handle = scheme_softap_prov_httpd_handle;
    }

    /* Start protocomm server on top of HTTP */
    esp_err_t err = protocomm_httpd_start(pc, httpd_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start protocomm HTTP server");
        return err;
    }

    /* Start Wi-Fi softAP with specified ssid and password */
    err = start_wifi_ap(softap_config->ssid, softap_config->password);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start Wi-Fi AP");
        protocomm_httpd_stop(pc);
        return err;
    }

    /* Add mDNS service for allowing discovery of provisioning
     * service on the SoftAP network (Optional). Even though
     * this is an http service we identify it by _esp_wifi_prov so
     * that application is free to use _http without conflict */
    err = mdns_service_add("Wi-Fi Provisioning Service", "_esp_wifi_prov", "_tcp",
                           softap_config->httpd_config.data.config.port, NULL, 0);
    if (err != ESP_OK) {
        /* mDNS is not mandatory for provisioning to work,
         * so print warning and return without failure */
        ESP_LOGW(TAG, "Error adding mDNS service! Check if mDNS is running");
    } else {
        /* Information to identify the roles of the various
         * protocomm endpoint URIs provided by the service */
        err |= mdns_service_txt_item_set("_esp_wifi_prov", "_tcp", "version_endpoint", "/proto-ver");
        err |= mdns_service_txt_item_set("_esp_wifi_prov", "_tcp", "session_endpoint", "/prov-session");
        err |= mdns_service_txt_item_set("_esp_wifi_prov", "_tcp", "config_endpoint", "/prov-config");
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error adding mDNS service text item");
        }
    }
    return ESP_OK;
}

static esp_err_t prov_stop(protocomm_t *pc)
{
    esp_err_t err = protocomm_httpd_stop(pc);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Error occurred while stopping protocomm_httpd");
    }

    mdns_service_remove("_esp_wifi_prov", "_tcp");
    return err;
}

static void *new_config(void)
{
    wifi_prov_softap_config_t *softap_config = calloc(1, sizeof(wifi_prov_softap_config_t));
    if (!softap_config) {
        ESP_LOGE(TAG, "Error allocating memory for new configuration");
        return NULL;
    }
    protocomm_httpd_config_t default_config = {
        .data = {
            .config = PROTOCOMM_HTTPD_DEFAULT_CONFIG()
        }
    };
    softap_config->httpd_config = default_config;
    return softap_config;
}

static void delete_config(void *config)
{
    if (!config) {
        ESP_LOGE(TAG, "Cannot delete null configuration");
        return;
    }

    wifi_prov_softap_config_t *softap_config = (wifi_prov_softap_config_t *) config;
    free(softap_config);
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

    wifi_prov_softap_config_t *softap_config = (wifi_prov_softap_config_t *) config;
    strlcpy(softap_config->ssid, service_name, sizeof(softap_config->ssid));
    if (service_key) {
        strlcpy(softap_config->password, service_key,  sizeof(softap_config->password));
    }
    return ESP_OK;
}

static esp_err_t set_config_endpoint(void *config, const char *endpoint_name, uint16_t uuid)
{
    return ESP_OK;
}

void wifi_prov_scheme_softap_set_httpd_handle(void *handle)
{
    scheme_softap_prov_httpd_handle = handle;
}

const wifi_prov_scheme_t wifi_prov_scheme_softap = {
    .prov_start          = prov_start,
    .prov_stop           = prov_stop,
    .new_config          = new_config,
    .delete_config       = delete_config,
    .set_config_service  = set_config_service,
    .set_config_endpoint = set_config_endpoint,
    .wifi_mode           = WIFI_MODE_APSTA
};
