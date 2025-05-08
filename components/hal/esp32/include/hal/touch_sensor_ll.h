/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The ll is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The Lowlevel layer for Touch Sensor

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "hal/misc.h"
#include "hal/assert.h"
#include "soc/touch_sensor_periph.h"
#include "soc/sens_struct.h"
#include "soc/rtc_io_struct.h"
#include "soc/rtc_cntl_struct.h"
#include "soc/soc_caps.h"
#include "hal/touch_sens_types.h"

#ifdef __cplusplus
extern "C" {
#endif

//Some register bits of touch sensor 8 and 9 are mismatched, we need to swap the bits.
#define TOUCH_LL_BIT_SWAP(data, n, m)   (((data >> n) &  0x1)  == ((data >> m) & 0x1) ? (data) : ((data) ^ ((0x1 <<n) | (0x1 << m))))
#define TOUCH_LL_BITS_SWAP(v)  TOUCH_LL_BIT_SWAP(v, 8, 9)
#define TOUCH_LL_CHAN_SWAP(chan)  ((chan) == 8 ? 9 : ((chan) == 9 ? 8 : (chan)))
#define TOUCH_LL_FULL_CHANNEL_MASK          ((uint16_t)((1U << SOC_TOUCH_SENSOR_NUM) - 1))
#define TOUCH_LL_READ_RAW                   0x0

#define TOUCH_LL_CHARGE_DURATION_MAX       (0xFFFF)
#define TOUCH_LL_PAD_MEASURE_WAIT_MAX      (0xFF)       // The timer frequency is 8Mhz, the max value is 0xff
#define TOUCH_LL_ACTIVE_THRESH_MAX         (0xFFFF)     // Max channel active threshold

#define TOUCH_LL_INTR_MASK_TRIGGER      (1UL << 6)     // Interrupt when the touch interrupt is triggered (depends on intr trigger mode)
#define TOUCH_LL_INTR_MASK_ALL          (TOUCH_LL_INTR_MASK_TRIGGER)

/*********************************** Interrupts *******************************/
/**
 * Enable touch sensor interrupt by bitmask.
 *
 * @param int_mask interrupt mask
 */
static inline void touch_ll_interrupt_enable(uint32_t int_mask)
{
    (void) int_mask;
    // Only one interrupt, set directly
    RTCCNTL.int_ena.rtc_touch = 1;
}

/**
 * Disable touch sensor interrupt by bitmask.
 *
 * @param int_mask interrupt mask
 */
static inline void touch_ll_interrupt_disable(uint32_t int_mask)
{
    (void) int_mask;
    // Only one interrupt, clear directly
    RTCCNTL.int_ena.rtc_touch = 0;
}

/**
 * Clear touch sensor interrupt by bitmask.
 *
 * @param int_mask Pad mask to clear interrupts
 */
__attribute__((always_inline))
static inline void touch_ll_interrupt_clear(uint32_t int_mask)
{
    (void) int_mask;
    RTCCNTL.int_clr.rtc_touch = 1;
}

/**
 * Get the bitmask of touch sensor interrupt status.
 *
 * @return type interrupt type
 */
__attribute__((always_inline))
static inline uint32_t touch_ll_get_intr_status_mask(void)
{
    // intr status bit of touch (bit7) is not same as the enable bit (bit6)
    return RTCCNTL.int_st.rtc_touch ? TOUCH_LL_INTR_MASK_TRIGGER : 0;
}

/**
 * Set touch sensor interrupt trigger mode.
 * Interrupt can be triggered either when touch value is below the threshold (touched) or
 * or when touch value exceed the threshold (release).
 *
 * @param mode Touch sensor interrupt trigger mode.
 *              - TOUCH_INTR_TRIG_ON_BELOW_THRESH: interrupt triggers when touched (touch value is below the threshold)
 *              - TOUCH_INTR_TRIG_ON_ABOVE_THRESH: interrupt triggers when released (touch value is exceed the threshold)
 */
static inline void touch_ll_set_intr_trigger_mode(touch_intr_trig_mode_t mode)
{
    SENS.sar_touch_ctrl1.touch_out_sel = mode;
}

/**
 * Set touch sensor interrupt trigger source. There are two sets of touch signals.
 * Set1 and set2 can be mapped to several touch signals. Either set will be triggered
 * if at least one of its touch signal is 'touched'. The interrupt can be configured to be generated
 * if set1 is triggered, or only if both sets are triggered.
 *
 * @param group Touch sensor interrupt trigger group.
 */
static inline void touch_ll_set_intr_trigger_group(touch_intr_trig_group_t group)
{
    SENS.sar_touch_ctrl1.touch_out_1en = group;
}

/**
 * Set touch sensor group mask.
 * Touch pad module has two sets of signals, 'Touched' signal is triggered only if
 * at least one of touch pad in this group is "touched".
 * This function will set the register bits according to the given bitmask.
 *
 * @param group1_mask bitmask of touch sensor signal group1, it's a 10-bit value
 * @param group2_mask bitmask of touch sensor signal group2, it's a 10-bit value
 */
static inline void touch_ll_config_trigger_group1(uint32_t touch_num, bool enable)
{
    touch_num = TOUCH_LL_CHAN_SWAP(touch_num);
    uint32_t mask = SENS.sar_touch_enable.touch_pad_outen1;
    if (enable) {
        mask |= (1 << touch_num);
    } else {
        mask &= ~(1 << touch_num);
    }
    SENS.sar_touch_enable.touch_pad_outen1 = mask;
}

static inline void touch_ll_config_trigger_group2(uint32_t touch_num, bool enable)
{
    touch_num = TOUCH_LL_CHAN_SWAP(touch_num);
    uint32_t mask = SENS.sar_touch_enable.touch_pad_outen2;
    if (enable) {
        mask |= (1 << touch_num);
    } else {
        mask &= ~(1 << touch_num);
    }
    SENS.sar_touch_enable.touch_pad_outen2 = mask;
}

static inline void touch_ll_reset_trigger_groups(void)
{
    SENS.sar_touch_enable.touch_pad_outen1 = 0;
    SENS.sar_touch_enable.touch_pad_outen2 = 0;
}

/********************************* Status Info ********************************/
/**
 * Get touch sensor measure status. No block.
 *
 * @return
 *      - If touch sensors measure done.
 */
__attribute__((always_inline))
static inline bool touch_ll_is_measure_done(void)
{
    return (bool)SENS.sar_touch_ctrl2.touch_meas_done;
}

/**
 * Get the touch sensor active channel mask, usually used in ISR to decide which channels are 'touched'.
 *
 * @param active_mask The touch channel status. e.g. Touch1 trigger status is `status_mask & (BIT1)`.
 */
__attribute__((always_inline))
static inline void touch_ll_get_active_channel_mask(uint32_t *active_mask)
{
    *active_mask = TOUCH_LL_BITS_SWAP(SENS.sar_touch_ctrl2.touch_meas_en);
}

/**************************** Measurement Configuration ***********************/

/**
 * Set touch sensor threshold of charge cycles that triggers pad active state.
 * The threshold determines the sensitivity of the touch sensor.
 * The threshold is the original value of the trigger state minus the benchmark value.
 *
 * @note  If set "TOUCH_PAD_THRESHOLD_MAX", the touch is never be triggered.
 * @param touch_num The touch pad id
 * @param thresh    The threshold of charge cycles
 */
static inline void touch_ll_set_chan_active_threshold(uint32_t touch_num, uint32_t thresh)
{
    // Workaround: swap chan 8 and chan 9
    touch_num = TOUCH_LL_CHAN_SWAP(touch_num);
    if (touch_num & 0x1) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.touch_thresh[touch_num >> 1], l_thresh, thresh);
    } else {
        HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.touch_thresh[touch_num >> 1], h_thresh, thresh);
    }
}

