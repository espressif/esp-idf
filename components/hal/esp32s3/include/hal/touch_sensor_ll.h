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
#include "esp_bit_defs.h"
#include "hal/misc.h"
#include "hal/assert.h"
#include "soc/touch_sensor_periph.h"
#include "soc/rtc_cntl_struct.h"
#include "soc/rtc_io_struct.h"
#include "soc/sens_struct.h"
#include "soc/soc_caps.h"
#include "hal/touch_sens_types.h"

#ifdef __cplusplus
extern "C" {
#endif
#define TOUCH_LL_READ_RAW           0x0
#define TOUCH_LL_READ_BENCHMARK     0x2
#define TOUCH_LL_READ_SMOOTH        0x3

#define TOUCH_LL_TIMER_FORCE_DONE   0x3
#define TOUCH_LL_TIMER_DONE         0x0

#define TOUCH_LL_INTR_MASK_SCAN_DONE        BIT(4)
#define TOUCH_LL_INTR_MASK_DONE             BIT(6)
#define TOUCH_LL_INTR_MASK_ACTIVE           BIT(7)
#define TOUCH_LL_INTR_MASK_INACTIVE         BIT(8)
#define TOUCH_LL_INTR_MASK_TIMEOUT          BIT(18)
#define TOUCH_LL_INTR_MASK_PROX_DONE        BIT(20)
#define TOUCH_LL_INTR_MASK_ALL              (TOUCH_LL_INTR_MASK_SCAN_DONE | \
                                             TOUCH_LL_INTR_MASK_DONE | \
                                             TOUCH_LL_INTR_MASK_ACTIVE | \
                                             TOUCH_LL_INTR_MASK_INACTIVE | \
                                             TOUCH_LL_INTR_MASK_TIMEOUT | \
                                             TOUCH_LL_INTR_MASK_PROX_DONE)

#define TOUCH_LL_FULL_CHANNEL_MASK          ((uint16_t)((1U << SOC_TOUCH_SENSOR_NUM) - 1))
#define TOUCH_LL_NULL_CHANNEL               (0)  // Null Channel id. Used for disabling some functions like sleep/proximity/waterproof

#define TOUCH_LL_PAD_MEASURE_WAIT_MAX      (0xFF)      // The timer frequency is 8Mhz, the max value is 0xff
#define TOUCH_LL_ACTIVE_THRESH_MAX         (0x3FFFFF)  // Max channel active threshold
#define TOUCH_LL_TIMEOUT_MAX               (0x3FFFFF)  // Max timeout value

/**
 * Enable/disable clock gate of touch sensor.
 *
 * @param enable true/false.
 */
static inline void touch_ll_enable_clock_gate(bool enable)
{
    RTCCNTL.touch_ctrl2.touch_clkgate_en = enable; //enable touch clock for FSM. or force enable.
}

/**
 * Enable/disable clock gate of touch sensor.
 *
 * @param enable true/false.
 */
static inline void touch_ll_reset_module(void)
{
    RTCCNTL.touch_ctrl2.touch_reset = 1;
    RTCCNTL.touch_ctrl2.touch_reset = 0;    // Should be set 0.
}

/*********************************** Interrupts *******************************/
/**
 * Enable touch sensor interrupt by bitmask.
 *
 * @param int_mask interrupt mask
 */
static inline void touch_ll_interrupt_enable(uint32_t int_mask)
{
    uint32_t mask = RTCCNTL.int_ena_w1ts.val;
    mask |= (int_mask & TOUCH_LL_INTR_MASK_ALL);
    RTCCNTL.int_ena_w1ts.val = mask;
}

/**
 * Disable touch sensor interrupt by bitmask.
 *
 * @param int_mask interrupt mask
 */
static inline void touch_ll_interrupt_disable(uint32_t int_mask)
{
    uint32_t mask = int_mask & TOUCH_LL_INTR_MASK_ALL;
    RTCCNTL.int_ena_w1tc.val = mask;
}

/**
 * Clear touch sensor interrupt by bitmask.
 *
 * @param int_mask Pad mask to clear interrupts
 */
__attribute__((always_inline))
static inline void touch_ll_interrupt_clear(uint32_t int_mask)
{
    RTCCNTL.int_clr.val = int_mask & TOUCH_LL_INTR_MASK_ALL;
}

/**
 * Get the bitmask of touch sensor interrupt status.
 *
 * @return type interrupt type
 */
__attribute__((always_inline))
static inline uint32_t touch_ll_get_intr_status_mask(void)
{
    uint32_t intr_st = RTCCNTL.int_st.val;
    return intr_st & TOUCH_LL_INTR_MASK_ALL;
}

/********************************* Status Info ********************************/
/**
 * Get the current measure channel. Touch sensor measurement is cyclic scan mode.
 *
 * @return
 *      - touch channel number
 */
__attribute__((always_inline))
static inline uint32_t touch_ll_get_current_meas_channel(void)
{
    return SENS.sar_touch_status0.touch_scan_curr;
}

/**
 * Get touch sensor measure status. No block.
 *
 * @return
 *      - If touch sensors measure done.
 */
__attribute__((always_inline))
static inline bool touch_ll_is_measure_done(void)
{
    return (bool)SENS.sar_touch_chn_st.touch_meas_done;
}

/**
 * Get the touch sensor active channel mask, usually used in ISR to decide which channels are 'touched'.
 *
 * @param active_mask The touch channel status. e.g. Touch1 trigger status is `status_mask & (BIT1)`.
 */
__attribute__((always_inline))
static inline void touch_ll_get_active_channel_mask(uint32_t *active_mask)
{
    *active_mask = SENS.sar_touch_chn_st.touch_pad_active;
}

/**************************** Measurement Configuration ***********************/
/**
 * @brief Enable or disable the channel that will be scanned.
 * @note  The shield channel should not be enabled to scan here
 *
 * @param chan_mask The channel mask to be enabled or disabled
 * @param enable Enable or disable the channel mask
 */
__attribute__((always_inline))
static inline void touch_ll_enable_scan_mask(uint16_t chan_mask, bool enable)
{
    uint16_t mask = chan_mask & TOUCH_LL_FULL_CHANNEL_MASK;
    uint16_t prev_mask = RTCCNTL.touch_scan_ctrl.touch_scan_pad_map;
    if (enable) {
        RTCCNTL.touch_scan_ctrl.touch_scan_pad_map = prev_mask | mask;
    } else {
        RTCCNTL.touch_scan_ctrl.touch_scan_pad_map = prev_mask & (~mask);
    }
}

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
    HAL_ASSERT(touch_num > 0);
    SENS.touch_thresh[touch_num - 1].thresh = thresh;
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
    HAL_ASSERT(touch_num > 0);
    return SENS.touch_thresh[touch_num - 1].thresh;
}

