/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "unity.h"
#include "esp_wifi.h"
#include "esp_coexist.h"
#include "soc/soc_caps.h"
#include "esp_log.h"
#include "test_utils.h"
#include "unity_test_utils.h"
#include "esp_private/periph_ctrl.h"

#define TAG "test_external_coex"
#define EMPH_STR(s) "****** "s" ******"

TEST_CASE("enable external coex", "[external_coex]")
{
#if !SOC_MODEM_CLOCK_IS_INDEPENDENT && !CONFIG_IDF_TARGET_ESP32C2
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    cfg.nvs_enable = false;
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_init"));
    TEST_ESP_OK(esp_wifi_init(&cfg));
#endif

#if CONFIG_IDF_TARGET_ESP32S2
#define EXTERNAL_COEX_CONF 0x60035118
#define WDEV_RW_BT_COEX_EN (BIT(0))
#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
#define EXTERNAL_COEX_CONF 0x60035084
#define WDEV_RW_BT_COEX_EN (BIT(0))
#elif CONFIG_IDF_TARGET_ESP32C2
#define EXTERNAL_COEX_CONF 0x6004C4A0
#define WDEV_RW_BT_COEX_EN (BIT(9))
#elif CONFIG_IDF_TARGET_ESP32C5 || CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32C61
#define EXTERNAL_COEX_CONF 0x600AF4A0
#define WDEV_RW_BT_COEX_EN (BIT(9))
#elif CONFIG_IDF_TARGET_ESP32H2
#define EXTERNAL_COEX_CONF 0x600AD4A0
#define WDEV_RW_BT_COEX_EN (BIT(9))
#endif

    esp_extern_coex_work_mode_t mode = EXTERNAL_COEX_LEADER_ROLE;
    external_coex_wire_t type = EXTERN_COEX_WIRE_3;
    esp_external_coex_gpio_set_t gpio_pin = {.request = 0, .priority = 1, .grant = 2};
    TEST_ESP_OK(esp_external_coex_set_work_mode(mode));
    TEST_ESP_OK(esp_enable_extern_coex_gpio_pin(type, gpio_pin));

    ESP_LOGI(TAG, "external coex config 0x%lx", REG_READ(EXTERNAL_COEX_CONF));
    TEST_ASSERT(REG_GET_BIT(EXTERNAL_COEX_CONF, WDEV_RW_BT_COEX_EN));

    TEST_ESP_OK(esp_disable_extern_coex_gpio_pin());
    coex_module_enable();
    TEST_ASSERT(REG_GET_BIT(EXTERNAL_COEX_CONF, WDEV_RW_BT_COEX_EN) == 0);
    coex_module_disable();

#if !SOC_MODEM_CLOCK_IS_INDEPENDENT && !CONFIG_IDF_TARGET_ESP32C2
    ESP_LOGI(TAG, EMPH_STR("esp_wifi_deinit"));
    TEST_ESP_OK(esp_wifi_deinit());
    vTaskDelay(1);
#endif

    ESP_LOGI(TAG, "test passed...");
}
