/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for touch sensor (ESP32-S3 specific part)

#pragma once

#include "hal/touch_sensor_ll.h"
#include "hal/touch_sensor_types.h"

#include_next "hal/touch_sensor_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Reset the whole of touch module.
 *
 * @note Call this function after `touch_pad_fsm_stop`,
 */
#define touch_hal_reset() touch_ll_reset()

/**
 * Set touch sensor measurement time.
 *
 * @param meas_time The duration of the touch sensor measurement.
 *                  t_meas = meas_time / (8MHz), the maximum measure time is 0xffff / 8M = 8.19 ms.
 */
#define touch_hal_set_meas_times(meas_time) touch_ll_set_meas_times(meas_time)

/**
 * Get touch sensor times of charge and discharge.
 *
 * @param meas_times Pointer to accept times count of charge and discharge.
 */
#define touch_hal_get_measure_times(meas_time) touch_ll_get_measure_times(meas_time)

/**
 * Set connection type of touch channel in idle status.
 *        When a channel is in measurement mode, other initialized channels are in idle mode.
 *        The touch channel is generally adjacent to the trace, so the connection state of the idle channel
 *        affects the stability and sensitivity of the test channel.
 *        The `CONN_HIGHZ`(high resistance) setting increases the sensitivity of touch channels.
 *        The `CONN_GND`(grounding) setting increases the stability of touch channels.
 *
 * @param type  Select idle channel connect to high resistance state or ground.
 */
#define touch_hal_set_idle_channel_connect(type) touch_ll_set_idle_channel_connect(type)

/**
 * Set connection type of touch channel in idle status.
 *        When a channel is in measurement mode, other initialized channels are in idle mode.
 *        The touch channel is generally adjacent to the trace, so the connection state of the idle channel
 *        affects the stability and sensitivity of the test channel.
 *        The `CONN_HIGHZ`(high resistance) setting increases the sensitivity of touch channels.
 *        The `CONN_GND`(grounding) setting increases the stability of touch channels.
 *
 * @param type  Select idle channel connect to high resistance state or ground.
 */
#define touch_hal_get_idle_channel_connect(type) touch_ll_get_idle_channel_connect(type)

/**
 * Get the current measure channel. Touch sensor measurement is cyclic scan mode.
 *
 * @return
 *      - touch channel number
 */
#define touch_hal_get_current_meas_channel() touch_ll_get_current_meas_channel()

/**
 * Enable touch sensor interrupt by bitmask.
 *
 * @param type interrupt type
 */
#define touch_hal_intr_enable(int_mask) touch_ll_intr_enable(int_mask)

/**
 * Disable touch sensor interrupt by bitmask.
 *
 * @param type interrupt type
 */
#define touch_hal_intr_disable(int_mask) touch_ll_intr_disable(int_mask)

/**
 * Clear touch sensor interrupt by bitmask.
 *
 * @param int_mask Pad mask to clear interrupts
 */
#define touch_hal_intr_clear(int_mask) touch_ll_intr_clear(int_mask)

/**
 * Get the bitmask of touch sensor interrupt status.
 *
 * @return type interrupt type
 */
#define touch_hal_read_intr_status_mask() touch_ll_read_intr_status_mask()

/**
 * Enable the timeout check for all touch sensor channels measurements.
 * When the touch reading of a touch channel exceeds the measurement threshold,
 * If enable: a timeout interrupt will be generated and it will go to the next channel measurement.
 * If disable: the FSM is always on the channel, until the measurement of this channel is over.
 *
 * @note Set the timeout threshold correctly before enabling it.
 */
#define touch_hal_timeout_enable() touch_ll_timeout_enable()

/**
 * Disable the timeout check for all touch sensor channels measurements.
 * When the touch reading of a touch channel exceeds the measurement threshold,
 * If enable: a timeout interrupt will be generated and it will go to the next channel measurement.
 * If disable: the FSM is always on the channel, until the measurement of this channel is over.
 *
 * @note Set the timeout threshold correctly before enabling it.
 */
