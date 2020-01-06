// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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
 * The ll is not public api, don't use in application code.
 * See readme.md in soc/include/hal/readme.md
 ******************************************************************************/

// The Lowlevel layer for Touch Sensor

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "soc/touch_sensor_periph.h"
#include "hal/touch_sensor_types.h"

/**
 * Set touch sensor touch sensor times of charge and discharge.
 *
 * @param meas_timers The times of charge and discharge in each measure process of touch channels.
 *                     The timer frequency is 8Mhz. Range: 0 ~ 0xffff.
 */
static inline void touch_ll_set_meas_times(uint16_t meas_time)
{
    //The times of charge and discharge in each measure process of touch channels.
    RTCCNTL.touch_ctrl1.touch_meas_num = meas_time;
    //the waiting cycles (in 8MHz) between TOUCH_START and TOUCH_XPD
    RTCCNTL.touch_ctrl2.touch_xpd_wait = SOC_TOUCH_PAD_MEASURE_WAIT; //wait volt stable
}

/**
 * Get touch sensor times of charge and discharge.
 *
 * @param meas_times Pointer to accept times count of charge and discharge.
 */
static inline void touch_ll_get_measure_times(uint16_t *meas_time)
{
    *meas_time = RTCCNTL.touch_ctrl1.touch_meas_num;
}

/**
 * Set touch sensor sleep time.
 *
 * @param sleep_cycle  The touch sensor will sleep after each measurement.
 *                     sleep_cycle decide the interval between each measurement.
 *                     t_sleep = sleep_cycle / (RTC_SLOW_CLK frequency).
 *                     The approximate frequency value of RTC_SLOW_CLK can be obtained using rtc_clk_slow_freq_get_hz function.
 */
static inline void touch_ll_set_sleep_time(uint16_t sleep_time)
{
    // touch sensor sleep cycle Time = sleep_cycle / RTC_SLOW_CLK(90k)
    RTCCNTL.touch_ctrl1.touch_sleep_cycles = sleep_time;
}

/**
 * Get touch sensor sleep time.
 *
 * @param sleep_cycle  Pointer to accept sleep cycle number.
 */
static inline void touch_ll_get_sleep_time(uint16_t *sleep_time)
{
    *sleep_time = RTCCNTL.touch_ctrl1.touch_sleep_cycles;
}

/**
 * Set touch sensor high voltage threshold of chanrge.
 * The touch sensor measures the channel capacitance value by charging and discharging the channel.
 * So the high threshold should be less than the supply voltage.
 *
 * @param refh The high voltage threshold of chanrge.
 */
static inline void touch_ll_set_voltage_high(touch_high_volt_t refh)
{
    RTCCNTL.touch_ctrl2.touch_drefh = refh;
}

/**
 * Get touch sensor high voltage threshold of chanrge.
 * The touch sensor measures the channel capacitance value by charging and discharging the channel.
 * So the high threshold should be less than the supply voltage.
 *
 * @param refh The high voltage threshold of chanrge.
 */
static inline void touch_ll_get_voltage_high(touch_high_volt_t *refh)
{
    *refh = (touch_high_volt_t)RTCCNTL.touch_ctrl2.touch_drefh;
}

/**
 * Set touch sensor low voltage threshold of discharge.
 * The touch sensor measures the channel capacitance value by charging and discharging the channel.
 *
 * @param refl The low voltage threshold of discharge.
 */
static inline void touch_ll_set_voltage_low(touch_low_volt_t refl)
{
    RTCCNTL.touch_ctrl2.touch_drefl = refl;
}

/**
 * Get touch sensor low voltage threshold of discharge.
 * The touch sensor measures the channel capacitance value by charging and discharging the channel.
 *
 * @param refl The low voltage threshold of discharge.
 */
static inline void touch_ll_get_voltage_low(touch_low_volt_t *refl)
{
    *refl = (touch_low_volt_t)RTCCNTL.touch_ctrl2.touch_drefl;
}

/**
 * Set touch sensor high voltage attenuation of chanrge. The actual charge threshold is high voltage threshold minus attenuation value.
 * The touch sensor measures the channel capacitance value by charging and discharging the channel.
 * So the high threshold should be less than the supply voltage.
 *
 * @param refh The high voltage threshold of chanrge.
 */
static inline void touch_ll_set_voltage_attenuation(touch_volt_atten_t atten)
{
    RTCCNTL.touch_ctrl2.touch_drange = atten;
}

/**
 * Get touch sensor high voltage attenuation of chanrge. The actual charge threshold is high voltage threshold minus attenuation value.
 * The touch sensor measures the channel capacitance value by charging and discharging the channel.
 * So the high threshold should be less than the supply voltage.
 *
 * @param refh The high voltage threshold of chanrge.
 */
