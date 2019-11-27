// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in soc/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for touch sensor (esp32s2beta specific part)

#pragma once

#include "hal/touch_sensor_ll.h"
#include "hal/touch_sensor_types.h"

/**
 * Reset the whole of touch module.
 *
 * @note Call this funtion after `touch_pad_fsm_stop`,
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
#define touch_hal_set_inactive_connect(type) touch_ll_set_inactive_connect(type)

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
#define touch_hal_get_inactive_connect(type) touch_ll_get_inactive_connect(type)

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
 * Get the bitmask of touch sensor interrupt status.
 *
 * @return type interrupt type
 */
#define touch_hal_read_intr_status_mask() touch_ll_read_intr_status_mask()

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
 * Get baseline value of touch sensor.
 *
 * @note After initialization, the baseline value is the maximum during the first measurement period.
 * @param touch_num touch pad index
 * @param touch_value pointer to accept touch sensor value
 */
#define touch_hal_filter_read_baseline(touch_num, basedata) touch_ll_filter_read_baseline(touch_num, basedata)

/**
 * Force reset baseline to raw data of touch sensor.
 *
 * @param touch_num touch pad index
 *                  - TOUCH_PAD_MAX Reset basaline of all channels.
 */
#define touch_hal_filter_reset_baseline(touch_num) touch_ll_filter_reset_baseline(touch_num)

/**
 * Set filter mode. The input to the filter is raw data and the output is the baseline value.
 * Larger filter coefficients increase the stability of the baseline.
 *
 * @param mode Filter mode type. Refer to `touch_filter_mode_t`.
 */
#define touch_hal_filter_set_filter_mode(mode) touch_ll_filter_set_filter_mode(mode)

/**
 * Get filter mode. The input to the filter is raw data and the output is the baseline value.
 *
 * @param mode Filter mode type. Refer to `touch_filter_mode_t`.
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
 * Set hysteresis threshold coefficient. hysteresis = hysteresis_thr * touch_threshold.
 * If (raw data - baseline) > (touch threshold + hysteresis), the touch channel be touched.
 * If (raw data - baseline) < (touch threshold - hysteresis), the touch channel be released.
 * Range: 0 ~ 3. The coefficient is 0: 1/8;  1: 3/32;  2: 1/16;  3: 1/32
 *
 * @param hys_thr hysteresis coefficient.
 */
#define touch_hal_filter_set_hysteresis(hys_thr) touch_ll_filter_set_hysteresis(hys_thr)

/**
 * Get hysteresis threshold coefficient. hysteresis = hysteresis_thr * touch_threshold.
 * If (raw data - baseline) > (touch threshold + hysteresis), the touch channel be touched.
 * If (raw data - baseline) < (touch threshold - hysteresis), the touch channel be released.
 * Range: 0 ~ 3. The coefficient is 0: 1/8;  1: 3/32;  2: 1/16;  3: 1/32
 *
 * @param hys_thr hysteresis coefficient.
 */
#define touch_hal_filter_get_hysteresis(hys_thr) touch_ll_filter_get_hysteresis(hys_thr)

/**
 * Set noise threshold coefficient. noise = noise_thr * touch threshold.
 * If (raw data - baseline) > (noise), the baseline stop updating.
 * If (raw data - baseline) < (noise), the baseline start updating.
 * Range: 0 ~ 3. The coefficient is 0: 1/2;  1: 3/8;   2: 1/4;   3: 1/8;
 *
 * @param hys_thr Noise threshold coefficient.
 */
#define touch_hal_filter_set_noise_thres(noise_thr) touch_ll_filter_set_noise_thres(noise_thr)

/**
 * Get noise threshold coefficient. noise = noise_thr * touch threshold.
 * If (raw data - baseline) > (noise), the baseline stop updating.
 * If (raw data - baseline) < (noise), the baseline start updating.
 * Range: 0 ~ 3. The coefficient is 0: 1/2;  1: 3/8;   2: 1/4;   3: 1/8;
 *
 * @param noise_thr Noise threshold coefficient.
 */
#define touch_hal_filter_get_noise_thres(noise_thr) touch_ll_filter_get_noise_thres(noise_thr)

/**
 * Set negative noise threshold coefficient. negative noise = noise_neg_thr * touch threshold.
 * If (baseline - raw data) > (negative noise), the baseline restart reset process(refer to `baseline_reset`).
 * If (baseline - raw data) < (negative noise), the baseline stop reset process(refer to `baseline_reset`).
 * Range: 0 ~ 3. The coefficient is 0: 1/2;  1: 3/8;   2: 1/4;   3: 1/8;
 *
 * @param noise_thr Negative threshold coefficient.
 */
#define touch_hal_filter_set_neg_noise_thres(noise_thr) touch_ll_filter_set_neg_noise_thres(noise_thr)

/**
 * Get negative noise threshold coefficient. negative noise = noise_neg_thr * touch threshold.
 * If (baseline - raw data) > (negative noise), the baseline restart reset process(refer to `baseline_reset`).
 * If (baseline - raw data) < (negative noise), the baseline stop reset process(refer to `baseline_reset`).
 * Range: 0 ~ 3. The coefficient is 0: 1/2;  1: 3/8;   2: 1/4;   3: 1/8;
 *
 * @param noise_thr Negative noise threshold coefficient.
 */
#define touch_hal_filter_get_neg_noise_thres(noise_thr) touch_ll_filter_get_neg_noise_thres(noise_thr)

