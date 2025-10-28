/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
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
#include "esp_sleep.h"
#include "esp_timer.h"
#include "esp_openthread.h"
#include "esp_openthread_netif_glue.h"
#include "esp_ot_sleepy_device_config.h"
#include "esp_vfs_eventfd.h"
#include "nvs_flash.h"
#include "driver/rtc_io.h"

#if !SOC_IEEE802154_SUPPORTED
#error "Openthread sleepy device is only supported for the SoCs which have IEEE 802.15.4 module"
#endif

#define TAG "ot_esp_power_save"

static RTC_DATA_ATTR struct timeval s_sleep_enter_time;
static esp_timer_handle_t s_oneshot_timer;

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

static void ot_state_change_callback(otChangedFlags changed_flags, void* ctx)
{
    OT_UNUSED_VARIABLE(ctx);
    static otDeviceRole s_previous_role = OT_DEVICE_ROLE_DISABLED;
    otInstance* instance = esp_openthread_get_instance();
    if (!instance) {
        return;
    }
    otDeviceRole role = otThreadGetDeviceRole(instance);
    if (role == OT_DEVICE_ROLE_CHILD && s_previous_role != OT_DEVICE_ROLE_CHILD) {
        // Start the one-shot timer
        const int before_deep_sleep_time_sec = 5;
        ESP_LOGI(TAG, "Start one-shot timer for %ds to enter the deep sleep", before_deep_sleep_time_sec);
        ESP_ERROR_CHECK(esp_timer_start_once(s_oneshot_timer, before_deep_sleep_time_sec * 1000000));
    }
    s_previous_role = role;
}

static void s_oneshot_timer_callback(void* arg)
{
    // Enter deep sleep
    ESP_LOGI(TAG, "Enter deep sleep");
    gettimeofday(&s_sleep_enter_time, NULL);
    esp_deep_sleep_start();
}

static void ot_deep_sleep_init(void)
{
    // Within this function, we print the reason for the wake-up and configure the method of waking up from deep sleep.
    // This example provides support for two wake-up sources from deep sleep: RTC timer and GPIO.

    // The one-shot timer will start when the device transitions to the CHILD state for the first time.
    // After a 5-second delay, the device will enter deep sleep.

    const esp_timer_create_args_t s_oneshot_timer_args = {
            .callback = &s_oneshot_timer_callback,
            .name = "one-shot"
    };

    ESP_ERROR_CHECK(esp_timer_create(&s_oneshot_timer_args, &s_oneshot_timer));

    // Print the wake-up reason:
    struct timeval now;
    gettimeofday(&now, NULL);
    int sleep_time_ms = (now.tv_sec - s_sleep_enter_time.tv_sec) * 1000 + (now.tv_usec - s_sleep_enter_time.tv_usec) / 1000;

    uint32_t wake_up_causes = esp_sleep_get_wakeup_causes();
    if (wake_up_causes & BIT(ESP_SLEEP_WAKEUP_UNDEFINED)) {
        ESP_LOGI(TAG, "Not a deep sleep reset");
    } else {
        if(wake_up_causes & BIT(ESP_SLEEP_WAKEUP_TIMER)) {
            ESP_LOGI(TAG, "Wake up from timer. Time spent in deep sleep and boot: %dms", sleep_time_ms);
        }
        if(wake_up_causes & BIT(ESP_SLEEP_WAKEUP_EXT1)) {
            ESP_LOGI(TAG, "Wake up from GPIO. Time spent in deep sleep and boot: %dms", sleep_time_ms);
        }
    }

    // Set the methods of how to wake up:
    // 1. RTC timer waking-up
    const int wakeup_time_sec = 20;
    ESP_LOGI(TAG, "Enabling timer wakeup, %ds\n", wakeup_time_sec);
    ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(wakeup_time_sec * 1000000));

    // 2. GPIO waking-up
#if CONFIG_IDF_TARGET_ESP32C6
    // For ESP32C6 boards, RTCIO only supports GPIO0~GPIO7
    // GPIO7 pull down to wake up
    const int gpio_wakeup_pin = 7;
#elif CONFIG_IDF_TARGET_ESP32H2
    // You can wake up by pulling down GPIO9. On ESP32H2 development boards, the BOOT button is connected to GPIO9.
    // You can use the BOOT button to wake up the boards directly.
    const int gpio_wakeup_pin = 9;
#endif
    const uint64_t gpio_wakeup_pin_mask = 1ULL << gpio_wakeup_pin;
    // The configuration mode depends on your hardware design.
    // Since the BOOT button is connected to a pull-up resistor, the wake-up mode is configured as LOW.
    const uint64_t ext_wakeup_mode = 0;
    ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup_io(gpio_wakeup_pin_mask, ext_wakeup_mode));

    // Also these two GPIO configurations are also depended on the hardware design.
    // The BOOT button is connected to the pull-up resistor, so enable the pull-up mode and disable the pull-down mode.

    // Notice: if these GPIO configurations do not match the hardware design, the deep sleep module will enable the GPIO hold
    // feature to hold the GPIO voltage when enter the sleep, which will ensure the board be waked up by GPIO. But it will cause
    // 3~4 times power consumption increasing during sleep.
    ESP_ERROR_CHECK(gpio_pullup_en(gpio_wakeup_pin));
    ESP_ERROR_CHECK(gpio_pulldown_dis(gpio_wakeup_pin));
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
    ot_deep_sleep_init();

    esp_openthread_platform_config_t config = {
        .radio_config = ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG(),
        .host_config = ESP_OPENTHREAD_DEFAULT_HOST_CONFIG(),
        .port_config = ESP_OPENTHREAD_DEFAULT_PORT_CONFIG(),
    };
    ESP_ERROR_CHECK(esp_openthread_start(&config));
    esp_netif_set_default_netif(esp_openthread_get_netif());

    otSetStateChangedCallback(esp_openthread_get_instance(), ot_state_change_callback, NULL);
    create_config_network(esp_openthread_get_instance());
}
