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

#include "driver/touch_sensor_common.h"

/**
 * @brief Configure touch pad interrupt threshold.
 *
 * @note  If FSM mode is set to TOUCH_FSM_MODE_TIMER, this function will be blocked for one measurement cycle and wait for data to be valid.
 *
 * @param touch_num touch pad index
 * @param threshold interrupt threshold,
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG if argument wrong
 *     - ESP_FAIL if touch pad not initialized
 */
esp_err_t touch_pad_config(touch_pad_t touch_num, uint16_t threshold);

/**
 * @brief get touch sensor counter value.
 *        Each touch sensor has a counter to count the number of charge/discharge cycles.
 *        When the pad is not 'touched', we can get a number of the counter.
 *        When the pad is 'touched', the value in counter will get smaller because of the larger equivalent capacitance.
 *
 * @note This API requests hardware measurement once. If IIR filter mode is enabled,
 *       please use 'touch_pad_read_raw_data' interface instead.
 *
 * @param touch_num touch pad index
 * @param touch_value pointer to accept touch sensor value
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Touch pad parameter error
 *     - ESP_ERR_INVALID_STATE This touch pad hardware connection is error, the value of "touch_value" is 0.
 *     - ESP_FAIL Touch pad not initialized
 */
esp_err_t touch_pad_read(touch_pad_t touch_num, uint16_t *touch_value);

/**
 * @brief get filtered touch sensor counter value by IIR filter.
 *
 * @note touch_pad_filter_start has to be called before calling touch_pad_read_filtered.
 *       This function can be called from ISR
 *
 * @param touch_num touch pad index
 * @param touch_value pointer to accept touch sensor value
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Touch pad parameter error
 *     - ESP_ERR_INVALID_STATE This touch pad hardware connection is error, the value of "touch_value" is 0.
 *     - ESP_FAIL Touch pad not initialized
 */
esp_err_t touch_pad_read_filtered(touch_pad_t touch_num, uint16_t *touch_value);

/**
 * @brief get raw data (touch sensor counter value) from IIR filter process.
 *        Need not request hardware measurements.
 *
 * @note touch_pad_filter_start has to be called before calling touch_pad_read_raw_data.
 *       This function can be called from ISR
 *
 * @param touch_num touch pad index
 * @param touch_value pointer to accept touch sensor value
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Touch pad parameter error
 *     - ESP_ERR_INVALID_STATE This touch pad hardware connection is error, the value of "touch_value" is 0.
 *     - ESP_FAIL Touch pad not initialized
 */
esp_err_t touch_pad_read_raw_data(touch_pad_t touch_num, uint16_t *touch_value);

/**
  * @brief Callback function that is called after each IIR filter calculation.
  * @note This callback is called in timer task in each filtering cycle.
  * @note This callback should not be blocked.
  * @param raw_value  The latest raw data(touch sensor counter value) that
  *        points to all channels(raw_value[0..TOUCH_PAD_MAX-1]).
  * @param filtered_value  The latest IIR filtered data(calculated from raw data) that
  *        points to all channels(filtered_value[0..TOUCH_PAD_MAX-1]).
  *
  */
typedef void (* filter_cb_t)(uint16_t *raw_value, uint16_t *filtered_value);

/**
 * @brief Register the callback function that is called after each IIR filter calculation.
 * @note The 'read_cb' callback is called in timer task in each filtering cycle.
 * @param read_cb  Pointer to filtered callback function.
 *                 If the argument passed in is NULL, the callback will stop.
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_ARG set error
 */
esp_err_t touch_pad_set_filter_read_cb(filter_cb_t read_cb);

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
esp_err_t touch_pad_isr_register(intr_handler_t fn, void *arg);

