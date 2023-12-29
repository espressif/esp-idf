/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The ll is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "hal/misc.h"
#include "hal/assert.h"
#include "soc/touch_sensor_periph.h"
#include "soc/lp_analog_peri_struct.h"
#include "soc/touch_struct.h"
#include "soc/pmu_struct.h"
#include "soc/soc_caps.h"
#include "hal/touch_sensor_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TOUCH_LL_READ_RAW           0x0
#define TOUCH_LL_READ_BENCHMARK     0x2
#define TOUCH_LL_READ_SMOOTH        0x3

#define TOUCH_LL_TIMER_FORCE_DONE_BY_SW   0x1
#define TOUCH_LL_TIMER_DONE_BY_FSM         0x0

// Interrupt mask
#define TOUCH_LL_INTR_MASK_SCAN_DONE        BIT(1)
#define TOUCH_LL_INTR_MASK_DONE             BIT(2)
#define TOUCH_LL_INTR_MASK_ACTIVE           BIT(3)
#define TOUCH_LL_INTR_MASK_INACTIVE         BIT(4)
#define TOUCH_LL_INTR_MASK_TIMEOUT          BIT(5)
#define TOUCH_LL_INTR_MASK_APPROACH_DONE    BIT(6)
#define TOUCH_LL_INTR_MASK_ALL              (0x3F)

#define TOUCH_LL_FULL_CHANNEL_MASK          ((uint16_t)((1U << SOC_TOUCH_SENSOR_NUM) - 1))
#define TOUCH_LL_NULL_CHANNEL               (15)  // Null Channel id. Used for disabling some functions like sleep/approach/waterproof

#define TOUCH_LL_PAD_MEASURE_WAIT_MAX      (0x7FFF)    // The timer frequency is 8Mhz, the max value is 0xff

/**
 * Enable/disable clock gate of touch sensor.
 *
 * @param enable true/false.
 */
static inline void touch_ll_enable_clock(bool enable)
{
    LP_ANA_PERI.date.clk_en = enable;
}

/**
 * Set the power on wait cycle
 *
 * @param wait_cycles
 */
static inline void touch_ll_set_power_on_wait_cycle(uint32_t wait_cycles)
{
    LP_ANA_PERI.touch_scan_ctrl1.touch_xpd_wait = wait_cycles;
}

/**
 * Set touch sensor touch sensor charge and discharge times of every measurement on a pad.
 *
 * @param sampler_id The sampler index
 * @param charge_times The times of charge and discharge in each measure process of touch channels.
 *                     The timer frequency is 8Mhz. Range: 0 ~ 0xffff.
 */
static inline void touch_ll_set_charge_times(uint8_t sampler_id, uint16_t charge_times)
{
    //The times of charge and discharge in each measure process of touch channels.
    switch (sampler_id) {
        case 0:
            LP_ANA_PERI.touch_work_meas_num.touch_meas_num0 = charge_times;
            break;
        case 1:
            LP_ANA_PERI.touch_work_meas_num.touch_meas_num1 = charge_times;
            break;
        case 2:
            LP_ANA_PERI.touch_work_meas_num.touch_meas_num2 = charge_times;
            break;
        default:
            abort();
    }
}

/**
 * Get touch sensor times of charge and discharge.
 *
 * @param meas_times Pointer to accept times count of charge and discharge.
 */
static inline void touch_ll_get_charge_times(uint8_t sampler_id, uint16_t *charge_times)
{
    switch (sampler_id) {
        case 0:
            *charge_times = LP_ANA_PERI.touch_work_meas_num.touch_meas_num0;
            break;
        case 1:
            *charge_times = LP_ANA_PERI.touch_work_meas_num.touch_meas_num1;
            break;
        case 2:
            *charge_times = LP_ANA_PERI.touch_work_meas_num.touch_meas_num2;
            break;
        default:
            abort();
    }
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
    // touch sensor sleep cycle Time = interval_ticks / RTC_SLOW_CLK
    HAL_FORCE_MODIFY_U32_REG_FIELD(PMU.touch_pwr_cntl, sleep_cycles, interval_ticks);
}

/**
 * Get touch sensor sleep time.
 *
 * @param interval_ticks   Pointer to accept measurement interval (sleep) cycle number.
 */
static inline void touch_ll_get_measure_interval_ticks(uint16_t *interval_ticks)
{
    *interval_ticks = HAL_FORCE_READ_U32_REG_FIELD(PMU.touch_pwr_cntl, sleep_cycles);
}

