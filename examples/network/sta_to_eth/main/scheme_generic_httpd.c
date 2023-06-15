/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "sdkconfig.h"
#include <esp_log.h>
#include <esp_err.h>
#include <wifi_provisioning/manager.h>
#include <protocomm.h>
#include <protocomm_httpd.h>

static const char *TAG = "wifi_prov_scheme_httpd";

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
    protocomm_httpd_config_t default_config = {
        .data = {
            .config = PROTOCOMM_HTTPD_DEFAULT_CONFIG()
        }
    };

    /* Start protocomm server on top of HTTP */
    esp_err_t err = protocomm_httpd_start(pc, &default_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start protocomm HTTP server");
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

/**
 * @brief Creates a configuration for this custom provisioning scheme.
 *
 * We don't need to pass any config option at this moment, so we create
 * a dummy configuration since provisioning manager check for non-nullptr.
 * If needed we can extend this scheme to provide some options for httpd
 * or wifi provisioning.
 */
static void *new_config(void)
{
    return (void *)1;
}

static void delete_config(void *config)
{
}

static esp_err_t set_config_service(void *config, const char *service_name, const char *service_key)
{
    return ESP_OK;
}

static esp_err_t set_config_endpoint(void *config, const char *endpoint_name, uint16_t uuid)
{
    return ESP_OK;
}

/**
 * @brief Creating a generic HTTPD scheme
 */
const wifi_prov_scheme_t wifi_prov_scheme_httpd = {
    .prov_start          = prov_start,
    .prov_stop           = prov_stop,
    .new_config          = new_config,
    .delete_config       = delete_config,
    .set_config_service  = set_config_service,
    .set_config_endpoint = set_config_endpoint,
    .wifi_mode           = WIFI_MODE_STA
};
