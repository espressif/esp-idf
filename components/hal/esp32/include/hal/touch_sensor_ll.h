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
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The Lowlevel layer for Touch Sensor

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "soc/touch_sensor_periph.h"
#include "hal/touch_sensor_types.h"


#ifdef __cplusplus
extern "C" {
#endif

//Some register bits of touch sensor 8 and 9 are mismatched, we need to swap the bits.
#define TOUCH_LL_BIT_SWAP(data, n, m)   (((data >> n) &  0x1)  == ((data >> m) & 0x1) ? (data) : ((data) ^ ((0x1 <<n) | (0x1 << m))))
#define TOUCH_LL_BITS_SWAP(v)  TOUCH_LL_BIT_SWAP(v, TOUCH_PAD_NUM8, TOUCH_PAD_NUM9)

/**
 * Swap the number of touch8 and touch9.
 *
 * @touch_num Touch channel num.
 */
static inline touch_pad_t touch_ll_num_wrap(touch_pad_t touch_num)
{
    if (touch_num == TOUCH_PAD_NUM8) {
        return TOUCH_PAD_NUM9;
    } else if (touch_num == TOUCH_PAD_NUM9) {
        return TOUCH_PAD_NUM8;
    }
    return touch_num;
}

/**
 * Set touch sensor measurement time.
 *
 * @param meas_time The duration of the touch sensor measurement.
 *                  t_meas = meas_time / (8MHz), the maximum measure time is 0xffff / 8M = 8.19 ms.
 */
static inline void touch_ll_set_meas_time(uint16_t meas_time)
{
    //touch sensor measure time= meas_cycle / 8Mhz
    SENS.sar_touch_ctrl1.touch_meas_delay = meas_time;
    //the waiting cycles (in 8MHz) between TOUCH_START and TOUCH_XPD
    SENS.sar_touch_ctrl1.touch_xpd_wait = SOC_TOUCH_PAD_MEASURE_WAIT_MAX;
}

/**
 * Get touch sensor measurement time.
 *
 * @param meas_time Pointer to accept measurement cycle count.
 */
static inline void touch_ll_get_meas_time(uint16_t *meas_time)
{
    *meas_time = SENS.sar_touch_ctrl1.touch_meas_delay;
}

/**
 * Set touch sensor sleep time (interval of measurement).
 *
 * @param sleep_time  The touch sensor will sleep after each measurement.
 *                    sleep_cycle decide the interval between each measurement.
 *                    t_sleep = sleep_cycle / (RTC_SLOW_CLK frequency).
 *                    The approximate frequency value of RTC_SLOW_CLK can be obtained using `rtc_clk_slow_freq_get_hz` function.
 */
static inline void touch_ll_set_sleep_time(uint16_t sleep_time)
{
    //touch sensor sleep cycle Time = sleep_cycle / RTC_SLOW_CLK( can be 150k or 32k depending on the options)
    SENS.sar_touch_ctrl2.touch_sleep_cycles = sleep_time;
}

/**
 * Get touch sensor sleep time.
 *
 * @param sleep_time Pointer to accept sleep cycle count.
 */
static inline void touch_ll_get_sleep_time(uint16_t *sleep_time)
{
    *sleep_time = SENS.sar_touch_ctrl1.touch_meas_delay;
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
    RTCIO.touch_cfg.drefh = refh;
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
    *refh = (touch_high_volt_t)RTCIO.touch_cfg.drefh;
}

/**
 * Set touch sensor low voltage threshold of discharge.
 * The touch sensor measures the channel capacitance value by charging and discharging the channel.
 *
 * @param refl The low voltage threshold of discharge.
 */
static inline void touch_ll_set_voltage_low(touch_low_volt_t refl)
{
    RTCIO.touch_cfg.drefl = refl;
}

/**
 * Get touch sensor low voltage threshold of discharge.
 * The touch sensor measures the channel capacitance value by charging and discharging the channel.
 *
 * @param refl The low voltage threshold of discharge.
 */
static inline void touch_ll_get_voltage_low(touch_low_volt_t *refl)
{
    *refl = (touch_low_volt_t)RTCIO.touch_cfg.drefl;
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
    RTCIO.touch_cfg.drange = atten;
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
    *atten = (touch_volt_atten_t)RTCIO.touch_cfg.drange;
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
    touch_pad_t touch_pad_wrap = touch_ll_num_wrap(touch_num);
    RTCIO.touch_pad[touch_pad_wrap].tie_opt = opt;
}

/**
 * Get initial voltage state of touch channel for each measurement.
 *
 * @param touch_num Touch pad index.
 * @param opt Initial voltage state.
 */
static inline void touch_ll_get_tie_option(touch_pad_t touch_num, touch_tie_opt_t *opt)
{
    touch_pad_t touch_pad_wrap = touch_ll_num_wrap(touch_num);
    *opt = (touch_tie_opt_t)RTCIO.touch_pad[touch_pad_wrap].tie_opt;
}

/**
 * Set touch sensor FSM mode.
 *        The measurement action can be triggered by the hardware timer, as well as by the software instruction.
 *
 * @param mode FSM mode.
 */
static inline void touch_ll_set_fsm_mode(touch_fsm_mode_t mode)
{
    SENS.sar_touch_ctrl2.touch_start_en = 0;
    SENS.sar_touch_ctrl2.touch_start_force = mode;
}

/**
 * Get touch sensor FSM mode.
 *        The measurement action can be triggered by the hardware timer, as well as by the software instruction.
 *
 * @param mode FSM mode.
 */
static inline void touch_ll_get_fsm_mode(touch_fsm_mode_t *mode)
{
    *mode = (touch_fsm_mode_t)SENS.sar_touch_ctrl2.touch_start_force;
}

/**
 * Start touch sensor FSM timer.
 *        The measurement action can be triggered by the hardware timer, as well as by the software instruction.
 *
 * @param mode FSM mode.
 */
static inline void touch_ll_start_fsm(void)
{
    RTCCNTL.state0.touch_slp_timer_en = 1;
}

/**
 * Stop touch sensor FSM timer.
 *        The measurement action can be triggered by the hardware timer, as well as by the software instruction.
 *
 * @param mode FSM mode.
 */
static inline void touch_ll_stop_fsm(void)
{
    RTCCNTL.state0.touch_slp_timer_en = 0;
}

/**
 * Trigger a touch sensor measurement, only support in SW mode of FSM.
 */
static inline void touch_ll_start_sw_meas(void)
{
    SENS.sar_touch_ctrl2.touch_start_en = 0;
    SENS.sar_touch_ctrl2.touch_start_en = 1;
}

/**
 * Set touch sensor interrupt threshold.
 *
 * @note Refer to `touch_pad_set_trigger_mode` to see how to set trigger mode.
 * @param touch_num touch pad index.
 * @param threshold threshold of touchpad count.
 */
static inline void touch_ll_set_threshold(touch_pad_t touch_num, uint16_t threshold)
{
    touch_pad_t tp_wrap = touch_ll_num_wrap(touch_num);
    if (tp_wrap & 0x1) {
        SENS.touch_thresh[tp_wrap / 2].l_thresh = threshold;
    } else {
        SENS.touch_thresh[tp_wrap / 2].h_thresh = threshold;
    }
}

/**
 * Get touch sensor interrupt threshold.
 *
 * @param touch_num touch pad index.
 * @param threshold pointer to accept threshold.
 */
static inline void touch_ll_get_threshold(touch_pad_t touch_num, uint16_t *threshold)
{
    touch_pad_t tp_wrap = touch_ll_num_wrap(touch_num);
    if (threshold) {
        *threshold = (tp_wrap & 0x1 ) ?
                     SENS.touch_thresh[tp_wrap / 2].l_thresh :
                     SENS.touch_thresh[tp_wrap / 2].h_thresh;
    }
}

/**
 * Set touch sensor interrupt trigger mode.
 * Interrupt can be triggered either when touch value is less than
 * threshold or when touch value is more than threshold.
 *
 * @param mode Touch sensor interrupt trigger mode.
 */
static inline void touch_ll_set_trigger_mode(touch_trigger_mode_t mode)
{
    SENS.sar_touch_ctrl1.touch_out_sel = mode;
}

/**
 * Get touch sensor interrupt trigger mode.
 * Interrupt can be triggered either when touch value is less than
 * threshold or when touch value is more than threshold.
 *
 * @param mode Touch sensor interrupt trigger mode.
 */
static inline void touch_ll_get_trigger_mode(touch_trigger_mode_t *mode)
{
    *mode = (touch_trigger_mode_t)SENS.sar_touch_ctrl1.touch_out_sel;
}

/**
 * Set touch sensor interrupt trigger source. There are two sets of touch signals.
 * Set1 and set2 can be mapped to several touch signals. Either set will be triggered
 * if at least one of its touch signal is 'touched'. The interrupt can be configured to be generated
 * if set1 is triggered, or only if both sets are triggered.
 *
 * @param src Touch sensor interrupt trigger source.
 */
static inline void touch_ll_set_trigger_source(touch_trigger_src_t src)
{
    SENS.sar_touch_ctrl1.touch_out_1en = src;
}

/**
 * Get touch sensor interrupt trigger source.
 *
 * @param src Pointer to accept touch sensor interrupt trigger source.
 */
static inline void touch_ll_get_trigger_source(touch_trigger_src_t *src)
{
    *src = (touch_trigger_src_t)SENS.sar_touch_ctrl1.touch_out_1en;
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
    SENS.sar_touch_enable.touch_pad_worken |= TOUCH_LL_BITS_SWAP(enable_mask);
}

/**
 * Get touch sensor channel mask.
 *
 * @param enable_mask bitmask of touch sensor scan group.
 *        e.g. TOUCH_PAD_NUM1 -> BIT(1)
 */
static inline void touch_ll_get_channel_mask(uint16_t *enable_mask)
{
    *enable_mask = TOUCH_LL_BITS_SWAP(SENS.sar_touch_enable.touch_pad_worken);
}

/**
 * Disable touch sensor channel by bitmask.
 *
 * @param enable_mask bitmask of touch sensor scan group.
 *        e.g. TOUCH_PAD_NUM1 -> BIT(1)
 */
static inline void touch_ll_clear_channel_mask(uint16_t disable_mask)
{
    SENS.sar_touch_enable.touch_pad_worken &= TOUCH_LL_BITS_SWAP(~disable_mask);
}

/**
 * Set touch sensor group mask.
 * Touch pad module has two sets of signals, 'Touched' signal is triggered only if
 * at least one of touch pad in this group is "touched".
 * This function will set the register bits according to the given bitmask.
 *
 * @param set1_mask bitmask of touch sensor signal group1, it's a 10-bit value
 * @param set2_mask bitmask of touch sensor signal group2, it's a 10-bit value
 */
static inline void touch_ll_set_group_mask(uint16_t group1_mask, uint16_t group2_mask)
{
    SENS.sar_touch_enable.touch_pad_outen1 |= TOUCH_LL_BITS_SWAP(group1_mask);
    SENS.sar_touch_enable.touch_pad_outen2 |= TOUCH_LL_BITS_SWAP(group2_mask);
}

/**
 * Get touch sensor group mask.
 *
 * @param set1_mask pointer to accept bitmask of touch sensor signal group1, it's a 10-bit value
 * @param set2_mask pointer to accept bitmask of touch sensor signal group2, it's a 10-bit value
 */
static inline void touch_ll_get_group_mask(uint16_t *group1_mask, uint16_t *group2_mask)
{
    *group1_mask = TOUCH_LL_BITS_SWAP(SENS.sar_touch_enable.touch_pad_outen1);
    *group2_mask = TOUCH_LL_BITS_SWAP(SENS.sar_touch_enable.touch_pad_outen2);
}

/**
 * Clear touch sensor group mask.
 *
 * @param set1_mask pointer to accept bitmask of touch sensor signal group1, it's a 10-bit value
 * @param set2_mask pointer to accept bitmask of touch sensor signal group2, it's a 10-bit value
 */
static inline void touch_ll_clear_group_mask(uint16_t group1_mask, uint16_t group2_mask)
{
    SENS.sar_touch_enable.touch_pad_outen1 &= TOUCH_LL_BITS_SWAP(~group1_mask);
    SENS.sar_touch_enable.touch_pad_outen2 &= TOUCH_LL_BITS_SWAP(~group2_mask);
}

/**
 * Get the touch sensor status, usually used in ISR to decide which pads are 'touched'.
 *
 * @param status_mask The touch sensor status. e.g. Touch1 trigger status is `status_mask & (BIT1)`.
 */
static inline void touch_ll_read_trigger_status_mask(uint32_t *status_mask)
{
    *status_mask = TOUCH_LL_BITS_SWAP(SENS.sar_touch_ctrl2.touch_meas_en);
}

/**
 * Clear all touch sensor status.
 */
static inline void touch_ll_clear_trigger_status_mask(void)
{
    SENS.sar_touch_ctrl2.touch_meas_en_clr = 1;
}

/**
 * To enable touch pad interrupt.
 */
static inline void touch_ll_intr_enable(void)
{
    RTCCNTL.int_ena.rtc_touch = 1;
}

/**
 * To disable touch pad interrupt.
 */
static inline void touch_ll_intr_disable(void)
{
    RTCCNTL.int_ena.rtc_touch = 0;
}

/**
 * To clear touch pad interrupt.
 */
static inline void touch_ll_intr_clear(void)
{
    RTCCNTL.int_clr.rtc_touch = 1;
}

/**
 * Get touch sensor raw data (touch sensor counter value) from register. No block.
 *
 * @param touch_num touch pad index.
 * @return touch_value pointer to accept touch sensor value.
 */
static inline uint32_t touch_ll_read_raw_data(touch_pad_t touch_num)
{
    touch_pad_t tp_wrap = touch_ll_num_wrap(touch_num);
    return ((tp_wrap & 0x1) ? SENS.touch_meas[tp_wrap / 2].l_val : SENS.touch_meas[tp_wrap / 2].h_val);
}

/**
 * Get touch sensor measure status. No block.
 *
 * @return
 *      - If touch sensors measure done.
 */
static inline bool touch_ll_meas_is_done(void)
{
    return (bool)SENS.sar_touch_ctrl2.touch_meas_done;
}

#ifdef __cplusplus
}
#endif