/**
 * @brief Set touch sensor measurement and sleep time.
 *        Excessive total time will slow down the touch response.
 *        Too small measurement time will not be sampled enough, resulting in inaccurate measurements.
 *
 * @note The greater the duty cycle of the measurement time, the more system power is consumed.
 * @param sleep_cycle  The touch sensor will sleep after each measurement.
 *                     sleep_cycle decide the interval between each measurement.
 *                     t_sleep = sleep_cycle / (RTC_SLOW_CLK frequency).
 *                     The approximate frequency value of RTC_SLOW_CLK can be obtained using rtc_clk_slow_freq_get_hz function.
 * @param meas_cycle The duration of the touch sensor measurement.
 *                   t_meas = meas_cycle / 8M, the maximum measure time is 0xffff / 8M = 8.19 ms
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_set_meas_time(uint16_t sleep_cycle, uint16_t meas_cycle);

/**
 * @brief Get touch sensor measurement and sleep time
 * @param sleep_cycle  Pointer to accept sleep cycle number
 * @param meas_cycle Pointer to accept measurement cycle count.
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_get_meas_time(uint16_t *sleep_cycle, uint16_t *meas_cycle);

/**
 * @brief Trigger a touch sensor measurement, only support in SW mode of FSM
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_sw_start(void);

/**
 * @brief Set touch sensor interrupt threshold
 * @param touch_num touch pad index
 * @param threshold threshold of touchpad count, refer to touch_pad_set_trigger_mode to see how to set trigger mode.
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_set_thresh(touch_pad_t touch_num, uint16_t threshold);

/**
 * @brief Get touch sensor interrupt threshold
 * @param touch_num touch pad index
 * @param threshold pointer to accept threshold
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_get_thresh(touch_pad_t touch_num, uint16_t *threshold);

/**
 * @brief Set touch sensor interrupt trigger mode.
 *        Interrupt can be triggered either when counter result is less than
 *        threshold or when counter result is more than threshold.
 * @param mode touch sensor interrupt trigger mode
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_set_trigger_mode(touch_trigger_mode_t mode);

/**
 * @brief Get touch sensor interrupt trigger mode
 * @param mode pointer to accept touch sensor interrupt trigger mode
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_get_trigger_mode(touch_trigger_mode_t *mode);

/**
 * @brief Set touch sensor interrupt trigger source. There are two sets of touch signals.
 *        Set1 and set2 can be mapped to several touch signals. Either set will be triggered
 *        if at least one of its touch signal is 'touched'. The interrupt can be configured to be generated
 *        if set1 is triggered, or only if both sets are triggered.
 * @param src touch sensor interrupt trigger source
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_set_trigger_source(touch_trigger_src_t src);

/**
 * @brief Get touch sensor interrupt trigger source
 * @param src pointer to accept touch sensor interrupt trigger source
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_get_trigger_source(touch_trigger_src_t *src);

/**
 * @brief Set touch sensor group mask.
 *        Touch pad module has two sets of signals, 'Touched' signal is triggered only if
 *        at least one of touch pad in this group is "touched".
 *        This function will set the register bits according to the given bitmask.
 * @param set1_mask bitmask of touch sensor signal group1, it's a 10-bit value
 * @param set2_mask bitmask of touch sensor signal group2, it's a 10-bit value
 * @param en_mask bitmask of touch sensor work enable, it's a 10-bit value
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_set_group_mask(uint16_t set1_mask, uint16_t set2_mask, uint16_t en_mask);

/**
 * @brief Get touch sensor group mask.
 * @param set1_mask pointer to accept bitmask of touch sensor signal group1, it's a 10-bit value
 * @param set2_mask pointer to accept bitmask of touch sensor signal group2, it's a 10-bit value
 * @param en_mask pointer to accept bitmask of touch sensor work enable, it's a 10-bit value
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_get_group_mask(uint16_t *set1_mask, uint16_t *set2_mask, uint16_t *en_mask);

/**
 * @brief Clear touch sensor group mask.
 *        Touch pad module has two sets of signals, Interrupt is triggered only if
 *        at least one of touch pad in this group is "touched".
 *        This function will clear the register bits according to the given bitmask.
 * @param set1_mask bitmask touch sensor signal group1, it's a 10-bit value
 * @param set2_mask bitmask touch sensor signal group2, it's a 10-bit value
 * @param en_mask bitmask of touch sensor work enable, it's a 10-bit value
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_clear_group_mask(uint16_t set1_mask, uint16_t set2_mask, uint16_t en_mask);

/**
 * @brief To enable touch pad interrupt
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_intr_enable(void);

/**
 * @brief To disable touch pad interrupt
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_intr_disable(void);

/**
 * @brief To clear touch pad interrupt
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_intr_clear(void);

/**
 * @brief set touch pad filter calibration period, in ms.
 *        Need to call touch_pad_filter_start before all touch filter APIs
 * @param new_period_ms filter period, in ms
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_STATE driver state error
 *      - ESP_ERR_INVALID_ARG parameter error
 */
esp_err_t touch_pad_set_filter_period(uint32_t new_period_ms);

/**
 * @brief get touch pad filter calibration period, in ms
 *        Need to call touch_pad_filter_start before all touch filter APIs
 * @param p_period_ms pointer to accept period
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_STATE driver state error
 *      - ESP_ERR_INVALID_ARG parameter error
 */
esp_err_t touch_pad_get_filter_period(uint32_t *p_period_ms);

/**
 * @brief start touch pad filter function
 *      This API will start a filter to process the noise in order to prevent false triggering
 *      when detecting slight change of capacitance.
 *      Need to call touch_pad_filter_start before all touch filter APIs
 *
 * @note This filter uses FreeRTOS timer, which is dispatched from a task with
 *       priority 1 by default on CPU 0. So if some application task with higher priority
 *       takes a lot of CPU0 time, then the quality of data obtained from this filter will be affected.
 *       You can adjust FreeRTOS timer task priority in menuconfig.
 * @param filter_period_ms filter calibration period, in ms
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_ARG parameter error
 *      - ESP_ERR_NO_MEM No memory for driver
 *      - ESP_ERR_INVALID_STATE driver state error
 */
esp_err_t touch_pad_filter_start(uint32_t filter_period_ms);

/**
 * @brief stop touch pad filter function
 *        Need to call touch_pad_filter_start before all touch filter APIs
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_STATE driver state error
 */
esp_err_t touch_pad_filter_stop(void);

/**
 * @brief delete touch pad filter driver and release the memory
 *        Need to call touch_pad_filter_start before all touch filter APIs
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_STATE driver state error
 */
esp_err_t touch_pad_filter_delete(void);

#ifdef __cplusplus
}
#endif