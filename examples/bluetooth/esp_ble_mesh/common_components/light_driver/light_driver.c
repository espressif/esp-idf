/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "errno.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"

#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_partition.h"

#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"

#include "nvs.h"
#include "nvs_flash.h"
#include "driver/i2c.h"
#include "sys/param.h"
#include "driver/gpio.h"

#include "light_driver.h"
#include "ble_mesh_example_nvs.h"

/**
 * @brief The state of the five-color light
 */
typedef struct {
    uint8_t mode;
    uint8_t on;
    uint16_t hue;
    uint8_t saturation;
    uint8_t value;
    uint8_t lightness;
    uint8_t color_temperature;
    uint8_t brightness;
    uint32_t fade_period_ms;
    uint32_t blink_period_ms;
} light_status_t;

/**
 * @brief The channel of the five-color light
 */
enum light_channel {
    CHANNEL_ID_RED = 0,
    CHANNEL_ID_GREEN,
    CHANNEL_ID_BLUE,
    CHANNEL_ID_WARM,
    CHANNEL_ID_COLD,
};

#define LIGHT_FADE_PERIOD_MAX_MS (3 * 1000)

static const char *TAG               = "light_driver";
static light_status_t g_light_status = {0};
static bool g_light_blink_flag       = false;
static TimerHandle_t g_fade_timer    = NULL;
static int g_fade_mode               = MODE_NONE;
static uint16_t g_fade_hue           = 0;
extern nvs_handle_t NVS_HANDLE;

esp_err_t light_driver_init(light_driver_config_t *config)
{
    LIGHT_PARAM_CHECK(config);
    bool exist = false;

    if (ble_mesh_nvs_restore(NVS_HANDLE, LIGHT_STATUS_STORE_KEY, &g_light_status, sizeof(light_status_t), &exist) != ESP_OK) {
        memset(&g_light_status, 0, sizeof(light_status_t));
        g_light_status.mode              = MODE_HSV;
        g_light_status.on                = 1;
        g_light_status.hue               = 360;
        g_light_status.saturation        = 0;
        g_light_status.value             = 100;
        g_light_status.lightness         = 100;
        g_light_status.color_temperature = 0;
        g_light_status.brightness        = 30;
    }

#if CONFIG_IDF_TARGET_ESP32C3
    iot_led_init(LEDC_TIMER_0, LEDC_LOW_SPEED_MODE, 1000);
#elif CONFIG_IDF_TARGET_ESP32
    iot_led_init(LEDC_TIMER_0, LEDC_HIGH_SPEED_MODE, 1000);
#endif

    g_light_status.fade_period_ms  = config->fade_period_ms;
    g_light_status.blink_period_ms = config->blink_period_ms;

    iot_led_regist_channel(CHANNEL_ID_RED, config->gpio_red);
    iot_led_regist_channel(CHANNEL_ID_GREEN, config->gpio_green);
    iot_led_regist_channel(CHANNEL_ID_BLUE, config->gpio_blue);
    iot_led_regist_channel(CHANNEL_ID_WARM, config->gpio_warm);
    iot_led_regist_channel(CHANNEL_ID_COLD, config->gpio_cold);

    ESP_LOGI(TAG, "hue: %d, saturation: %d, value: %d, lightness: %d",
             g_light_status.hue, g_light_status.saturation, g_light_status.value, g_light_status.lightness);
    ESP_LOGI(TAG, "brightness: %d, color_temperature: %d",
             g_light_status.brightness, g_light_status.color_temperature);

    return ESP_OK;
}

esp_err_t light_driver_deinit(void)
{
    esp_err_t ret = ESP_OK;

    iot_led_deinit();

    return ret;
}

esp_err_t light_driver_config(uint32_t fade_period_ms, uint32_t blink_period_ms)
{
    g_light_status.fade_period_ms = fade_period_ms;
    g_light_status.blink_period_ms = blink_period_ms;

    return ESP_OK;
}

esp_err_t light_driver_set_rgb(uint8_t red, uint8_t green, uint8_t blue)
{
    esp_err_t ret = 0;

    ret = iot_led_set_channel(CHANNEL_ID_RED, red, 0);
    LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);

    ret = iot_led_set_channel(CHANNEL_ID_GREEN, green, 0);
    LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);

    ret = iot_led_set_channel(CHANNEL_ID_BLUE, blue, 0);
    LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);

    ret = iot_led_set_channel(CHANNEL_ID_WARM, 0, 0);
    LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);

    ret = iot_led_set_channel(CHANNEL_ID_COLD, 0, 0);
    LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);

    return ESP_OK;
}

