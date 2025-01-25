/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 *
 * OpenThread Border Router Example
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */

#include <stdio.h>
#include <string.h>

#include "sdkconfig.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_openthread.h"
#include "esp_openthread_border_router.h"
#include "esp_openthread_cli.h"
#include "esp_openthread_lock.h"
#include "esp_openthread_netif_glue.h"
#include "esp_openthread_types.h"
#if CONFIG_OPENTHREAD_CLI_ESP_EXTENSION
#include "esp_ot_cli_extension.h"
#endif // CONFIG_OPENTHREAD_CLI_ESP_EXTENSION
#include "esp_ot_config.h"
#include "esp_ot_wifi_cmd.h"
#include "esp_vfs_dev.h"
#include "esp_vfs_eventfd.h"
#include "esp_wifi.h"
#include "mdns.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/uart_types.h"
#include "openthread/error.h"
#include "openthread/logging.h"
#include "openthread/tasklet.h"

#if CONFIG_OPENTHREAD_STATE_INDICATOR_ENABLE
#include "ot_led_strip.h"
#endif

#if CONFIG_OPENTHREAD_BR_AUTO_START
#include "example_common_private.h"
#include "protocol_examples_common.h"
#endif

#if !CONFIG_OPENTHREAD_BR_AUTO_START && CONFIG_EXAMPLE_CONNECT_ETHERNET
// TZ-1109: Add a menchanism for connecting ETH manually.
#error Currently we do not support a manual way to connect ETH, if you want to use ETH, please enable OPENTHREAD_BR_AUTO_START.
#endif

#define TAG "esp_ot_br"

#if CONFIG_OPENTHREAD_SUPPORT_HW_RESET_RCP
#define PIN_TO_RCP_RESET CONFIG_OPENTHREAD_HW_RESET_RCP_PIN
static void rcp_failure_hardware_reset_handler(void)
{
    gpio_config_t reset_pin_config;
    memset(&reset_pin_config, 0, sizeof(reset_pin_config));
    reset_pin_config.intr_type = GPIO_INTR_DISABLE;
    reset_pin_config.pin_bit_mask = BIT(PIN_TO_RCP_RESET);
    reset_pin_config.mode = GPIO_MODE_OUTPUT;
    reset_pin_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    reset_pin_config.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&reset_pin_config);
    gpio_set_level(PIN_TO_RCP_RESET, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(PIN_TO_RCP_RESET, 1);
    vTaskDelay(pdMS_TO_TICKS(30));
    gpio_reset_pin(PIN_TO_RCP_RESET);
}
#endif

#if CONFIG_EXTERNAL_COEX_ENABLE
static void ot_br_external_coexist_init(void)
{
    esp_external_coex_gpio_set_t gpio_pin = ESP_OPENTHREAD_DEFAULT_EXTERNAL_COEX_CONFIG();
    esp_external_coex_set_work_mode(EXTERNAL_COEX_LEADER_ROLE);
    ESP_ERROR_CHECK(esp_enable_extern_coex_gpio_pin(CONFIG_EXTERNAL_COEX_WIRE_TYPE, gpio_pin));
}
#endif /* CONFIG_EXTERNAL_COEX_ENABLE */

static void ot_task_worker(void *aContext)
{
    esp_openthread_platform_config_t config = {
        .radio_config = ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG(),
        .host_config = ESP_OPENTHREAD_DEFAULT_HOST_CONFIG(),
        .port_config = ESP_OPENTHREAD_DEFAULT_PORT_CONFIG(),
    };

    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_OPENTHREAD();
    esp_netif_t       *openthread_netif = esp_netif_new(&cfg);
    assert(openthread_netif != NULL);

    // Initialize the OpenThread stack
    ESP_ERROR_CHECK(esp_openthread_init(&config));
    ESP_ERROR_CHECK(esp_netif_attach(openthread_netif, esp_openthread_netif_glue_init(&config)));
    esp_openthread_lock_acquire(portMAX_DELAY);
#if CONFIG_OPENTHREAD_LOG_LEVEL_DYNAMIC
    // The OpenThread log level directly matches ESP log level
    (void)otLoggingSetLevel(CONFIG_LOG_DEFAULT_LEVEL);
#endif // CONFIG_OPENTHREAD_LOG_LEVEL_DYNAMIC
#if CONFIG_OPENTHREAD_CLI
    esp_openthread_cli_init();
#if CONFIG_OPENTHREAD_CLI_ESP_EXTENSION
    esp_cli_custom_command_init();
#endif // CONFIG_OPENTHREAD_CLI_ESP_EXTENSION
    esp_openthread_cli_create_task();
#endif // CONFIG_OPENTHREAD_CLI
    esp_openthread_lock_release();

    // Run the main loop
    esp_openthread_launch_mainloop();

    // Clean up
    esp_openthread_netif_glue_deinit();
    esp_netif_destroy(openthread_netif);
    esp_vfs_eventfd_unregister();
    vTaskDelete(NULL);
}

