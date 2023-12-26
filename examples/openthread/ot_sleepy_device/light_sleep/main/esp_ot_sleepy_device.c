/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 *
 * OpenThread Command Line Example
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_openthread.h"
#include "esp_openthread_netif_glue.h"
#include "esp_ot_sleepy_device_config.h"
#include "esp_vfs_eventfd.h"
#include "driver/uart.h"
#include "nvs_flash.h"
#include "openthread/logging.h"
#include "openthread/thread.h"

#ifdef CONFIG_PM_ENABLE
#include "esp_pm.h"
#endif

#if !SOC_IEEE802154_SUPPORTED
#error "Openthread sleepy device is only supported for the SoCs which have IEEE 802.15.4 module"
#endif

#define TAG "ot_esp_power_save"

static void create_config_network(otInstance *instance)
{
    otLinkModeConfig linkMode = { 0 };

    linkMode.mRxOnWhenIdle = false;
    linkMode.mDeviceType = false;
    linkMode.mNetworkData = false;

    if (otLinkSetPollPeriod(instance, CONFIG_OPENTHREAD_NETWORK_POLLPERIOD_TIME) != OT_ERROR_NONE) {
        ESP_LOGE(TAG, "Failed to set OpenThread pollperiod.");
        abort();
    }

    if (otThreadSetLinkMode(instance, linkMode) != OT_ERROR_NONE) {
        ESP_LOGE(TAG, "Failed to set OpenThread linkmode.");
        abort();
    }
    ESP_ERROR_CHECK(esp_openthread_auto_start(NULL));
}

static esp_netif_t *init_openthread_netif(const esp_openthread_platform_config_t *config)
{
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_OPENTHREAD();
    esp_netif_t *netif = esp_netif_new(&cfg);
    assert(netif != NULL);
    ESP_ERROR_CHECK(esp_netif_attach(netif, esp_openthread_netif_glue_init(config)));

    return netif;
}

static void ot_task_worker(void *aContext)
{
    esp_openthread_platform_config_t config = {
        .radio_config = ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG(),
        .host_config = ESP_OPENTHREAD_DEFAULT_HOST_CONFIG(),
        .port_config = ESP_OPENTHREAD_DEFAULT_PORT_CONFIG(),
    };

    // Initialize the OpenThread stack
    ESP_ERROR_CHECK(esp_openthread_init(&config));

#if CONFIG_OPENTHREAD_LOG_LEVEL_DYNAMIC
    // The OpenThread log level directly matches ESP log level
    (void)otLoggingSetLevel(CONFIG_LOG_DEFAULT_LEVEL);
#endif
    esp_netif_t *openthread_netif;
    // Initialize the esp_netif bindings
    openthread_netif = init_openthread_netif(&config);
    esp_netif_set_default_netif(openthread_netif);

    create_config_network(esp_openthread_get_instance());

    // Run the main loop
    esp_openthread_launch_mainloop();

    // Clean up
    esp_openthread_netif_glue_deinit();
    esp_netif_destroy(openthread_netif);

    esp_vfs_eventfd_unregister();
    vTaskDelete(NULL);
}

static esp_err_t ot_power_save_init(void)
{
    esp_err_t rc = ESP_OK;
#ifdef CONFIG_PM_ENABLE
    int cur_cpu_freq_mhz = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ;

    esp_pm_config_t pm_config = {
        .max_freq_mhz = cur_cpu_freq_mhz,
        .min_freq_mhz = cur_cpu_freq_mhz,
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
        .light_sleep_enable = true
#endif
    };

    rc = esp_pm_configure(&pm_config);
#endif
    return rc;
}

void app_main(void)
{
    // Used eventfds:
    // * netif
    // * ot task queue
    // * radio driver
    esp_vfs_eventfd_config_t eventfd_config = {
        .max_fds = 3,
    };

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_vfs_eventfd_register(&eventfd_config));
    ESP_ERROR_CHECK(ot_power_save_init());

    xTaskCreate(ot_task_worker, "ot_power_save_main", 4096, NULL, 5, NULL);
}
