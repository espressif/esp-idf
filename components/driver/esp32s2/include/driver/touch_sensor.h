/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "driver/touch_sensor_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set touch sensor FSM start
 * @note  Start FSM after the touch sensor FSM mode is set.
 * @note  Call this function will reset benchmark of all touch channels.
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
 * @note  Though this API name is same as ESP32, it has opposite logic of capacity.
 *        The touch sensor on ESP32-S2 will fix the count of charge and discharge cycles (specified by the second parameter)
 *        and then record the count of the clock cycles(which is 8 MHz) during the sensing period as the raw value.
 *        That means the raw value will increase as the capacity of the touch pad increasing.
 * @note The greater the duty cycle of the measurement time, the more system power is consumed.
 *
 * @param sleep_cycle The touch sensor will sleep after each measurement.
 *                    sleep_cycle decide the interval between each measurement.
 *                    t_sleep = sleep_cycle / (RTC_SLOW_CLK frequency).
 *                    The approximate frequency value of RTC_SLOW_CLK can be obtained using rtc_clk_slow_freq_get_hz function.
 * @param meas_times The times of charge and discharge in each measurement of touch channels.  Range: 0 ~ 0xffff.
 *                   Recommended typical value: Modify this value to make the measurement time around 1 ms.
 *                   The clock frequency is 8 MHz, so the raw value will be about 8000 if the measurement time is 1 ms
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
esp_err_t touch_pad_set_idle_channel_connect(touch_pad_conn_type_t type);

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
esp_err_t touch_pad_get_idle_channel_connect(touch_pad_conn_type_t *type);

/**
 * @brief Set the trigger threshold of touch sensor.
 *        The threshold determines the sensitivity of the touch sensor.
 *        The threshold is the original value of the trigger state minus the benchmark value.
 * @note  If set "TOUCH_PAD_THRESHOLD_MAX", the touch is never be triggered.
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
 * @brief Reset the FSM of touch module.
 * @note  Call this function after `touch_pad_fsm_stop`.
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_reset(void);

/**
 * @brief Get the current measure channel.
 * @note  Should be called when touch sensor measurement is in cyclic scan mode.
 * @return
 *      - touch channel number
 */
touch_pad_t touch_pad_get_current_meas_channel(void);

/**
 * @brief Get the touch sensor interrupt status mask.
 * @return
 *      - touch interrupt bit
 */
uint32_t touch_pad_read_intr_status_mask(void);

/**
 * @brief Enable touch sensor interrupt by bitmask.
 * @note  This API can be called in ISR handler.
 * @param int_mask Pad mask to enable interrupts
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_intr_enable(touch_pad_intr_mask_t int_mask);

/**
 * @brief Disable touch sensor interrupt by bitmask.
 * @note  This API can be called in ISR handler.
 * @param int_mask Pad mask to disable interrupts
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_intr_disable(touch_pad_intr_mask_t int_mask);

/**
 * @brief Clear touch sensor interrupt by bitmask.
 * @param int_mask Pad mask to clear interrupts
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_intr_clear(touch_pad_intr_mask_t int_mask);

/**
 * @brief   Register touch-pad ISR.
 *          The handler will be attached to the same CPU core that this function is running on.
 * @param fn  Pointer to ISR handler
 * @param arg  Parameter for ISR
 * @param intr_mask Enable touch sensor interrupt handler by bitmask.
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Arguments error
 *     - ESP_ERR_NO_MEM No memory
 */
esp_err_t touch_pad_isr_register(intr_handler_t fn, void *arg, touch_pad_intr_mask_t intr_mask);