/**
 * Set touch sensor FSM mode.
 *        The measurement action can be triggered by the hardware timer, as well as by the software instruction.
 *
 * @param mode FSM mode.
 *        TOUCH_FSM_MODE_TIMER: the FSM will trigger scanning repeatedly under the control of the hardware timer
 *        TOUCH_FSM_MODE_SW: the FSM will trigger scanning once under the control of the software
 */
static inline void touch_ll_set_fsm_mode(touch_fsm_mode_t mode)
{
    // FSM controlled by timer or software
    LP_ANA_PERI.touch_mux0.touch_fsm_en = mode;
    // Start by timer or software
    LP_ANA_PERI.touch_mux0.touch_start_force = mode;
    // Stop by timer or software
    LP_ANA_PERI.touch_mux0.touch_done_force = mode;
}

/**
 * Get touch sensor FSM mode.
 *        The measurement action can be triggered by the hardware timer, as well as by the software instruction.
 *
 * @param mode FSM mode.
 */
static inline void touch_ll_get_fsm_mode(touch_fsm_mode_t *mode)
{
    *mode = (touch_fsm_mode_t)LP_ANA_PERI.touch_mux0.touch_fsm_en;
}

/**
 * Is touch sensor FSM using hardware timer to trigger scanning.
 *        The measurement action can be triggered by the hardware timer, as well as by the software instruction.
 *
 * @return Whether FSM timer enabled.
 */
static inline bool touch_ll_is_fsm_using_timer(void)
{
    return !LP_ANA_PERI.touch_mux0.touch_start_force;
}

/**
 * Touch timer trigger measurement and always wait measurement done.
 * Force done for touch timer ensures that the timer always can get the measurement done signal.
 */
static inline void touch_ll_force_done_curr_measurement(void)
{
    if (touch_ll_is_fsm_using_timer()) {
        LP_ANA_PERI.touch_mux0.touch_done_force = TOUCH_LL_TIMER_FORCE_DONE_BY_SW;
        LP_ANA_PERI.touch_mux0.touch_done_en = 1;
        LP_ANA_PERI.touch_mux0.touch_done_en = 0;
        LP_ANA_PERI.touch_mux0.touch_done_force = TOUCH_LL_TIMER_DONE_BY_FSM;
    } else {
        LP_ANA_PERI.touch_mux0.touch_done_en = 1;
        LP_ANA_PERI.touch_mux0.touch_done_en = 0;
    }
}

/**
 * Start touch sensor FSM timer to run FSM repeatedly
 * The measurement action can be triggered by the hardware timer, as well as by the software instruction.
 * @note
 *      The timer should be triggered
 * @param is_sleep   Whether in sleep state
 */
static inline void touch_ll_start_fsm_repeated_timer(bool is_sleep)
{
    /**
     * Touch timer trigger measurement and always wait measurement done.
     * Force done for touch timer ensures that the timer always can get the measurement done signal.
     */
    touch_ll_force_done_curr_measurement();
    if (is_sleep) {
        PMU.touch_pwr_cntl.sleep_timer_en = 1;
    } else {
        LP_ANA_PERI.touch_mux0.touch_start_en = 1;
    }
}

/**
 * Stop touch sensor FSM timer.
 *        The measurement action can be triggered by the hardware timer, as well as by the software instruction.
 * @param is_sleep   Whether in sleep state
 */
static inline void touch_ll_stop_fsm_repeated_timer(bool is_sleep)
{
    if (is_sleep) {
        PMU.touch_pwr_cntl.sleep_timer_en = 0;
    } else {
        LP_ANA_PERI.touch_mux0.touch_start_en = 0;
    }
    touch_ll_force_done_curr_measurement();
}

/**
 * Start touch sensor FSM once by software
 * @note Every trigger means measuring one channel, not scanning all enabled channels
 */
static inline void touch_ll_start_fsm_once(void)
{
    /* Trigger once measurement */
    LP_ANA_PERI.touch_mux0.touch_start_en = 1;
    LP_ANA_PERI.touch_mux0.touch_start_en = 0;
}

static inline void touch_ll_measure_channel_once(uint16_t chan_mask)
{
    LP_ANA_PERI.touch_mux1.touch_start = chan_mask;
}