static esp_err_t light_driver_hsv2rgb(uint16_t hue, uint8_t saturation, uint8_t value,
                                      uint8_t *red, uint8_t *green, uint8_t *blue)
{
    uint16_t hi = (hue / 60) % 6;
    uint16_t F = 100 * hue / 60 - 100 * hi;
    uint16_t P = value * (100 - saturation) / 100;
    uint16_t Q = value * (10000 - F * saturation) / 10000;
    uint16_t T = value * (10000 - saturation * (100 - F)) / 10000;

    switch (hi) {
        case 0:
            *red   = value;
            *green = T;
            *blue  = P;
            break;

        case 1:
            *red   = Q;
            *green = value;
            *blue  = P;
            break;

        case 2:
            *red   = P;
            *green = value;
            *blue  = T;
            break;

        case 3:
            *red   = P;
            *green = Q;
            *blue  = value;
            break;

        case 4:
            *red   = T;
            *green = P;
            *blue  = value;
            break;

        case 5:
            *red   = value;
            *green = P;
            *blue  = Q;
            break;

        default:
            return ESP_FAIL;
    }

    *red   = *red * 255 / 100;
    *green = *green * 255 / 100;
    *blue  = *blue * 255 / 100;

    return ESP_OK;
}

static void light_driver_rgb2hsv(uint16_t red, uint16_t green, uint16_t blue,
                                 uint16_t *h, uint8_t *s, uint8_t *v)
{
    double hue, saturation, value;
    double m_max = MAX(red, MAX(green, blue));
    double m_min = MIN(red, MIN(green, blue));
    double m_delta = m_max - m_min;

    value = m_max / 255.0;

    if (m_delta == 0) {
        hue = 0;
        saturation = 0;
    } else {
        saturation = m_delta / m_max;

        if (red == m_max) {
            hue = (green - blue) / m_delta;
        } else if (green == m_max) {
            hue = 2 + (blue - red) / m_delta;
        } else {
            hue = 4 + (red - green) / m_delta;
        }

        hue = hue * 60;

        if (hue < 0) {
            hue = hue + 360;
        }
    }

    *h = (int)(hue + 0.5);
    *s = (int)(saturation * 100 + 0.5);
    *v = (int)(value * 100 + 0.5);
}

// refence: https://axonflux.com/handy-rgb-to-hsl-and-rgb-to-hsv-color-model-c
static esp_err_t light_driver_hsl2rgb(uint16_t hue, uint8_t saturation, uint8_t lightness,
                                      uint8_t *red, uint8_t *green, uint8_t *blue)
{
    uint16_t hi = (hue / 60) % 6;
    uint16_t C  = (100 - abs(2 * lightness - 100)) * saturation / 100;
    uint16_t M  = 100 * (lightness - 0.5 * C) / 100;
    uint16_t X  = C * (100 - abs((hue * 100 / 60 ) % 200 - 100)) / 100;

    switch (hi) {
        case 0: /* hue 0~60 */
            *red   = C + M;
            *green = X + M;
            *blue  = M;
            break;

        case 1: /* hue 60~120 */
            *red   = X + M;
            *green = C + M;
            *blue  = M;
            break;

        case 2: /* hue 120~180 */
            *red   = M;
            *green = C + M;
            *blue  = X + M;
            break;

        case 3: /* hue 180~240 */
            *red   = M;
            *green = X + M;
            *blue  = C + M;
            break;

        case 4: /* hue 240~300 */
            *red   = X + M;
            *green = M;
            *blue  = C + M;
            break;

        case 5: /* hue 300~360 */
            *red   = C + M;
            *green = M;
            *blue  = X + M;
            break;

        default:
            return ESP_FAIL;
    }

    *red   = *red * 255 / 100;
    *green = *green * 255 / 100;
    *blue  = *blue * 255 / 100;

    return ESP_OK;
}