/**
 * @brief Enable/disable the timeout check and set timeout threshold for all touch sensor channels measurements.
 *        If enable: When the touch reading of a touch channel exceeds the measurement threshold, a timeout interrupt will be generated.
 *        If disable: the FSM does not check if the channel under measurement times out.
 *
 * @note The threshold compared with touch readings.
 * @note In order to avoid abnormal short circuit of some touch channels. This function should be turned on.
 *       Ensure the normal operation of other touch channels.
 *
 * @param enable true(default): Enable the timeout check; false: Disable the timeout check.
 * @param threshold For all channels, the maximum value that will not be exceeded during normal operation.
 *
* @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_timeout_set(bool enable, uint32_t threshold);

/**
 * @brief Call this interface after timeout to make the touch channel resume normal work. Point on the next channel to measure.
 *        If this API is not called, the touch FSM will stop the measurement after timeout interrupt.
 *
 * @note  Call this API after finishes the exception handling by user.
 *
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_timeout_resume(void);

/**
 * @brief get raw data of touch sensor.
 * @note After the initialization is complete, the "raw_data" is max value. You need to wait for a measurement
 *       cycle before you can read the correct touch value.
 * @param touch_num touch pad index
 * @param raw_data pointer to accept touch sensor value
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Touch channel 0 haven't this parameter.
 */

esp_err_t touch_pad_read_raw_data(touch_pad_t touch_num, uint32_t *raw_data);

/**
 * @brief get benchmark of touch sensor.
 * @note After initialization, the benchmark value is the maximum during the first measurement period.
 * @param touch_num touch pad index
 * @param benchmark pointer to accept touch sensor benchmark value
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Touch channel 0 haven't this parameter.
 */
esp_err_t touch_pad_read_benchmark(touch_pad_t touch_num, uint32_t *benchmark);

/**
 * @brief Get smoothed data that obtained by filtering the raw data.
 *
 * @param touch_num touch pad index
 * @param smooth pointer to smoothed data
 */
esp_err_t touch_pad_filter_read_smooth(touch_pad_t touch_num, uint32_t *smooth);

/**
 * @brief Force reset benchmark to raw data of touch sensor.
 * @param touch_num touch pad index
 *                  - TOUCH_PAD_MAX Reset basaline of all channels
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_reset_benchmark(touch_pad_t touch_num);

/**
 * @brief set parameter of touch sensor filter and detection algorithm.
 *        For more details on the detection algorithm, please refer to the application documentation.
 * @param filter_info select filter type and threshold of detection algorithm
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_filter_set_config(const touch_filter_config_t *filter_info);

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
 * @brief disable touch sensor filter for detection algorithm.
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
 *        The noise reduction function filters out interference introduced simultaneously on all channels,
 *        such as noise introduced by power supplies and external EMI.
 * @param denoise parameter of denoise
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_denoise_set_config(const touch_pad_denoise_t *denoise);

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
 *        The noise reduction function filters out interference introduced simultaneously on all channels,
 *        such as noise introduced by power supplies and external EMI.
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
 * @param data Pointer to receive denoise value
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_denoise_read_data(uint32_t *data);

/**
 * @brief set parameter of waterproof function.
 *
 *        The waterproof function includes a shielded channel (TOUCH_PAD_NUM14) and a guard channel.
 *        Guard pad is used to detect the large area of water covering the touch panel.
 *        Shield pad is used to shield the influence of water droplets covering the touch panel.
 *        It is generally designed as a grid and is placed around the touch buttons.
 *
 * @param waterproof parameter of waterproof
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_waterproof_set_config(const touch_pad_waterproof_t *waterproof);

/**
 * @brief get parameter of waterproof function.
 * @param waterproof parameter of waterproof
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_waterproof_get_config(touch_pad_waterproof_t *waterproof);

/**
 * @brief Enable parameter of waterproof function.
 *        Should be called after function ``touch_pad_waterproof_set_config``.
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_waterproof_enable(void);

/**
 * @brief Disable parameter of waterproof function.
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_waterproof_disable(void);

/**
 * @brief Enable/disable proximity function of touch channels.
 *        The proximity sensor measurement is the accumulation of touch channel measurements.
 *
 * @note Supports up to three touch channels configured as proximity sensors.
 * @param touch_num touch pad index
 * @param enabled true: enable the proximity function; false:  disable the proximity function
 * @return
 *     - ESP_OK: Configured correctly.
 *     - ESP_ERR_INVALID_ARG: Touch channel number error.
 *     - ESP_ERR_NOT_SUPPORTED: Don't support configured.
 */