void ot_br_init(void *ctx)
{
#if CONFIG_OPENTHREAD_CLI_WIFI
    ESP_ERROR_CHECK(esp_ot_wifi_config_init());
#endif
#if CONFIG_OPENTHREAD_BR_AUTO_START
#if CONFIG_EXAMPLE_CONNECT_WIFI || CONFIG_EXAMPLE_CONNECT_ETHERNET
    bool wifi_or_ethernet_connected = false;
#else
#error No backbone netif!
#endif
#if CONFIG_EXAMPLE_CONNECT_WIFI
    char wifi_ssid[32] = "";
    char wifi_password[64] = "";
    if (esp_ot_wifi_config_get_ssid(wifi_ssid) == ESP_OK) {
        ESP_LOGI(TAG, "use the Wi-Fi config from NVS");
        esp_ot_wifi_config_get_password(wifi_password);
    } else {
        ESP_LOGI(TAG, "use the Wi-Fi config from Kconfig");
        strcpy(wifi_ssid, CONFIG_EXAMPLE_WIFI_SSID);
        strcpy(wifi_password, CONFIG_EXAMPLE_WIFI_PASSWORD);
    }
    if (esp_ot_wifi_connect(wifi_ssid, wifi_password) == ESP_OK) {
        wifi_or_ethernet_connected = true;
    } else {
        ESP_LOGE(TAG, "Fail to connect to Wi-Fi, please try again manually");
    }
#endif
#if CONFIG_EXAMPLE_CONNECT_ETHERNET
    ESP_ERROR_CHECK(example_ethernet_connect());
    wifi_or_ethernet_connected = true;
#endif
#endif // CONFIG_OPENTHREAD_BR_AUTO_START

#if CONFIG_EXTERNAL_COEX_ENABLE
    ot_br_external_coexist_init();
#endif // CONFIG_EXTERNAL_COEX_ENABLE
    ESP_ERROR_CHECK(mdns_init());
    ESP_ERROR_CHECK(mdns_hostname_set("esp-ot-br"));

    esp_openthread_lock_acquire(portMAX_DELAY);
#if CONFIG_OPENTHREAD_STATE_INDICATOR_ENABLE
    ESP_ERROR_CHECK(esp_openthread_state_indicator_init(esp_openthread_get_instance()));
#endif
#if CONFIG_OPENTHREAD_BR_AUTO_START
    if (wifi_or_ethernet_connected) {
        esp_openthread_set_backbone_netif(get_example_netif());
        ESP_ERROR_CHECK(esp_openthread_border_router_init());
#if CONFIG_EXAMPLE_CONNECT_WIFI
        esp_ot_wifi_border_router_init_flag_set(true);
#endif
        otOperationalDatasetTlvs dataset;
        otError error = otDatasetGetActiveTlvs(esp_openthread_get_instance(), &dataset);
        ESP_ERROR_CHECK(esp_openthread_auto_start((error == OT_ERROR_NONE) ? &dataset : NULL));
    } else {
        ESP_LOGE(TAG, "Auto-start mode failed, please try to start manually");
    }
#endif // CONFIG_OPENTHREAD_BR_AUTO_START
    esp_openthread_lock_release();
    vTaskDelete(NULL);
}

void app_main(void)
{
    // Used eventfds:
    // * netif
    // * task queue
    // * border router
    esp_vfs_eventfd_config_t eventfd_config = {
#if CONFIG_OPENTHREAD_RADIO_NATIVE || CONFIG_OPENTHREAD_RADIO_SPINEL_SPI
        // * radio driver (A native radio device needs a eventfd for radio driver.)
        // * SpiSpinelInterface (The Spi Spinel Interface needs a eventfd.)
        // The above will not exist at the same time.
        .max_fds = 4,
#else
        .max_fds = 3,
#endif
    };
    ESP_ERROR_CHECK(esp_vfs_eventfd_register(&eventfd_config));
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
#if CONFIG_OPENTHREAD_SUPPORT_HW_RESET_RCP
    esp_openthread_register_rcp_failure_handler(rcp_failure_hardware_reset_handler);
#endif
    xTaskCreate(ot_task_worker, "ot_br_main", 8192, xTaskGetCurrentTaskHandle(), 5, NULL);
    xTaskCreate(ot_br_init, "ot_br_init", 6144, NULL, 4, NULL);
}