static inline void touch_ll_get_voltage_attenuation(touch_volt_atten_t *atten)
{
    *atten = (touch_volt_atten_t)RTCCNTL.touch_ctrl2.touch_drange;
}

/**
 * Set touch sensor charge/discharge speed(currents) for each pad.
 *        If the slope is 0, the counter would always be zero.
 *        If the slope is 1, the charging and discharging would be slow. The measurement time becomes longer.
 *        If the slope is set 7, which is the maximum value, the charging and discharging would be fast.
 *        The measurement time becomes shorter.
 *
 * @note The higher the charge and discharge current, the greater the immunity of the touch channel,
 *       but it will increase the system power consumption.
 * @param touch_num Touch pad index.
 * @param slope touch pad charge/discharge speed(currents).
 */
static inline void touch_ll_set_slope(touch_pad_t touch_num, touch_cnt_slope_t slope)
{
    RTCIO.touch_pad[touch_num].dac = slope;
}

/**
 * Get touch sensor charge/discharge speed(currents) for each pad.
 *        If the slope is 0, the counter would always be zero.
 *        If the slope is 1, the charging and discharging would be slow. The measurement time becomes longer.
 *        If the slope is set 7, which is the maximum value, the charging and discharging would be fast.
 *        The measurement time becomes shorter.
 *
 * @param touch_num Touch pad index.
 * @param slope touch pad charge/discharge speed(currents).
 */
static inline void touch_ll_get_slope(touch_pad_t touch_num, touch_cnt_slope_t *slope)
{
    *slope = (touch_cnt_slope_t)RTCIO.touch_pad[touch_num].dac;
}

/**
 * Set initial voltage state of touch channel for each measurement.
 *
 * @param touch_num Touch pad index.
 * @param opt Initial voltage state.
 */
static inline void touch_ll_set_tie_option(touch_pad_t touch_num, touch_tie_opt_t opt)
{
    RTCIO.touch_pad[touch_num].tie_opt = opt;
}

/**
 * Get initial voltage state of touch channel for each measurement.
 *
 * @param touch_num Touch pad index.
 * @param opt Initial voltage state.
 */
static inline void touch_ll_get_tie_option(touch_pad_t touch_num, touch_tie_opt_t *opt)
{
    *opt = (touch_tie_opt_t)RTCIO.touch_pad[touch_num].tie_opt;
}

/**
 * Set touch sensor FSM mode.
 *        The measurement action can be triggered by the hardware timer, as well as by the software instruction.
 *
 * @param mode FSM mode.
 */
static inline void touch_ll_set_fsm_mode(touch_fsm_mode_t mode)
{
    RTCCNTL.touch_ctrl2.touch_start_force = mode;
}

/**
 * Get touch sensor FSM mode.
 *        The measurement action can be triggered by the hardware timer, as well as by the software instruction.
 *
 * @param mode FSM mode.
 */
static inline void touch_ll_get_fsm_mode(touch_fsm_mode_t *mode)
{
    *mode = (touch_fsm_mode_t)RTCCNTL.touch_ctrl2.touch_start_force;
}

/**
 * Start touch sensor FSM timer.
 *        The measurement action can be triggered by the hardware timer, as well as by the software instruction.
 *
 * @param mode FSM mode.
 */
static inline void touch_ll_start_fsm(void)
{
    RTCCNTL.touch_ctrl2.touch_clkgate_en = 1; //enable touch clock for FSM. or force enable.
    RTCCNTL.touch_ctrl2.touch_slp_timer_en = (RTCCNTL.touch_ctrl2.touch_start_force == TOUCH_FSM_MODE_TIMER ? 1 : 0);
}

/**
 * Stop touch sensor FSM timer.
 *        The measurement action can be triggered by the hardware timer, as well as by the software instruction.
 *
 * @param mode FSM mode.
 */
static inline void touch_ll_stop_fsm(void)
{
    RTCCNTL.touch_ctrl2.touch_start_en = 0; //stop touch fsm
    RTCCNTL.touch_ctrl2.touch_slp_timer_en = 0;
    RTCCNTL.touch_ctrl2.touch_clkgate_en = 0; //enable touch clock for FSM. or force enable.
}

/**
 * Trigger a touch sensor measurement, only support in SW mode of FSM.
 */
static inline void touch_ll_start_sw_meas(void)
{
    RTCCNTL.touch_ctrl2.touch_start_en = 0;
    RTCCNTL.touch_ctrl2.touch_start_en = 1;
}

/**
 * Set touch sensor interrupt threshold.
 *
 * @param touch_num touch pad index.
 * @param threshold threshold of touchpad count.
 */
/**
 * Set the trigger threshold of touch sensor.
 * The threshold determines the sensitivity of the touch sensor.
 * The threshold is the original value of the trigger state minus the baseline value.
 *
 * @note  If set "TOUCH_PAD_THRESHOLD_MAX", the touch is never be trigered.
 * @param touch_num touch pad index
 * @param threshold threshold of touch sensor.
 */