/**
 * Set the power on wait cycle
 *
 * @param wait_cycles
 */
static inline void touch_ll_set_power_on_wait_cycle(uint32_t wait_cycles)
{
    //the waiting cycles (in 8MHz) between TOUCH_START and TOUCH_XPD
    HAL_FORCE_MODIFY_U32_REG_FIELD(RTCCNTL.touch_ctrl2, touch_xpd_wait, wait_cycles); //wait volt stable
}

/**
 * Set touch sensor touch sensor charge and discharge times of every measurement on a pad.
 *
 * @param charge_times The times of charge and discharge in each measure process of touch channels.
 *                     The timer frequency is RTC_FAST (about 16M). Range: 0 ~ 0xffff.
 */
static inline void touch_ll_set_charge_times( uint16_t charge_times)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(RTCCNTL.touch_ctrl1, touch_meas_num, charge_times);
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
    HAL_FORCE_MODIFY_U32_REG_FIELD(RTCCNTL.touch_ctrl1, touch_sleep_cycles, interval_ticks);
}

/**
 * Set the Touch pad charge speed.
 *
 * @param touch_num     Touch channel number
 * @param charge_speed  Charge speed of this touch channel
 */
static inline void touch_ll_set_charge_speed(uint32_t touch_num, touch_charge_speed_t charge_speed)
{
#define CHARGE_SPEED_MASK(val, num) ((val) << (29 - (num) * 3))
    uint32_t speed_mask = 0;
    if (touch_num < 10) {
        speed_mask = RTCCNTL.touch_dac.val;
        speed_mask &= ~CHARGE_SPEED_MASK(0x07, touch_num);  // clear the old value
        RTCCNTL.touch_dac.val = speed_mask | CHARGE_SPEED_MASK(charge_speed, touch_num);
    } else {
        speed_mask = RTCCNTL.touch_dac1.val;
        speed_mask &= ~CHARGE_SPEED_MASK(0x07, touch_num - 10);  // clear the old value
        RTCCNTL.touch_dac1.val = speed_mask | CHARGE_SPEED_MASK(charge_speed, touch_num - 10);
    }
#undef CHARGE_SPEED_MASK
}

/**
 * Set the upper limitation of the touch channel voltage while charging
 *
 * @param high_lim      The high(upper) limitation of charge
 */
static inline void touch_ll_set_charge_voltage_high_limit(touch_volt_lim_h_t high_lim)
{
    RTCCNTL.touch_ctrl2.touch_drefh = (uint32_t)high_lim & 0x3;
    RTCCNTL.touch_ctrl2.touch_drange = (uint32_t)high_lim >> 2;
}

/**
 * Set the lower limitation of the touch channel voltage while discharging
 *
 * @param low_lim      The lower limitation of discharge
 */
static inline void touch_ll_set_charge_voltage_low_limit(touch_volt_lim_l_t low_lim)
{
    RTCCNTL.touch_ctrl2.touch_drefl = low_lim;
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
    if (init_charge_volt == TOUCH_INIT_CHARGE_VOLT_FLOAT) {
        RTCIO.touch_pad[touch_num].xpd = 0;
    } else {
        RTCIO.touch_pad[touch_num].xpd = 1;
        RTCIO.touch_pad[touch_num].tie_opt = init_charge_volt;
    }
}

/**
 * Set the connection of the idle channel
 * The idle channel is the channel that is enabled and powered on but not under measurement.
 *
 * @param idle_conn
 */
static inline void touch_ll_set_idle_channel_connection(touch_idle_conn_t idle_conn)
{
    RTCCNTL.touch_scan_ctrl.touch_inactive_connection = idle_conn;
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
    RTCCNTL.touch_scan_ctrl.touch_scan_pad_map  = enable_mask;
    SENS.sar_touch_conf.touch_outen = enable_mask;
}

/**
 * Set the timeout to enable or disable the check for all touch sensor channels measurements.
 * When the touch reading of a touch channel exceeds the measurement threshold,
 * If enable: a timeout interrupt will be generated and it will go to the next channel measurement.
 * If disable: the FSM is always on the channel, until the measurement of this channel is over.
 *
 * @param timeout_cycles The maximum time cycles of the measurement on one channel.
 *                       Set to 0 to disable the timeout.
 *                       Set to non-zero to enable the timeout and set the timeout cycles.
 */
static inline void touch_ll_set_timeout(uint32_t timeout_cycles)
{
    if (timeout_cycles) {
        RTCCNTL.touch_timeout_ctrl.touch_timeout_num = timeout_cycles;
        RTCCNTL.touch_timeout_ctrl.touch_timeout_en = 1;
    } else {
        RTCCNTL.touch_timeout_ctrl.touch_timeout_en = 0;
    }
}

/**
 * Clear all touch sensor channels active status.
 *
 * @note Generally no manual removal is required.
 */
static inline void touch_ll_clear_active_channel_status(void)
{
    SENS.sar_touch_conf.touch_status_clr = 1;
}


/**
 * Select touch sensor dbias to save power in sleep mode.
 *
 * @note If change the dbias, the reading of touch sensor will changed. Users should make sure the threshold.
 */
static inline void touch_ll_set_bias_type(touch_bias_type_t bias_type)
{
    RTCCNTL.touch_ctrl2.touch_dbias = bias_type;
}

/********************************* FSM Operation ******************************/
/**
 * Touch timer trigger measurement and always wait measurement done.
 * Force done for touch timer ensures that the timer always can get the measurement done signal.
 * @note The `force done` signal should last as least one slow clock tick
 */
__attribute__((always_inline))
static inline void touch_ll_force_done_curr_measurement(void)
{
    RTCCNTL.touch_ctrl2.touch_timer_force_done = TOUCH_LL_TIMER_FORCE_DONE;
    RTCCNTL.touch_ctrl2.touch_timer_force_done = TOUCH_LL_TIMER_DONE;
}

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
    touch_ll_force_done_curr_measurement();
    // Set 0 to start by timer, otherwise by software
    RTCCNTL.touch_ctrl2.touch_start_force = !enable;
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
    /**
     * Touch timer trigger measurement and always wait measurement done.
     * Force done for touch timer ensures that the timer always can get the measurement done signal.
     */
    touch_ll_force_done_curr_measurement();
    RTCCNTL.touch_ctrl2.touch_slp_timer_en = 1;
}

/**
 * Stop touch sensor FSM timer.
 *        The measurement action can be triggered by the hardware timer, as well as by the software instruction.
 */