/**
 * Get touch sensor threshold of charge cycles that triggers pad active state.
 * The threshold determines the sensitivity of the touch sensor.
 * The threshold is the original value of the trigger state minus the benchmark value.
 *
 * @note  If set "TOUCH_PAD_THRESHOLD_MAX", the touch is never be triggered.
 * @param touch_num The touch pad id
 * @return
 *      - The threshold of charge cycles
 */
static inline uint32_t touch_ll_get_chan_active_threshold(uint32_t touch_num)
{
    // Workaround: swap chan 8 and chan 9
    touch_num = TOUCH_LL_CHAN_SWAP(touch_num);
    if (touch_num & 0x1) {
        return HAL_FORCE_READ_U32_REG_FIELD(SENS.touch_thresh[touch_num >> 1], l_thresh);
    } else {
        return HAL_FORCE_READ_U32_REG_FIELD(SENS.touch_thresh[touch_num >> 1], h_thresh);
    }
}

/**
 * Set the power on wait cycle
 *
 * @param wait_cycles
 */
static inline void touch_ll_set_power_on_wait_cycle(uint32_t wait_cycles)
{
    //the waiting cycles (in 8MHz) between TOUCH_START and TOUCH_XPD
    HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.sar_touch_ctrl1, touch_xpd_wait, wait_cycles);  //wait volt stable
}

