// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "hal/touch_sensor_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize touch module.
 * @note  If default parameter don't match the usage scenario, it can be changed after this function.
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_NO_MEM Touch pad init error
 *     - ESP_ERR_NOT_SUPPORTED Touch pad is providing current to external XTAL
 */
esp_err_t touch_pad_init(void);

/**
 * @brief Un-install touch pad driver.
 * @note  After this function is called, other touch functions are prohibited from being called.
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Touch pad driver not initialized
 */
esp_err_t touch_pad_deinit(void);

/**
 * @brief Initialize touch pad GPIO
 * @param touch_num touch pad index
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_io_init(touch_pad_t touch_num);

/**
 * @brief Set touch sensor high voltage threshold of chanrge.
 *        The touch sensor measures the channel capacitance value by charging and discharging the channel.
 *        So the high threshold should be less than the supply voltage.
 * @param refh the value of DREFH
 * @param refl the value of DREFL
 * @param atten the attenuation on DREFH
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_set_voltage(touch_high_volt_t refh, touch_low_volt_t refl, touch_volt_atten_t atten);

/**
 * @brief Get touch sensor reference voltage,
 * @param refh pointer to accept DREFH value
 * @param refl pointer to accept DREFL value
 * @param atten pointer to accept the attenuation on DREFH
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_get_voltage(touch_high_volt_t *refh, touch_low_volt_t *refl, touch_volt_atten_t *atten);

/**
 * @brief Set touch sensor charge/discharge speed for each pad.
 *        If the slope is 0, the counter would always be zero.
 *        If the slope is 1, the charging and discharging would be slow, accordingly.
 *        If the slope is set 7, which is the maximum value, the charging and discharging would be fast.
 * @note The higher the charge and discharge current, the greater the immunity of the touch channel,
 *       but it will increase the system power consumption.
 * @param touch_num touch pad index
 * @param slope touch pad charge/discharge speed
 * @param opt the initial voltage
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_set_cnt_mode(touch_pad_t touch_num, touch_cnt_slope_t slope, touch_tie_opt_t opt);

/**
 * @brief Get touch sensor charge/discharge speed for each pad
 * @param touch_num touch pad index
 * @param slope pointer to accept touch pad charge/discharge slope
 * @param opt pointer to accept the initial voltage
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_get_cnt_mode(touch_pad_t touch_num, touch_cnt_slope_t *slope, touch_tie_opt_t *opt);

/**
 * @brief Deregister the handler previously registered using touch_pad_isr_handler_register
 * @param fn  handler function to call (as passed to touch_pad_isr_handler_register)
 * @param arg  argument of the handler (as passed to touch_pad_isr_handler_register)
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if a handler matching both fn and
 *        arg isn't registered
 */
esp_err_t touch_pad_isr_deregister(void(*fn)(void *), void *arg);

/**
 * @brief Get the touch pad which caused wakeup from deep sleep.
 * @param pad_num pointer to touch pad which caused wakeup
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_ARG parameter is NULL
 */
esp_err_t touch_pad_get_wakeup_status(touch_pad_t *pad_num);

/**
 * @brief Set touch sensor FSM mode, the test action can be triggered by the timer,
 *        as well as by the software.
 * @param mode FSM mode
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_set_fsm_mode(touch_fsm_mode_t mode);

/**
 * @brief Get touch sensor FSM mode
 * @param mode pointer to accept FSM mode
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_get_fsm_mode(touch_fsm_mode_t *mode);


/**
 * @brief To clear the touch sensor channel active status.
 *
 * @note The FSM automatically updates the touch sensor status. It is generally not necessary to call this API to clear the status.
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_clear_status(void);

/**
 * @brief Get the touch sensor channel active status mask.
 *        The bit position represents the channel number. The 0/1 status of the bit represents the trigger status.
 *
 * @return
 *      - The touch sensor status. e.g. Touch1 trigger status is `status_mask & (BIT1)`.
 */
uint32_t touch_pad_get_status(void);

/**
 * @brief Check touch sensor measurement status.
 *
 * @return
 *      - True measurement is under way
 *      - False measurement done
 */
bool touch_pad_meas_is_done(void);

#ifdef __cplusplus
}
#endif