esp_err_t light_driver_set_hsv(uint16_t hue, uint8_t saturation, uint8_t value)
{
    LIGHT_PARAM_CHECK(hue <= 360);
    LIGHT_PARAM_CHECK(saturation <= 100);
    LIGHT_PARAM_CHECK(value <= 100);

    esp_err_t ret = ESP_OK;
    uint8_t red   = 0;
    uint8_t green = 0;
    uint8_t blue  = 0;

    ret = light_driver_hsv2rgb(hue, saturation, value, &red, &green, &blue);
    LIGHT_ERROR_CHECK(ret < 0, ret, "light_driver_hsv2rgb, ret: %d", ret);

    ESP_LOGV(TAG, "red: %d, green: %d, blue: %d", red, green, blue);

    ret = iot_led_set_channel(CHANNEL_ID_RED, red, g_light_status.fade_period_ms);
    LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);

    ret = iot_led_set_channel(CHANNEL_ID_GREEN, green, g_light_status.fade_period_ms);
    LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);

    ret = iot_led_set_channel(CHANNEL_ID_BLUE, blue, g_light_status.fade_period_ms);
    LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);

    if (g_light_status.mode != MODE_HSV) {
        ret = iot_led_set_channel(CHANNEL_ID_WARM, 0, g_light_status.fade_period_ms);
        LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);

        ret = iot_led_set_channel(CHANNEL_ID_COLD, 0, g_light_status.fade_period_ms);
        LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);
    }

    g_light_status.mode       = MODE_HSV;
    g_light_status.on         = 1;
    g_light_status.hue        = hue;
    g_light_status.value      = value;
    g_light_status.saturation = saturation;

    ret = ble_mesh_nvs_store(NVS_HANDLE, LIGHT_STATUS_STORE_KEY, &g_light_status, sizeof(light_status_t));
    LIGHT_ERROR_CHECK(ret < 0, ret, "ble_mesh_nvs_store, ret: %d", ret);

    return ESP_OK;
}

esp_err_t light_driver_set_hue(uint16_t hue)
{
    if (g_light_status.mode == MODE_HSV) {
        return light_driver_set_hsv(hue, g_light_status.saturation, g_light_status.value);
    } else if (g_light_status.mode == MODE_HSL) {
        return light_driver_set_hsl(hue, g_light_status.saturation, g_light_status.lightness);
    } else {
        return ESP_FAIL;
    }
}

esp_err_t light_driver_set_saturation(uint8_t saturation)
{
    if (g_light_status.mode == MODE_HSV) {
        return light_driver_set_hsv(g_light_status.hue, saturation, g_light_status.value);
    } else if (g_light_status.mode == MODE_HSL) {
        return light_driver_set_hsl(g_light_status.hue, saturation, g_light_status.lightness);
    } else {
        return ESP_FAIL;
    }
}

esp_err_t light_driver_set_value(uint8_t value)
{
    return light_driver_set_hsv(g_light_status.hue, g_light_status.saturation, value);
}

esp_err_t light_driver_get_hsv(uint16_t *hue, uint8_t *saturation, uint8_t *value)
{
    LIGHT_PARAM_CHECK(hue);
    LIGHT_PARAM_CHECK(saturation);
    LIGHT_PARAM_CHECK(value);

    *hue        = g_light_status.hue;
    *saturation = g_light_status.saturation;
    *value      = g_light_status.value;

    return ESP_OK;
}

uint16_t light_driver_get_hue(void)
{
    return g_light_status.hue;
}

uint8_t light_driver_get_saturation(void)
{
    return g_light_status.saturation;
}

uint8_t light_driver_get_value(void)
{
    return g_light_status.value;
}

uint8_t light_driver_get_mode(void)
{
    return g_light_status.mode;
}