#define touch_hal_timeout_disable() touch_ll_timeout_disable()

/**
 * Set timeout threshold for all touch sensor channels measurements.
 * Compared with touch readings.
 *
 * @param threshold Set to the maximum time measured on one channel.
 */
#define touch_hal_timeout_set_threshold(threshold) touch_ll_timeout_set_threshold(threshold)

/**
 * Get timeout threshold for all touch sensor channels measurements.
 * Compared with touch readings.
 *
 * @param threshold Point to timeout threshold.
 */
#define touch_hal_timeout_get_threshold(threshold) touch_ll_timeout_get_threshold(threshold)

/**
 * Touch timer trigger measurement and always wait measurement done.
 * Force done for touch timer ensures that the timer always can get the measurement done signal.
 */
#define touch_hal_timer_force_done() touch_ll_timer_force_done()

/************************ Filter register setting ************************/

/**
 * Set parameter of touch sensor filter and detection algorithm.
 * For more details on the detection algorithm, please refer to the application documentation.
 *
 * @param filter_info select filter type and threshold of detection algorithm
 */
void touch_hal_filter_set_config(const touch_filter_config_t *filter_info);

/**
 * Get parameter of touch sensor filter and detection algorithm.
 * For more details on the detection algorithm, please refer to the application documentation.
 *
 * @param filter_info select filter type and threshold of detection algorithm
 */
void touch_hal_filter_get_config(touch_filter_config_t *filter_info);

/**
 * Get smoothed data that obtained by filtering the raw data.
 *
 * @param touch_num touch pad index
 * @param smooth_data pointer to smoothed data
 */
#define touch_hal_filter_read_smooth(touch_num, smooth_data) touch_ll_filter_read_smooth(touch_num, smooth_data)

/**
 * Get benchmark value of touch sensor.
 *
 * @note After initialization, the benchmark value is the maximum during the first measurement period.
 * @param touch_num touch pad index
 * @param touch_value pointer to accept touch sensor value
 */
#define touch_hal_read_benchmark(touch_num, benchmark) touch_ll_read_benchmark(touch_num, benchmark)

/**
 * Force reset benchmark to raw data of touch sensor.
 *
 * @param touch_num touch pad index
 *                  - TOUCH_PAD_MAX Reset basaline of all channels.
 */
#define touch_hal_reset_benchmark(touch_num) touch_ll_reset_benchmark(touch_num)

/**
 * Set filter mode. The input of the filter is the raw value of touch reading,
 * and the output of the filter is involved in the judgment of the touch state.
 *
 * @param mode Filter mode type. Refer to ``touch_filter_mode_t``.
 */
#define touch_hal_filter_set_filter_mode(mode) touch_ll_filter_set_filter_mode(mode)

/**
 * Get filter mode. The input of the filter is the raw value of touch reading,
 * and the output of the filter is involved in the judgment of the touch state.
 *
 * @param mode Filter mode type. Refer to ``touch_filter_mode_t``.
 */
#define touch_hal_filter_get_filter_mode(mode) touch_ll_filter_get_filter_mode(mode)

/**
 * Set debounce count, such as `n`. If the measured values continue to exceed
 * the threshold for `n` times, it is determined that the touch sensor state changes.
 *
 * @param dbc_cnt Debounce count value.
 */
#define touch_hal_filter_set_debounce(dbc_cnt) touch_ll_filter_set_debounce(dbc_cnt)

/**
 * Get debounce count.
 *
 * @param dbc_cnt Debounce count value.
 */
#define touch_hal_filter_get_debounce(dbc_cnt) touch_ll_filter_get_debounce(dbc_cnt)

/**
 * Set noise threshold coefficient. Higher = More noise resistance.
 * The actual noise should be less than (noise coefficient * touch threshold).
 * Range: 0 ~ 3. The coefficient is 0: 4/8;  1: 3/8;   2: 2/8;   3: 1;
 *
 * @param hys_thr Noise threshold coefficient.
 */