/**
 * Set touch sensor threshold of charge cycles that triggers pad active state.
 * The threshold determines the sensitivity of the touch sensor.
 * The threshold is the original value of the trigger state minus the benchmark value.
 *
 * @note  If set "TOUCH_PAD_THRESHOLD_MAX", the touch is never be triggered.
 * @param touch_num The touch pad id
 * @param sampler_id The sampler index
 * @param thresh    The threshold of charge cycles
 */
static inline void touch_ll_set_chan_active_threshold(uint32_t touch_num, uint8_t sampler_id, uint32_t thresh)
{
    HAL_ASSERT(sampler_id < SOC_TOUCH_SAMPLER_NUM);
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_ANA_PERI.touch_padx_thn[touch_num].thn[sampler_id], threshold, thresh);
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
static inline void touch_ll_set_channel_mask(uint16_t enable_mask)
{
    uint16_t mask = enable_mask & TOUCH_PAD_BIT_MASK_ALL;
    LP_ANA_PERI.touch_scan_ctrl1.touch_scan_pad_map = mask;
    LP_ANA_PERI.touch_filter2.touch_outen = mask;
}

/**
 * @brief Power on the channel by mask
 *
 * @param chan_mask The channel mask that needs to power on
 */
static inline void touch_ll_channel_power_on(uint16_t chan_mask)
{
    uint32_t curr_mask = LP_ANA_PERI.touch_mux1.touch_xpd;
    LP_ANA_PERI.touch_mux1.touch_xpd = chan_mask | curr_mask;
}

/**
 * @brief Power off the channel by mask
 *
 * @param chan_mask The channel mask that needs to power off
 */
static inline void touch_ll_channel_power_off(uint16_t chan_mask)
{
    uint32_t curr_mask = LP_ANA_PERI.touch_mux1.touch_xpd;
    LP_ANA_PERI.touch_mux1.touch_xpd = (~chan_mask) & curr_mask;
}

/**
 * @brief Start channel by mask
 * @note  Only start the specified channels
 *
 * @param chan_mask The channel mask that needs to start
 */
static inline void touch_ll_channel_start(uint16_t chan_mask)
{
    uint32_t curr_mask = LP_ANA_PERI.touch_mux1.touch_start;
    LP_ANA_PERI.touch_mux1.touch_start = chan_mask | curr_mask;
    LP_ANA_PERI.touch_mux1.touch_start = (~chan_mask) & curr_mask;
}

/**
 * Get the touch sensor active channel mask, usually used in ISR to decide which channels are 'touched'.
 *
 * @param active_mask The touch channel status. e.g. Touch1 trigger status is `status_mask & (BIT1)`.
 */
static inline void touch_ll_get_active_channel_mask(uint32_t *active_mask)
{
    *active_mask = LP_TOUCH.chn_status.pad_active;
}

/**
 * Clear all touch sensor channels active status.
 *
 * @note Generally no manual removal is required.
 */
static inline void touch_ll_clear_active_channel_status(void)
{
    LP_ANA_PERI.touch_clr.touch_status_clr = 1;
}

/**
 * Get the data of the touch channel according to the types
 *
 * @param touch_num touch pad index
 * @param sampler_id The sampler index
 * @param type  data type
 *              0/1: TOUCH_LL_READ_RAW, the raw data of the touch channel
 *              2:   TOUCH_LL_READ_BENCHMARK, benchmark value of touch channel,
 *                   the benchmark value is the maximum during the first measurement period
 *              3:   TOUCH_LL_READ_SMOOTH, the smoothed data that obtained by filtering the raw data.
 * @param data pointer to the data
 */
__attribute__((always_inline))
static inline void touch_ll_read_chan_data(uint32_t touch_num, uint8_t sampler_id, uint8_t type, uint32_t *data)
{
    HAL_ASSERT(sampler_id < SOC_TOUCH_SAMPLER_NUM);
    HAL_ASSERT(type <= TOUCH_LL_READ_SMOOTH);
    LP_ANA_PERI.touch_mux0.touch_freq_sel = sampler_id + 1;
    LP_ANA_PERI.touch_mux0.touch_data_sel = type;
    *data = LP_TOUCH.chn_data[touch_num - 1].pad_data;
}

/**
 * Get touch sensor measure status. No block.
 *
 * @return
 *      - If touch sensors measure done.
 */
static inline bool touch_ll_is_measure_done(uint32_t *touch_num)
{
    *touch_num = (uint32_t)(LP_TOUCH.chn_status.scan_curr);
    return (bool)LP_TOUCH.chn_status.meas_done;
}