static inline void touch_ll_set_threshold(touch_pad_t touch_num, uint32_t threshold)
{
    SENS.touch_thresh[touch_num - 1].thresh = threshold;
}

/**
 * Get the trigger threshold of touch sensor.
 * The threshold determines the sensitivity of the touch sensor.
 * The threshold is the original value of the trigger state minus the baseline value.
 *
 * @param touch_num touch pad index.
 * @param threshold pointer to accept threshold.
 */
static inline void touch_ll_get_threshold(touch_pad_t touch_num, uint32_t *threshold)
{
    *threshold = SENS.touch_thresh[touch_num - 1].thresh;
}

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
static inline void touch_ll_set_channel_mask(uint16_t enable_mask)
{
    RTCCNTL.touch_scan_ctrl.touch_scan_pad_map  |= (enable_mask & SOC_TOUCH_SENSOR_BIT_MASK_MAX);
    SENS.sar_touch_conf.touch_outen |= (enable_mask & SOC_TOUCH_SENSOR_BIT_MASK_MAX);
}

/**
 * Get touch sensor channel mask.
 *
 * @param enable_mask bitmask of touch sensor scan group.
 *        e.g. TOUCH_PAD_NUM1 -> BIT(1)
 */
static inline void touch_ll_get_channel_mask(uint16_t *enable_mask)
{
    *enable_mask = SENS.sar_touch_conf.touch_outen \
                   & RTCCNTL.touch_scan_ctrl.touch_scan_pad_map \
                   & SOC_TOUCH_SENSOR_BIT_MASK_MAX;
}

/**
 * Disable touch sensor channel by bitmask.
 *
 * @param enable_mask bitmask of touch sensor scan group.
 *        e.g. TOUCH_PAD_NUM1 -> BIT(1)
 */
static inline void touch_ll_clear_channel_mask(uint16_t disable_mask)
{
    SENS.sar_touch_conf.touch_outen &= ~(disable_mask & SOC_TOUCH_SENSOR_BIT_MASK_MAX);
    RTCCNTL.touch_scan_ctrl.touch_scan_pad_map  &= ~(disable_mask & SOC_TOUCH_SENSOR_BIT_MASK_MAX);
}

/**
 * Get the touch sensor trigger status, usually used in ISR to decide which pads are 'touched'.
 *
 * @param status_mask The touch sensor status. e.g. Touch1 trigger status is `status_mask & (BIT1)`.
 */
static inline void touch_ll_read_trigger_status_mask(uint32_t *status_mask)
{
    *status_mask = SENS.sar_touch_chn_st.touch_pad_active;
}

/**
 * Clear all touch sensor status.
 *
 * @note Generally no manual removal is required.
 */
static inline void touch_ll_clear_trigger_status_mask(void)
{
    SENS.sar_touch_conf.touch_status_clr = 1;
}

/**
 * To enable touch pad interrupt.
 */
static inline void touch_ll_enable_interrupt(touch_pad_intr_mask_t int_mask)
{
    if (int_mask & TOUCH_PAD_INTR_MASK_DONE) {
        RTCCNTL.int_ena.rtc_touch_done = 1;
    }
    if (int_mask & TOUCH_PAD_INTR_MASK_ACTIVE) {
        RTCCNTL.int_ena.rtc_touch_active = 1;
    }
    if (int_mask & TOUCH_PAD_INTR_MASK_INACTIVE) {
        RTCCNTL.int_ena.rtc_touch_inactive = 1;
    }
}

/**
 * To disable touch pad interrupt.
 */
static inline void touch_ll_disable_interrupt(touch_pad_intr_mask_t int_mask)
{
    if (int_mask & TOUCH_PAD_INTR_MASK_DONE) {
        RTCCNTL.int_ena.rtc_touch_done = 0;
    }
    if (int_mask & TOUCH_PAD_INTR_MASK_ACTIVE) {
        RTCCNTL.int_ena.rtc_touch_active = 0;
    }
    if (int_mask & TOUCH_PAD_INTR_MASK_INACTIVE) {
        RTCCNTL.int_ena.rtc_touch_inactive = 0;
    }
}

/**
 * Get touch sensor raw data (touch sensor counter value) from register. No block.
 *
 * @param touch_num touch pad index.
 * @return touch_value pointer to accept touch sensor value.
 */
static inline uint32_t touch_ll_read_raw_data(touch_pad_t touch_num)
{
    return SENS.touch_meas[touch_num].meas_out;
}

/**
 * Get touch sensor measure status. No block.
 *
 * @return
 *      - If touch sensors measure done.
 */
static inline bool touch_ll_meas_is_done(void)
{
    return (bool)SENS.sar_touch_chn_st.touch_meas_done;
}

/************************* esp32s2beta only *************************/