/**
 * Set the duration of the charge window
 *
 * @param duration The duration of the touch channel charge / discharge window.
 *                 duration = meas_ticks / (8MHz), the maximum duration is 0xffff / 8M = 8.19 ms.
 */
static inline void touch_ll_set_charge_window_duration(uint32_t duration)
{
    //touch sensor charge/discharge window duration= meas_cycle / 8Mhz
    HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.sar_touch_ctrl1, touch_meas_delay, duration);
}

/**
 * Set touch sensor sleep time.
 *
 * @param interval_ticks    The touch sensor will sleep for some cycles after each measurement.
 *                          interval_ticks decide the interval between each measurement.
 *                          t_sleep = interval_ticks / (RTC_SLOW_CLK frequency).
 *                          The approximate frequency value of RTC_SLOW_CLK can be obtained using rtc_clk_slow_freq_get_hz function.
 */
static inline void touch_ll_set_measure_interval_ticks(uint16_t interval_ticks)
{
    //touch sensor sleep cycle Time = sleep_cycle / RTC_SLOW_CLK
    HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.sar_touch_ctrl2, touch_sleep_cycles, interval_ticks);
}

/**
 * Set the Touch pad charge speed.
 *
 * @param touch_num     Touch channel number
 * @param charge_speed  Charge speed of this touch channel
 */
static inline void touch_ll_set_charge_speed(uint32_t touch_num, touch_charge_speed_t charge_speed)
{
    RTCIO.touch_pad[touch_num].slope = charge_speed;
}

/**
 * Set the upper limitation of the touch channel voltage while charging
 *
 * @param high_lim      The high(upper) limitation of charge
 */
static inline void touch_ll_set_charge_voltage_high_limit(touch_volt_lim_h_t high_lim)
{
    RTCIO.touch_cfg.drefh = (uint32_t)high_lim & 0x3;
    RTCIO.touch_cfg.drange = (uint32_t)high_lim >> 2;
}

/**
 * Set the lower limitation of the touch channel voltage while discharging
 *
 * @param low_lim      The lower limitation of discharge
 */
static inline void touch_ll_set_charge_voltage_low_limit(touch_volt_lim_l_t low_lim)
{
    RTCIO.touch_cfg.drefl = low_lim;
}

/**
 * Set the initial charge voltage of touch channel
 * i.e., the touch pad measurement start from a low voltage or a high voltage
 *
 * @param touch_num         Touch channel number
 * @param init_charge_volt  The initial charge voltage
 */
static inline void touch_ll_set_init_charge_voltage(uint32_t touch_num, touch_init_charge_volt_t init_charge_volt)
{
    // Workaround: swap chan 8 and chan 9
    touch_num = TOUCH_LL_CHAN_SWAP(touch_num);
    if (init_charge_volt == TOUCH_INIT_CHARGE_VOLT_FLOAT) {
        RTCIO.touch_pad[touch_num].xpd = 0;
    } else {
        RTCIO.touch_pad[touch_num].xpd = 1;
        RTCIO.touch_pad[touch_num].tie_opt = init_charge_volt;
    }
}

/**
 * Enable touch sensor channel. Register touch channel into touch sensor measurement group.
 * The working mode of the touch sensor is simultaneous measurement.
 * This function will set the measure bits according to the given bitmask.
 *
 * @note  If set this mask, the FSM timer should be stop firstly.
 * @note  The touch sensor that in scan map, should be deinit GPIO function firstly.
 * @param enable_mask bitmask of touch sensor scan group.
 *        e.g. TOUCH_PAD_NUM1 -> BIT(1)
 * @return
 *      - ESP_OK on success
 */
static inline void touch_ll_enable_channel_mask(uint16_t enable_mask)
{
    SENS.sar_touch_enable.touch_pad_worken = enable_mask;
}

/**
 * Clear all touch sensor channels active status.
 *
 * @note Generally no manual removal is required.
 */