/**
 * Select the counting mode of the binarized touch out wave
 *
 * @param mode 0: as data, the value will be smaller than actual value but more sensitive when the frequency of touch_out is close to the source clock
 *             1: as clock, the value is accurate but less sensitive when the frequency of touch_out is close to the source clock
 *             No big difference when the frequency of touch_out is far more lower than the source clock
 */
static inline void touch_ll_set_out_mode(touch_out_mode_t mode)
{
    LP_ANA_PERI.touch_work.touch_out_sel = mode;
}

/**
 * @brief Set the clock division of the sampling frequency
 *
 * @param sampler_id The sampler index
 * @param div_num   Division number
 */
static inline void touch_ll_set_clock_div(uint8_t sampler_id, uint32_t div_num)
{
    switch (sampler_id) {
        case 0:
            LP_ANA_PERI.touch_work.div_num0 = div_num;
            break;
        case 1:
            LP_ANA_PERI.touch_work.div_num1 = div_num;
            break;
        case 2:
            LP_ANA_PERI.touch_work.div_num2 = div_num;
            break;
        default:
            // invalid sampler_id
            abort();
    }
}

/**
 * Set connection type of touch channel in idle status.
 *        When a channel is in measurement mode, other initialized channels are in idle mode.
 *        The touch channel is generally adjacent to the trace, so the connection state of the idle channel
 *        affects the stability and sensitivity of the test channel.
 *        The `CONN_HIGHZ`(high resistance) setting increases the sensitivity of touch channels.
 *        The `CONN_GND`(grounding) setting increases the stability of touch channels.
 * @note  This option does not take effect,  it is default to HIGH Z
 *        Only remained here to be compatible to other version
 *
 * @param type  Select idle channel connect to high resistance state or ground. (No effect)
 */
static inline void touch_ll_set_idle_channel_connect(touch_pad_conn_type_t type)
{
    (void)type;
}

/**
 * Get the current channel that under measuring.
 *
 * @return
 *      - touch channel number
 */
__attribute__((always_inline))
static inline uint32_t touch_ll_get_current_meas_channel(void)
{
    return (uint32_t)(LP_TOUCH.chn_status.scan_curr);
}

/**
 * Enable touch sensor interrupt by bitmask.
 *
 * @param int_mask interrupt mask
 */
static inline void touch_ll_intr_enable(uint32_t int_mask)
{
    uint32_t mask = LP_TOUCH.int_ena.val;
    mask |= (int_mask & TOUCH_LL_INTR_MASK_ALL);
    LP_TOUCH.int_ena.val = mask;
}

/**
 * Disable touch sensor interrupt by bitmask.
 *
 * @param int_mask interrupt mask
 */
static inline void touch_ll_intr_disable(uint32_t int_mask)
{
    uint32_t mask = LP_TOUCH.int_ena.val;
    mask &= ~(int_mask & TOUCH_LL_INTR_MASK_ALL);
    LP_TOUCH.int_ena.val = mask;
}

/**
 * Clear touch sensor interrupt by bitmask.
 *
 * @param int_mask Pad mask to clear interrupts
 */
static inline void touch_ll_intr_clear_all(void)
{
    LP_TOUCH.int_clr.val = TOUCH_LL_INTR_MASK_ALL;
}

/**
 * Get the bitmask of touch sensor interrupt status.
 *
 * @return type interrupt type
 */
static inline uint32_t touch_ll_get_intr_status_mask(void)
{
    uint32_t intr_st = LP_TOUCH.int_st.val;
    return (intr_st & TOUCH_LL_INTR_MASK_ALL);
}

/**
 * Enable the timeout check for all touch sensor channels measurements.
 * When the touch reading of a touch channel exceeds the measurement threshold,
 * If enable: a timeout interrupt will be generated and it will go to the next channel measurement.
 * If disable: the FSM is always on the channel, until the measurement of this channel is over.
 *
 * @param timeout_cycles The maximum time cycles of the measurement on one channel.
 */
static inline void touch_ll_timeout_enable(uint32_t timeout_cycles)
{
    LP_ANA_PERI.touch_scan_ctrl2.touch_timeout_num = timeout_cycles;
    LP_ANA_PERI.touch_scan_ctrl2.touch_timeout_en = 1;
}

/**
 * Disable the timeout check for all touch sensor channels measurements.
 * When the touch reading of a touch channel exceeds the measurement threshold,
 * If enable: a timeout interrupt will be generated and it will go to the next channel measurement.
 * If disable: the FSM is always on the channel, until the measurement of this channel is over.
 */