esp_err_t light_driver_set_ctb(uint8_t color_temperature, uint8_t brightness)
{
    LIGHT_PARAM_CHECK(brightness <= 100);
    LIGHT_PARAM_CHECK(color_temperature <= 100);

    esp_err_t ret = ESP_OK;
    uint8_t warm_tmp = color_temperature * brightness / 100;
    uint8_t cold_tmp = (100 - color_temperature) * brightness / 100;
    warm_tmp         = warm_tmp < 15 ? warm_tmp : 14 + warm_tmp * 86 / 100;
    cold_tmp         = cold_tmp < 15 ? cold_tmp : 14 + cold_tmp * 86 / 100;

    ret = iot_led_set_channel(CHANNEL_ID_COLD,
                              cold_tmp * 255 / 100, g_light_status.fade_period_ms);
    LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);

    ret = iot_led_set_channel(CHANNEL_ID_WARM,
                              warm_tmp * 255 / 100, g_light_status.fade_period_ms);
    LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);

    if (g_light_status.mode != MODE_CTB) {
        ret = iot_led_set_channel(CHANNEL_ID_RED, 0, g_light_status.fade_period_ms);
        LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);

        ret = iot_led_set_channel(CHANNEL_ID_GREEN, 0, g_light_status.fade_period_ms);
        LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);

        ret = iot_led_set_channel(CHANNEL_ID_BLUE, 0, g_light_status.fade_period_ms);
        LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);
    }

    g_light_status.mode              = MODE_CTB;
    g_light_status.on                = 1;
    g_light_status.brightness        = brightness;
    g_light_status.color_temperature = color_temperature;

    ret = ble_mesh_nvs_store(NVS_HANDLE, LIGHT_STATUS_STORE_KEY, &g_light_status, sizeof(light_status_t));
    LIGHT_ERROR_CHECK(ret < 0, ret, "ble_mesh_nvs_store, ret: %d", ret);

    return ESP_OK;
}

esp_err_t light_driver_set_color_temperature(uint8_t color_temperature)
{
    return light_driver_set_ctb(color_temperature, g_light_status.brightness);
}

esp_err_t light_driver_set_brightness(uint8_t brightness)
{
    return light_driver_set_ctb(g_light_status.color_temperature, brightness);
}

esp_err_t light_driver_get_ctb(uint8_t *color_temperature, uint8_t *brightness)
{
    LIGHT_PARAM_CHECK(color_temperature);
    LIGHT_PARAM_CHECK(brightness);

    *brightness        = g_light_status.brightness;
    *color_temperature = g_light_status.color_temperature;

    return ESP_OK;
}

uint8_t light_driver_get_color_temperature(void)
{
    return g_light_status.color_temperature;
}

uint8_t light_driver_get_brightness(void)
{
    return g_light_status.brightness;
}

esp_err_t light_driver_set_hsl(uint16_t hue, uint8_t saturation, uint8_t lightness)
{
    LIGHT_PARAM_CHECK(hue <= 360);
    LIGHT_PARAM_CHECK(saturation <= 100);
    LIGHT_PARAM_CHECK(lightness <= 100);

    esp_err_t ret = ESP_OK;
    uint8_t red   = 0;
    uint8_t green = 0;
    uint8_t blue  = 0;

    ret = light_driver_hsl2rgb(hue, saturation, lightness, &red, &green, &blue);
    LIGHT_ERROR_CHECK(ret < 0, ret, "light_driver_hsl2rgb, ret: %d", ret);

    ESP_LOGV(TAG, "red: %d, green: %d, blue: %d", red, green, blue);

    ret = iot_led_set_channel(CHANNEL_ID_RED, red, g_light_status.fade_period_ms);
    LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);

    ret = iot_led_set_channel(CHANNEL_ID_GREEN, green, g_light_status.fade_period_ms);
    LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);

    ret = iot_led_set_channel(CHANNEL_ID_BLUE, blue, g_light_status.fade_period_ms);
    LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);

    if (g_light_status.mode != MODE_HSL) {
        ret = iot_led_set_channel(CHANNEL_ID_WARM, 0, g_light_status.fade_period_ms);
        LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);

        ret = iot_led_set_channel(CHANNEL_ID_COLD, 0, g_light_status.fade_period_ms);
        LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);
    }

    g_light_status.mode       = MODE_HSL;
    g_light_status.on         = 1;
    g_light_status.hue        = hue;
    g_light_status.saturation = saturation;
    g_light_status.lightness  = lightness;

    ret = ble_mesh_nvs_store(NVS_HANDLE, LIGHT_STATUS_STORE_KEY, &g_light_status, sizeof(light_status_t));
    LIGHT_ERROR_CHECK(ret < 0, ret, "ble_mesh_nvs_store, ret: %d", ret);

    return ESP_OK;
}

esp_err_t light_driver_set_lightness(uint8_t lightness)
{
    return light_driver_set_hsl(g_light_status.hue, g_light_status.saturation, lightness);
}

uint8_t light_driver_get_lightness(void)
{
    return g_light_status.lightness;
}