esp_err_t touch_pad_proximity_enable(touch_pad_t touch_num, bool enabled);

/**
 * @brief Set measure count of proximity channel.
 *        The proximity sensor measurement is the accumulation of touch channel measurements.
 *
 * @note All proximity channels use the same `count` value. So please pass the parameter `TOUCH_PAD_MAX`.
 * @param touch_num Touch pad index. In this version, pass the parameter `TOUCH_PAD_MAX`.
 * @param count The cumulative times of measurements for proximity pad. Range: 0 ~ 255.
 * @return
 *     - ESP_OK: Configured correctly.
 *     - ESP_ERR_INVALID_ARG: Touch channel number error.
 */
esp_err_t touch_pad_proximity_set_count(touch_pad_t touch_num, uint32_t count);

/**
 * @brief Get measure count of proximity channel.
 *        The proximity sensor measurement is the accumulation of touch channel measurements.
 *
 * @note All proximity channels use the same `count` value. So please pass the parameter `TOUCH_PAD_MAX`.
 * @param touch_num Touch pad index. In this version, pass the parameter `TOUCH_PAD_MAX`.
 * @param count The cumulative times of measurements for proximity pad. Range: 0 ~ 255.
 * @return
 *     - ESP_OK: Configured correctly.
 *     - ESP_ERR_INVALID_ARG: Touch channel number error.
 */
esp_err_t touch_pad_proximity_get_count(touch_pad_t touch_num, uint32_t *count);

/**
 * @brief Get the accumulated measurement of the proximity sensor.
 *        The proximity sensor measurement is the accumulation of touch channel measurements.
 * @param touch_num touch pad index
 * @param measure_out If the accumulation process does not end, the `measure_out` is the process value.
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Touch num is not proximity
 */
esp_err_t touch_pad_proximity_get_data(touch_pad_t touch_num, uint32_t *measure_out);

/**
 * @brief Get parameter of touch sensor sleep channel.
 *        The touch sensor can works in sleep mode to wake up sleep.
 *
 * @note  After the sleep channel is configured, Please use special functions for sleep channel.
 *        e.g. The user should uses `touch_pad_sleep_channel_read_data` instead of `touch_pad_read_raw_data` to obtain the sleep channel reading.
 *
 * @param slp_config touch sleep pad config.
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_sleep_channel_get_info(touch_pad_sleep_channel_t *slp_config);

/**
 * @brief Enable/Disable sleep channel function for touch sensor.
 *        The touch sensor can works in sleep mode to wake up sleep.
 *
 * @note  ESP32S2 only support one sleep channel.
 * @note  After the sleep channel is configured, Please use special functions for sleep channel.
 *        e.g. The user should uses `touch_pad_sleep_channel_read_data` instead of `touch_pad_read_raw_data` to obtain the sleep channel reading.
 *
 * @param pad_num Set touch channel number for sleep pad. Only one touch sensor channel is supported in deep sleep mode.
 * @param enable true: enable sleep pad for touch sensor; false: disable sleep pad for touch sensor;
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_sleep_channel_enable(touch_pad_t pad_num, bool enable);

/**
 * @brief Enable/Disable proximity function for sleep channel.
 *        The touch sensor can works in sleep mode to wake up sleep.
 *
 * @note  ESP32S2 only support one sleep channel.
 *
 * @param pad_num Set touch channel number for sleep pad. Only one touch sensor channel is supported in deep sleep mode.
 * @param enable true: enable proximity for sleep channel; false: disable proximity for sleep channel;
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_sleep_channel_enable_proximity(touch_pad_t pad_num, bool enable);

/**
 * @brief Set the trigger threshold of touch sensor in deep sleep.
 *        The threshold determines the sensitivity of the touch sensor.
 *
 * @note  In general, the touch threshold during sleep can use the threshold parameter parameters before sleep.
 *
 * @param pad_num Set touch channel number for sleep pad. Only one touch sensor channel is supported in deep sleep mode.
 * @param touch_thres touch sleep pad threshold
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_sleep_set_threshold(touch_pad_t pad_num, uint32_t touch_thres);

/**
 * @brief Get the trigger threshold of touch sensor in deep sleep.
 *        The threshold determines the sensitivity of the touch sensor.
 *
 * @note In general, the touch threshold during sleep can use the threshold parameter parameters before sleep.
 *
 * @param pad_num Set touch channel number for sleep pad. Only one touch sensor channel is supported in deep sleep mode.
 * @param touch_thres touch sleep pad threshold
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_sleep_get_threshold(touch_pad_t pad_num, uint32_t *touch_thres);

/**
 * @brief Read benchmark of touch sensor sleep channel.
 * @param pad_num Set touch channel number for sleep pad. Only one touch sensor channel is supported in deep sleep mode.
 * @param benchmark pointer to accept touch sensor benchmark value
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG parameter is NULL
 */