static inline void touch_ll_timeout_disable(void)
{
    LP_ANA_PERI.touch_scan_ctrl2.touch_timeout_en = 0;
}

/**
 * Set the engaged sampler number
 *
 * @param sampler_num The enabled sampler number, range 0~3.
 *                    0/1 means only one sampler enabled, which can not support frequency hopping
 */
static inline void touch_ll_sampler_set_engaged_num(uint8_t sampler_num)
{
    HAL_ASSERT(sampler_num < SOC_TOUCH_SAMPLER_NUM);
    LP_ANA_PERI.touch_scan_ctrl2.freq_scan_en = !!sampler_num;
    LP_ANA_PERI.touch_scan_ctrl2.freq_scan_cnt_limit = sampler_num ? sampler_num : 1;
}

/**
 * Set capacitance and resistance of the RC filter of the sampling frequency.
 *
 * @param sampler_id The sampler index
 * @param cap       Capacitance of the RC filter.
 * @param res       Resistance of the RC filter.
 */
static inline void touch_ll_sampler_set_rc_filter(uint8_t sampler_id, uint32_t cap, uint32_t res)
{
    HAL_ASSERT(sampler_id < SOC_TOUCH_SAMPLER_NUM);
    LP_ANA_PERI.touch_freq_scan_para[sampler_id].touch_freq_dcap_lpf = cap;
    LP_ANA_PERI.touch_freq_scan_para[sampler_id].touch_freq_dres_lpf = res;
}

/**
 * @brief Set the driver of the sampling frequency
 *
 * @param sampler_id The sampler index
 * @param ls_drv    Low speed touch driver
 * @param hs_drv    High speed touch driver
 */
static inline void touch_ll_sampler_set_driver(uint8_t sampler_id, uint32_t ls_drv, uint32_t hs_drv)
{
    HAL_ASSERT(sampler_id < SOC_TOUCH_SAMPLER_NUM);
    LP_ANA_PERI.touch_freq_scan_para[sampler_id].touch_freq_drv_ls = ls_drv;
    LP_ANA_PERI.touch_freq_scan_para[sampler_id].touch_freq_drv_hs = hs_drv;
}

/**
 * Set the touch internal LDO bias voltage of the sampling frequency
 *
 * @param sampler_id The sampler index
 * @param bias_volt LDO bias voltage
 */
static inline void touch_ll_sampler_set_bias_voltage(uint8_t sampler_id, uint32_t bias_volt)
{
    HAL_ASSERT(sampler_id < SOC_TOUCH_SAMPLER_NUM);
    LP_ANA_PERI.touch_freq_scan_para[sampler_id].touch_freq_dbias = bias_volt;
}

/**
 * @brief Set the internal loop capacitance
 *        Can simulate the touch signal via the internal capacitance
 *        Need to turn off touch pad
 * @param cap   The internal capacitance
 */
static inline void touch_ll_set_internal_loop_capacitance(int cap)
{
    bool enable = cap > 0;
    LP_ANA_PERI.touch_ana_para.touch_touch_en_cal = enable;
    LP_ANA_PERI.touch_ana_para.touch_touch_dcap_cal = enable ? cap : 0;
}

/************************ Filter register setting ************************/

/**
 * Force reset benchmark to raw data of touch sensor.
 *
 * @note If call this API, make sure enable clock gate(`touch_ll_clkgate`) first.
 * @param chan_mask touch channel mask
 */
static inline void touch_ll_reset_chan_benchmark(uint32_t chan_mask)
{
    LP_ANA_PERI.touch_clr.touch_channel_clr = chan_mask;
}

/**
 * Set filter mode. The input of the filter is the raw value of touch reading,
 * and the output of the filter is involved in the judgment of the touch state.
 *
 * @param mode Filter mode type. Refer to ``touch_filter_mode_t``.
 */
static inline void touch_ll_filter_set_filter_mode(touch_filter_mode_t mode)
{
    LP_ANA_PERI.touch_filter1.touch_filter_mode = mode;
}

/**
 * Set filter mode. The input to the filter is raw data and the output is the smooth data.
 * The smooth data is used to determine the touch status.
 *
 * @param mode Filter mode type. Refer to ``touch_smooth_mode_t``.
 */
static inline void touch_ll_filter_set_smooth_mode(touch_smooth_mode_t mode)
{
    LP_ANA_PERI.touch_filter1.touch_smooth_lvl = mode;
}

