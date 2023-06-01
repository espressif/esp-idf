/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <wifi_provisioning/manager.h>
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "dns_server.h"

static const char *TAG = "NCM_provisioning";

struct events {
    EventGroupHandle_t *flags;
    int success_bit;
    int fail_bit;
    bool success;
};

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    struct events *handler_args = arg;
    switch (event_id) {
        case WIFI_PROV_START:
            ESP_LOGI(TAG, "Provisioning started");
            break;
        case WIFI_PROV_CRED_RECV: {
            wifi_sta_config_t *wifi_sta_cfg = (wifi_sta_config_t *) event_data;
            ESP_LOGI(TAG, "Received Wi-Fi credentials"
                          "\n\tSSID     : %s\n\tPassword : %s",
                     (const char *) wifi_sta_cfg->ssid,
                     (const char *) wifi_sta_cfg->password);
            break;
        }
        case WIFI_PROV_CRED_FAIL: {
            wifi_prov_sta_fail_reason_t *reason = (wifi_prov_sta_fail_reason_t *) event_data;
            ESP_LOGE(TAG, "Provisioning failed!\n\tReason : %s"
                          "\n\tPlease reset to factory and retry provisioning",
                     (*reason == WIFI_PROV_STA_AUTH_ERROR) ?
                     "Wi-Fi station authentication failed" : "Wi-Fi access-point not found");
            handler_args->success = false;

            break;
        }
        case WIFI_PROV_CRED_SUCCESS:
            ESP_LOGI(TAG, "Provisioning successful");
            handler_args->success = true;
            break;
        case WIFI_PROV_END:
            /* De-initialize manager once provisioning is finished */
            wifi_prov_mgr_deinit();
            xEventGroupSetBits(*handler_args->flags, handler_args->success ? handler_args->success_bit : handler_args->fail_bit);
            free(handler_args);
            break;
        default:
            break;
    }
}

extern const wifi_prov_scheme_t wifi_prov_scheme_httpd;

esp_err_t start_provisioning(EventGroupHandle_t *flags, int success_bit, int fail_bit)
{
    // Start the DNS server that will reply to "wifi.settings" with "usb" network interface address
    dns_server_config_t dns_config = DNS_SERVER_CONFIG_SINGLE("wifi.settings" /* name */, "wired" /* wired netif ID */);
    start_dns_server(&dns_config);
    struct events *handler_args = malloc(sizeof(struct events));
    handler_args->flags = flags;
    handler_args->success_bit = success_bit;
    handler_args->fail_bit = fail_bit;
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_PROV_EVENT, ESP_EVENT_ANY_ID, event_handler, handler_args));
    /* Configuration for the provisioning manager */
    wifi_prov_mgr_config_t config = {
            .scheme = wifi_prov_scheme_httpd,
    };

    /* Initialize provisioning manager with the
     * configuration parameters set above */
    ESP_ERROR_CHECK(wifi_prov_mgr_init(config));

    /* TODO: Add more security options to menuconfig
     */
    ESP_ERROR_CHECK(wifi_prov_mgr_start_provisioning(WIFI_PROV_SECURITY_0, NULL, NULL, NULL));
    return ESP_OK;
}

bool is_provisioned(void)
{
    bool provisioned = false;
    ESP_ERROR_CHECK(wifi_prov_mgr_is_provisioned(&provisioned));
    return provisioned;
}