#define touch_hal_filter_set_noise_thres(noise_thr) touch_ll_filter_set_noise_thres(noise_thr)

/**
 * Get noise threshold coefficient. Higher = More noise resistance.
 * The actual noise should be less than (noise coefficient * touch threshold).
 * Range: 0 ~ 3. The coefficient is 0: 4/8;  1: 3/8;   2: 2/8;   3: 1;
 *
 * @param noise_thr Noise threshold coefficient.
 */
#define touch_hal_filter_get_noise_thres(noise_thr) touch_ll_filter_get_noise_thres(noise_thr)

/**
 * Set the cumulative number of benchmark reset processes. such as `n`. If the measured values continue to exceed
 * the negative noise threshold for `n` times, the benchmark reset to raw data.
 * Range: 0 ~ 15
 *
 * @param reset_cnt The cumulative number of benchmark reset processes.
 */
#define touch_hal_filter_set_benchmark_reset(reset_cnt) touch_ll_filter_set_benchmark_reset(reset_cnt)

/**
 * Get the cumulative number of benchmark reset processes. such as `n`. If the measured values continue to exceed
 * the negative noise threshold for `n` times, the benchmark reset to raw data.
 * Range: 0 ~ 15
 *
 * @param reset_cnt The cumulative number of benchmark reset processes.
 */
#define touch_hal_filter_get_benchmark_reset(reset_cnt) touch_ll_filter_get_benchmark_reset(reset_cnt)

/**
 * Set jitter filter step size.
 * If filter mode is jitter, should set filter step for jitter.
 * Range: 0 ~ 15
 *
 * @param step The step size of the data change.
 */
#define touch_hal_filter_set_jitter_step(step) touch_ll_filter_set_jitter_step(step)

/**
 * Get jitter filter step size.
 * If filter mode is jitter, should set filter step for jitter.
 * Range: 0 ~ 15
 *
 * @param step The step size of the data change.
 */
#define touch_hal_filter_get_jitter_step(step) touch_ll_filter_get_jitter_step(step)

/**
 * Enable touch sensor filter and detection algorithm.
 * For more details on the detection algorithm, please refer to the application documentation.
 */
#define touch_hal_filter_enable() touch_ll_filter_enable()

/**
 * Disable touch sensor filter and detection algorithm.
 * For more details on the detection algorithm, please refer to the application documentation.
 */
#define touch_hal_filter_disable() touch_ll_filter_disable()

/************************ Denoise register setting ************************/

/**
 * set parameter of denoise pad (TOUCH_PAD_NUM0).
 *        T0 is an internal channel that does not have a corresponding external GPIO.
 *        T0 will work simultaneously with the measured channel Tn. Finally, the actual
 *        measured value of Tn is the value after subtracting lower bits of T0.
 *        This denoise function filters out interference introduced on all channels,
 *        such as noise introduced by the power supply and external EMI.
 *
 * @param denoise parameter of denoise
 */
void touch_hal_denoise_set_config(const touch_pad_denoise_t *denoise);

/**
 * @brief get parameter of denoise pad (TOUCH_PAD_NUM0).
 *
 * @param denoise Pointer to parameter of denoise
 */
void touch_hal_denoise_get_config(touch_pad_denoise_t *denoise);

/**
 * Enable denoise function.
 * T0 is an internal channel that does not have a corresponding external GPIO.
 * T0 will work simultaneously with the measured channel Tn. Finally, the actual
 * measured value of Tn is the value after subtracting lower bits of T0.
 * This denoise function filters out interference introduced on all channels,
 * such as noise introduced by the power supply and external EMI.
 */
void touch_hal_denoise_enable(void);

/**
 * Enable denoise function.
 * T0 is an internal channel that does not have a corresponding external GPIO.
 * T0 will work simultaneously with the measured channel Tn. Finally, the actual
 * measured value of Tn is the value after subtracting lower bits of T0.
 * This denoise function filters out interference introduced on all channels,
 * such as noise introduced by the power supply and external EMI.
 */
