/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "unity.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "test_utils.h"
#include "unity_test_utils.h"

#define TAG "test_wifi"
#define EMPH_STR(s) "****** "s" ******"

TEST_CASE("wifi set country code", "[wifi_init]")
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    cfg.nvs_enable = false;
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_init"));
    TEST_ESP_OK(esp_wifi_init(&cfg));

    wifi_country_t country;
    wifi_country_t country_01 = {.cc="01", .schan=1, .nchan=11, .policy=WIFI_COUNTRY_POLICY_MANUAL};
    wifi_country_t country_CN = {.cc="CN", .schan=1, .nchan=13, .policy=WIFI_COUNTRY_POLICY_MANUAL};

    ESP_LOGI(TAG, EMPH_STR("esp_wifi_get_country (default)"));
    TEST_ESP_OK(esp_wifi_get_country(&country));
    TEST_ASSERT(country.cc[0] == country_01.cc[0] && country.cc[1] == country_01.cc[1]);

    ESP_LOGI(TAG, EMPH_STR("esp_wifi_set_country"));
    TEST_ESP_OK(esp_wifi_set_country(&country_CN));

    ESP_LOGI(TAG, EMPH_STR("esp_wifi_get_country"));
    TEST_ESP_OK(esp_wifi_get_country(&country));
    TEST_ASSERT(country.cc[0] == country_CN.cc[0] && country.cc[1] == country_CN.cc[1]);


    ESP_LOGI(TAG, EMPH_STR("esp_wifi_deinit"));
    TEST_ESP_OK(esp_wifi_deinit());

    ESP_LOGI(TAG, EMPH_STR("esp_wifi_init"));
    TEST_ESP_OK(esp_wifi_init(&cfg));

    char country_code_string[3];
    char country_code_string_01[3] = "01";
    char country_code_string_CN[3] = "CN";
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_get_country_code (default)"));
    TEST_ESP_OK(esp_wifi_get_country_code(&country_code_string[0]));
    TEST_ASSERT(country_code_string[0] == country_code_string_01[0] && country_code_string[1] == country_code_string_01[1]);

    ESP_LOGI(TAG, EMPH_STR("esp_wifi_set_country_code"));
    TEST_ESP_OK(esp_wifi_set_country_code(&country_code_string_CN[0], false));

    ESP_LOGI(TAG, EMPH_STR("esp_wifi_get_country_code"));
    TEST_ESP_OK(esp_wifi_get_country_code(&country_code_string[0]));
    TEST_ASSERT(country_code_string[0] == country_code_string_CN[0] && country_code_string[1] == country_code_string_CN[1]);


    ESP_LOGI(TAG, EMPH_STR("esp_wifi_deinit"));
    TEST_ESP_OK(esp_wifi_deinit());

    ESP_LOGI(TAG, "test passed...");
}
