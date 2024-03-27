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
#include "esp_openthread_cli.h"
#include "esp_openthread_lock.h"
#include "esp_openthread_netif_glue.h"
#include "esp_ot_sleepy_device_config.h"
#include "esp_vfs_eventfd.h"
#include "driver/uart.h"
#include "nvs_flash.h"
#include "openthread/logging.h"
#include "openthread/thread.h"
#if CONFIG_ESP_SLEEP_DEBUG
#include "esp_timer.h"
#include "esp_sleep.h"
#include "esp_private/esp_pmu.h"
#include "esp_private/esp_sleep_internal.h"
#endif

#ifdef CONFIG_PM_ENABLE
#include "esp_pm.h"
#endif

#if !SOC_IEEE802154_SUPPORTED
#error "Openthread sleepy device is only supported for the SoCs which have IEEE 802.15.4 module"
#endif

#define TAG "ot_esp_power_save"

static esp_pm_lock_handle_t s_cli_pm_lock = NULL;

#if CONFIG_OPENTHREAD_AUTO_START
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

    otOperationalDatasetTlvs dataset;
    otError error = otDatasetGetActiveTlvs(esp_openthread_get_instance(), &dataset);
    ESP_ERROR_CHECK(esp_openthread_auto_start((error == OT_ERROR_NONE) ? &dataset : NULL));
}
#endif // CONFIG_OPENTHREAD_AUTO_START

static esp_err_t esp_openthread_sleep_device_init(void)
{
    esp_err_t ret = ESP_OK;

    ret = esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "otcli", &s_cli_pm_lock);
    if (ret == ESP_OK) {
        esp_pm_lock_acquire(s_cli_pm_lock);
        ESP_LOGI(TAG, "Successfully created CLI pm lock");
    } else {
        if (s_cli_pm_lock != NULL) {
            esp_pm_lock_delete(s_cli_pm_lock);
            s_cli_pm_lock = NULL;
        }
        ESP_LOGI(TAG, " Failed to create CLI pm lock");
    }
    return ret;
}

static void process_state_change(otChangedFlags flags, void* context)
{
    otDeviceRole ot_device_role = otThreadGetDeviceRole(esp_openthread_get_instance());
    if(ot_device_role == OT_DEVICE_ROLE_CHILD) {
        if (s_cli_pm_lock != NULL) {
            esp_pm_lock_release(s_cli_pm_lock);
            esp_pm_lock_delete(s_cli_pm_lock);
            s_cli_pm_lock = NULL;
        }
    }
}

static esp_netif_t *init_openthread_netif(const esp_openthread_platform_config_t *config)
{
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_OPENTHREAD();
    esp_netif_t *netif = esp_netif_new(&cfg);
    assert(netif != NULL);
    ESP_ERROR_CHECK(esp_netif_attach(netif, esp_openthread_netif_glue_init(config)));

    return netif;
}

#if CONFIG_ESP_SLEEP_DEBUG
static esp_sleep_context_t s_sleep_ctx;

static void print_sleep_flag(void *arg)
{
    ESP_LOGD(TAG, "sleep_flags %lu", s_sleep_ctx.sleep_flags);
    ESP_LOGD(TAG, "PMU_SLEEP_PD_TOP: %s", (s_sleep_ctx.sleep_flags & PMU_SLEEP_PD_TOP) ? "True":"False");
    ESP_LOGD(TAG, "PMU_SLEEP_PD_MODEM: %s", (s_sleep_ctx.sleep_flags & PMU_SLEEP_PD_MODEM) ? "True":"False");
}
#endif

static void ot_task_worker(void *aContext)
{
    otError ret;
    esp_openthread_platform_config_t config = {
        .radio_config = ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG(),
        .host_config = ESP_OPENTHREAD_DEFAULT_HOST_CONFIG(),
        .port_config = ESP_OPENTHREAD_DEFAULT_PORT_CONFIG(),
    };

    // Initialize the OpenThread stack
    ESP_ERROR_CHECK(esp_openthread_init(&config));

    esp_openthread_lock_acquire(portMAX_DELAY);
    ret = otSetStateChangedCallback(esp_openthread_get_instance(), process_state_change, esp_openthread_get_instance());
    esp_openthread_lock_release();
    if(ret != OT_ERROR_NONE) {
        ESP_LOGE(TAG, "Failed to set state changed callback");
    }
#if CONFIG_OPENTHREAD_LOG_LEVEL_DYNAMIC
    // The OpenThread log level directly matches ESP log level
    (void)otLoggingSetLevel(CONFIG_LOG_DEFAULT_LEVEL);
#endif
    // Initialize the OpenThread cli
#if CONFIG_OPENTHREAD_CLI
    esp_openthread_cli_init();
#endif
    esp_netif_t *openthread_netif;
    // Initialize the esp_netif bindings
    openthread_netif = init_openthread_netif(&config);
    esp_netif_set_default_netif(openthread_netif);
#if CONFIG_OPENTHREAD_AUTO_START
    create_config_network(esp_openthread_get_instance());
#endif // CONFIG_OPENTHREAD_AUTO_START

#if CONFIG_OPENTHREAD_CLI
    esp_openthread_cli_create_task();
#endif
#if CONFIG_ESP_SLEEP_DEBUG
    esp_sleep_set_sleep_context(&s_sleep_ctx);
    esp_log_level_set(TAG, ESP_LOG_DEBUG);

    // create a timer to print the status of sleepy device
    int periods = 2000;
    const esp_timer_create_args_t timer_args = {
            .name = "print_sleep_flag",
            .arg  = NULL,
            .callback = &print_sleep_flag,
            .skip_unhandled_events = true,
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, periods * 1000));
#endif

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
    ESP_ERROR_CHECK(esp_openthread_sleep_device_init());
    xTaskCreate(ot_task_worker, "ot_power_save_main", 4096, NULL, 5, NULL);
}