__attribute__((always_inline))
static inline void touch_ll_stop_fsm_repeated_timer(void)
{
    RTCCNTL.touch_ctrl2.touch_slp_timer_en = 0;
    touch_ll_force_done_curr_measurement();
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
    return (bool)RTCCNTL.touch_ctrl2.touch_slp_timer_en;
}

/**
 * Enable the touch sensor FSM start signal from software
 */
__attribute__((always_inline))
static inline void touch_ll_trigger_oneshot_measurement(void)
{
    RTCCNTL.touch_ctrl2.touch_start_en = 1;
    RTCCNTL.touch_ctrl2.touch_start_en = 0;
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

/****************************** Benchmark Operation ***************************/
/**
 * Force reset benchmark to raw data of touch sensor.
 *
 * @note If call this API, make sure enable clock gate(`touch_ll_clkgate`) first.
 * @param chan_mask touch channel mask
 */
__attribute__((always_inline))
static inline void touch_ll_reset_chan_benchmark(uint32_t chan_mask)
{
    SENS.sar_touch_chn_st.touch_channel_clr = chan_mask;
}

static inline void touch_ll_sleep_reset_benchmark(void)
{
    RTCCNTL.touch_approach.touch_slp_channel_clr = 1;
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
    HAL_ASSERT(type <= TOUCH_LL_READ_SMOOTH);
    HAL_ASSERT(touch_num > 0);
    SENS.sar_touch_conf.touch_data_sel = type;
    *data = SENS.sar_touch_status[touch_num - 1].touch_pad_data;
}

/****************************** Filter Configuration **************************/

/**
 * Enable or disable touch sensor filter and detection algorithm.
 * For more details on the detection algorithm, please refer to the application documentation.
 */
static inline void touch_ll_filter_enable(bool enable)
{
    RTCCNTL.touch_filter_ctrl.touch_filter_en = enable;
}


/**
 * Set filter mode. The input of the filter is the raw value of touch reading,
 * and the output of the filter is involved in the judgment of the touch state.
 *
 * @param mode Filter mode type. Refer to ``touch_benchmark_filter_mode_t``.
 */
static inline void touch_ll_filter_set_filter_mode(touch_benchmark_filter_mode_t mode)
{
    RTCCNTL.touch_filter_ctrl.touch_filter_mode = mode;
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
    RTCCNTL.touch_filter_ctrl.touch_jitter_step = step;
}

/**
 * Set the denoise coefficient regarding the denoise level.
 *
 * @param denoise_lvl   Range [0 ~ 4]. 0 = no noise resistance, otherwise higher denoise_lvl means more noise resistance.
 *                      0 = no noise resistance
 *                      1 = noise resistance is 1/4 benchmark
 *                      2 = noise resistance is 3/8 benchmark
 *                      3 = noise resistance is 1/2 benchmark
 *                      4 = noise resistance is 1 benchmark
 */
static inline void touch_ll_filter_set_denoise_level(int denoise_lvl)
{
    HAL_ASSERT(denoise_lvl >= 0 && denoise_lvl <= 4);
    bool always_update = denoise_lvl == 0;
    /* Map denoise level to actual noise threshold coefficients
       denoise_lvl=1 -> noise_thresh=2, 1/4 benchmark
       denoise_lvl=2 -> noise_thresh=1, 3/8 benchmark
       denoise_lvl=3 -> noise_thresh=0, 1/2 benchmark
       denoise_lvl=4 -> noise_thresh=3, 1   benchmark */
    uint32_t noise_thresh = denoise_lvl == 4 ? 3 : 3 - denoise_lvl;

    RTCCNTL.touch_filter_ctrl.touch_bypass_noise_thres = always_update;
    RTCCNTL.touch_filter_ctrl.touch_noise_thres = always_update ? 0 : noise_thresh;

    RTCCNTL.touch_filter_ctrl.touch_bypass_nn_thres = always_update;
    RTCCNTL.touch_filter_ctrl.config2 = always_update ? 0 : noise_thresh;
    RTCCNTL.touch_filter_ctrl.config1 = 0xF;
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
    RTCCNTL.touch_filter_ctrl.config3 = hysteresis;
}

/**
 * Set filter mode. The input to the filter is raw data and the output is the smooth data.
 * The smooth data is used to determine the touch status.
 *
 * @param mode Filter mode type. Refer to ``touch_smooth_filter_mode_t``.
 */
static inline void touch_ll_filter_set_smooth_mode(touch_smooth_filter_mode_t mode)
{
    RTCCNTL.touch_filter_ctrl.touch_smooth_lvl = mode;
}

/**
 * Set debounce count, such as `n`. If the measured values continue to exceed
 * the threshold for `n+1` times, it is determined that the touch sensor state changes.
 *
 * @param dbc_cnt Debounce count value.
 */
static inline void touch_ll_filter_set_debounce(uint32_t dbc_cnt)
{
    RTCCNTL.touch_filter_ctrl.touch_debounce = dbc_cnt;
}

/**************************** Sleep Configurations ****************************/
/**
 * Set the trigger threshold of touch sensor in deep sleep.
 * The threshold determines the sensitivity of the touch sensor.
 * The threshold is the original value of the trigger state minus the benchmark value.
 *
 * @note In general, the touch threshold during sleep can use the threshold parameter parameters before sleep.
 */
static inline void touch_ll_sleep_set_threshold(uint32_t touch_thres)
{
    RTCCNTL.touch_slp_thres.touch_slp_th = touch_thres;
}

/**
 * Set touch channel number for sleep channel.
 *
 * @note Only one touch sensor channel is supported in deep sleep mode.
 * @param touch_num Touch sensor channel number.
 */
static inline void touch_ll_sleep_set_channel_num(uint32_t touch_num)
{
    RTCCNTL.touch_slp_thres.touch_slp_pad = touch_num;
}

/**
 * Get touch channel number for sleep pad.
 *
 * @note Only one touch sensor channel is supported in deep sleep mode.
 * @param touch_num Touch sensor channel number.
 */
static inline void touch_ll_sleep_get_channel_num(uint32_t *touch_num)
{
    *touch_num = RTCCNTL.touch_slp_thres.touch_slp_pad;
}

/**
 * Enable proximity sensing function for sleep channel.
 */
static inline void touch_ll_sleep_enable_proximity_sensing(bool enable)
{
    RTCCNTL.touch_slp_thres.touch_slp_approach_en = enable;
}

/************************* Waterproof Configurations **************************/
/**
 * Enable parameter of waterproof function.
 *
 * The waterproof function includes a shielded channel (TOUCH_PAD_NUM14) and a guard channel.
 * Guard pad is used to detect the large area of water covering the touch panel.
 * Shield pad is used to shield the influence of water droplets covering the touch panel.
 * It is generally designed as a grid and is placed around the touch buttons.
 * @param enable Enable or disable waterproof function.
 */
static inline void touch_ll_waterproof_enable(bool enable)
{
    RTCCNTL.touch_scan_ctrl.touch_shield_pad_en = enable;
}

/**
 * Set touch channel use for guard channel.
 *
 * @param pad_num Touch sensor channel number.
 */
static inline void touch_ll_waterproof_set_guard_chan(uint32_t pad_num)
{
    RTCCNTL.touch_scan_ctrl.touch_out_ring = pad_num;
}

/**
 * Set max equivalent capacitance for shield channel.
 * The equivalent capacitance of the shielded channel can be calculated
 * from the reading of denoise channel.
 *
 * @param pad_num Touch sensor channel number. Refer to ``touch_chan_shield_cap_t``
 */
static inline void touch_ll_waterproof_set_shield_driver(touch_chan_shield_cap_t driver_level)
{
    RTCCNTL.touch_scan_ctrl.touch_bufdrv = driver_level;
}

/****************************** Proximity Sensing *****************************/
/**
 * Set the proximity sensing channel to the specific touch channel
 * To disable the proximity channel, point this pad to `TOUCH_LL_NULL_CHANNEL`
 *
 * @param prox_chan  proximity sensing channel.
 * @param touch_num  The touch channel that supposed to be used as proximity sensing channel
 */
static inline void touch_ll_set_proximity_sensing_channel(uint8_t prox_chan, uint32_t touch_num)
{
    switch (prox_chan) {
        case 0:
            SENS.sar_touch_conf.touch_approach_pad0 = touch_num;
            break;
        case 1:
            SENS.sar_touch_conf.touch_approach_pad1 = touch_num;
            break;
        case 2:
            SENS.sar_touch_conf.touch_approach_pad2 = touch_num;
            break;
        default:
            // invalid proximity channel
            abort();
    }
}

/**
 * Set the total scan times of the proximity sensing channel.
 *
 * @param scan_times The total scan times of the proximity sensing channel
 */
static inline void touch_ll_proximity_set_total_scan_times(uint32_t scan_times)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(RTCCNTL.touch_approach, touch_approach_meas_time, scan_times);
}