esp_err_t light_driver_get_hsl(uint16_t *hue, uint8_t *saturation, uint8_t *lightness)
{
    LIGHT_PARAM_CHECK(hue);
    LIGHT_PARAM_CHECK(saturation);
    LIGHT_PARAM_CHECK(lightness);

    *hue        = g_light_status.hue;
    *saturation = g_light_status.saturation;
    *lightness  = g_light_status.lightness;

    return ESP_OK;
}

esp_err_t light_driver_set_switch(bool on)
{
    esp_err_t ret     = ESP_OK;
    g_light_status.on = on;

    if (!g_light_status.on) {
        ret = iot_led_set_channel(CHANNEL_ID_RED, 0, g_light_status.fade_period_ms);
        LIGHT_ERROR_CHECK(ret < 0, ESP_FAIL, "iot_led_set_channel, ret: %d", ret);

        ret = iot_led_set_channel(CHANNEL_ID_GREEN, 0, g_light_status.fade_period_ms);
        LIGHT_ERROR_CHECK(ret < 0, ESP_FAIL, "iot_led_set_channel, ret: %d", ret);

        ret = iot_led_set_channel(CHANNEL_ID_BLUE, 0, g_light_status.fade_period_ms);
        LIGHT_ERROR_CHECK(ret < 0, ESP_FAIL, "iot_led_set_channel, ret: %d", ret);

        ret = iot_led_set_channel(CHANNEL_ID_COLD, 0, g_light_status.fade_period_ms);
        LIGHT_ERROR_CHECK(ret < 0, ESP_FAIL, "iot_led_set_channel, ret: %d", ret);

        ret = iot_led_set_channel(CHANNEL_ID_WARM, 0, g_light_status.fade_period_ms);
        LIGHT_ERROR_CHECK(ret < 0, ESP_FAIL, "iot_led_set_channel, ret: %d", ret);

    } else {
        switch (g_light_status.mode) {
            case MODE_HSV:
                g_light_status.value = (g_light_status.value) ? g_light_status.value : 100;
                ret = light_driver_set_hsv(g_light_status.hue, g_light_status.saturation, g_light_status.value);
                LIGHT_ERROR_CHECK(ret < 0, ESP_FAIL, "light_driver_set_hsv, ret: %d", ret);
                break;

            case MODE_HSL:
                g_light_status.lightness = (g_light_status.lightness) ? g_light_status.lightness : 100;
                ret = light_driver_set_hsl(g_light_status.hue, g_light_status.saturation, g_light_status.lightness);
                LIGHT_ERROR_CHECK(ret < 0, ESP_FAIL, "light_driver_set_hsl, ret: %d", ret);
                break;

            case MODE_CTB:
                g_light_status.brightness = (g_light_status.brightness) ? g_light_status.brightness : 100;
                ret = light_driver_set_ctb(g_light_status.color_temperature, g_light_status.brightness);
                LIGHT_ERROR_CHECK(ret < 0, ESP_FAIL, "light_driver_set_ctb, ret: %d", ret);
                break;

            default:
                ESP_LOGW(TAG, "This operation is not supported");
                break;
        }
    }

    ret = ble_mesh_nvs_store(NVS_HANDLE, LIGHT_STATUS_STORE_KEY, &g_light_status, sizeof(light_status_t));
    LIGHT_ERROR_CHECK(ret < 0, ESP_FAIL, "ble_mesh_nvs_store, ret: %d", ret);

    return ESP_OK;
}

esp_err_t light_driver_set_mode(light_mode_t mode)
{
    g_light_status.mode = mode;

    return ESP_OK;
}

bool light_driver_get_switch(void)
{
    return g_light_status.on;
}

esp_err_t light_driver_breath_start(uint8_t red, uint8_t green, uint8_t blue)
{
    esp_err_t ret = ESP_OK;

    ret = iot_led_start_blink(CHANNEL_ID_RED,
                              red, g_light_status.blink_period_ms, true);
    LIGHT_ERROR_CHECK(ret < 0, ESP_FAIL, "iot_led_start_blink, ret: %d", ret);
    ret = iot_led_start_blink(CHANNEL_ID_GREEN,
                              green, g_light_status.blink_period_ms, true);
    LIGHT_ERROR_CHECK(ret < 0, ESP_FAIL, "iot_led_start_blink, ret: %d", ret);
    ret = iot_led_start_blink(CHANNEL_ID_BLUE,
                              blue, g_light_status.blink_period_ms, true);
    LIGHT_ERROR_CHECK(ret < 0, ESP_FAIL, "iot_led_start_blink, ret: %d", ret);

    g_light_blink_flag = true;

    return ESP_OK;
}