#define touch_hal_denoise_disable() touch_ll_denoise_disable()

/**
 * Set internal reference capacitance of denoise channel.
 * Select the appropriate internal reference capacitance value so that
 * the reading of denoise channel is closest to the reading of the channel being measured.
 *
 * @param cap_level Capacitance level.
 */
#define touch_hal_denoise_set_cap_level(cap_level) touch_ll_denoise_set_cap_level(cap_level)

/**
 * Get internal reference capacitance of denoise channel.
 * Select the appropriate internal reference capacitance value so that
 * the reading of denoise channel is closest to the reading of the channel being measured.
 *
 * @param cap_level Capacitance level.
 */
#define touch_hal_denoise_get_cap_level(cap_level) touch_ll_denoise_get_cap_level(cap_level)

/**
 * Set denoise range of denoise channel.
 * Determined by measuring the noise amplitude of the denoise channel.
 *
 * @param grade Denoise range of denoise channel.
 */
#define touch_hal_denoise_set_grade(grade) touch_ll_denoise_set_grade(grade)

/**
 * Set denoise range of denoise channel.
 * Determined by measuring the noise amplitude of the denoise channel.
 *
 * @param grade Denoise range of denoise channel.
 */
#define touch_hal_denoise_get_grade(grade) touch_ll_denoise_get_grade(grade)

/**
 * Read denoise measure value (TOUCH_PAD_NUM0).
 *
 * @param denoise value of denoise.
 */
#define touch_hal_denoise_read_data(data) touch_ll_denoise_read_data(data)

/************************ Waterproof register setting ************************/
/**
 * Set touch channel use for guard pad.
 *
 * @param pad_num Touch sensor channel number.
 */
#define touch_hal_waterproof_set_guard_pad(pad_num) touch_ll_waterproof_set_guard_pad(pad_num)

/**
 * Get touch channel use for guard pad.
 *
 * @param pad_num Touch sensor channel number.
 */
#define touch_hal_waterproof_get_guard_pad(pad_num) touch_ll_waterproof_get_guard_pad(pad_num)

/**
 * Set max equivalent capacitance for shield channel.
 * The equivalent capacitance of the shielded channel can be calculated
 * from the reading of denoise channel.
 *
 * @param pad_num Touch sensor channel number.
 */
#define touch_hal_waterproof_set_sheild_driver(driver_level) touch_ll_waterproof_set_sheild_driver(driver_level)

/**
 * Get max equivalent capacitance for shield channel.
 * The equivalent capacitance of the shielded channel can be calculated
 * from the reading of denoise channel.
 *
 * @param pad_num Touch sensor channel number.
 */
#define touch_hal_waterproof_get_sheild_driver(driver_level) touch_ll_waterproof_get_sheild_driver(driver_level)

/**
 * Set parameter of waterproof function.
 *
 * The waterproof function includes a shielded channel (TOUCH_PAD_NUM14) and a guard channel.
 * Guard pad is used to detect the large area of water covering the touch panel.
 * Shield pad is used to shield the influence of water droplets covering the touch panel.
 * It is generally designed as a grid and is placed around the touch buttons.
 *
 * @param waterproof parameter of waterproof
 */
void touch_hal_waterproof_set_config(const touch_pad_waterproof_t *waterproof);

/**
 * Get parameter of waterproof function.
 *
 * @param waterproof parameter of waterproof.
 */
void touch_hal_waterproof_get_config(touch_pad_waterproof_t *waterproof);

/**
 * Enable parameter of waterproof function.
 * Should be called after function ``touch_hal_waterproof_set_config``.
 */
void touch_hal_waterproof_enable(void);

/**
 * Disable parameter of waterproof function.
 */
#define touch_hal_waterproof_disable() touch_ll_waterproof_disable()

/************************ Proximity register setting ************************/

