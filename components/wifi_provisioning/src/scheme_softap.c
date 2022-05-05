/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"

#include <string.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_wifi.h>

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

    return ESP_OK;
}

static esp_err_t prov_stop(protocomm_t *pc)
{
    esp_err_t err = protocomm_httpd_stop(pc);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Error occurred while stopping protocomm_httpd");
    }

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
    if (service_key) {
        const int service_key_len = strlen(service_key);
        if (service_key_len < 8 || service_key_len >= sizeof(softap_config->password)) {
            ESP_LOGE(TAG, "Incorrect passphrase length for softAP: %d (Expected: Min - 8, Max - 64)", service_key_len);
            return ESP_ERR_INVALID_ARG;
        }
        strlcpy(softap_config->password, service_key,  sizeof(softap_config->password));
    }
    strlcpy(softap_config->ssid, service_name, sizeof(softap_config->ssid));
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