esp_err_t light_driver_breath_stop(void)
{
    esp_err_t ret = ESP_OK;

    if (g_light_blink_flag == false) {
        return ESP_OK;
    }

    ret = iot_led_stop_blink(CHANNEL_ID_RED);
    LIGHT_ERROR_CHECK(ret < 0, ESP_FAIL, "iot_led_stop_blink, ret: %d", ret);

    ret = iot_led_stop_blink(CHANNEL_ID_GREEN);
    LIGHT_ERROR_CHECK(ret < 0, ESP_FAIL, "iot_led_stop_blink, ret: %d", ret);

    ret = iot_led_stop_blink(CHANNEL_ID_BLUE);
    LIGHT_ERROR_CHECK(ret < 0, ESP_FAIL, "iot_led_stop_blink, ret: %d", ret);

    light_driver_set_switch(true);

    return ESP_OK;
}

esp_err_t light_driver_fade_brightness(uint8_t brightness)
{
    esp_err_t ret = ESP_OK;
    g_fade_mode   = MODE_ON;
    uint32_t fade_period_ms = 0;

    if (g_light_status.mode == MODE_HSV) {
        uint8_t red   = 0;
        uint8_t green = 0;
        uint8_t blue  = 0;

        ret = light_driver_hsv2rgb(g_light_status.hue, g_light_status.saturation, g_light_status.value, &red, &green, &blue);
        LIGHT_ERROR_CHECK(ret < 0, ret, "light_driver_hsv2rgb, ret: %d", ret);

        if (brightness != 0) {
            ret = iot_led_get_channel((ledc_channel_t)CHANNEL_ID_RED, &red);
            LIGHT_ERROR_CHECK(ret < 0, ESP_FAIL, "iot_led_get_channel, ret: %d", ret);
            ret = iot_led_get_channel((ledc_channel_t)CHANNEL_ID_GREEN, &green);
            LIGHT_ERROR_CHECK(ret < 0, ESP_FAIL, "iot_led_get_channel, ret: %d", ret);
            ret = iot_led_get_channel((ledc_channel_t)CHANNEL_ID_BLUE, &blue);
            LIGHT_ERROR_CHECK(ret < 0, ESP_FAIL, "iot_led_get_channel, ret: %d", ret);

            uint8_t max_color       = MAX(MAX(red, green), blue);
            uint8_t change_value    = brightness * 255 / 100 - max_color;
            fade_period_ms = LIGHT_FADE_PERIOD_MAX_MS * change_value / 255;
        } else {
            fade_period_ms = LIGHT_FADE_PERIOD_MAX_MS * MAX(MAX(red, green), blue) / 255;
            red   = 0;
        }

        g_light_status.value = brightness;
        light_driver_hsv2rgb(g_light_status.hue, g_light_status.saturation, g_light_status.value, &red, &green, &blue);

        ret = iot_led_set_channel(CHANNEL_ID_RED, red, fade_period_ms);
        LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);

        ret = iot_led_set_channel(CHANNEL_ID_GREEN, green, fade_period_ms);
        LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);

        ret = iot_led_set_channel(CHANNEL_ID_BLUE, blue, fade_period_ms);
        LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);

    } else if (g_light_status.mode == MODE_CTB) {
        uint8_t warm_tmp = 0;
        uint8_t cold_tmp = 0;
        fade_period_ms = LIGHT_FADE_PERIOD_MAX_MS * g_light_status.brightness / 100;

        if (brightness != 0) {
            uint8_t change_value = brightness - g_light_status.brightness;
            warm_tmp = g_light_status.color_temperature;
            cold_tmp = (brightness - g_light_status.color_temperature);
            fade_period_ms = LIGHT_FADE_PERIOD_MAX_MS * change_value / 100;
        }

        ret = iot_led_set_channel(CHANNEL_ID_COLD,
                                  cold_tmp * 255 / 100, fade_period_ms);
        LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);

        ret = iot_led_set_channel(CHANNEL_ID_WARM,
                                  warm_tmp * 255 / 100, fade_period_ms);
        LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);

        g_light_status.brightness = brightness;
    }

    ret = ble_mesh_nvs_store(NVS_HANDLE, LIGHT_STATUS_STORE_KEY, &g_light_status, sizeof(light_status_t));
    LIGHT_ERROR_CHECK(ret < 0, ret, "ble_mesh_nvs_store, ret: %d", ret);

    return ESP_OK;
}