/**
 * Reset the whole of touch module.
 *
 * @note Call this funtion after `touch_pad_fsm_stop`,
 */
static inline void touch_ll_reset(void)
{
    RTCCNTL.touch_ctrl2.touch_reset = 0;
    RTCCNTL.touch_ctrl2.touch_reset = 1;
}

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
static inline void touch_ll_set_inactive_connect(touch_pad_conn_type_t type)
{
    RTCCNTL.touch_scan_ctrl.touch_inactive_connection = type;
}

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
static inline void touch_ll_get_inactive_connect(touch_pad_conn_type_t *type)
{
    *type = RTCCNTL.touch_scan_ctrl.touch_inactive_connection;
}

/**
 * Get the current measure channel. Touch sensor measurement is cyclic scan mode.
 *
 * @return
 *      - touch channel number
 */
static inline touch_pad_t touch_ll_get_current_meas_channel(void)
{
    return (touch_pad_t)(SENS.sar_touch_status0.touch_scan_curr);
}

/**
 * Enable touch sensor interrupt by bitmask.
 *
 * @param type interrupt type
 */
static inline void touch_ll_intr_enable(touch_pad_intr_mask_t int_mask)
{
    if (int_mask & TOUCH_PAD_INTR_MASK_DONE) {
        RTCCNTL.int_ena.rtc_touch_done = 1;
    }
    if (int_mask & TOUCH_PAD_INTR_MASK_ACTIVE) {
        RTCCNTL.int_ena.rtc_touch_active = 1;
    }
    if (int_mask & TOUCH_PAD_INTR_MASK_INACTIVE) {
        RTCCNTL.int_ena.rtc_touch_inactive = 1;
    }
}

/**
 * Disable touch sensor interrupt by bitmask.
 *
 * @param type interrupt type
 */
static inline void touch_ll_intr_disable(touch_pad_intr_mask_t int_mask)
{
    if (int_mask & TOUCH_PAD_INTR_MASK_DONE) {
        RTCCNTL.int_ena.rtc_touch_done = 0;
    }
    if (int_mask & TOUCH_PAD_INTR_MASK_ACTIVE) {
        RTCCNTL.int_ena.rtc_touch_active = 0;
    }
    if (int_mask & TOUCH_PAD_INTR_MASK_INACTIVE) {
        RTCCNTL.int_ena.rtc_touch_inactive = 0;
    }
}

/**
 * Get the bitmask of touch sensor interrupt status.
 *
 * @return type interrupt type
 */
static inline uint32_t touch_ll_read_intr_status_mask(void)
{
    uint32_t intr_st = RTCCNTL.int_st.val;
    uint32_t intr_msk = 0;

    if (intr_st & RTC_CNTL_TOUCH_DONE_INT_ST_M) {
        intr_msk |= TOUCH_PAD_INTR_MASK_DONE;
    }
    if (intr_st & RTC_CNTL_TOUCH_ACTIVE_INT_ST_M) {
        intr_msk |= TOUCH_PAD_INTR_MASK_ACTIVE;
    }
    if (intr_st & RTC_CNTL_TOUCH_INACTIVE_INT_ST_M) {
        intr_msk |= TOUCH_PAD_INTR_MASK_INACTIVE;
    }

    return (intr_msk & TOUCH_PAD_INTR_MASK_ALL);
}

/************************ Filter register setting ************************/

/**
 * Get baseline value of touch sensor.
 *
 * @note After initialization, the baseline value is the maximum during the first measurement period.
 * @param touch_num touch pad index
 * @param touch_value pointer to accept touch sensor value
 */
static inline void touch_ll_filter_read_baseline(touch_pad_t touch_num, uint32_t *basedata)
{
    *basedata = SENS.sar_touch_status[touch_num - 1].touch_pad_baseline;
}

/**
 * Force reset baseline to raw data of touch sensor.
 *
 * @param touch_num touch pad index
 *                  - TOUCH_PAD_MAX Reset basaline of all channels.
 */
static inline void touch_ll_filter_reset_baseline(touch_pad_t touch_num)
{
    if (touch_num == TOUCH_PAD_MAX) {
        SENS.sar_touch_chn_st.touch_channel_clr = SOC_TOUCH_SENSOR_BIT_MASK_MAX;
    } else {
        SENS.sar_touch_chn_st.touch_channel_clr = (1U << touch_num);
    }
}

/**
 * Set filter mode. The input to the filter is raw data and the output is the baseline value.
 * Larger filter coefficients increase the stability of the baseline.
 *
 * @param mode Filter mode type. Refer to `touch_filter_mode_t`.
 */
static inline void touch_ll_filter_set_filter_mode(touch_filter_mode_t mode)
{
    RTCCNTL.touch_filter_ctrl.touch_filter_mode = mode;
}

/**
 * Get filter mode. The input to the filter is raw data and the output is the baseline value.
 *
 * @param mode Filter mode type. Refer to `touch_filter_mode_t`.
 */
