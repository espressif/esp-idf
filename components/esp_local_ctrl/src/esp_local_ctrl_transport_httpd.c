/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <esp_err.h>
#include <esp_log.h>

#if defined __has_include
#   if __has_include("mdns.h")
#       define WITH_MDNS
#       include "mdns.h"
#   endif
#endif

#include <esp_netif.h>
#include <protocomm_httpd.h>
#include <esp_local_ctrl.h>
#ifdef CONFIG_ESP_HTTPS_SERVER_ENABLE
#include <esp_https_server.h>
#else
#include <esp_http_server.h>
#endif

#ifdef CONFIG_ESP_HTTPS_SERVER_ENABLE
#define esp_local_ctrl_httpd_start  httpd_ssl_start
#define esp_local_ctrl_httpd_stop   httpd_ssl_stop
#else
#define esp_local_ctrl_httpd_start  httpd_start
#define esp_local_ctrl_httpd_stop   httpd_stop
#endif

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

    esp_err_t err;
#ifdef WITH_MDNS
    uint16_t port = 0;
#ifdef CONFIG_ESP_HTTPS_SERVER_ENABLE
    /* Extract configured port */
    port = (
        config->httpd->transport_mode == HTTPD_SSL_TRANSPORT_SECURE ?
            config->httpd->port_secure :
            config->httpd->port_insecure
    );
#else
    port = config->httpd->server_port;
#endif
    err = mdns_service_add("Local Control Service", "_esp_local_ctrl",
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
#endif
    err = esp_local_ctrl_httpd_start(&server_handle, config->httpd);
    if (ESP_OK != err) {
        ESP_LOGE(TAG, "Error starting HTTP/S service!");
#ifdef WITH_MDNS
        mdns_service_remove("_esp_local_ctrl", "_tcp");
#endif
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
#ifdef WITH_MDNS
    mdns_service_remove("_esp_local_ctrl", "_tcp");
#endif
    protocomm_httpd_stop(pc);
    if (esp_local_ctrl_httpd_stop(server_handle) == ESP_OK) {
        server_handle = NULL;
    }
}

static esp_err_t copy_httpd_config(esp_local_ctrl_transport_config_t *dest_config, const esp_local_ctrl_transport_config_t *src_config)
{
    if (!dest_config || !src_config || !src_config->httpd) {
        ESP_LOGE(TAG, "NULL configuration provided");
        return ESP_ERR_INVALID_ARG;
    }

    dest_config->httpd = calloc(1, sizeof(esp_local_ctrl_transport_config_httpd_t));
    if (!dest_config->httpd) {
        ESP_LOGE(TAG, "Failed to allocate memory for HTTPD transport config");
        return ESP_ERR_NO_MEM;
    }
    memcpy(dest_config->httpd,
           src_config->httpd,
           sizeof(esp_local_ctrl_transport_config_httpd_t));

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
