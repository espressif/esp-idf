// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set touch sensor FSM start
 * @note  Start FSM after the touch sensor FSM mode is set.
 * @note  Call this function will reset beseline of all touch channels.
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_fsm_start(void);

/**
 * @brief Stop touch sensor FSM.
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_fsm_stop(void);

/**
 * @brief Trigger a touch sensor measurement, only support in SW mode of FSM
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_sw_start(void);

/**
 * @brief Set touch sensor times of charge and discharge and sleep time.
 *        Excessive total time will slow down the touch response. 
 *        Too small measurement time will not be sampled enough, resulting in inaccurate measurements.
 * 
 * @note The greater the duty cycle of the measurement time, the more system power is consumed.
 * @param sleep_cycle The touch sensor will sleep after each measurement.
 *                    sleep_cycle decide the interval between each measurement.
 *                    t_sleep = sleep_cycle / (RTC_SLOW_CLK frequency).
 *                    The approximate frequency value of RTC_SLOW_CLK can be obtained using rtc_clk_slow_freq_get_hz function.
 * @param meas_timers The times of charge and discharge in each measure process of touch channels.
 *                    The timer frequency is 8Mhz. Range: 0 ~ 0xffff.
 *                    Recommended typical value: Modify this value to make the measurement time around 1ms.
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_set_meas_time(uint16_t sleep_cycle, uint16_t meas_times);

/**
 * @brief Get touch sensor times of charge and discharge and sleep time
 * @param sleep_cycle  Pointer to accept sleep cycle number
 * @param meas_times Pointer to accept measurement times count.
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_get_meas_time(uint16_t *sleep_cycle, uint16_t *meas_times);

/**
 * @brief Set connection type of touch channel in idle status.
 *        When a channel is in measurement mode, other initialized channels are in idle mode. 
 *        The touch channel is generally adjacent to the trace, so the connection state of the idle channel
 *        affects the stability and sensitivity of the test channel.
 *        The `CONN_HIGHZ`(high resistance) setting increases the sensitivity of touch channels. 
 *        The `CONN_GND`(grounding) setting increases the stability of touch channels.
 * @param type  Select idle channel connect to high resistance state or ground.
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_set_inactive_connect(touch_pad_conn_type_t type);

/**
 * @brief Set connection type of touch channel in idle status.
 *        When a channel is in measurement mode, other initialized channels are in idle mode. 
 *        The touch channel is generally adjacent to the trace, so the connection state of the idle channel
 *        affects the stability and sensitivity of the test channel.
 *        The `CONN_HIGHZ`(high resistance) setting increases the sensitivity of touch channels. 
 *        The `CONN_GND`(grounding) setting increases the stability of touch channels.
 * @param type  Pointer to connection type.
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_get_inactive_connect(touch_pad_conn_type_t *type);

/**
 * @brief Set the trigger threshold of touch sensor.
 *        The threshold determines the sensitivity of the touch sensor.
 *        The threshold is the original value of the trigger state minus the baseline value.
 * @note  If set "TOUCH_PAD_THRESHOLD_MAX", the touch is never be trigered.
 * @param touch_num touch pad index
 * @param threshold threshold of touch sensor. Should be less than the max change value of touch.
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_set_thresh(touch_pad_t touch_num, uint32_t threshold);

/**
 * @brief Get touch sensor trigger threshold
 * @param touch_num touch pad index
 * @param threshold pointer to accept threshold
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_get_thresh(touch_pad_t touch_num, uint32_t *threshold);

/**
 * @brief Register touch channel into touch sensor scan group.
 *        The working mode of the touch sensor is cyclically scanned.
 *        This function will set the scan bits according to the given bitmask.
 * @note  If set this mask, the FSM timer should be stop firsty.
 * @note  The touch sensor that in scan map, should be deinit GPIO function firstly by `touch_pad_io_init`.
 * @param enable_mask bitmask of touch sensor scan group. 
 *        e.g. TOUCH_PAD_NUM14 -> BIT(14)
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_set_channel_mask(uint16_t enable_mask);

/**
 * @brief Get the touch sensor scan group bit mask.
 * @param enable_mask Pointer to bitmask of touch sensor scan group. 
 *        e.g. TOUCH_PAD_NUM14 -> BIT(14)
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_get_channel_mask(uint16_t *enable_mask);

/**
 * @brief Clear touch channel from touch sensor scan group.
 *        The working mode of the touch sensor is cyclically scanned.
 *        This function will clear the scan bits according to the given bitmask.
 * @note  If clear all mask, the FSM timer should be stop firsty.
 * @param enable_mask bitmask of touch sensor scan group. 
 *        e.g. TOUCH_PAD_NUM14 -> BIT(14)
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_clear_channel_mask(uint16_t enable_mask);

/**
 * @brief Configure parameter for each touch channel.
 * @note  Touch num 0 is denoise channel, please use `touch_pad_denoise_enable` to set denoise function
 * @param touch_num touch pad index
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG if argument wrong
 *     - ESP_FAIL if touch pad not initialized
 */