static inline void touch_ll_filter_get_filter_mode(touch_filter_mode_t *mode)
{
    *mode = (touch_filter_mode_t)RTCCNTL.touch_filter_ctrl.touch_filter_mode;
}

/**
 * Set debounce count, such as `n`. If the measured values continue to exceed
 * the threshold for `n` times, it is determined that the touch sensor state changes.
 *
 * @param dbc_cnt Debounce count value.
 */
static inline void touch_ll_filter_set_debounce(uint32_t dbc_cnt)
{
    RTCCNTL.touch_filter_ctrl.touch_debounce = dbc_cnt;
}

/**
 * Get debounce count.
 *
 * @param dbc_cnt Debounce count value.
 */
static inline void touch_ll_filter_get_debounce(uint32_t *dbc_cnt)
{
    *dbc_cnt = RTCCNTL.touch_filter_ctrl.touch_debounce;
}

/**
 * Set hysteresis threshold coefficient. hysteresis = hysteresis_thr * touch_threshold.
 * If (raw data - baseline) > (touch threshold + hysteresis), the touch channel be touched.
 * If (raw data - baseline) < (touch threshold - hysteresis), the touch channel be released.
 * Range: 0 ~ 3. The coefficient is 0: 1/8;  1: 3/32;  2: 1/16;  3: 1/32
 *
 * @param hys_thr hysteresis coefficient.
 */
static inline void touch_ll_filter_set_hysteresis(uint32_t hys_thr)
{
    RTCCNTL.touch_filter_ctrl.touch_hysteresis = hys_thr;
}

/**
 * Get hysteresis threshold coefficient. hysteresis = hysteresis_thr * touch_threshold.
 * If (raw data - baseline) > (touch threshold + hysteresis), the touch channel be touched.
 * If (raw data - baseline) < (touch threshold - hysteresis), the touch channel be released.
 * Range: 0 ~ 3. The coefficient is 0: 1/8;  1: 3/32;  2: 1/16;  3: 1/32
 *
 * @param hys_thr hysteresis coefficient.
 */
static inline void touch_ll_filter_get_hysteresis(uint32_t *hys_thr)
{
    *hys_thr = RTCCNTL.touch_filter_ctrl.touch_hysteresis;
}

/**
 * Set noise threshold coefficient. noise = noise_thr * touch threshold.
 * If (raw data - baseline) > (noise), the baseline stop updating.
 * If (raw data - baseline) < (noise), the baseline start updating.
 * Range: 0 ~ 3. The coefficient is 0: 1/2;  1: 3/8;   2: 1/4;   3: 1/8;
 *
 * @param hys_thr Noise threshold coefficient.
 */
static inline void touch_ll_filter_set_noise_thres(uint32_t noise_thr)
{
    RTCCNTL.touch_filter_ctrl.touch_noise_thres = noise_thr;
}

/**
 * Get noise threshold coefficient. noise = noise_thr * touch threshold.
 * If (raw data - baseline) > (noise), the baseline stop updating.
 * If (raw data - baseline) < (noise), the baseline start updating.
 * Range: 0 ~ 3. The coefficient is 0: 1/2;  1: 3/8;   2: 1/4;   3: 1/8;
 *
 * @param noise_thr Noise threshold coefficient.
 */
static inline void touch_ll_filter_get_noise_thres(uint32_t *noise_thr)
{
    *noise_thr = RTCCNTL.touch_filter_ctrl.touch_noise_thres;
}

/**
 * Set negative noise threshold coefficient. negative noise = noise_neg_thr * touch threshold.
 * If (baseline - raw data) > (negative noise), the baseline restart reset process(refer to `baseline_reset`).
 * If (baseline - raw data) < (negative noise), the baseline stop reset process(refer to `baseline_reset`).
 * Range: 0 ~ 3. The coefficient is 0: 1/2;  1: 3/8;   2: 1/4;   3: 1/8;
 *
 * @param noise_thr Negative threshold coefficient.
 */
static inline void touch_ll_filter_set_neg_noise_thres(uint32_t noise_thr)
{
    RTCCNTL.touch_filter_ctrl.touch_neg_noise_thres = noise_thr;
}

/**
 * Get negative noise threshold coefficient. negative noise = noise_neg_thr * touch threshold.
 * If (baseline - raw data) > (negative noise), the baseline restart reset process(refer to `baseline_reset`).
 * If (baseline - raw data) < (negative noise), the baseline stop reset process(refer to `baseline_reset`).
 * Range: 0 ~ 3. The coefficient is 0: 1/2;  1: 3/8;   2: 1/4;   3: 1/8;
 *
 * @param noise_thr Negative noise threshold coefficient.
 */