/**
 * Get the total scan times of the proximity sensing channel.
 *
 * @return
 *      - The total scan times of the proximity sensing channel
 */
__attribute__((always_inline))
static inline uint32_t touch_ll_proximity_get_total_scan_times(void)
{
    return HAL_FORCE_READ_U32_REG_FIELD(RTCCNTL.touch_approach, touch_approach_meas_time);
}

/**
 * Get the current scan count for proximity channel.
 *
 * @param touch_num Touch channel number.
 * @return
 *      - Current scan count for proximity channel
 */
__attribute__((always_inline))
static inline uint32_t touch_ll_proximity_get_curr_scan_cnt(uint32_t touch_num)
{
    if (SENS.sar_touch_conf.touch_approach_pad0 == touch_num) {
        return HAL_FORCE_READ_U32_REG_FIELD(SENS.sar_touch_appr_status, touch_approach_pad0_cnt);
    } else if (SENS.sar_touch_conf.touch_approach_pad1 == touch_num) {
        return HAL_FORCE_READ_U32_REG_FIELD(SENS.sar_touch_appr_status, touch_approach_pad1_cnt);
    } else if (SENS.sar_touch_conf.touch_approach_pad2 == touch_num) {
        return HAL_FORCE_READ_U32_REG_FIELD(SENS.sar_touch_appr_status, touch_approach_pad2_cnt);
    }
    return 0;
}

/******************************* Denoise Channel ******************************/
/**
 * Enable denoise function.
 * T0 is an internal channel that does not have a corresponding external GPIO.
 * T0 will work simultaneously with the measured channel Tn. Finally, the actual
 * measured value of Tn is the value after subtracting lower bits of T0.
 * This denoise function filters out interference introduced on all channels,
 * such as noise introduced by the power supply and external EMI.
 * @param enable  enable the denoise channel
 */
static inline void touch_ll_denoise_enable(bool enable)
{
    RTCCNTL.touch_scan_ctrl.touch_denoise_en = enable;
}

/**
 * Set internal reference capacitance of denoise channel.
 * Select the appropriate internal reference capacitance value so that
 * the reading of denoise channel is closest to the reading of the channel being measured.
 *
 * @param capacitance   Reference capacitance level.
 */
static inline void touch_ll_denoise_set_reference_cap(touch_denoise_chan_cap_t capacitance)
{
    RTCCNTL.touch_ctrl2.touch_refc = capacitance;
}

/**
 * Set denoise resolution of denoise channel.
 * Determined by measuring the noise amplitude of the denoise channel.
 *
 * @param resolution Denoise resolution of denoise channel.
 */
static inline void touch_ll_denoise_set_resolution(touch_denoise_chan_resolution_t resolution)
{
    RTCCNTL.touch_scan_ctrl.touch_denoise_res = resolution;
}

/**
 * Read denoise measure value (TOUCH_PAD_NUM0).
 *
 * @param denoise value of denoise.
 */
static inline void touch_ll_denoise_read_data(uint32_t *data)
{
    *data =  SENS.sar_touch_denoise.touch_denoise_data;
}

/******************************************************************************/
/*                   Legacy APIs (to be removed in esp-idf v6.0)              */
/******************************************************************************/
#include "hal/touch_sensor_legacy_types.h"
/**
 * Set touch sensor touch sensor times of charge and discharge.
 *
 * @param meas_timers The times of charge and discharge in each measure process of touch channels.
 *                     The timer frequency is 8Mhz. Range: 0 ~ 0xffff.
 */
static inline void touch_ll_set_meas_times(uint16_t meas_time)
{
    //The times of charge and discharge in each measure process of touch channels.
    HAL_FORCE_MODIFY_U32_REG_FIELD(RTCCNTL.touch_ctrl1, touch_meas_num, meas_time);
    //the waiting cycles (in 8MHz) between TOUCH_START and TOUCH_XPD
    HAL_FORCE_MODIFY_U32_REG_FIELD(RTCCNTL.touch_ctrl2, touch_xpd_wait, TOUCH_LL_PAD_MEASURE_WAIT_MAX); //wait volt stable
}

/**
 * Get touch sensor times of charge and discharge.
 *
 * @param meas_times Pointer to accept times count of charge and discharge.
 */