static inline void touch_ll_clear_active_channel_status(void)
{
    SENS.sar_touch_ctrl2.touch_meas_en_clr = 1;
}

/********************************* FSM Operation ******************************/
/**
 * Enable touch sensor FSM timer trigger (continuous) mode or software trigger (oneshot) mode.
 *
 * @param enable Enable FSM timer mode.
 *        True: the FSM will trigger scanning repeatedly under the control of the hardware timer (continuous mode)
 *        False: the FSM will trigger scanning once under the control of the software (continuous mode)
 */
__attribute__((always_inline))
static inline void touch_ll_enable_fsm_timer(bool enable)
{
    SENS.sar_touch_ctrl2.touch_start_force = !enable;
}

/**
 * Start touch sensor FSM timer to run FSM repeatedly
 * The measurement action can be triggered by the hardware timer, as well as by the software instruction.
 * @note
 *      The timer should be triggered
 */
__attribute__((always_inline))
static inline void touch_ll_start_fsm_repeated_timer(void)
{
    RTCCNTL.state0.touch_slp_timer_en = 1;
}

/**
 * Stop touch sensor FSM timer.
 *        The measurement action can be triggered by the hardware timer, as well as by the software instruction.
 */
__attribute__((always_inline))
static inline void touch_ll_stop_fsm_repeated_timer(void)
{
    RTCCNTL.state0.touch_slp_timer_en = 0;
}

/**
 * Is the FSM repeated timer enabled.
 * @note when the timer is enabled, RTC clock should not be power down
 *
 * @return
 *      - true: enabled
 *      - false: disabled
 */
__attribute__((always_inline))
static inline bool touch_ll_is_fsm_repeated_timer_enabled(void)
{
    return (bool)RTCCNTL.state0.touch_slp_timer_en;
}


/**
 * Enable the touch sensor FSM start signal from software
 */
__attribute__((always_inline))
static inline void touch_ll_trigger_oneshot_measurement(void)
{
    SENS.sar_touch_ctrl2.touch_start_en = 1;
    SENS.sar_touch_ctrl2.touch_start_en = 0;
}

/**
 * @brief Power on the channel by mask
 *
 * @param chan_mask The channel mask that needs to power on
 */
__attribute__((always_inline))
static inline void touch_ll_channel_sw_measure_mask(uint16_t chan_mask)
{
    (void) chan_mask;
    // Only for compatibility
}

/************************************** Data **********************************/
/**
 * Get the data of the touch channel according to the types
 *
 * @param touch_num touch pad index
 * @param type  data type
 *              0/1: TOUCH_LL_READ_RAW, raw data of touch channel
 *              2:   TOUCH_LL_READ_BENCHMARK, benchmark value of touch channel,
 *                   the benchmark value is the maximum during the first measurement period
 *              3:   TOUCH_LL_READ_SMOOTH, the smoothed data that obtained by filtering the raw data.
 * @param data pointer to the data
 */
__attribute__((always_inline))
static inline void touch_ll_read_chan_data(uint32_t touch_num, uint8_t type, uint32_t *data)
{
    HAL_ASSERT(type == TOUCH_LL_READ_RAW);
    HAL_ASSERT(touch_num < SOC_TOUCH_SENSOR_NUM);
    touch_num = TOUCH_LL_CHAN_SWAP(touch_num);
    if (touch_num & 0x1) {
        *data = HAL_FORCE_READ_U32_REG_FIELD(SENS.touch_meas[touch_num >> 1], l_val);
    } else {
        *data = HAL_FORCE_READ_U32_REG_FIELD(SENS.touch_meas[touch_num >> 1], h_val);
    }
}

/******************************************************************************/
/*                   Legacy APIs (to be removed in esp-idf v6.0)              */
/******************************************************************************/

#include "hal/touch_sensor_legacy_types.h"

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
    HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.sar_touch_ctrl1, touch_meas_delay, meas_time);
    //the waiting cycles (in 8MHz) between TOUCH_START and TOUCH_XPD
    HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.sar_touch_ctrl1, touch_xpd_wait, TOUCH_LL_PAD_MEASURE_WAIT_MAX);
}

/**
 * Get touch sensor measurement time.
 *
 * @param meas_time Pointer to accept measurement cycle count.
 */