static inline void touch_ll_filter_get_neg_noise_thres(uint32_t *noise_thr)
{
    *noise_thr = RTCCNTL.touch_filter_ctrl.touch_neg_noise_thres;
}

/**
 * Set the cumulative number of baseline reset processes. such as `n`. If the measured values continue to exceed
 * the negative noise threshold for `n` times, the baseline reset to raw data.
 * Range: 0 ~ 15
 *
 * @param reset_cnt The cumulative number of baseline reset processes.
 */
static inline void touch_ll_filter_set_baseline_reset(uint32_t reset_cnt)
{
    RTCCNTL.touch_filter_ctrl.touch_neg_noise_limit = reset_cnt;
}

/**
 * Get the cumulative number of baseline reset processes. such as `n`. If the measured values continue to exceed
 * the negative noise threshold for `n` times, the baseline reset to raw data.
 * Range: 0 ~ 15
 *
 * @param reset_cnt The cumulative number of baseline reset processes.
 */
static inline void touch_ll_filter_get_baseline_reset(uint32_t *reset_cnt)
{
    *reset_cnt = RTCCNTL.touch_filter_ctrl.touch_neg_noise_limit;
}

/**
 * Set jitter filter step size.
 * If filter mode is jitter, should set filter step for jitter.
 * Range: 0 ~ 15
 *
 * @param step The step size of the data change when the baseline is updated.
 */
static inline void touch_ll_filter_set_jitter_step(uint32_t step)
{
    RTCCNTL.touch_filter_ctrl.touch_jitter_step = step;
}

/**
 * Get jitter filter step size.
 * If filter mode is jitter, should set filter step for jitter.
 * Range: 0 ~ 15
 *
 * @param step The step size of the data change when the baseline is updated.
 */
static inline void touch_ll_filter_get_jitter_step(uint32_t *step)
{
    *step = RTCCNTL.touch_filter_ctrl.touch_jitter_step;
}

/**
 * Enable touch sensor filter and detection algorithm.
 * For more details on the detection algorithm, please refer to the application documentation.
 */
static inline void touch_ll_filter_enable(void)
{
    RTCCNTL.touch_filter_ctrl.touch_filter_en = 1;
}

/**
 * Disable touch sensor filter and detection algorithm.
 * For more details on the detection algorithm, please refer to the application documentation.
 */
static inline void touch_ll_filter_disable(void)
{
    RTCCNTL.touch_filter_ctrl.touch_filter_en = 0;
}

/************************ Denoise register setting ************************/

/**
 * Enable denoise function.
 * T0 is an internal channel that does not have a corresponding external GPIO.
 * T0 will work simultaneously with the measured channel Tn. Finally, the actual
 * measured value of Tn is the value after subtracting lower bits of T0.
 * This denoise function filters out interference introduced on all channels,
 * such as noise introduced by the power supply and external EMI.
 */
static inline void touch_ll_denoise_enable(void)
{
    RTCCNTL.touch_scan_ctrl.touch_denoise_en = 1;
}

/**
 * Enable denoise function.
 * T0 is an internal channel that does not have a corresponding external GPIO.
 * T0 will work simultaneously with the measured channel Tn. Finally, the actual
 * measured value of Tn is the value after subtracting lower bits of T0.
 * This denoise function filters out interference introduced on all channels,
 * such as noise introduced by the power supply and external EMI.
 */
static inline void touch_ll_denoise_disable(void)
{
    RTCCNTL.touch_scan_ctrl.touch_denoise_en = 0;
}

/**
 * Set internal reference capacitance of denoise channel.
 * Select the appropriate internal reference capacitance value so that
 * the reading of denoise channel is closest to the reading of the channel being measured.
 *
 * @param cap_level Capacitance level.
 */
static inline void touch_ll_denoise_set_cap_level(touch_pad_denoise_cap_t cap_level)
{
    RTCCNTL.touch_ctrl2.touch_refc = cap_level;
}

/**
 * Get internal reference capacitance of denoise channel.
 * Select the appropriate internal reference capacitance value so that
 * the reading of denoise channel is closest to the reading of the channel being measured.
 *
 * @param cap_level Capacitance level.
 */
static inline void touch_ll_denoise_get_cap_level(touch_pad_denoise_cap_t *cap_level)
{
    *cap_level = RTCCNTL.touch_ctrl2.touch_refc;
}

/**
 * Set denoise range of denoise channel.
 * Determined by measuring the noise amplitude of the denoise channel.
 *
 * @param grade Denoise range of denoise channel.
 */
static inline void touch_ll_denoise_set_grade(touch_pad_denoise_grade_t grade)
{
    RTCCNTL.touch_scan_ctrl.touch_denoise_res = grade;
}

/**
 * Set denoise range of denoise channel.
 * Determined by measuring the noise amplitude of the denoise channel.
 *
 * @param grade Denoise range of denoise channel.
 */