/**
 * Set the cumulative number of baseline reset processes. such as `n`. If the measured values continue to exceed
 * the negative noise threshold for `n` times, the baseline reset to raw data.
 * Range: 0 ~ 15
 *
 * @param reset_cnt The cumulative number of baseline reset processes.
 */
#define touch_hal_filter_set_baseline_reset(reset_cnt) touch_ll_filter_set_baseline_reset(reset_cnt)

/**
 * Get the cumulative number of baseline reset processes. such as `n`. If the measured values continue to exceed
 * the negative noise threshold for `n` times, the baseline reset to raw data.
 * Range: 0 ~ 15
 *
 * @param reset_cnt The cumulative number of baseline reset processes.
 */
#define touch_hal_filter_get_baseline_reset(reset_cnt) touch_ll_filter_get_baseline_reset(reset_cnt)

/**
 * Set jitter filter step size.
 * If filter mode is jitter, should set filter step for jitter.
 * Range: 0 ~ 15
 *
 * @param step The step size of the data change when the baseline is updated.
 */
#define touch_hal_filter_set_jitter_step(step) touch_ll_filter_set_jitter_step(step)

/**
 * Get jitter filter step size.
 * If filter mode is jitter, should set filter step for jitter.
 * Range: 0 ~ 15
 *
 * @param step The step size of the data change when the baseline is updated.
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
 * Set max equivalent capacitance for sheild channel.
 * The equivalent capacitance of the shielded channel can be calculated
 * from the reading of denoise channel.
 *
 * @param pad_num Touch sensor channel number.
 */
#define touch_hal_waterproof_set_sheild_driver(driver_level) touch_ll_waterproof_set_sheild_driver(driver_level)

/**
 * Get max equivalent capacitance for sheild channel.
 * The equivalent capacitance of the shielded channel can be calculated
 * from the reading of denoise channel.
 *
 * @param pad_num Touch sensor channel number.
 */
#define touch_hal_waterproof_get_sheild_driver(driver_level) touch_ll_waterproof_get_sheild_driver(driver_level)

/**
 * Set parameter of waterproof function.
 *        The waterproof function includes a shielded channel (TOUCH_PAD_NUM14) and a guard channel.
 *        The shielded channel outputs the same signal as the channel being measured.
 *        It is generally designed as a grid and is placed around the touch buttons.
 *        The shielded channel does not follow the measurement signal of the protection channel.
 *        So that the guard channel can detect a large area of water.
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
 * The waterproof function includes a shielded channel (TOUCH_PAD_NUM14) and a guard channel.
 * The shielded channel outputs the same signal as the channel being measured.
 * It is generally designed as a grid and is placed around the touch buttons.
 * The shielded channel does not follow the measurement signal of the protection channel.
 * So that the guard channel can detect a large area of water.
 */
void touch_hal_waterproof_enable(void);

/**
 * Disable parameter of waterproof function.
 * The waterproof function includes a shielded channel (TOUCH_PAD_NUM14) and a guard channel.
 * The shielded channel outputs the same signal as the channel being measured.
 * It is generally designed as a grid and is placed around the touch buttons.
 * The shielded channel does not follow the measurement signal of the protection channel.
 * So that the guard channel can detect a large area of water.
 */
#define touch_hal_waterproof_disable() touch_ll_waterproof_disable()

/************************ Proximity register setting ************************/
/**
 * Set parameter of proximity channel. Three proximity sensing channels can be set.
 * The proximity sensor measurement is the accumulation of touch channel measurements.
 *
 * @note  If stop the proximity function for the channel, point this proximity channel to `TOUCH_PAD_NUM0`.
 * @param proximity parameter of proximity
 */
void touch_hal_proximity_set_config(const touch_pad_proximity_t *proximity);

/**
 * Get parameter of proximity channel. Three proximity sensing channels can be set.
 * The proximity sensor measurement is the accumulation of touch channel measurements.
 *
 * @param proximity parameter of proximity.
 */
void touch_hal_proximity_get_config(touch_pad_proximity_t *proximity);

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
 * Set parameter of touch sensor in sleep mode.
 *        In order to achieve low power consumption in sleep mode, other circuits except the RTC part of the register are in a power-off state.
 *        Only one touch channel is supported in the sleep state, which can be used as a wake-up function.
 *        If in non-sleep mode, the sleep parameters do not work.
 *
 * @param slp_config touch pad config.
 */
void touch_hal_sleep_channel_config(const touch_pad_sleep_channel_t *slp_config);

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
 * The threshold is the original value of the trigger state minus the baseline value.
 *
 * @note The threshold at sleep is the same as the threshold before sleep.
 */
#define touch_hal_sleep_set_threshold(touch_thres) touch_ll_sleep_set_threshold(touch_thres)

/**
 * Get the trigger threshold of touch sensor in deep sleep.
 * The threshold determines the sensitivity of the touch sensor.
 * The threshold is the original value of the trigger state minus the baseline value.
 *
 * @note The threshold at sleep is the same as the threshold before sleep.
 */
#define touch_hal_sleep_get_threshold(touch_thres) touch_ll_sleep_get_threshold(touch_thres)

/**
 * Enable proximity function for sleep pad.
 */
#define touch_hal_sleep_enable_approach() touch_ll_sleep_enable_approach()

/**
 * Disable proximity function for sleep pad.
 */
#define touch_hal_sleep_disable_approach() touch_ll_sleep_disable_approach()

/**
 * Read baseline of touch sensor for sleep pad.
 *
 * @param baseline Pointer to accept touch sensor baseline value.
 */
#define touch_hal_sleep_read_baseline(baseline) touch_ll_sleep_read_baseline(baseline)

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