static inline void touch_ll_get_meas_time(uint16_t *meas_time)
{
    *meas_time = HAL_FORCE_READ_U32_REG_FIELD(SENS.sar_touch_ctrl1, touch_meas_delay);
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
    HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.sar_touch_ctrl2, touch_sleep_cycles, sleep_time);
}

/**
 * Get touch sensor sleep time.
 *
 * @param sleep_time Pointer to accept sleep cycle count.
 */
static inline void touch_ll_get_sleep_time(uint16_t *sleep_time)
{
    *sleep_time = HAL_FORCE_READ_U32_REG_FIELD(SENS.sar_touch_ctrl2, touch_sleep_cycles);
}

/**
 * Set touch sensor high voltage threshold of charge.
 * The touch sensor measures the channel capacitance value by charging and discharging the channel.
 * So the high threshold should be less than the supply voltage.
 *
 * @param refh The high voltage threshold of charge.
 */
static inline void touch_ll_set_voltage_high(touch_high_volt_t refh)
{
    RTCIO.touch_cfg.drefh = refh;
}

/**
 * Get touch sensor high voltage threshold of charge.
 * The touch sensor measures the channel capacitance value by charging and discharging the channel.
 * So the high threshold should be less than the supply voltage.
 *
 * @param refh The high voltage threshold of charge.
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
 * Set touch sensor high voltage attenuation of charge. The actual charge threshold is high voltage threshold minus attenuation value.
 * The touch sensor measures the channel capacitance value by charging and discharging the channel.
 * So the high threshold should be less than the supply voltage.
 *
 * @param refh The high voltage threshold of charge.
 */
static inline void touch_ll_set_voltage_attenuation(touch_volt_atten_t atten)
{
    RTCIO.touch_cfg.drange = atten;
}

/**
 * Get touch sensor high voltage attenuation of charge. The actual charge threshold is high voltage threshold minus attenuation value.
 * The touch sensor measures the channel capacitance value by charging and discharging the channel.
 * So the high threshold should be less than the supply voltage.
 *
 * @param refh The high voltage threshold of charge.
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
    RTCIO.touch_pad[touch_num].slope = slope;
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
    *slope = (touch_cnt_slope_t)RTCIO.touch_pad[touch_num].slope;
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
    if (opt == TOUCH_PAD_TIE_OPT_FLOAT) {
        RTCIO.touch_pad[touch_pad_wrap].xpd = 0;
    } else {
        RTCIO.touch_pad[touch_pad_wrap].xpd = 1;
        RTCIO.touch_pad[touch_pad_wrap].tie_opt = opt;
    }
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
    if (RTCIO.touch_pad[touch_pad_wrap].xpd) {
        *opt = (touch_tie_opt_t)RTCIO.touch_pad[touch_pad_wrap].tie_opt;
    } else {
        *opt = TOUCH_PAD_TIE_OPT_FLOAT;
    }
}

/**
 * Set touch sensor FSM mode.
 *        The measurement action can be triggered by the hardware timer, as well as by the software instruction.
 *
 * @param mode FSM mode.
 */
__attribute__((always_inline))
static inline void touch_ll_set_fsm_mode(touch_fsm_mode_t mode)
{
    SENS.sar_touch_ctrl2.touch_start_fsm_en = 1;
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
__attribute__((always_inline))
static inline void touch_ll_stop_fsm(void)
{
    RTCCNTL.state0.touch_slp_timer_en = 0;
}

/**
 * Trigger a touch sensor measurement, only support in SW mode of FSM.
 */
static inline void touch_ll_start_sw_meas(void)
{
    SENS.sar_touch_ctrl2.touch_start_en = 1;
    SENS.sar_touch_ctrl2.touch_start_en = 0;
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
        HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.touch_thresh[tp_wrap / 2], l_thresh, threshold);
    } else {
        HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.touch_thresh[tp_wrap / 2], h_thresh, threshold);
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
            HAL_FORCE_READ_U32_REG_FIELD(SENS.touch_thresh[tp_wrap / 2], l_thresh) :
            HAL_FORCE_READ_U32_REG_FIELD(SENS.touch_thresh[tp_wrap / 2], h_thresh);
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
    return ((tp_wrap & 0x1) ? HAL_FORCE_READ_U32_REG_FIELD(SENS.touch_meas[tp_wrap / 2], l_val) :
                              HAL_FORCE_READ_U32_REG_FIELD(SENS.touch_meas[tp_wrap / 2], h_val));
}

#ifdef __cplusplus
}
#endif