static inline void touch_ll_denoise_get_grade(touch_pad_denoise_grade_t *grade)
{
    *grade = RTCCNTL.touch_scan_ctrl.touch_denoise_res;
}

/**
 * Read denoise measure value (TOUCH_PAD_NUM0).
 *
 * @param denoise value of denoise.
 */
static inline uint32_t touch_ll_denoise_read_data(uint32_t *data)
{
    return (uint32_t)SENS.sar_touch_status0.touch_denoise_data;
}

/************************ Waterproof register setting ************************/

/**
 * Set touch channel use for guard pad.
 *
 * @param pad_num Touch sensor channel number.
 */
static inline void touch_ll_waterproof_set_guard_pad(touch_pad_t pad_num)
{
    RTCCNTL.touch_scan_ctrl.touch_out_ring = pad_num;
}

/**
 * Get touch channel use for guard pad.
 *
 * @param pad_num Touch sensor channel number.
 */
static inline void touch_ll_waterproof_get_guard_pad(touch_pad_t *pad_num)
{
    *pad_num = RTCCNTL.touch_scan_ctrl.touch_out_ring;
}

/**
 * Set max equivalent capacitance for sheild channel.
 * The equivalent capacitance of the shielded channel can be calculated
 * from the reading of denoise channel.
 *
 * @param pad_num Touch sensor channel number.
 */
static inline void touch_ll_waterproof_set_sheild_driver(touch_pad_shield_driver_t driver_level)
{
    RTCCNTL.touch_scan_ctrl.touch_bufdrv = driver_level;
}

/**
 * Get max equivalent capacitance for sheild channel.
 * The equivalent capacitance of the shielded channel can be calculated
 * from the reading of denoise channel.
 *
 * @param pad_num Touch sensor channel number.
 */
static inline void touch_ll_waterproof_get_sheild_driver(touch_pad_shield_driver_t *driver_level)
{
    *driver_level = RTCCNTL.touch_scan_ctrl.touch_bufdrv;
}

/**
 * Enable parameter of waterproof function.
 * The waterproof function includes a shielded channel (TOUCH_PAD_NUM14) and a guard channel.
 * The shielded channel outputs the same signal as the channel being measured.
 * It is generally designed as a grid and is placed around the touch buttons.
 * The shielded channel does not follow the measurement signal of the protection channel.
 * So that the guard channel can detect a large area of water.
 */
static inline void touch_ll_waterproof_enable(void)
{
    RTCCNTL.touch_scan_ctrl.touch_shield_pad_en = 1;
}

/**
 * Disable parameter of waterproof function.
 * The waterproof function includes a shielded channel (TOUCH_PAD_NUM14) and a guard channel.
 * The shielded channel outputs the same signal as the channel being measured.
 * It is generally designed as a grid and is placed around the touch buttons.
 * The shielded channel does not follow the measurement signal of the protection channel.
 * So that the guard channel can detect a large area of water.
 */
static inline void touch_ll_waterproof_disable(void)
{
    RTCCNTL.touch_scan_ctrl.touch_shield_pad_en = 0;
}

/************************ Proximity register setting ************************/

/**
 * Set touch channel number for proximity pad.
 * If disable the proximity pad, point this pad to `TOUCH_PAD_NUM0`
 *
 * @param prox_pad The array of three proximity pads.
 */
static inline void touch_ll_proximity_set_channel_num(const touch_pad_t prox_pad[])
{
    SENS.sar_touch_conf.touch_approach_pad0 = prox_pad[0];
    SENS.sar_touch_conf.touch_approach_pad1 = prox_pad[1];
    SENS.sar_touch_conf.touch_approach_pad2 = prox_pad[2];
}

/**
 * Get touch channel number for proximity pad.
 * If disable the proximity pad, point this pad to `TOUCH_PAD_NUM0`
 *
 * @param prox_pad The array of three proximity pads.
 */
static inline void touch_ll_proximity_get_channel_num(touch_pad_t prox_pad[])
{
    prox_pad[0] = SENS.sar_touch_conf.touch_approach_pad0;
    prox_pad[1] = SENS.sar_touch_conf.touch_approach_pad1;
    prox_pad[2] = SENS.sar_touch_conf.touch_approach_pad2;
}

/**
 * Set cumulative measurement times for proximity pad.
 *
 * @param times The cumulative number of measurement cycles.
 */
static inline void touch_ll_proximity_set_meas_times(uint32_t times)
{
    RTCCNTL.touch_approach.touch_approach_meas_time = times;
}

/**
 * Get cumulative measurement times for proximity pad.
 *
 * @param times The cumulative number of measurement cycles.
 */
static inline void touch_ll_proximity_get_meas_times(uint32_t *times)
{
    *times = RTCCNTL.touch_approach.touch_approach_meas_time;
}

/**
 * Read current cumulative measurement times for proximity pad.
 *
 * @param times The cumulative number of measurement cycles.
 */