static void light_fade_timer_stop(void)
{
    if (!g_fade_timer) {
        return ;
    }

    if (!xTimerStop(g_fade_timer, portMAX_DELAY)) {
        ESP_LOGW(TAG, "xTimerStop timer: %p", g_fade_timer);
    }

    if (!xTimerDelete(g_fade_timer, portMAX_DELAY)) {
        ESP_LOGW(TAG, "xTimerDelete timer: %p", g_fade_timer);
    }

    g_fade_timer = NULL;
}

static void light_fade_timer_cb(TimerHandle_t timer)
{
    uint8_t red   = 0;
    uint8_t green = 0;
    uint8_t blue  = 0;
    uint32_t fade_period_ms = LIGHT_FADE_PERIOD_MAX_MS * 2 / 6;
    int variety = (g_fade_hue > 180) ? 60 : -60;

    if (g_light_status.hue >= 360 || g_light_status.hue <= 0) {
        light_fade_timer_stop();
    }

    g_light_status.hue = g_light_status.hue >= 360 ? 360 : g_light_status.hue + variety;
    g_light_status.hue = g_light_status.hue <= 60 ? 0 : g_light_status.hue + variety;

    light_driver_hsv2rgb(g_light_status.hue, g_light_status.saturation, g_light_status.value, &red, &green, &blue);

    iot_led_set_channel(CHANNEL_ID_RED, red, fade_period_ms);
    iot_led_set_channel(CHANNEL_ID_GREEN, green, fade_period_ms);
    iot_led_set_channel(CHANNEL_ID_BLUE, blue, fade_period_ms);
}

esp_err_t light_driver_fade_hue(uint16_t hue)
{
    esp_err_t ret = ESP_OK;
    g_fade_mode   = MODE_HSV;
    g_fade_hue    = hue;

    light_fade_timer_stop();

    if (g_light_status.mode != MODE_HSV) {
        ret = iot_led_set_channel(CHANNEL_ID_WARM, 0, 0);
        LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);

        ret = iot_led_set_channel(CHANNEL_ID_COLD, 0, 0);
        LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);
    }

    g_light_status.mode     = MODE_HSV;
    g_light_status.value    = (g_light_status.value == 0) ? 100 : g_light_status.value;
    uint32_t fade_period_ms = LIGHT_FADE_PERIOD_MAX_MS * 2 / 6;

    light_fade_timer_cb(NULL);

    g_fade_timer = xTimerCreate("light_timer", fade_period_ms,
                                true, NULL, light_fade_timer_cb);
    xTimerStart(g_fade_timer, 0);

    return ESP_OK;
}

esp_err_t light_driver_fade_warm(uint8_t color_temperature)
{
    esp_err_t ret = ESP_OK;
    g_fade_mode   = MODE_CTB;

    if (g_light_status.mode != MODE_CTB) {
        ret = iot_led_set_channel(CHANNEL_ID_RED, 0, g_light_status.fade_period_ms);
        LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);

        ret = iot_led_set_channel(CHANNEL_ID_GREEN, 0, g_light_status.fade_period_ms);
        LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);

        ret = iot_led_set_channel(CHANNEL_ID_BLUE, 0, g_light_status.fade_period_ms);
        LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);
    }

    uint8_t warm_tmp =  color_temperature * g_light_status.brightness / 100;
    uint8_t cold_tmp = (100 - color_temperature) * g_light_status.brightness / 100;

    ret = iot_led_set_channel(CHANNEL_ID_COLD, cold_tmp * 255 / 100, LIGHT_FADE_PERIOD_MAX_MS);
    LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);

    ret = iot_led_set_channel(CHANNEL_ID_WARM, warm_tmp * 255 / 100, LIGHT_FADE_PERIOD_MAX_MS);
    LIGHT_ERROR_CHECK(ret < 0, ret, "iot_led_set_channel, ret: %d", ret);

    g_light_status.mode              = MODE_CTB;
    g_light_status.color_temperature = color_temperature;
    ret = ble_mesh_nvs_store(NVS_HANDLE, LIGHT_STATUS_STORE_KEY, &g_light_status, sizeof(light_status_t));
    LIGHT_ERROR_CHECK(ret < 0, ret, "ble_mesh_nvs_store, ret: %d", ret);

    return ESP_OK;
}