/**
 * Enable/disable proximity function of touch channels.
 * The proximity sensor measurement is the accumulation of touch channel measurements.
 *
 * @note Supports up to three touch channels configured as proximity sensors.
 * @param touch_num touch pad index
 * @param enabled true: enable the proximity function; false:  disable the proximity function
 * @return
 *     - true: Configured correctly.
 *     - false: Configured error.
 */
bool touch_hal_enable_proximity(touch_pad_t touch_num, bool enabled);

/**
 * Set touch channel number for proximity pad.
 * If disable the proximity pad, point this pad to `TOUCH_PAD_NUM0`
 *
 * @param prox_pad The array of three proximity pads.
 */
#define touch_hal_proximity_set_channel_num(prox_pad) touch_ll_proximity_set_channel_num(prox_pad)

/**
 * Get touch channel number for proximity pad.
 * If disable the proximity pad, point this pad to `TOUCH_PAD_NUM0`
 *
 * @param prox_pad The array of three proximity pads.
 */
#define touch_hal_proximity_get_channel_num(prox_pad) touch_ll_proximity_get_channel_num(prox_pad)

/**
 * Set cumulative measurement times for proximity pad.
 *
 * @param times The cumulative number of measurement cycles.
 */
#define touch_hal_proximity_set_meas_times(times) touch_ll_proximity_set_meas_times(times)

/**
 * Get cumulative measurement times for proximity pad.
 *
 * @param times The cumulative number of measurement cycles.
 */
#define touch_hal_proximity_get_meas_times(times) touch_ll_proximity_get_meas_times(times)

/**
 * Read current cumulative measurement times for proximity pad.
 *
 * @param times The cumulative number of measurement cycles.
 */
#define touch_hal_proximity_read_meas_cnt(touch_num, cnt) touch_ll_proximity_read_meas_cnt(touch_num, cnt)

/**
 * Check if the touch sensor channel is the proximity pad.
 *
 * @param touch_num The touch sensor channel number.
 */
#define touch_hal_proximity_pad_check(touch_num) touch_ll_proximity_pad_check(touch_num)

/************** sleep pad setting ***********************/

/**
 * Get parameter of touch sensor sleep channel.
 * The touch sensor can works in sleep mode to wake up sleep.
 * After the sleep channel is configured, users should query the channel reading using a specific function.
 *
 * @param slp_config Point to touch sleep pad config.
 */
void touch_hal_sleep_channel_get_config(touch_pad_sleep_channel_t *slp_config);

/**
 * Set parameter of touch sensor sleep channel.
 * The touch sensor can works in sleep mode to wake up sleep.
 * After the sleep channel is configured, users should query the channel reading using a specific function.
 *
 * @note ESP32S2 only support one channel to be set sleep channel.
 *
 * @param pad_num touch sleep pad number.
 * @param enable Enable/disable sleep pad function.
 */
void touch_hal_sleep_channel_enable(touch_pad_t pad_num, bool enable);

/**
 * Set touch channel number for sleep pad.
 *
 * @note Only one touch sensor channel is supported in deep sleep mode.
 * @param touch_num Touch sensor channel number.
 */
#define touch_hal_sleep_set_channel_num(touch_num) touch_ll_sleep_set_channel_num(touch_num)

/**
 * Get touch channel number for sleep pad.
 *
 * @note Only one touch sensor channel is supported in deep sleep mode.
 * @param touch_num Touch sensor channel number.
 */
#define touch_hal_sleep_get_channel_num(touch_num) touch_ll_sleep_get_channel_num(touch_num)

/**
 * Set the trigger threshold of touch sensor in deep sleep.
 * The threshold determines the sensitivity of the touch sensor.
 * The threshold is the original value of the trigger state minus the benchmark value.
 *
 * @note The threshold at sleep is the same as the threshold before sleep.
 */
#define touch_hal_sleep_set_threshold(touch_thres) touch_ll_sleep_set_threshold(touch_thres)

/**
 * Get the trigger threshold of touch sensor in deep sleep.
 * The threshold determines the sensitivity of the touch sensor.
 * The threshold is the original value of the trigger state minus the benchmark value.
 *
 * @note The threshold at sleep is the same as the threshold before sleep.
 */
