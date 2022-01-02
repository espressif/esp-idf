/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __IOT_LIGHT_H__
#define __IOT_LIGHT_H__

#include "driver/ledc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void *light_handle_t;

#define DUTY_SET_CYCLE         (20)  /**< Set duty cycle */
#define DUTY_SET_GAMMA         (0.6) /**< Set the Gamma value for the fade curve, default value is 0.6 */

#define LIGHT_MAX_CHANNEL_NUM  (5)

/**
  * @brief  light initialize
  *
  * @param  timer the LEDC timer used by light
  * @param  speed_mode speed mode of LEDC timer
  * @param  freq_hz frequency of LEDC timer
  * @param  channel_num decide how many channels the light contains
  * @param  timer_bit LEDC PWM duty resolution
  *
  * @return  the handle of light
  */
light_handle_t iot_light_create(ledc_timer_t timer, ledc_mode_t speed_mode, uint32_t freq_hz, uint8_t channel_num, ledc_timer_bit_t timer_bit);

/**
  * @brief  add an output channel to light
  *
  * @param  light_handle light handle
  * @param  channel_idx the id of channel (0 ~ channel_num-1)
  * @param  io_num the IO number use to output LEDC PWM
  * @param  channel the ledc channel you want to use
  *
  * @return
  *     - ESP_OK: succeed
  *     - others: fail
  */
esp_err_t iot_light_channel_regist(light_handle_t light_handle, uint8_t channel_idx, gpio_num_t io_num, ledc_channel_t channel);

/**
  * @brief  free the memory of light
  *
  * @param  light_handle light handle
  *
  * @return
  *     - ESP_OK: succeed
  *     - others: fail
  */
esp_err_t iot_light_delete(light_handle_t light_handle);

/**
  * @brief  get channel duty
  *
  * @param  light_handle light handle
  * @param  channel_id the id of channel (0 ~ channel_num-1)
  *
  * @return
  *     - LEDC_ERR_DUTY if parameter error
  *     - Others Current LEDC duty
  */
uint32_t iot_light_duty_get(light_handle_t light_handle, uint8_t channel_id);

/**
  * @brief  set light fade with time. if set fade_period_ms as 0, set the duty directly.
  *
  * @param  light_handle light handle
  * @param  channel_id the id of channel (0 ~ channel_num-1)
  * @param  duty target duty
  * @param  fade_period_ms fade time (uint: ms)
  *
  * @return
  *     - ESP_OK: succeed
  *     - others: fail
  */
esp_err_t iot_light_fade_with_time(light_handle_t light_handle, uint8_t channel_id, uint32_t duty, uint32_t fade_period_ms);

/**
  * @brief  set breath config of a light channel, call `iot_light_operate_start` to start breath operation
  *
  * @param  light_handle light handle
  * @param  channel_id the id of channel (0 ~ channel_num-1)
  * @param  duty the maximum duty when breath
  * @param  breath_period_ms breath period (uint: ms)
  *
  * @return
  *     - ESP_OK: succeed
  *     - others: fail
  */
esp_err_t iot_light_breath_config(light_handle_t light_handle, uint8_t channel_id, uint32_t duty, uint32_t breath_period_ms);

/**
  * @brief  set blink config of a light channel, call `iot_light_operate_start` to start blink operation
  *
  * @param  light_handle light handle
  * @param  channel_id the id of channel (0 ~ channel_num-1)
  * @param  blink_period_ms blink period (uint: ms)
  *
  * @return
  *     - ESP_OK: succeed
  *     - others: fail
  */
esp_err_t iot_light_blink_config(light_handle_t light_handle, uint8_t channel_id, uint32_t blink_period_ms);

/**
  * @brief  start breath or blink operation, user need to set breath or blink config before call this API
  *
  * @param  light_handle light handle
  * @param  channel_id the id of channel (0 ~ channel_num-1)
  *
  * @return
  *     - ESP_OK: succeed
  *     - others: fail
  */
esp_err_t iot_light_operate_start(light_handle_t light_handle, uint8_t channel_id);

/**
  * @brief  stop breath or blink operation
  *
  * @param  light_handle light handle
  * @param  channel_id the id of channel (0 ~ channel_num-1)
  *
  * @return
  *     - ESP_OK: succeed
  *     - others: fail
  */
esp_err_t iot_light_operate_stop(light_handle_t light_handle, uint8_t channel_id);

#ifdef __cplusplus
}
#endif

#endif /**< __IOT_LIGHT_H__ */