esp_err_t touch_pad_config(touch_pad_t touch_num);

/**
 * @brief Reset the whole of touch module.
 * @note  Call this funtion after `touch_pad_fsm_stop`,
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_reset(void);

/**
 * @brief Check touch sensor measurement status.
 *        If doing measurement, the flag will be clear.
 *        If finish measurement. the flag will be set.
 * @return
 *      - TRUE finish measurement
 *      - FALSE doing measurement
 */
bool touch_pad_meas_is_done(void);

/**
 * @brief Get the current measure channel. 
 *        Touch sensor measurement is cyclic scan mode. 
 * @return
 *      - touch channel number
 */
touch_pad_t touch_pad_get_current_meas_channel(void);

/**
 * @brief Get the touch sensor interrupt status mask.
 * @return
 *      - touch intrrupt bit 
 */
uint32_t touch_pad_read_intr_status_mask(void);

/**
 * @brief Enable touch sensor interrupt by bitmask.
 * @param type interrupt type
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_intr_enable(touch_pad_intr_mask_t int_mask);

/**
 * @brief Disable touch sensor interrupt by bitmask.
 * @param type interrupt type
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_intr_disable(touch_pad_intr_mask_t int_mask);

/**
 * @brief   Register touch-pad ISR.
 *          The handler will be attached to the same CPU core that this function is running on.
 * @param fn  Pointer to ISR handler
 * @param arg  Parameter for ISR
 * @return
 *     - ESP_OK Success ;
 *     - ESP_ERR_INVALID_ARG GPIO error
 *     - ESP_ERR_NO_MEM No memory
 */
esp_err_t touch_pad_isr_register(intr_handler_t fn, void* arg, touch_pad_intr_mask_t intr_mask);

/**
 * @brief get raw data of touch sensor.
 * @note After the initialization is complete, the "raw_data" is max value. You need to wait for a measurement
 *       cycle before you can read the correct touch value.
 * @param touch_num touch pad index
 * @param raw_data pointer to accept touch sensor value
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Touch channel 0 havent this parameter.
 */

esp_err_t touch_pad_read_raw_data(touch_pad_t touch_num, uint32_t *raw_data);

/**
 * @brief get baseline of touch sensor.
 * @note After initialization, the baseline value is the maximum during the first measurement period.
 * @param touch_num touch pad index
 * @param touch_value pointer to accept touch sensor value
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Touch channel 0 havent this parameter.
 */
esp_err_t touch_pad_filter_read_baseline(touch_pad_t touch_num, uint32_t *basedata);

