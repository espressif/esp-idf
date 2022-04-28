/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __LIGHT_DRIVER_H__
#define __LIGHT_DRIVER_H__

#include "iot_led.h"

#ifdef  __cplusplus
extern "C" {
#endif

#define LIGHT_STATUS_STORE_KEY   "light_status"

/**
 * @brief The mode of the five-color light
 */
typedef enum light_mode {
    MODE_NONE                = 0,
    MODE_RGB                 = 1,
    MODE_HSV                 = 2,
    MODE_CTB                 = 3,
    MODE_ON                  = 4,
    MODE_OFF                 = 5,
    MODE_HUE_INCREASE        = 4,
    MODE_HUE_DECREASE        = 5,
    MODE_WARM_INCREASE       = 6,
    MODE_WARM_DECREASE       = 7,
    MODE_BRIGHTNESS_INCREASE = 8,
    MODE_BRIGHTNESS_DECREASE = 9,
    MODE_HSL                 = 10,
} light_mode_t;

/**
 * @brief Light driven configuration
 */
typedef struct {
    gpio_num_t gpio_red;      /**< Red corresponds to GPIO */
    gpio_num_t gpio_green;    /**< Green corresponds to GPIO */
    gpio_num_t gpio_blue;     /**< Blue corresponds to GPIO */
    gpio_num_t gpio_cold;     /**< Cool corresponds to GPIO */
    gpio_num_t gpio_warm;     /**< Warm corresponds to GPIO */
    uint32_t fade_period_ms;  /**< The time from the current color to the next color */
    uint32_t blink_period_ms; /**< Period of flashing lights */
} light_driver_config_t;

/**
 * @brief  Light initialize
 *
 * @param  config [description]
 *
 * @return
 *      - ESP_OK
 *      - ESP_ERR_INVALID_ARG
 */
esp_err_t light_driver_init(light_driver_config_t *config);

/**
 * @brief  Light deinitialize
 *
 * @return
 *      - ESP_OK
 *      - ESP_ERR_INVALID_ARG
 */
esp_err_t light_driver_deinit(void);


/**
 * @brief Set the fade time of the light
 *
 * @param  fade_period_ms  The time from the current color to the next color
 * @param  blink_period_ms Light flashing frequency
 *
 * @return
 *      - ESP_OK
 *      - ESP_FAIL
 */
esp_err_t light_driver_config(uint32_t fade_period_ms, uint32_t blink_period_ms);

/**@{*/
/**
 * @brief  Set the status of the light
 *
 *
 * @return
 *      - ESP_OK
 *      - ESP_ERR_INVALID_ARG
 */
esp_err_t light_driver_set_hue(uint16_t hue);
esp_err_t light_driver_set_saturation(uint8_t saturation);
esp_err_t light_driver_set_value(uint8_t value);
esp_err_t light_driver_set_color_temperature(uint8_t color_temperature);
esp_err_t light_driver_set_brightness(uint8_t brightness);
esp_err_t light_driver_set_hsv(uint16_t hue, uint8_t saturation, uint8_t value);
esp_err_t light_driver_set_hsl(uint16_t hue, uint8_t saturation, uint8_t lightness);
esp_err_t light_driver_set_lightness(uint8_t lightness);
esp_err_t light_driver_set_ctb(uint8_t color_temperature, uint8_t brightness);
esp_err_t light_driver_set_switch(bool status);
esp_err_t light_driver_set_mode(light_mode_t mode);

/**@}*/

/**@{*/
/**
 * @brief  Set the status of the light
 */
uint16_t light_driver_get_hue(void);
uint8_t light_driver_get_saturation(void);
uint8_t light_driver_get_value(void);
esp_err_t light_driver_get_hsv(uint16_t *hue, uint8_t *saturation, uint8_t *value);
uint8_t light_driver_get_lightness(void);
esp_err_t light_driver_get_hsl(uint16_t *hue, uint8_t *saturation, uint8_t *lightness);
uint8_t light_driver_get_color_temperature(void);
uint8_t light_driver_get_brightness(void);
esp_err_t light_driver_get_ctb(uint8_t *color_temperature, uint8_t *brightness);
bool light_driver_get_switch(void);
uint8_t light_driver_get_mode(void);
/**@}*/

/**@{*/
/**
 * @brief  Used to indicate the operating mode, such as configuring the network mode, upgrading mode
 *
 * @note   The state of the light is not saved in nvs
 *
 * @return
 *      - ESP_OK
 *      - ESP_ERR_INVALID_ARG
 */
esp_err_t light_driver_set_rgb(uint8_t red, uint8_t green, uint8_t blue);
esp_err_t light_driver_breath_start(uint8_t red, uint8_t green, uint8_t blue);
esp_err_t light_driver_breath_stop(void);
esp_err_t light_driver_blink_start(uint8_t red, uint8_t green, uint8_t blue);
esp_err_t light_driver_blink_stop(void);
/**@}*/

/**@{*/
/**
 * @brief  Color gradient
 *
 * @return
 *      - ESP_OK
 *      - ESP_ERR_INVALID_ARG
 */
esp_err_t light_driver_fade_brightness(uint8_t brightness);
esp_err_t light_driver_fade_hue(uint16_t hue);
esp_err_t light_driver_fade_warm(uint8_t color_temperature);
esp_err_t light_driver_fade_stop(void);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif/**< __LIGHT_DRIVER_H__ */