static inline void touch_ll_get_measure_times(uint16_t *meas_time)
{
    *meas_time = HAL_FORCE_READ_U32_REG_FIELD(RTCCNTL.touch_ctrl1, touch_meas_num);
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
    // touch sensor sleep cycle Time = sleep_cycle / RTC_SLOW_CLK(150k)
    HAL_FORCE_MODIFY_U32_REG_FIELD(RTCCNTL.touch_ctrl1, touch_sleep_cycles, sleep_time);
}

/**
 * Get touch sensor sleep time.
 *
 * @param sleep_cycle  Pointer to accept sleep cycle number.
 */
static inline void touch_ll_get_sleep_time(uint16_t *sleep_time)
{
    *sleep_time = HAL_FORCE_READ_U32_REG_FIELD(RTCCNTL.touch_ctrl1, touch_sleep_cycles);
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
    RTCCNTL.touch_ctrl2.touch_drefh = refh;
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
 * Set touch sensor high voltage attenuation of charge. The actual charge threshold is high voltage threshold minus attenuation value.
 * The touch sensor measures the channel capacitance value by charging and discharging the channel.
 * So the high threshold should be less than the supply voltage.
 *
 * @param refh The high voltage threshold of charge.
 */
static inline void touch_ll_set_voltage_attenuation(touch_volt_atten_t atten)
{
    RTCCNTL.touch_ctrl2.touch_drange = atten;
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
#define PAD_SLOP_MASK(val, num) ((val) << (29 - (num) * 3))
    uint32_t curr_slop = 0;
    if (touch_num < TOUCH_PAD_NUM10) {
        curr_slop = RTCCNTL.touch_dac.val;
        curr_slop &= ~PAD_SLOP_MASK(0x07, touch_num);  // clear the old value
        RTCCNTL.touch_dac.val = curr_slop | PAD_SLOP_MASK(slope, touch_num);
    } else {
        curr_slop = RTCCNTL.touch_dac1.val;
        curr_slop &= ~PAD_SLOP_MASK(0x07, touch_num - TOUCH_PAD_NUM10);  // clear the old value
        RTCCNTL.touch_dac1.val = curr_slop | PAD_SLOP_MASK(slope, touch_num - TOUCH_PAD_NUM10);
    }
#undef PAD_SLOP_MASK
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
    if (touch_num < TOUCH_PAD_NUM10) {
        *slope = (touch_cnt_slope_t)((RTCCNTL.touch_dac.val >> (29 - touch_num * 3)) & 0x07);
    } else {
        *slope = (touch_cnt_slope_t)((RTCCNTL.touch_dac1.val >> (29 - (touch_num - TOUCH_PAD_NUM10) * 3)) & 0x07);
    }
}

/**
 * Set initial voltage state of touch channel for each measurement.
 *
 * @param touch_num Touch pad index.
 * @param opt Initial voltage state.
 */
static inline void touch_ll_set_tie_option(touch_pad_t touch_num, touch_tie_opt_t opt)
{
    if (opt == TOUCH_PAD_TIE_OPT_FLOAT) {
        RTCIO.touch_pad[touch_num].xpd = 0;
    } else {
        RTCIO.touch_pad[touch_num].xpd = 1;
        RTCIO.touch_pad[touch_num].tie_opt = opt;
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
    if (RTCIO.touch_pad[touch_num].xpd) {
        *opt = (touch_tie_opt_t)RTCIO.touch_pad[touch_num].tie_opt;
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
 * Enable/disable clock gate of touch sensor.
 *
 * @param enable true/false.
 */
static inline void touch_ll_clkgate(bool enable)
{
    RTCCNTL.touch_ctrl2.touch_clkgate_en = enable; //enable touch clock for FSM. or force enable.
}

/**
 * Get touch sensor FSM state.
 * @return
 *          - true: fsm state is open.
 *          - false: fsm state is close.
 */
static inline bool touch_ll_clkgate_get_state(void)
{
    return RTCCNTL.touch_ctrl2.touch_clkgate_en;
}

/**
 * Touch timer trigger measurement and always wait measurement done.
 * Force done for touch timer ensures that the timer always can get the measurement done signal.
 */
static inline void touch_ll_timer_force_done(void)
{
    RTCCNTL.touch_ctrl2.touch_timer_force_done = TOUCH_LL_TIMER_FORCE_DONE;
    RTCCNTL.touch_ctrl2.touch_timer_force_done = TOUCH_LL_TIMER_DONE;
}

/**
 * Start touch sensor FSM timer.
 *        The measurement action can be triggered by the hardware timer, as well as by the software instruction.
 */
static inline void touch_ll_start_fsm(void)
{
    /**
     * Touch timer trigger measurement and always wait measurement done.
     * Force done for touch timer ensures that the timer always can get the measurement done signal.
     */
    RTCCNTL.touch_ctrl2.touch_timer_force_done = TOUCH_LL_TIMER_FORCE_DONE;
    RTCCNTL.touch_ctrl2.touch_timer_force_done = TOUCH_LL_TIMER_DONE;
    RTCCNTL.touch_ctrl2.touch_slp_timer_en = (RTCCNTL.touch_ctrl2.touch_start_force == TOUCH_FSM_MODE_TIMER ? 1 : 0);
}

/**
 * Stop touch sensor FSM timer.
 *        The measurement action can be triggered by the hardware timer, as well as by the software instruction.
 */
__attribute__((always_inline))
static inline void touch_ll_stop_fsm(void)
{
    RTCCNTL.touch_ctrl2.touch_start_en = 0; //stop touch fsm
    RTCCNTL.touch_ctrl2.touch_slp_timer_en = 0;
    RTCCNTL.touch_ctrl2.touch_timer_force_done = TOUCH_LL_TIMER_FORCE_DONE;
    RTCCNTL.touch_ctrl2.touch_timer_force_done = TOUCH_LL_TIMER_DONE;
}

/**
 * Get touch sensor FSM timer state.
 * @return
 *     - true: FSM enabled
 *     - false: FSM disabled
 */
static inline bool touch_ll_get_fsm_state(void)
{
    return (bool)RTCCNTL.touch_ctrl2.touch_slp_timer_en;
}

/**
 * Trigger a touch sensor measurement, only support in SW mode of FSM.
 */
static inline void touch_ll_start_sw_meas(void)
{
    RTCCNTL.touch_ctrl2.touch_start_en = 1;
    RTCCNTL.touch_ctrl2.touch_start_en = 0;
}

/**
 * Set the trigger threshold of touch sensor.
 * The threshold determines the sensitivity of the touch sensor.
 * The threshold is the original value of the trigger state minus the benchmark value.
 *
 * @note  If set "TOUCH_PAD_THRESHOLD_MAX", the touch is never be triggered.
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
 * The threshold is the original value of the trigger state minus the benchmark value.
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
    RTCCNTL.touch_scan_ctrl.touch_scan_pad_map  |= (enable_mask & TOUCH_PAD_BIT_MASK_ALL);
    SENS.sar_touch_conf.touch_outen |= (enable_mask & TOUCH_PAD_BIT_MASK_ALL);
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
                   & TOUCH_PAD_BIT_MASK_ALL;
}

/**
 * Disable touch sensor channel by bitmask.
 *
 * @param enable_mask bitmask of touch sensor scan group.
 *        e.g. TOUCH_PAD_NUM1 -> BIT(1)
 */
static inline void touch_ll_clear_channel_mask(uint16_t disable_mask)
{
    SENS.sar_touch_conf.touch_outen &= ~(disable_mask & TOUCH_PAD_BIT_MASK_ALL);
    RTCCNTL.touch_scan_ctrl.touch_scan_pad_map  &= ~(disable_mask & TOUCH_PAD_BIT_MASK_ALL);
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
 * Get touch sensor raw data (touch sensor counter value) from register. No block.
 *
 * @param touch_num touch pad index.
 * @return touch_value pointer to accept touch sensor value.
 */
static inline uint32_t IRAM_ATTR touch_ll_read_raw_data(touch_pad_t touch_num)
{
    SENS.sar_touch_conf.touch_data_sel = TOUCH_LL_READ_RAW;
    return SENS.sar_touch_status[touch_num - 1].touch_pad_data;
}

/**
 * Reset the whole of touch module.
 *
 * @note Call this function after `touch_pad_fsm_stop`.
 */
static inline void touch_ll_reset(void)
{
    RTCCNTL.touch_ctrl2.touch_reset = 0;
    RTCCNTL.touch_ctrl2.touch_reset = 1;
    RTCCNTL.touch_ctrl2.touch_reset = 0;    // Should be set 0.
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
static inline void touch_ll_set_idle_channel_connect(touch_pad_conn_type_t type)
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
static inline void touch_ll_get_idle_channel_connect(touch_pad_conn_type_t *type)
{
    *type = (touch_pad_conn_type_t)(RTCCNTL.touch_scan_ctrl.touch_inactive_connection);
}

/**
 * Enable touch sensor interrupt by bitmask.
 *
 * @param type interrupt type
 */
static inline void touch_ll_intr_enable(touch_pad_intr_mask_t int_mask)
{
    if (int_mask & TOUCH_PAD_INTR_MASK_DONE) {
        RTCCNTL.int_ena_w1ts.rtc_touch_done_w1ts = 1;
    }
    if (int_mask & TOUCH_PAD_INTR_MASK_ACTIVE) {
        RTCCNTL.int_ena_w1ts.rtc_touch_active_w1ts = 1;
    }
    if (int_mask & TOUCH_PAD_INTR_MASK_INACTIVE) {
        RTCCNTL.int_ena_w1ts.rtc_touch_inactive_w1ts = 1;
    }
    if (int_mask & TOUCH_PAD_INTR_MASK_SCAN_DONE) {
        RTCCNTL.int_ena_w1ts.rtc_touch_scan_done_w1ts = 1;
    }
    if (int_mask & TOUCH_PAD_INTR_MASK_TIMEOUT) {
        RTCCNTL.int_ena_w1ts.rtc_touch_timeout_w1ts = 1;
    }
    if (int_mask & TOUCH_PAD_INTR_MASK_PROXI_MEAS_DONE) {
        RTCCNTL.int_ena_w1ts.rtc_touch_approach_loop_done_w1ts = 1;
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
        RTCCNTL.int_ena_w1tc.rtc_touch_done_w1tc = 1;
    }
    if (int_mask & TOUCH_PAD_INTR_MASK_ACTIVE) {
        RTCCNTL.int_ena_w1tc.rtc_touch_active_w1tc = 1;
    }
    if (int_mask & TOUCH_PAD_INTR_MASK_INACTIVE) {
        RTCCNTL.int_ena_w1tc.rtc_touch_inactive_w1tc = 1;
    }
    if (int_mask & TOUCH_PAD_INTR_MASK_SCAN_DONE) {
        RTCCNTL.int_ena_w1tc.rtc_touch_scan_done_w1tc = 1;
    }
    if (int_mask & TOUCH_PAD_INTR_MASK_TIMEOUT) {
        RTCCNTL.int_ena_w1tc.rtc_touch_timeout_w1tc = 1;
    }
    if (int_mask & TOUCH_PAD_INTR_MASK_PROXI_MEAS_DONE) {
        RTCCNTL.int_ena_w1tc.rtc_touch_approach_loop_done_w1tc = 1;
    }
}

/**
 * Clear touch sensor interrupt by bitmask.
 *
 * @param int_mask Pad mask to clear interrupts
 */
static inline void touch_ll_intr_clear(touch_pad_intr_mask_t int_mask)
{
    if (int_mask & TOUCH_PAD_INTR_MASK_DONE) {
        RTCCNTL.int_clr.rtc_touch_done = 1;
    }
    if (int_mask & TOUCH_PAD_INTR_MASK_ACTIVE) {
        RTCCNTL.int_clr.rtc_touch_active = 1;
    }
    if (int_mask & TOUCH_PAD_INTR_MASK_INACTIVE) {
        RTCCNTL.int_clr.rtc_touch_inactive = 1;
    }
    if (int_mask & TOUCH_PAD_INTR_MASK_SCAN_DONE) {
        RTCCNTL.int_clr.rtc_touch_scan_done = 1;
    }
    if (int_mask & TOUCH_PAD_INTR_MASK_TIMEOUT) {
        RTCCNTL.int_clr.rtc_touch_timeout = 1;
    }
    if (int_mask & TOUCH_PAD_INTR_MASK_PROXI_MEAS_DONE) {
        RTCCNTL.int_clr.rtc_touch_approach_loop_done = 1;
    }
}

/**
 * Get the bitmask of touch sensor interrupt status.
 *
 * @return type interrupt type
 */
static inline uint32_t touch_ll_read_intr_status_mask(void)
{
    typeof(RTCCNTL.int_st) intr_st;
    intr_st.val = RTCCNTL.int_st.val;
    uint32_t intr_msk = 0;

    if (intr_st.rtc_touch_done) {
        intr_msk |= TOUCH_PAD_INTR_MASK_DONE;
    }
    if (intr_st.rtc_touch_active) {
        intr_msk |= TOUCH_PAD_INTR_MASK_ACTIVE;
    }
    if (intr_st.rtc_touch_inactive) {
        intr_msk |= TOUCH_PAD_INTR_MASK_INACTIVE;
    }
    if (intr_st.rtc_touch_scan_done) {
        intr_msk |= TOUCH_PAD_INTR_MASK_SCAN_DONE;
    }
    if (intr_st.rtc_touch_timeout) {
        intr_msk |= TOUCH_PAD_INTR_MASK_TIMEOUT;
    }
    if (intr_st.rtc_touch_approach_loop_done) {
        intr_msk |= TOUCH_PAD_INTR_MASK_PROXI_MEAS_DONE;
    }
    return (intr_msk & TOUCH_PAD_INTR_MASK_ALL);
}

/**
 * Enable the timeout check for all touch sensor channels measurements.
 * When the touch reading of a touch channel exceeds the measurement threshold,
 * If enable: a timeout interrupt will be generated and it will go to the next channel measurement.
 * If disable: the FSM is always on the channel, until the measurement of this channel is over.
 *
 * @note Set the timeout threshold correctly before enabling it.
 */
static inline void touch_ll_timeout_enable(void)
{
    RTCCNTL.touch_timeout_ctrl.touch_timeout_en = 1;
}

/**
 * Disable the timeout check for all touch sensor channels measurements.
 * When the touch reading of a touch channel exceeds the measurement threshold,
 * If enable: a timeout interrupt will be generated and it will go to the next channel measurement.
 * If disable: the FSM is always on the channel, until the measurement of this channel is over.
 *
 * @note Set the timeout threshold correctly before enabling it.
 */
static inline void touch_ll_timeout_disable(void)
{
    RTCCNTL.touch_timeout_ctrl.touch_timeout_en = 0;
}

/**
 * Set timeout threshold for all touch sensor channels measurements.
 * Compared with touch readings.
 *
 * @param threshold Set to the maximum time measured on one channel.
 */
static inline void touch_ll_timeout_set_threshold(uint32_t threshold)
{
    RTCCNTL.touch_timeout_ctrl.touch_timeout_num = threshold;
}

/**
 * Get timeout threshold for all touch sensor channels measurements.
 * Compared with touch readings.
 *
 * @param threshold Point to timeout threshold.
 */
static inline void touch_ll_timeout_get_threshold(uint32_t *threshold)
{
    *threshold = RTCCNTL.touch_timeout_ctrl.touch_timeout_num;
}

/************************ Filter register setting ************************/

/**
 * Get smoothed data that obtained by filtering the raw data.
 *
 * @param touch_num touch pad index
 * @param smooth_data pointer to smoothed data
 */
static inline void IRAM_ATTR touch_ll_filter_read_smooth(touch_pad_t touch_num, uint32_t *smooth_data)
{
    SENS.sar_touch_conf.touch_data_sel = TOUCH_LL_READ_SMOOTH;
    *smooth_data = SENS.sar_touch_status[touch_num - 1].touch_pad_data;
}

/**
 * Get benchmark value of touch sensor.
 *
 * @note After initialization, the benchmark value is the maximum during the first measurement period.
 * @param touch_num touch pad index
 * @param touch_value pointer to accept touch sensor value
 */
static inline void IRAM_ATTR touch_ll_read_benchmark(touch_pad_t touch_num, uint32_t *benchmark)
{
    SENS.sar_touch_conf.touch_data_sel = TOUCH_LL_READ_BENCHMARK;
    *benchmark = SENS.sar_touch_status[touch_num - 1].touch_pad_data;
}

/**
 * Force reset benchmark to raw data of touch sensor.
 *
 * @note If call this API, make sure enable clock gate(`touch_ll_clkgate`) first.
 * @param touch_num touch pad index
 *                  - TOUCH_PAD_MAX Reset baseline of all channels.
 */
static inline void touch_ll_reset_benchmark(touch_pad_t touch_num)
{
    /* Clear touch channels to initialize the channel value (benchmark, raw_data).
     */
    if (touch_num == TOUCH_PAD_MAX) {
        SENS.sar_touch_chn_st.touch_channel_clr = TOUCH_PAD_BIT_MASK_ALL;
    } else {
        SENS.sar_touch_chn_st.touch_channel_clr = (1U << touch_num);
    }
}

/**
 * Get filter mode. The input of the filter is the raw value of touch reading,
 * and the output of the filter is involved in the judgment of the touch state.
 *
 * @param mode Filter mode type. Refer to ``touch_filter_mode_t``.
 */
static inline void touch_ll_filter_get_filter_mode(touch_filter_mode_t *mode)
{
    *mode = (touch_filter_mode_t)RTCCNTL.touch_filter_ctrl.touch_filter_mode;
}

/**
 * Get filter mode. The smooth data is used to determine the touch status.
 *
 * @param mode Filter mode type. Refer to ``touch_smooth_mode_t``.
 */
static inline void touch_ll_filter_get_smooth_mode(touch_smooth_mode_t *mode)
{
    *mode = (touch_smooth_mode_t)(RTCCNTL.touch_filter_ctrl.touch_smooth_lvl);
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
 * Set noise threshold coefficient. Higher = More noise resistance.
 * The actual noise should be less than (noise coefficient * touch threshold).
 * Range: 0 ~ 3. The coefficient is 0: 4/8;  1: 3/8;   2: 2/8;   3: 1;
 *
 * @param hys_thr Noise threshold coefficient.
 */
static inline void touch_ll_filter_set_noise_thres(uint32_t noise_thr)
{
    RTCCNTL.touch_filter_ctrl.touch_noise_thres = noise_thr;
    RTCCNTL.touch_filter_ctrl.config2 = noise_thr;
    RTCCNTL.touch_filter_ctrl.config1 = 0xF;
    RTCCNTL.touch_filter_ctrl.config3 = 2;
}

/**
 * Get noise threshold coefficient. Higher = More noise resistance.
 * The actual noise should be less than (noise coefficient * touch threshold).
 * Range: 0 ~ 3. The coefficient is 0: 4/8;  1: 3/8;   2: 2/8;   3: 1;
 *
 * @param noise_thr Noise threshold coefficient.
 */
static inline void touch_ll_filter_get_noise_thres(uint32_t *noise_thr)
{
    *noise_thr = RTCCNTL.touch_filter_ctrl.touch_noise_thres;
}

/**
 * Get jitter filter step size.
 * If filter mode is jitter, should set filter step for jitter.
 * Range: 0 ~ 15
 *
 * @param step The step size of the data change.
 */
static inline void touch_ll_filter_get_jitter_step(uint32_t *step)
{
    *step = RTCCNTL.touch_filter_ctrl.touch_jitter_step;
}

/************************ Denoise register setting ************************/

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
    *cap_level = (touch_pad_denoise_cap_t)(RTCCNTL.touch_ctrl2.touch_refc);
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
    *grade = (touch_pad_denoise_grade_t)(RTCCNTL.touch_scan_ctrl.touch_denoise_res);
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
    *pad_num = (touch_pad_t)(RTCCNTL.touch_scan_ctrl.touch_out_ring);
}

/**
 * Get max equivalent capacitance for shield channel.
 * The equivalent capacitance of the shielded channel can be calculated
 * from the reading of denoise channel.
 *
 * @param pad_num Touch sensor channel number.
 */
static inline void touch_ll_waterproof_get_shield_driver(touch_pad_shield_driver_t *driver_level)
{
    *driver_level = (touch_pad_shield_driver_t)(RTCCNTL.touch_scan_ctrl.touch_bufdrv);
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
    prox_pad[0] = (touch_pad_t)(SENS.sar_touch_conf.touch_approach_pad0);
    prox_pad[1] = (touch_pad_t)(SENS.sar_touch_conf.touch_approach_pad1);
    prox_pad[2] = (touch_pad_t)(SENS.sar_touch_conf.touch_approach_pad2);
}

/**
 * Set cumulative measurement times for proximity pad.
 *
 * @param times The cumulative number of measurement cycles.
 */
static inline void touch_ll_proximity_set_meas_times(uint32_t times)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(RTCCNTL.touch_approach, touch_approach_meas_time, times);
}

/**
 * Get cumulative measurement times for proximity pad.
 *
 * @param times The cumulative number of measurement cycles.
 */
static inline void touch_ll_proximity_get_meas_times(uint32_t *times)
{
    *times = HAL_FORCE_READ_U32_REG_FIELD(RTCCNTL.touch_approach, touch_approach_meas_time);
}

/**
 * Read current cumulative measurement times for proximity pad.
 *
 * @param times The cumulative number of measurement cycles.
 */
static inline void touch_ll_proximity_read_meas_cnt(touch_pad_t touch_num, uint32_t *cnt)
{
    if (SENS.sar_touch_conf.touch_approach_pad0 == touch_num) {
        *cnt = HAL_FORCE_READ_U32_REG_FIELD(SENS.sar_touch_appr_status, touch_approach_pad0_cnt);
    } else if (SENS.sar_touch_conf.touch_approach_pad1 == touch_num) {
        *cnt = HAL_FORCE_READ_U32_REG_FIELD(SENS.sar_touch_appr_status, touch_approach_pad1_cnt);
    } else if (SENS.sar_touch_conf.touch_approach_pad2 == touch_num) {
        *cnt = HAL_FORCE_READ_U32_REG_FIELD(SENS.sar_touch_appr_status, touch_approach_pad2_cnt);
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
 * Get the trigger threshold of touch sensor in deep sleep.
 * The threshold determines the sensitivity of the touch sensor.
 * The threshold is the original value of the trigger state minus the benchmark value.
 *
 * @note In general, the touch threshold during sleep can use the threshold parameter parameters before sleep.
 */
static inline void touch_ll_sleep_get_threshold(uint32_t *touch_thres)
{
    *touch_thres = RTCCNTL.touch_slp_thres.touch_slp_th;
}

/**
 * Get proximity function status for sleep pad.
 */
static inline bool touch_ll_sleep_is_proximity_enabled(void)
{
    return (bool)RTCCNTL.touch_slp_thres.touch_slp_approach_en;
}

/**
 * Read benchmark of touch sensor for sleep pad.
 *
 * @param benchmark Pointer to accept touch sensor benchmark value.
 */
static inline void touch_ll_sleep_read_benchmark(uint32_t *benchmark)
{
    SENS.sar_touch_conf.touch_data_sel = TOUCH_LL_READ_BENCHMARK;
    *benchmark = SENS.sar_touch_slp_status.touch_slp_data;
}

static inline void touch_ll_sleep_read_smooth(uint32_t *smooth_data)
{
    SENS.sar_touch_conf.touch_data_sel = TOUCH_LL_READ_SMOOTH;
    *smooth_data = SENS.sar_touch_slp_status.touch_slp_data;
}

/* Workaround: Note: sleep pad raw data is not in `sar_touch_slp_status` */
static inline void touch_ll_sleep_read_data(uint32_t *raw_data)
{
    uint32_t touch_num = RTCCNTL.touch_slp_thres.touch_slp_pad;
    SENS.sar_touch_conf.touch_data_sel = TOUCH_LL_READ_RAW;
    *raw_data = SENS.sar_touch_status[touch_num - 1].touch_pad_data;
}

/**
 * Get the data of the touch channel according to the types
 *
 * @param sample_cfg_id The sample configuration index
 * @param type  data type
 *              0/1: TOUCH_LL_READ_RAW, the raw data of the touch channel
 *              2:   TOUCH_LL_READ_BENCHMARK, benchmark value of touch channel,
 *                   the benchmark value is the maximum during the first measurement period
 *              3:   TOUCH_LL_READ_SMOOTH, the smoothed data that obtained by filtering the raw data.
 * @param smooth_data pointer to smoothed data
 */
__attribute__((always_inline))
static inline void touch_ll_sleep_read_chan_data(uint8_t type, uint32_t *data)
{
    SENS.sar_touch_conf.touch_data_sel = type;
    if (type == TOUCH_LL_READ_RAW) {
        uint32_t touch_num = RTCCNTL.touch_slp_thres.touch_slp_pad;
        *data = SENS.sar_touch_status[touch_num - 1].touch_pad_data;
    } else {
        *data = SENS.sar_touch_slp_status.touch_slp_data;
    }
}


/**
 * Select touch sensor dbias to save power in sleep mode.
 *
 * @note If change the dbias, the reading of touch sensor will changed. Users should make sure the threshold.
 */
static inline void touch_ll_sleep_low_power(bool is_low_power)
{
    RTCCNTL.touch_ctrl2.touch_dbias = is_low_power;
}

/**
 * Read debounce of touch sensor for sleep pad.
 *
 * @param debounce Pointer to accept touch sensor debounce value.
 */
static inline void touch_ll_sleep_read_debounce(uint32_t *debounce)
{
    *debounce = SENS.sar_touch_slp_status.touch_slp_debounce;
}

/**
 * Read proximity count of touch sensor for sleep pad.
 * @param prox_cnt Pointer to accept touch sensor proximity count value.
 */
static inline void touch_ll_sleep_read_proximity_cnt(uint32_t *prox_cnt)
{
    *prox_cnt = HAL_FORCE_READ_U32_REG_FIELD(SENS.sar_touch_appr_status, touch_slp_approach_cnt);
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

#ifdef __cplusplus
}
#endif