/**
 * Set debounce count, such as `n`. If the measured values continue to exceed
 * the threshold for `n+1` times, it is determined that the touch sensor state changes.
 *
 * @param dbc_cnt Debounce count value.
 */
static inline void touch_ll_filter_set_debounce(uint32_t dbc_cnt)
{
    LP_ANA_PERI.touch_filter1.touch_debounce_limit = dbc_cnt;
}

/**
 * Set the positive noise threshold coefficient. Higher = More noise resistance.
 * The benchmark will update to the new value if the touch data is within (benchmark + active_threshold * pos_coeff)
 *
 *
 * @param pos_noise_thresh Range [-1 ~ 3]. The coefficient is -1: always; 0: 4/8;  1: 3/8;   2: 2/8;   3: 1;
 *                         -1: the benchmark will always update to the new touch data without considering the positive noise threshold
 */
static inline void touch_ll_filter_set_pos_noise_thresh(int pos_noise_thresh)
{
    bool always_update = pos_noise_thresh < 0;
    LP_ANA_PERI.touch_filter2.touch_bypass_noise_thres = always_update;
    LP_ANA_PERI.touch_filter1.touch_noise_thres = always_update ? 0 : pos_noise_thresh;
}

/**
 * Set the negative noise threshold coefficient. Higher = More noise resistance.
 * The benchmark will update to the new value if the touch data is greater than (benchmark - active_threshold * neg_coeff)
 *
 * @param neg_noise_thresh Range [-2 ~ 3]. The coefficient is -2: never; -1: always; 0: 4/8;  1: 3/8;   2: 2/8;   3: 1;
 *                         -1: the benchmark will always update to the new touch data without considering the negative noise threshold
 *                         -2: the benchmark will never update to the new touch data with negative growth
 * @param neg_noise_limit  Only when neg_noise_thresh >= 0, if the touch data keep blow the negative threshold for mare than neg_noise_limit ticks,
 *                         the benchmark will still update to the new value.
 *                         It is normally used for updating the benchmark at the first scanning
 */
static inline void touch_ll_filter_set_neg_noise_thresh(int neg_noise_thresh, uint8_t neg_noise_limit)
{
    bool always_update = neg_noise_thresh == -1;
    bool stop_update = neg_noise_thresh == -2;
    LP_ANA_PERI.touch_filter2.touch_bypass_neg_noise_thres = always_update;
    LP_ANA_PERI.touch_filter1.touch_neg_noise_disupdate_baseline_en = stop_update;
    LP_ANA_PERI.touch_filter1.touch_neg_noise_thres = always_update || stop_update ? 0 : neg_noise_thresh;
    LP_ANA_PERI.touch_filter1.touch_neg_noise_limit = always_update || stop_update ? 5 : neg_noise_limit; // 5 is the default value
}

/**
 * Set the hysteresis value of the active threshold
 * While the touch data is greater than active_threshold + hysteresis and last for several ticks, the channel is activated,
 * and while the touch data is smaller than active_threshold - hysteresis and last for several ticks, the channel is inactivated
 *
 * @param hysteresis The hysteresis value of active threshold
 */
static inline void touch_ll_filter_set_active_hysteresis(uint32_t hysteresis)
{
    LP_ANA_PERI.touch_filter1.touch_hysteresis = hysteresis;
}

/**
 * Set jitter filter step size.
 * If filter mode is jitter, should set filter step for jitter.
 * Range: 0 ~ 15
 *
 * @param step The step size of the data change.
 */
static inline void touch_ll_filter_set_jitter_step(uint32_t step)
{
    LP_ANA_PERI.touch_filter1.touch_jitter_step = step;
}

/**
 * Enable or disable touch sensor filter and detection algorithm.
 * For more details on the detection algorithm, please refer to the application documentation.
 */
static inline void touch_ll_filter_enable(bool enable)
{
    LP_ANA_PERI.touch_filter1.touch_filter_en = enable;
}

/**
 * Force the update the benchmark by software
 * @note  This benchmark will be applied to all enabled channel and all sampling frequency
 *
 * @param benchmark The benchmark specified by software
 */
static inline void touch_ll_force_update_benchmark(uint32_t benchmark)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_ANA_PERI.touch_filter3, touch_baseline_sw, benchmark);
    LP_ANA_PERI.touch_filter3.touch_update_baseline_sw = 1;
    // waiting for update
    while (LP_ANA_PERI.touch_filter3.touch_update_baseline_sw);
}

