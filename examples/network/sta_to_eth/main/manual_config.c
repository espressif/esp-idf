/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include "esp_wifi.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_http_server.h"
#include "dns_server.h"

static const char *TAG = "NCM_configuration";
static httpd_handle_t s_web_server = NULL;
static EventGroupHandle_t *s_flags = NULL;
static int s_success_bit;

bool is_provisioned(void)
{
    wifi_config_t wifi_cfg;
    if (esp_wifi_get_config(WIFI_IF_STA, &wifi_cfg) != ESP_OK) {
        return false;
    }

    if (strlen((const char *) wifi_cfg.sta.ssid)) {
        return true;
    }

    return false;
}

static esp_err_t http_get_handler(httpd_req_t *req)
{
    const char page[] = "<form action=\"/\" method=\"get\"><br><br>\n"
                        "SSID:  <input type=\"text\" id=\"ssid\" name=\"ssid\"><br><br>\n"
                        "Password:  <input type=\"text\" id=\"password\" name=\"password\"><br><br>\n"
                        "  <input type=\"submit\" value=\"Connect\">"
                        "</form>";
    char  *buf = NULL;
    size_t buf_len;
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found URL query => %s", buf);
            char param[32];
            wifi_config_t wifi_cfg = {};

            if (httpd_query_key_value(buf, "ssid", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "ssid=%s", param);
                strncpy((char *)wifi_cfg.sta.ssid, param, sizeof(wifi_cfg.sta.ssid));
            }
            if (httpd_query_key_value(buf, "password", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "password=%s", param);
                strncpy((char *)wifi_cfg.sta.password, param, sizeof(wifi_cfg.sta.password));
            }

            if (strlen((char *)wifi_cfg.sta.ssid) > 0 && strlen((char *)wifi_cfg.sta.password)) {
                httpd_resp_set_type(req, "text/html");
                esp_wifi_set_mode(WIFI_MODE_STA);
                if (esp_wifi_set_storage(WIFI_STORAGE_FLASH) == ESP_OK &&
                        esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg) == ESP_OK) {
                    const char wifi_configured[] = "<h1>Connecting...</h1>";
                    ESP_LOGI(TAG, "WiFi settings accepted!");
                    httpd_resp_send(req, wifi_configured, strlen(wifi_configured));
                } else {
                    const char wifi_config_failed[] = "<h1>Failed to configure WiFi settings</h1>";
                    ESP_LOGE(TAG, "Failed to set WiFi config to flash");
                    httpd_resp_send(req, wifi_config_failed, strlen(wifi_config_failed));
                }

                free(buf);
                if (s_flags) {
                    xEventGroupSetBits(*s_flags, s_success_bit);
                }
                return ESP_OK;
            }
        }
        free(buf);
    }

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, page, sizeof(page));

    return ESP_OK;
}

static const httpd_uri_t root = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = http_get_handler,
};

static void start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_open_sockets = 3;
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&s_web_server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(s_web_server, &root);
    }
}

esp_err_t start_provisioning(EventGroupHandle_t *flags, int success_bit, int fail_bit)
{
    start_webserver();
    // Start the DNS server that will reply to "wifi.settings" with "usb" network interface address
    dns_server_config_t config = DNS_SERVER_CONFIG_SINGLE("wifi.settings" /* name */, "wired" /* USB netif ID */);
    start_dns_server(&config);

    s_flags = flags;
    s_success_bit = success_bit;
    return ESP_OK;
}