esp_err_t touch_pad_sleep_channel_read_benchmark(touch_pad_t pad_num, uint32_t *benchmark);

/**
 * @brief Read smoothed data of touch sensor sleep channel.
 *        Smoothed data is filtered from the raw data.
 * @param pad_num Set touch channel number for sleep pad. Only one touch sensor channel is supported in deep sleep mode.
 * @param smooth_data pointer to accept touch sensor smoothed data
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG parameter is NULL
 */
esp_err_t touch_pad_sleep_channel_read_smooth(touch_pad_t pad_num, uint32_t *smooth_data);

/**
 * @brief Read raw data of touch sensor sleep channel.
 * @param pad_num Set touch channel number for sleep pad. Only one touch sensor channel is supported in deep sleep mode.
 * @param raw_data pointer to accept touch sensor raw data
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG parameter is NULL
 */
esp_err_t touch_pad_sleep_channel_read_data(touch_pad_t pad_num, uint32_t *raw_data);

/**
 * @brief Reset benchmark of touch sensor sleep channel.
 *
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_sleep_channel_reset_benchmark(void);

/**
 * @brief Read proximity count of touch sensor sleep channel.
 * @param pad_num Set touch channel number for sleep pad. Only one touch sensor channel is supported in deep sleep mode.
 * @param proximity_cnt pointer to accept touch sensor proximity count value
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG parameter is NULL
 */
esp_err_t touch_pad_sleep_channel_read_proximity_cnt(touch_pad_t pad_num, uint32_t *proximity_cnt);

/**
 * @brief Change the operating frequency of touch pad in deep sleep state. Reducing the operating frequency can effectively reduce power consumption.
 *        If this function is not called, the working frequency of touch in the deep sleep state is the same as that in the wake-up state.
 *
 * @param sleep_cycle The touch sensor will sleep after each measurement.
 *                    sleep_cycle decide the interval between each measurement.
 *                    t_sleep = sleep_cycle / (RTC_SLOW_CLK frequency).
 *                    The approximate frequency value of RTC_SLOW_CLK can be obtained using rtc_clk_slow_freq_get_hz function.
 * @param meas_times The times of charge and discharge in each measure process of touch channels.
 *                  The timer frequency is 8Mhz. Range: 0 ~ 0xffff.
 *                  Recommended typical value: Modify this value to make the measurement time around 1ms.
 * @return
 *     - ESP_OK Success
 */
esp_err_t touch_pad_sleep_channel_set_work_time(uint16_t sleep_cycle, uint16_t meas_times);

#ifdef __cplusplus
}
#endif