#define touch_hal_sleep_get_threshold(touch_thres) touch_ll_sleep_get_threshold(touch_thres)

/**
 * Enable proximity function for sleep pad.
 */
#define touch_hal_sleep_enable_approach() touch_ll_sleep_enable_proximity_sensing()

/**
 * Disable proximity function for sleep pad.
 */
#define touch_hal_sleep_disable_approach() touch_ll_sleep_disable_proximity_sensing()

/**
 * Read benchmark of touch sensor for sleep pad.
 *
 * @param benchmark Pointer to accept touch sensor benchmark value.
 */
#define touch_hal_sleep_read_benchmark(benchmark) touch_ll_sleep_read_benchmark(benchmark)

/**
 * Read smooth data of touch sensor for sleep pad.
 */
#define touch_hal_sleep_read_smooth(smooth_data) touch_ll_sleep_read_smooth(smooth_data)

/**
 * Read raw data of touch sensor for sleep pad.
 */
#define touch_hal_sleep_read_data(raw_data) touch_ll_sleep_read_data(raw_data)

/**
 * Reset benchmark of touch sensor for sleep pad.
 */
#define touch_hal_sleep_reset_benchmark() touch_ll_sleep_reset_benchmark()

/**
 * Read debounce of touch sensor for sleep pad.
 *
 * @param debounce Pointer to accept touch sensor debounce value.
 */
#define touch_hal_sleep_read_debounce(debounce) touch_ll_sleep_read_debounce(debounce)

/**
 * Read proximity count of touch sensor for sleep pad.
 * @param proximity_cnt Pointer to accept touch sensor proximity count value.
 */
#define touch_hal_sleep_read_proximity_cnt(approach_cnt) touch_ll_sleep_read_proximity_cnt(approach_cnt)

/**
 * Get the touch pad which caused wakeup from deep sleep.
 *
 * @param pad_num pointer to touch pad which caused wakeup.
 */
#define touch_hal_get_wakeup_status(pad_num) touch_ll_get_wakeup_status(pad_num)

/**
 * Change the operating frequency of touch pad in deep sleep state. Reducing the operating frequency can effectively reduce power consumption.
 * If this function is not called, the working frequency of touch in the deep sleep state is the same as that in the wake-up state.
 *
 * @param sleep_cycle The touch sensor will sleep after each measurement.
 *                    sleep_cycle decide the interval between each measurement.
 *                    t_sleep = sleep_cycle / (RTC_SLOW_CLK frequency).
 *                    The approximate frequency value of RTC_SLOW_CLK can be obtained using rtc_clk_slow_freq_get_hz function.
 * @param meas_times The times of charge and discharge in each measure process of touch channels.
 *                  The timer frequency is 8Mhz. Range: 0 ~ 0xffff.
 *                  Recommended typical value: Modify this value to make the measurement time around 1ms.
 */
void touch_hal_sleep_channel_set_work_time(uint16_t sleep_cycle, uint16_t meas_times);

/**
 * Get the operating frequency of touch pad in deep sleep state. Reducing the operating frequency can effectively reduce power consumption.
 *
 * @param sleep_cycle The touch sensor will sleep after each measurement.
 *                    sleep_cycle decide the interval between each measurement.
 *                    t_sleep = sleep_cycle / (RTC_SLOW_CLK frequency).
 *                    The approximate frequency value of RTC_SLOW_CLK can be obtained using rtc_clk_slow_freq_get_hz function.
 * @param meas_times The times of charge and discharge in each measure process of touch channels.
 *                  The timer frequency is 8Mhz. Range: 0 ~ 0xffff.
 *                  Recommended typical value: Modify this value to make the measurement time around 1ms.
 */
void touch_hal_sleep_channel_get_work_time(uint16_t *sleep_cycle, uint16_t *meas_times);

#ifdef __cplusplus
}
#endif
