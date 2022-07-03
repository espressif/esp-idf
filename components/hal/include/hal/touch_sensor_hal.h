/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for touch sensor (common part)

#pragma once

#include "hal/touch_sensor_ll.h"
#include "hal/touch_sensor_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    touch_high_volt_t refh;
    touch_low_volt_t refl;
    touch_volt_atten_t atten;
} touch_hal_volt_t;

typedef struct {
    touch_cnt_slope_t slope;    /*!<Set touch sensor charge/discharge speed(currents) for each pad.*/
    touch_tie_opt_t tie_opt;    /*!<Set initial voltage state of touch channel for each measurement.*/
} touch_hal_meas_mode_t;

/**
 * Set touch sensor sleep time (interval of measurement).
 *
 * @param sleep_time  The touch sensor will sleep after each measurement.
 *                    sleep_cycle decide the interval between each measurement.
 *                    t_sleep = sleep_cycle / (RTC_SLOW_CLK frequency).
 *                    The approximate frequency value of RTC_SLOW_CLK can be obtained using `rtc_clk_slow_freq_get_hz` function.
 */
#define touch_hal_set_sleep_time(sleep_time) touch_ll_set_sleep_time(sleep_time)

/**
 * Get touch sensor sleep time.
 *
 * @param sleep_time Pointer to accept sleep cycle count.
 */
#define touch_hal_get_sleep_time(sleep_time) touch_ll_get_sleep_time(sleep_time)

/**
 * Set touch sensor high / low voltage threshold of chanrge.
 * The touch sensor measures the channel capacitance value by charging and discharging the channel.
 * So charge threshold should be less than the supply voltage.
 * The actual charge threshold is high voltage threshold minus attenuation value.
 *
 * @param refh The high voltage threshold of chanrge.
 */
void touch_hal_set_voltage(const touch_hal_volt_t *volt);

/**
 * Get touch sensor high / low voltage threshold of chanrge.
 * The touch sensor measures the channel capacitance value by charging and discharging the channel.
 * So charge threshold should be less than the supply voltage.
 * The actual charge threshold is high voltage threshold minus attenuation value.
 *
 * @param refh The voltage threshold of chanrge / discharge.
 */
void touch_hal_get_voltage(touch_hal_volt_t *volt);

/**
 * Set touch sensor charge/discharge speed(currents) and initial voltage state for each pad measurement.
 *
 * @param touch_num Touch pad index.
 * @param meas Touch pad measurement config.
 */
void touch_hal_set_meas_mode(touch_pad_t touch_num, const touch_hal_meas_mode_t *meas);

/**
 * Get touch sensor charge/discharge speed(currents) and initial voltage state for each pad measurement.
 *
 * @param touch_num Touch pad index.
 * @param meas Touch pad measurement config.
 */
void touch_hal_get_meas_mode(touch_pad_t touch_num, touch_hal_meas_mode_t *meas);

/**
 * Set touch sensor FSM mode.
 *        The measurement action can be triggered by the hardware timer, as well as by the software instruction.
 *
 * @param mode FSM mode.
 */
#define touch_hal_set_fsm_mode(mode) touch_ll_set_fsm_mode(mode)

/**
 * Get touch sensor FSM mode.
 *        The measurement action can be triggered by the hardware timer, as well as by the software instruction.
 *
 * @param mode FSM mode.
 */
#define touch_hal_get_fsm_mode(mode) touch_ll_get_fsm_mode(mode)

/**
 * Start touch sensor FSM timer.
 *        The measurement action can be triggered by the hardware timer, as well as by the software instruction.
 */
#define touch_hal_start_fsm() touch_ll_start_fsm()

/**
 * Stop touch sensor FSM timer.
 *        The measurement action can be triggered by the hardware timer, as well as by the software instruction.
 */
#define touch_hal_stop_fsm() touch_ll_stop_fsm()

/**
 * Trigger a touch sensor measurement, only support in SW mode of FSM.
 */
#define touch_hal_start_sw_meas() touch_ll_start_sw_meas()

/**
 * Set touch sensor interrupt threshold.
 *
 * @note Refer to `touch_pad_set_trigger_mode` to see how to set trigger mode.
 * @param touch_num touch pad index.
 * @param threshold threshold of touchpad count.
 */
#define touch_hal_set_threshold(touch_num, threshold) touch_ll_set_threshold(touch_num, threshold)

/**
 * Get touch sensor interrupt threshold.
 *
 * @param touch_num touch pad index.
 * @param threshold pointer to accept threshold.
 */
#define touch_hal_get_threshold(touch_num, threshold) touch_ll_get_threshold(touch_num, threshold)

/**
 * Enable touch sensor channel. Register touch channel into touch sensor measurement group.
 * The working mode of the touch sensor is simultaneous measurement.
 * This function will set the measure bits according to the given bitmask.
 *
 * @note  If set this mask, the FSM timer should be stop firsty.
 * @note  The touch sensor that in scan map, should be deinit GPIO function firstly.
 * @param enable_mask bitmask of touch sensor scan group.
 *        e.g. TOUCH_PAD_NUM1 -> BIT(1)
 * @return
 *      - ESP_OK on success
 */
#define touch_hal_set_channel_mask(enable_mask) touch_ll_set_channel_mask(enable_mask)

/**
 * Get touch sensor channel mask.
 *
 * @param enable_mask bitmask of touch sensor scan group.
 *        e.g. TOUCH_PAD_NUM1 -> BIT(1)
 */
#define touch_hal_get_channel_mask(enable_mask) touch_ll_get_channel_mask(enable_mask)

/**
 * Disable touch sensor channel by bitmask.
 *
 * @param enable_mask bitmask of touch sensor scan group.
 *        e.g. TOUCH_PAD_NUM1 -> BIT(1)
 */
#define touch_hal_clear_channel_mask(disable_mask) touch_ll_clear_channel_mask(disable_mask)

/**
 * Get the touch sensor status, usually used in ISR to decide which pads are 'touched'.
 *
 * @param status_mask The touch sensor status. e.g. Touch1 trigger status is `status_mask & (BIT1)`.
 */
#define touch_hal_read_trigger_status_mask(status_mask) touch_ll_read_trigger_status_mask(status_mask)

/**
 * Clear all touch sensor status.
 */
#define touch_hal_clear_trigger_status_mask() touch_ll_clear_trigger_status_mask()

/**
 * Get touch sensor raw data (touch sensor counter value) from register. No block.
 *
 * @param touch_num touch pad index.
 * @return touch_value pointer to accept touch sensor value.
 */
#define touch_hal_read_raw_data(touch_num) touch_ll_read_raw_data(touch_num)

/**
 * Get touch sensor measure status. No block.
 *
 * @return
 *      - If touch sensors measure done.
 */
#define touch_hal_meas_is_done() touch_ll_meas_is_done()

/**
 * Initialize touch module.
 *
 * @note  If default parameter don't match the usage scenario, it can be changed after this function.
 */
void touch_hal_init(void);

/**
 * Un-install touch pad driver.
 *
 * @note  After this function is called, other touch functions are prohibited from being called.
 */
void touch_hal_deinit(void);

/**
 * Configure touch sensor for each channel.
 */
void touch_hal_config(touch_pad_t touch_num);

#ifdef __cplusplus
}
#endif