/**
 * @brief Force reset baseline to raw data of touch sensor.
 * @param touch_num touch pad index
 *                  - TOUCH_PAD_MAX Reset basaline of all channels
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_filter_reset_baseline(touch_pad_t touch_num);

/**
 * @brief set parameter of touch sensor filter and detection algorithm.
 *        For more details on the detection algorithm, please refer to the application documentation.
 * @param filter_info select filter type and threshold of detection algorithm
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_filter_set_config(touch_filter_config_t *filter_info);

/**
 * @brief get parameter of touch sensor filter and detection algorithm.
 *        For more details on the detection algorithm, please refer to the application documentation.
 * @param filter_info select filter type and threshold of detection algorithm
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_filter_get_config(touch_filter_config_t *filter_info);

/**
 * @brief enable touch sensor filter for detection algorithm.
 *        For more details on the detection algorithm, please refer to the application documentation.
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_filter_enable(void);

/**
 * @brief diaable touch sensor filter for detection algorithm.
 *        For more details on the detection algorithm, please refer to the application documentation.
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_filter_disable(void);

/**
 * @brief set parameter of denoise pad (TOUCH_PAD_NUM0).
 *        T0 is an internal channel that does not have a corresponding external GPIO. 
 *        T0 will work simultaneously with the measured channel Tn. Finally, the actual 
 *        measured value of Tn is the value after subtracting lower bits of T0.
 *        This denoise function filters out interference introduced on all channels, 
 *        such as noise introduced by the power supply and external EMI.
 * @param denoise parameter of denoise
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_denoise_set_config(touch_pad_denoise_t *denoise);

/**
 * @brief get parameter of denoise pad (TOUCH_PAD_NUM0).
 * @param denoise Pointer to parameter of denoise
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_denoise_get_config(touch_pad_denoise_t *denoise);

/**
 * @brief enable denoise function.
 *        T0 is an internal channel that does not have a corresponding external GPIO. 
 *        T0 will work simultaneously with the measured channel Tn. Finally, the actual 
 *        measured value of Tn is the value after subtracting lower bits of T0.
 *        This denoise function filters out interference introduced on all channels, 
 *        such as noise introduced by the power supply and external EMI.
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_denoise_enable(void);

/**
 * @brief disable denoise function.
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_denoise_disable(void);

/**
 * @brief Get denoise measure value (TOUCH_PAD_NUM0).
 * @param denoise value of denoise
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_denoise_read_data(uint32_t *data);

/**
 * @brief set parameter of waterproof function.
 *        The waterproof function includes a shielded channel (TOUCH_PAD_NUM14) and a guard channel.
 *        The shielded channel outputs the same signal as the channel being measured. 
 *        It is generally designed as a grid and is placed around the touch buttons.
 *        The shielded channel does not follow the measurement signal of the protection channel. 
 *        So that the guard channel can detect a large area of water.
 * @param waterproof parameter of waterproof
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_waterproof_set_config(touch_pad_waterproof_t *waterproof);

/**
 * @brief get parameter of waterproof function.
 * @param waterproof parameter of waterproof
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_waterproof_get_config(touch_pad_waterproof_t *waterproof);

/**
 * @brief Enable parameter of waterproof function.
 *        The waterproof function includes a shielded channel (TOUCH_PAD_NUM14) and a guard channel.
 *        The shielded channel outputs the same signal as the channel being measured. 
 *        It is generally designed as a grid and is placed around the touch buttons.
 *        The shielded channel does not follow the measurement signal of the protection channel. 
 *        So that the guard channel can detect a large area of water.
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_waterproof_enable(void);

/**
 * @brief Enable parameter of waterproof function.
 *        The waterproof function includes a shielded channel (TOUCH_PAD_NUM14) and a guard channel.
 *        The shielded channel outputs the same signal as the channel being measured. 
 *        It is generally designed as a grid and is placed around the touch buttons.
 *        The shielded channel does not follow the measurement signal of the protection channel. 
 *        So that the guard channel can detect a large area of water.
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_waterproof_disable(void);

/**
 * @brief Set parameter of proximity channel. Three proximity sensing channels can be set. 
 *        The proximity sensor measurement is the accumulation of touch channel measurements.
 * @note  If stop the proximity function for the channel, point this proximity channel to `TOUCH_PAD_NUM0`.
 * @param proximity parameter of proximity
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_proximity_set_config(touch_pad_proximity_t *proximity);

/**
 * @brief Get parameter of proximity channel. Three proximity sensing channels can be set. 
 *        The proximity sensor measurement is the accumulation of touch channel measurements.
 * @param proximity parameter of proximity
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_proximity_get_config(touch_pad_proximity_t *proximity);

/**
 * @brief Get measure count of proximity channel.
 *        The proximity sensor measurement is the accumulation of touch channel measurements.
 * @param touch_num touch pad index
 * @param proximity parameter of proximity
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG parameter is NULL
 */
esp_err_t touch_pad_proximity_read_meas_cnt(touch_pad_t touch_num, uint32_t *cnt);

/**
 * @brief Get the accumulated measurement of the proximity sensor.
 *        The proximity sensor measurement is the accumulation of touch channel measurements.
 * @param touch_num touch pad index
 * @param measure_out If the accumulation process does not end, the `measure_out` is the process value.
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_proximity_data_get(touch_pad_t touch_num, uint32_t *measure_out);

/**
 * @brief Set parameter of touch sensor in sleep mode. 
 *        In order to achieve low power consumption in sleep mode, other circuits except the RTC part of the register are in a power-off state.
 *        Only one touch channel is supported in the sleep state, which can be used as a wake-up function.
 *        If in non-sleep mode, the sleep parameters do not work.
 * @param slp_config touch pad config
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_sleep_channel_config(touch_pad_sleep_channel_t *slp_config);

/**
 * @brief Read baseline of touch sensor in sleep mode.
 * @param baseline pointer to accept touch sensor baseline value
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG parameter is NULL
 */
esp_err_t touch_pad_sleep_channel_read_baseline(uint32_t *baseline);

/**
 * @brief Read debounce of touch sensor in sleep mode.
 * @param debounce pointer to accept touch sensor debounce value
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG parameter is NULL
 */
esp_err_t touch_pad_sleep_channel_read_debounce(uint32_t *debounce);

/**
 * @brief Read proximity count of touch sensor in sleep mode.
 * @param proximity_cnt pointer to accept touch sensor proximity count value
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG parameter is NULL
 */
esp_err_t touch_pad_sleep_channel_read_proximity_cnt(uint32_t *proximity_cnt);

#ifdef __cplusplus
}
#endif