esp_err_t light_driver_fade_stop(void)
{
    esp_err_t ret = ESP_OK;

    light_fade_timer_stop();

    if (g_light_status.mode != MODE_CTB) {
        uint16_t hue       = 0;
        uint8_t saturation = 0;
        uint8_t value      = 0;

        ret = iot_led_stop_blink(CHANNEL_ID_RED);
        LIGHT_ERROR_CHECK(ret < 0, ESP_FAIL, "iot_led_stop_blink, ret: %d", ret);

        ret = iot_led_stop_blink(CHANNEL_ID_GREEN);
        LIGHT_ERROR_CHECK(ret < 0, ESP_FAIL, "iot_led_stop_blink, ret: %d", ret);

        ret = iot_led_stop_blink(CHANNEL_ID_BLUE);
        LIGHT_ERROR_CHECK(ret < 0, ESP_FAIL, "iot_led_stop_blink, ret: %d", ret);

        uint8_t red, green, blue;

        ret = iot_led_get_channel(CHANNEL_ID_RED, &red);
        LIGHT_ERROR_CHECK(ret < 0, ESP_FAIL, "iot_led_get_channel, ret: %d", ret);
        ret = iot_led_get_channel(CHANNEL_ID_GREEN, &green);
        LIGHT_ERROR_CHECK(ret < 0, ESP_FAIL, "iot_led_get_channel, ret: %d", ret);
        ret = iot_led_get_channel(CHANNEL_ID_BLUE, &blue);
        LIGHT_ERROR_CHECK(ret < 0, ESP_FAIL, "iot_led_get_channel, ret: %d", ret);

        light_driver_rgb2hsv(red, green, blue, &hue, &saturation, &value);

        g_light_status.hue   = (g_fade_mode == MODE_HSV) ? hue : g_light_status.hue;
        g_light_status.value = (g_fade_mode == MODE_OFF || g_fade_mode == MODE_ON) ? value : g_light_status.value;
    } else {
        uint8_t color_temperature = 0;
        uint8_t brightness        = 0;

        ret = iot_led_stop_blink(CHANNEL_ID_COLD);
        LIGHT_ERROR_CHECK(ret < 0, ESP_FAIL, "iot_led_stop_blink, ret: %d", ret);

        ret = iot_led_stop_blink(CHANNEL_ID_WARM);
        LIGHT_ERROR_CHECK(ret < 0, ESP_FAIL, "iot_led_stop_blink, ret: %d", ret);

        uint8_t warm_tmp, cold_tmp;
        uint8_t tmp;

        ret = iot_led_get_channel(CHANNEL_ID_WARM, &tmp);
        LIGHT_ERROR_CHECK(ret < 0, ESP_FAIL, "iot_led_get_channel, ret: %d", ret);
        warm_tmp = (int32_t)tmp * 100 / 255;

        ret = iot_led_get_channel(CHANNEL_ID_COLD, &tmp);
        LIGHT_ERROR_CHECK(ret < 0, ESP_FAIL, "iot_led_get_channel, ret: %d", ret);
        cold_tmp = (int32_t)tmp * 100 / 255;

        color_temperature = (!warm_tmp) ? 0 : 100 / (cold_tmp / warm_tmp + 1);
        brightness        = (!color_temperature) ? cold_tmp : warm_tmp * 100 / color_temperature;

        g_light_status.brightness        = (g_fade_mode == MODE_OFF || g_fade_mode == MODE_ON) ? brightness : g_light_status.brightness;
        g_light_status.color_temperature = (g_fade_mode == MODE_CTB) ? color_temperature : g_light_status.color_temperature;
    }

    ret = ble_mesh_nvs_store(NVS_HANDLE, LIGHT_STATUS_STORE_KEY, &g_light_status, sizeof(light_status_t));
    LIGHT_ERROR_CHECK(ret < 0, ret, "ble_mesh_nvs_store, ret: %d", ret);

    g_fade_mode = MODE_NONE;
    return ESP_OK;
}
