/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_log.h"
#include "test_utils.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "soc/temperature_sensor_periph.h"
#include "unity.h"
#include "unity_test_utils.h"
#include "driver/temperature_sensor.h"

#if SOC_WIFI_SUPPORTED

#ifndef TEST_SUFFIX_STR
#define TEST_SUFFIX_STR "_0000"
#endif

#define TEST_DEFAULT_SSID "SSID_" CONFIG_IDF_TARGET TEST_SUFFIX_STR
#define TEST_DEFAULT_PWD "PASS_" CONFIG_IDF_TARGET TEST_SUFFIX_STR
#define TEST_DEFAULT_CHANNEL (1)

static const char* TAG = "test_temperature&phy";

// helper struct to get the value in temperature sensor handle
struct temperature_sensor_obj_t {
    const temperature_sensor_attribute_t *tsens_attribute;
};

static void start_wifi_as_softap(void)
{
    wifi_config_t w_config = {
        .ap.ssid = TEST_DEFAULT_SSID,
        .ap.password = TEST_DEFAULT_PWD,
        .ap.ssid_len = strlen(TEST_DEFAULT_SSID),
        .ap.channel = TEST_DEFAULT_CHANNEL,
        .ap.authmode = WIFI_AUTH_WPA2_PSK,
        .ap.ssid_hidden = false,
        .ap.max_connection = 4,
        .ap.beacon_interval = 100,
    };

    TEST_ESP_OK(esp_wifi_set_mode(WIFI_MODE_AP));
    TEST_ESP_OK(esp_wifi_set_config(WIFI_IF_AP, &w_config));
    TEST_ESP_OK(esp_wifi_start());
    ESP_LOGI(TAG, "start wifi softap: %s", TEST_DEFAULT_SSID);
}

static void start_wifi_as_sta(void)
{
    TEST_ESP_OK(esp_wifi_set_mode(WIFI_MODE_STA));
    TEST_ESP_OK(esp_wifi_start());
}

static void stop_wifi(void)
{
    TEST_ESP_OK(esp_wifi_stop());
    vTaskDelay(500/portTICK_PERIOD_MS);
}

static void wifi_connect(void)
{
    wifi_config_t w_config = {
        .sta.ssid = TEST_DEFAULT_SSID,
        .sta.password = TEST_DEFAULT_PWD,
    };

    TEST_ESP_OK(esp_wifi_set_config(WIFI_IF_STA, &w_config));
    TEST_ESP_OK(esp_wifi_connect());
    ESP_LOGI(TAG, "start esp_wifi_connect: %s", TEST_DEFAULT_SSID);
}

static void test_wifi_establish_sta(void)
{
    TEST_ESP_OK(nvs_flash_init());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    cfg.nvs_enable = false;
    TEST_ESP_OK(esp_wifi_init(&cfg));
    start_wifi_as_sta();
    unity_wait_for_signal("AP start");

    // make sure softap has started
    vTaskDelay(1000/portTICK_PERIOD_MS);
    wifi_connect();
    unity_send_signal("connect");
    unity_wait_for_signal("tsens test done");

    // do not auto reconnect after connected
    stop_wifi();
    TEST_ESP_OK(esp_wifi_deinit());
    TEST_ESP_OK(nvs_flash_deinit());
}

static void test_wifi_temperature_softap(void)
{
    temperature_sensor_handle_t temp_sensor = NULL;
    // Initialize it in an bad value.
    temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(-40, -30);
    float tsens_value;
    TEST_ESP_OK(temperature_sensor_install(&temp_sensor_config, &temp_sensor));
    ESP_LOGI(TAG, "Enable temperature sensor");
    TEST_ESP_OK(temperature_sensor_enable(temp_sensor));
    TEST_ASSERT_EQUAL_INT(10, temp_sensor->tsens_attribute->reg_val);
    TEST_ESP_OK(temperature_sensor_get_celsius(temp_sensor, &tsens_value));
    ESP_LOGI(TAG, "Temperature value %.02f ℃", tsens_value);
    // Update to the correct one automatically.
    TEST_ASSERT_EQUAL_INT(15, temp_sensor->tsens_attribute->reg_val);
    int cnt = 10;
    TEST_ESP_OK(nvs_flash_init());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    cfg.nvs_enable = false;
    TEST_ESP_OK(esp_wifi_init(&cfg));
    start_wifi_as_softap();
    unity_send_signal("AP start");
    unity_wait_for_signal("connect");
    while (cnt--) {
        TEST_ESP_OK(temperature_sensor_get_celsius(temp_sensor, &tsens_value));
        ESP_LOGI(TAG, "Temperature value %.02f ℃", tsens_value);
        if (tsens_value < 20 || tsens_value > 50) {
            ESP_LOGE(TAG, "Temperature value is completely wrong");
            abort();
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    TEST_ESP_OK(temperature_sensor_disable(temp_sensor));
    TEST_ESP_OK(temperature_sensor_uninstall(temp_sensor));
    unity_send_signal("tsens test done");
    stop_wifi();
    TEST_ESP_OK(esp_wifi_deinit());
    TEST_ESP_OK(nvs_flash_deinit());
}

TEST_CASE_MULTIPLE_DEVICES("test temperature sensor work together with wifi", "[test_env=tsens_phy]", test_wifi_establish_sta, test_wifi_temperature_softap);

#endif
