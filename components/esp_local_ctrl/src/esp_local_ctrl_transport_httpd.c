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

#include <mdns.h>
#include <protocomm_httpd.h>
#include <esp_local_ctrl.h>
#include <esp_https_server.h>

#include "esp_local_ctrl_priv.h"

#define LOCAL_CTRL_VERSION "v1.0"

static const char *TAG = "esp_local_ctrl_transport_httpd";

static httpd_handle_t server_handle = NULL;

static esp_err_t start_httpd_transport(protocomm_t *pc, const esp_local_ctrl_transport_config_t *config)
{
    if (!config || !config->httpd) {
        ESP_LOGE(TAG, "NULL configuration provided");
        return ESP_ERR_INVALID_ARG;
    }

    /* Extract configured port */
    uint16_t port = (
        config->httpd->transport_mode == HTTPD_SSL_TRANSPORT_SECURE ?
            config->httpd->port_secure :
            config->httpd->port_insecure
    );

    esp_err_t err = mdns_service_add("Local Control Service", "_esp_local_ctrl",
                                     "_tcp", port, NULL, 0);
    if (err != ESP_OK) {
        /* mDNS is not mandatory for provisioning to work,
         * so print warning and return without failure */
        ESP_LOGW(TAG, "Error adding mDNS service! Check if mDNS is running");
    } else {
        /* Information to identify the roles of the various
         * protocomm endpoint URIs provided by the service */
        err |= mdns_service_txt_item_set("_esp_local_ctrl", "_tcp",
                                         "version_endpoint", "/esp_local_ctrl/version");
        err |= mdns_service_txt_item_set("_esp_local_ctrl", "_tcp",
                                         "session_endpoint", "/esp_local_ctrl/session");
        err |= mdns_service_txt_item_set("_esp_local_ctrl", "_tcp",
                                         "control_endpoint", "/esp_local_ctrl/control");
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error adding mDNS service text item");
        }
    }

    err = httpd_ssl_start(&server_handle, config->httpd);
    if (ESP_OK != err) {
        ESP_LOGE(TAG, "Error starting HTTPS service!");
        mdns_service_remove("_esp_local_ctrl", "_tcp");
        return err;
    }

    protocomm_httpd_config_t pc_conf = {
        .ext_handle_provided = true,
        .data = {
            .handle = &server_handle
        }
    };

    return protocomm_httpd_start(pc, &pc_conf);
}

static void stop_httpd_transport(protocomm_t *pc)
{
    mdns_service_remove("_esp_local_ctrl", "_tcp");
    protocomm_httpd_stop(pc);
    httpd_ssl_stop(server_handle);
    server_handle = NULL;
}

static esp_err_t copy_httpd_config(esp_local_ctrl_transport_config_t *dest_config, const esp_local_ctrl_transport_config_t *src_config)
{
    if (!dest_config || !src_config || !src_config->httpd) {
        ESP_LOGE(TAG, "NULL configuration provided");
        return ESP_ERR_INVALID_ARG;
    }

    dest_config->httpd = calloc(1, sizeof(httpd_ssl_config_t));
    if (!dest_config->httpd) {
        ESP_LOGE(TAG, "Failed to allocate memory for HTTPD transport config");
        return ESP_ERR_NO_MEM;
    }
    memcpy(dest_config->httpd,
           src_config->httpd,
           sizeof(httpd_ssl_config_t));
    return ESP_OK;
}

static void free_config(esp_local_ctrl_transport_config_t *config)
{
    if (config && config->httpd) {
        free(config->httpd);
        config->httpd = NULL;
    }
}

const esp_local_ctrl_transport_t *esp_local_ctrl_get_transport_httpd(void)
{
    static const esp_local_ctrl_transport_t tp = {
        .start_service = start_httpd_transport,
        .stop_service  = stop_httpd_transport,
        .copy_config   = copy_httpd_config,
        .declare_ep    = NULL,
        .free_config   = free_config
    };
    return &tp;
};