/************************ Waterproof register setting ************************/

/**
 * Set touch channel use for guard pad.
 *
 * @param pad_num Touch sensor channel number.
 */
static inline void touch_ll_waterproof_set_guard_chan(uint32_t pad_num)
{
    LP_ANA_PERI.touch_scan_ctrl2.touch_out_ring = pad_num;
}

/**
 * Enable or disable parameter of waterproof function.
 *
 * The waterproof function includes a shielded channel (TOUCH_PAD_NUM14) and a guard channel.
 * Guard pad is used to detect the large area of water covering the touch panel.
 * Shield pad is used to shield the influence of water droplets covering the touch panel.
 * It is generally designed as a grid and is placed around the touch buttons.
 */
static inline void touch_ll_waterproof_enable(bool enable)
{
    LP_ANA_PERI.touch_scan_ctrl1.touch_shield_pad_en = enable;
}

/**
 * Set the shield channel mask
 *
 * @param mask The channels that set in the mask will be used as shield channel
 */
static inline void touch_ll_waterproof_set_shield_chan_mask(uint32_t mask)
{
    LP_ANA_PERI.touch_mux0.touch_bufsel = (mask & TOUCH_LL_FULL_CHANNEL_MASK);
}

/**
 * Set the touch buff driver for the shield channel.
 *
 * @param driver_level The driver level of the touch buff
 */
static inline void touch_ll_waterproof_set_shield_driver(touch_pad_shield_driver_t driver_level)
{
    LP_ANA_PERI.touch_ana_para.touch_touch_buf_drv = driver_level;
}

/************************ Approach register setting ************************/

/**
 * Set the approach channel to the specific touch channel
 * To disable the approach channel, point this pad to `TOUCH_LL_NULL_CHANNEL`
 *
 * @param aprch_chan approach channel.
 * @param touch_num  The touch channel that supposed to be used as approach channel
 */
static inline void touch_ll_set_approach_channel(uint8_t aprch_chan, uint32_t touch_num)
{
    switch (aprch_chan) {
        case 0:
            LP_ANA_PERI.touch_approach.touch_approach_pad0 = touch_num;
            break;
        case 1:
            LP_ANA_PERI.touch_approach.touch_approach_pad1 = touch_num;
            break;
        case 2:
            LP_ANA_PERI.touch_approach.touch_approach_pad2 = touch_num;
            break;
        default:
            // invalid approach channel
            abort();
    }
}

/**
 * Set cumulative measurement times for approach channel.
 *
 * @param sampler_id The sampler index
 * @param times The cumulative number of measurement cycles.
 */
static inline void touch_ll_approach_set_measure_times(uint8_t sampler_id, uint32_t times)
{
    switch (sampler_id) {
        case 0:
            LP_ANA_PERI.touch_approach_work_meas_num.touch_approach_meas_num0 = times;
            break;
        case 1:
            LP_ANA_PERI.touch_approach_work_meas_num.touch_approach_meas_num1 = times;
            break;
        case 2:
            LP_ANA_PERI.touch_approach_work_meas_num.touch_approach_meas_num2 = times;
            break;
        default:
            // invalid sampler_id
            abort();
    }
}

/**
 * Read current cumulative measurement times for approach channel.
 *
 * @param aprch_chan approach channel.
 * @param cnt The cumulative number of measurement cycles.
 */
static inline void touch_ll_approach_read_measure_cnt(uint8_t aprch_chan, uint32_t *cnt)
{
    switch (aprch_chan) {
        case 0:
            *cnt = HAL_FORCE_READ_U32_REG_FIELD(LP_TOUCH.aprch_ch_data, approach_pad0_cnt);
            break;
        case 1:
            *cnt = HAL_FORCE_READ_U32_REG_FIELD(LP_TOUCH.aprch_ch_data, approach_pad1_cnt);
            break;
        case 2:
            *cnt = HAL_FORCE_READ_U32_REG_FIELD(LP_TOUCH.aprch_ch_data, approach_pad2_cnt);
            break;
        default:  // sleep channel
            *cnt = HAL_FORCE_READ_U32_REG_FIELD(LP_TOUCH.aprch_ch_data, slp_approach_cnt);
            break;
    }
}

/**
 * Check if the touch sensor channel is the approach channel.
 *
 * @param touch_num The touch sensor channel number.
 */
