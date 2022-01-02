/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __IOT_LED_H__
#define __IOT_LED_H__

#include "driver/ledc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GAMMA_CORRECTION 0.8                               /**< Gamma curve parameter */
#define GAMMA_TABLE_SIZE 256                               /**< Gamma table size, used for led fade*/
#define DUTY_SET_CYCLE (20)                                /**< Set duty cycle */

/**
 * Macro which can be used to check the error code,
 * and terminate the program in case the code is not ESP_OK.
 * Prints the error code, error location, and the failed statement to serial output.
 *
 * Disabled if assertions are disabled.
 */
#define LIGHT_ERROR_CHECK(con, err, format, ...) do { \
        if (con) { \
            if(*format != '\0') \
                ESP_LOGW(TAG, "<%s> " format, esp_err_to_name(err), ##__VA_ARGS__); \
            return err; \
        } \
    } while(0)

#define LIGHT_PARAM_CHECK(con) do { \
        if (!(con)) { \
            ESP_LOGE(TAG, "<ESP_ERR_INVALID_ARG> !(%s)", #con); \
            return ESP_ERR_INVALID_ARG; \
        } \
    } while(0)

/**
  * @brief Initialize and set the ledc timer for the iot led
  *
  * @param timer_num The timer index of ledc timer group used for iot led
  *     This parameter can be one of LEDC_TIMER_x where x can be (0 .. 3)
  *
  * @param speed_mode speed mode of ledc timer
  *     This parameter can be one of LEDC_x_SPEED_MODE where x can be (LOW, HIGH)
  *
  * @param freq_hz frequency of ledc timer
  *     This parameter must be less than 5000
  *
  * @return
  *     - ESP_OK if sucess
  *     - ESP_ERR_INVALID_ARG Parameter error
  *     - ESP_FAIL Can not find a proper pre-divider number base on the given frequency
  *         and the current duty_resolution.
*/
esp_err_t iot_led_init(ledc_timer_t timer_num, ledc_mode_t speed_mode, uint32_t freq_hz);

/**
  * @brief DeInitializes the iot led and free resource
  *
  * @return
  *     - ESP_OK if sucess
*/
esp_err_t iot_led_deinit(void);

/**
  * @brief Set the ledc channel used by iot led and associate the gpio port used
  *     for output
  *
  * @param channel The ledc channel
  *     This parameter can be LEDC_CHANNEL_x where x can be (0 .. 15)
  * @param gpio_num the ledc output gpio_num
  *     This parameter can be GPIO_NUM_x where x can be (0, 33)
  *
  * @note If the operation of esp32 depends on SPI FLASH or PSRAM, then these related
  *     pins should not be set to output.
  *
  * @return
  *     - ESP_OK if sucess
  *     - ESP_ERR_INVALID_ARG Parameter error
*/
esp_err_t iot_led_regist_channel(ledc_channel_t channel, gpio_num_t gpio_num);

/**
  * @brief Returns the channel value
  * @note before calling this function, you need to call iot_led_regist_channel() to
  *     set the channel
  *
  * @param channel The ledc channel
  *     This parameter can be LEDC_CHANNEL_x where x can be (0 .. 15)
  * @param dst The address where the channel value is stored
  * @return
  *     - ESP_OK if sucess
  *     - ESP_ERR_INVALID_ARG if dst is NULL
*/
esp_err_t iot_led_get_channel(ledc_channel_t channel, uint8_t *dst);

/**
  * @brief Set the fade state for the specified channel
  * @note before calling this function, you need to call iot_led_regist_channel() to
  *     set the channel
  *
  * @param channel The ledc channel
  *     This parameter can be LEDC_CHANNEL_x where x can be (0 .. 15)
  * @param value The target output brightness of iot led
  *     This parameter can be (0 .. 255)
  * @param fade_ms The time from the current value to the target value
  * @return
  *     - ESP_OK if sucess
*/
esp_err_t iot_led_set_channel(ledc_channel_t channel, uint8_t value, uint32_t fade_ms);

/**
  * @brief Set the blink state or loop fade for the specified channel
  * @note before calling this function, you need to call iot_led_regist_channel() to
  *     set the channel
  *
  * @param channel The ledc channel
  *     This parameter can be LEDC_CHANNEL_x where x can be (0 .. 15)
  * @param value The output brightness of iot led
  *     This parameter can be (0 .. 255)
  * @param period_ms Blink cycle
  * @param fade_flag select loop fade or blink
  *     1 for loop fade
  *     0 for blink
  * @return
  *     - ESP_OK if sucess
*/
esp_err_t iot_led_start_blink(ledc_channel_t channel, uint8_t value, uint32_t period_ms, bool fade_flag);

/**
  * @brief Stop the blink state or loop fade for the specified channel
  *
  * @param channel The ledc channel
  *     This parameter can be LEDC_CHANNEL_x where x can be (0 .. 15)
  * @return
  *     - ESP_OK if sucess
*/
esp_err_t iot_led_stop_blink(ledc_channel_t channel);

/**
  * @brief Set the specified gamma_table to control the fade effect, usually
  *     no need to set
  *
  * @param gamma_table[GAMMA_TABLE_SIZE] Expected gamma table value
  *
  * @note  Gamma_table is the dimming curve used by the iot_led driver.
  *     The element type is uint16_t. Each element is treated as a binary
  *     fixed-point number. The decimal point is before the eighth bit
  *     and after the ninth bit, so the range of expressions can be
  *     0x00.00 ~ 0xff.ff.
  * @note default gamma_table is created in iot_led_init()
  *
  * @return
  *     - ESP_OK if sucess
*/
esp_err_t iot_led_set_gamma_table(const uint16_t gamma_table[GAMMA_TABLE_SIZE]);

#ifdef __cplusplus
}
#endif

#endif /**< __IOT_LED_H__ */
