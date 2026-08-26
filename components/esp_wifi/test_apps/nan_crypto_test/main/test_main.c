/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/*
 * NAN Crypto Test Main
 *
 * Test application for NAN ND-PMK derivation
 */

#include <stdio.h>
#include "unity.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "nan_pmk_test.h"

static const char *TAG = "nan_test";

void app_main(void)
{
    /* Initialize NVS */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* Initialize Wi-Fi (needed for crypto funcs) */
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "  NAN ND-PMK Derivation Test");
    ESP_LOGI(TAG, "========================================\n");

    /* Run test vectors */
    nan_pmk_run_test_vectors();

    ESP_LOGI(TAG, "\n========================================");
    ESP_LOGI(TAG, "  All tests completed");
    ESP_LOGI(TAG, "========================================");
}