static inline void touch_ll_proximity_read_meas_cnt(touch_pad_t touch_num, uint32_t *cnt)
{
    if (SENS.sar_touch_conf.touch_approach_pad0 == touch_num) {
        *cnt = SENS.sar_touch_appr_status.touch_approach_pad0_cnt;
    } else if (SENS.sar_touch_conf.touch_approach_pad1 == touch_num) {
        *cnt = SENS.sar_touch_appr_status.touch_approach_pad1_cnt;
    } else if (SENS.sar_touch_conf.touch_approach_pad2 == touch_num) {
        *cnt = SENS.sar_touch_appr_status.touch_approach_pad2_cnt;
    }
}

/**
 * Check if the touch sensor channel is the proximity pad.
 *
 * @param touch_num The touch sensor channel number.
 */
static inline bool touch_ll_proximity_pad_check(touch_pad_t touch_num)
{
    if ((SENS.sar_touch_conf.touch_approach_pad0 != touch_num)
            && (SENS.sar_touch_conf.touch_approach_pad1 != touch_num)
            && (SENS.sar_touch_conf.touch_approach_pad2 != touch_num)) {
        return false;
    } else {
        return true;
    }
}

/************** sleep pad setting ***********************/

/**
 * Set touch channel number for sleep pad.
 *
 * @note Only one touch sensor channel is supported in deep sleep mode.
 * @param touch_num Touch sensor channel number.
 */
static inline void touch_ll_sleep_set_channel_num(touch_pad_t touch_num)
{
    RTCCNTL.touch_slp_thres.touch_slp_pad = touch_num;
}

/**
 * Get touch channel number for sleep pad.
 *
 * @note Only one touch sensor channel is supported in deep sleep mode.
 * @param touch_num Touch sensor channel number.
 */
static inline void touch_ll_sleep_get_channel_num(touch_pad_t *touch_num)
{
    *touch_num = RTCCNTL.touch_slp_thres.touch_slp_pad;
}

/**
 * Set the trigger threshold of touch sensor in deep sleep.
 * The threshold determines the sensitivity of the touch sensor.
 * The threshold is the original value of the trigger state minus the baseline value.
 *
 * @note The threshold at sleep is the same as the threshold before sleep.
 */
static inline void touch_ll_sleep_set_threshold(uint32_t touch_thres)
{
    RTCCNTL.touch_slp_thres.touch_slp_th = touch_thres;
}

/**
 * Get the trigger threshold of touch sensor in deep sleep.
 * The threshold determines the sensitivity of the touch sensor.
 * The threshold is the original value of the trigger state minus the baseline value.
 *
 * @note The threshold at sleep is the same as the threshold before sleep.
 */
static inline void touch_ll_sleep_get_threshold(uint32_t *touch_thres)
{
    *touch_thres = RTCCNTL.touch_slp_thres.touch_slp_th;
}

/**
 * Enable proximity function for sleep pad.
 */
static inline void touch_ll_sleep_enable_approach(void)
{
    RTCCNTL.touch_slp_thres.touch_slp_approach_en = 1;
}

/**
 * Disable proximity function for sleep pad.
 */
static inline void touch_ll_sleep_disable_approach(void)
{
    RTCCNTL.touch_slp_thres.touch_slp_approach_en = 0;
}

/**
 * Read baseline of touch sensor for sleep pad.
 *
 * @param baseline Pointer to accept touch sensor baseline value.
 */
static inline void touch_ll_sleep_read_baseline(uint32_t *baseline)
{
    *baseline = REG_GET_FIELD(SENS_SAR_TOUCH_SLP_STATUS_REG, SENS_TOUCH_SLP_BASELINE);
}

/**
 * Read debounce of touch sensor for sleep pad.
 *
 * @param debounce Pointer to accept touch sensor debounce value.
 */
static inline void touch_ll_sleep_read_debounce(uint32_t *debounce)
{
    *debounce = REG_GET_FIELD(SENS_SAR_TOUCH_SLP_STATUS_REG, SENS_TOUCH_SLP_DEBOUNCE);
}

/**
 * Read proximity count of touch sensor for sleep pad.
 * @param proximity_cnt Pointer to accept touch sensor proximity count value.
 */
static inline void touch_ll_sleep_read_proximity_cnt(uint32_t *approach_cnt)
{
    *approach_cnt = REG_GET_FIELD(SENS_SAR_TOUCH_APPR_STATUS_REG, SENS_TOUCH_SLP_APPROACH_CNT);
}

/**
 * Get the touch pad which caused wakeup from deep sleep.
 *
 * @param pad_num pointer to touch pad which caused wakeup.
 */
static inline void touch_ll_get_wakeup_status(touch_pad_t *pad_num)
{
    *pad_num = (touch_pad_t)RTCCNTL.touch_slp_thres.touch_slp_pad;
}