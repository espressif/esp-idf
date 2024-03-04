/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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

#include "ot_led_strip.h"
#include "esp_check.h"
#include "led_strip.h"
#include "esp_err.h"
#include "esp_openthread.h"

static led_strip_handle_t s_led_strip;

#define TAG "OT_LED"

static void configure_led(void)
{
    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num = CONFIG_OPENTHREAD_STATE_INDICATOR_GPIO,
        .max_leds = 1, // at least one LED on board
    };
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .flags.with_dma = false,
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &s_led_strip));
    /* Set all LED off to clear all pixels */
    ESP_ERROR_CHECK(led_strip_clear(s_led_strip));
}

static void ot_indicator_change_callback(otChangedFlags changed_flags, void* ctx)
{
    otInstance *instance = esp_openthread_get_instance();
    otDeviceRole role = otThreadGetDeviceRole(instance);
    if (role == OT_DEVICE_ROLE_DISABLED) {
        esp_openthread_state_indicator_clear();
    } else if (role == OT_DEVICE_ROLE_DETACHED) {
        esp_openthread_state_indicator_set(0, CONFIG_DETACHED_INDICATOR_RED, CONFIG_DETACHED_INDICATOR_GREEN, CONFIG_DETACHED_INDICATOR_BLUE);
    } else if (role == OT_DEVICE_ROLE_LEADER) {
        esp_openthread_state_indicator_set(0, CONFIG_LEADER_INDICATOR_RED, CONFIG_LEADER_INDICATOR_GREEN, CONFIG_LEADER_INDICATOR_BLUE);
    } else if (role == OT_DEVICE_ROLE_ROUTER) {
        esp_openthread_state_indicator_set(0, CONFIG_ROUTER_INDICATOR_RED, CONFIG_ROUTER_INDICATOR_GREEN, CONFIG_ROUTER_INDICATOR_BLUE);
    } else if (role == OT_DEVICE_ROLE_CHILD) {
        esp_openthread_state_indicator_set(0, CONFIG_CHILD_INDICATOR_RED, CONFIG_CHILD_INDICATOR_GREEN, CONFIG_CHILD_INDICATOR_BLUE);
    }
}

esp_err_t esp_openthread_state_indicator_init(otInstance *instance)
{
    configure_led();
    ESP_RETURN_ON_FALSE(otSetStateChangedCallback(instance, ot_indicator_change_callback, NULL) == OT_ERROR_NONE,
        ESP_FAIL, TAG, "Failed to install state change callback");
    return ESP_OK;
}

esp_err_t esp_openthread_state_indicator_set(uint32_t index, uint32_t red, uint32_t green, uint32_t blue)
{
    ESP_RETURN_ON_ERROR(led_strip_set_pixel(s_led_strip, index, red, green, blue), TAG, "Failed to set color for openthread state");
    ESP_RETURN_ON_ERROR(led_strip_refresh(s_led_strip), TAG, "Failed to refresh color for openthread state");
    return ESP_OK;
}

esp_err_t esp_openthread_state_indicator_clear(void)
{
    ESP_RETURN_ON_ERROR(led_strip_clear(s_led_strip), TAG, "Failed to clear color for openthread state");
    return ESP_OK;
}
