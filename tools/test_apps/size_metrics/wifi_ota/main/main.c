/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "esp_event.h"
#include "esp_https_ota.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_config));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    esp_http_client_config_t http_config = {
        .url = "https://example.invalid/firmware.bin",
    };
    esp_https_ota_config_t ota_config = {
        .http_config = &http_config,
    };
    ESP_ERROR_CHECK(esp_https_ota(&ota_config));
}