static inline bool touch_ll_is_approach_channel(uint32_t touch_num)
{
    if ((LP_ANA_PERI.touch_approach.touch_approach_pad0 != touch_num)
        && (LP_ANA_PERI.touch_approach.touch_approach_pad1 != touch_num)
        && (LP_ANA_PERI.touch_approach.touch_approach_pad2 != touch_num)) {
        return false;
    } else {
        return true;
    }
}

/************** sleep channel setting ***********************/

/**
 * Set touch channel number for sleep channel.
 *
 * @note Only one touch sensor channel is supported in deep sleep mode.
 * @param touch_num Touch sensor channel number.
 */
static inline void touch_ll_sleep_set_channel_num(uint32_t touch_num)
{
    LP_ANA_PERI.touch_slp0.touch_slp_pad = touch_num;
}

/**
 * Get touch channel number for sleep pad.
 *
 * @note Only one touch sensor channel is supported in deep sleep mode.
 * @param touch_num Touch sensor channel number.
 */
static inline void touch_ll_sleep_get_channel_num(uint32_t *touch_num)
{
    *touch_num = (uint32_t)(LP_ANA_PERI.touch_slp0.touch_slp_pad);
}

/**
 * Set the trigger threshold of touch sensor in deep sleep.
 * The threshold determines the sensitivity of the touch sensor.
 * The threshold is the original value of the trigger state minus the benchmark value.
 *
 * @note In general, the touch threshold during sleep can use the threshold parameter parameters before sleep.
 */
static inline void touch_ll_sleep_set_threshold(uint8_t sampler_id, uint32_t touch_thresh)
{
    switch (sampler_id) {
        case 0:
            HAL_FORCE_MODIFY_U32_REG_FIELD(LP_ANA_PERI.touch_slp0, touch_slp_th0, touch_thresh);
            break;
        case 1:
            HAL_FORCE_MODIFY_U32_REG_FIELD(LP_ANA_PERI.touch_slp1, touch_slp_th1, touch_thresh);
            break;
        case 2:
            HAL_FORCE_MODIFY_U32_REG_FIELD(LP_ANA_PERI.touch_slp1, touch_slp_th2, touch_thresh);
            break;
        default:
            // invalid sampler_id
            abort();
    }
}

/**
 * Enable approach function for sleep channel.
 */
static inline void touch_ll_sleep_enable_approach(bool enable)
{
    LP_ANA_PERI.touch_approach.touch_slp_approach_en = enable;
}

/**
 * Get the data of the touch channel according to the types
 *
 * @param sampler_id The sampler index
 * @param type  data type
 *              0/1: TOUCH_LL_READ_RAW, the raw data of the touch channel
 *              2:   TOUCH_LL_READ_BENCHMARK, benchmark value of touch channel,
 *                   the benchmark value is the maximum during the first measurement period
 *              3:   TOUCH_LL_READ_SMOOTH, the smoothed data that obtained by filtering the raw data.
 * @param smooth_data pointer to smoothed data
 */
__attribute__((always_inline))
static inline void touch_ll_sleep_read_chan_data(uint8_t type, uint8_t sampler_id, uint32_t *data)
{
    HAL_ASSERT(type <= TOUCH_LL_READ_SMOOTH);
    LP_ANA_PERI.touch_mux0.touch_freq_sel = sampler_id + 1;
    LP_ANA_PERI.touch_mux0.touch_data_sel = type;
    *data = HAL_FORCE_READ_U32_REG_FIELD(LP_TOUCH.slp_ch_data, slp_data);
}

/**
 * @brief Reset the benchmark of the sleep channel
 *
 */
static inline void touch_ll_sleep_reset_benchmark(void)
{
    LP_ANA_PERI.touch_slp0.touch_slp_channel_clr = 1;
}

/**
 * Read debounce of touch sensor for sleep channel.
 *
 * @param debounce Pointer to accept touch sensor debounce value.
 */
static inline void touch_ll_sleep_read_debounce(uint32_t *debounce)
{
    *debounce = LP_TOUCH.slp_ch_data.slp_debounce_cnt;
}

/**
 * Read approach count of touch sensor for sleep channel.
 * @param approach_cnt Pointer to accept touch sensor approach count value.
 */
static inline void touch_ll_sleep_read_approach_cnt(uint32_t *approach_cnt)
{
    *approach_cnt = HAL_FORCE_READ_U32_REG_FIELD(LP_TOUCH.aprch_ch_data, slp_approach_cnt);
}

#ifdef __cplusplus
}
#endif
