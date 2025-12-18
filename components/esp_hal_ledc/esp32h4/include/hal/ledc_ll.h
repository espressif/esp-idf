/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for LEDC register operations.
// Note that most of the register operations in this layer are non-atomic operations.

#pragma once

#include "hal/assert.h"
#include "hal/ledc_types.h"
#include "soc/ledc_struct.h"
#include "soc/ledc_reg.h"
#include "soc/clk_tree_defs.h"
#include "soc/pcr_struct.h"
#include "soc/soc_caps.h"
#include "soc/soc_etm_source.h"

#define LEDC_LL_GET_HW()           &LEDC

#define LEDC_LL_CHANNEL_SUPPORT_OVF_CNT     1

#define LEDC_LL_DUTY_NUM_MAX       (LEDC_CH0_FADE_PARAM_DUTY_NUM_V)
#define LEDC_LL_DUTY_CYCLE_MAX     (LEDC_CH0_FADE_PARAM_DUTY_CYCLE_V)
#define LEDC_LL_DUTY_SCALE_MAX     (LEDC_CH0_FADE_PARAM_SCALE_V)
#define LEDC_LL_HPOINT_VAL_MAX     (LEDC_HPOINT_CH0_V)
#define LEDC_LL_OVF_CNT_MAX        (LEDC_OVF_NUM_CH0_V + 1)
#define LEDC_LL_FRACTIONAL_BITS    (8)
#define LEDC_LL_FRACTIONAL_MAX     ((1 << LEDC_LL_FRACTIONAL_BITS) - 1)
#define LEDC_LL_GLOBAL_CLOCKS      SOC_LEDC_CLKS
/// Get the mask of the fade end interrupt status register.
#define LEDC_LL_FADE_END_INTR_MASK  (0xffUL << LEDC_DUTY_CHNG_END_CH0_INT_ENA_S)

// Channel tasks: ID, enable register and bit
#define LEDC_LL_ETM_CHANNEL_TASK_ID(group, channel, task) \
    ((uint32_t [1][LEDC_CHANNEL_ETM_TASK_MAX]){{ \
        [LEDC_CHANNEL_ETM_TASK_FADE_SCALE_UPDATE] = LEDC_TASK_DUTY_SCALE_UPDATE_CH0, \
        [LEDC_CHANNEL_ETM_TASK_SIG_OUT_DIS]      = LEDC_TASK_SIG_OUT_DIS_CH0, \
        [LEDC_CHANNEL_ETM_TASK_OVF_CNT_RST]      = LEDC_TASK_OVF_CNT_RST_CH0, \
        [LEDC_CHANNEL_ETM_TASK_FADE_RESTART]     = LEDC_TASK_FADE_RESTART_CH0, \
        [LEDC_CHANNEL_ETM_TASK_FADE_PAUSE]       = LEDC_TASK_FADE_PAUSE_CH0, \
        [LEDC_CHANNEL_ETM_TASK_FADE_RESUME]      = LEDC_TASK_FADE_RESUME_CH0, \
    }}[(group)][(task)] + (channel))

#define LEDC_LL_ETM_CHANNEL_TASK_EN_REG(group, task) \
    ((volatile uint32_t *[1][LEDC_CHANNEL_ETM_TASK_MAX]){{ \
        [LEDC_CHANNEL_ETM_TASK_FADE_SCALE_UPDATE] = (volatile uint32_t *)LEDC_EVT_TASK_EN0_REG, \
        [LEDC_CHANNEL_ETM_TASK_SIG_OUT_DIS]      = (volatile uint32_t *)LEDC_EVT_TASK_EN1_REG, \
        [LEDC_CHANNEL_ETM_TASK_OVF_CNT_RST]      = (volatile uint32_t *)LEDC_EVT_TASK_EN1_REG, \
        [LEDC_CHANNEL_ETM_TASK_FADE_RESTART]     = (volatile uint32_t *)LEDC_EVT_TASK_EN2_REG, \
        [LEDC_CHANNEL_ETM_TASK_FADE_PAUSE]       = (volatile uint32_t *)LEDC_EVT_TASK_EN2_REG, \
        [LEDC_CHANNEL_ETM_TASK_FADE_RESUME]      = (volatile uint32_t *)LEDC_EVT_TASK_EN2_REG, \
    }}[(group)][(task)])

#define LEDC_LL_ETM_CHANNEL_TASK_EN_BIT(group, channel, task) \
    ((uint32_t [1][LEDC_CHANNEL_ETM_TASK_MAX]){{ \
        [LEDC_CHANNEL_ETM_TASK_FADE_SCALE_UPDATE] = BIT(LEDC_TASK_DUTY_SCALE_UPDATE_CH0_EN_S), \
        [LEDC_CHANNEL_ETM_TASK_SIG_OUT_DIS]      = BIT(LEDC_TASK_SIG_OUT_DIS_CH0_EN_S), \
        [LEDC_CHANNEL_ETM_TASK_OVF_CNT_RST]      = BIT(LEDC_TASK_OVF_CNT_RST_CH0_EN_S), \
        [LEDC_CHANNEL_ETM_TASK_FADE_RESTART]     = BIT(LEDC_TASK_FADE_RESTART_CH0_EN_S), \
        [LEDC_CHANNEL_ETM_TASK_FADE_PAUSE]       = BIT(LEDC_TASK_FADE_PAUSE_CH0_EN_S), \
        [LEDC_CHANNEL_ETM_TASK_FADE_RESUME]      = BIT(LEDC_TASK_FADE_RESUME_CH0_EN_S), \
    }}[(group)][(task)] << (channel))

// Channel events: ID, enable register and bit
#define LEDC_LL_ETM_CHANNEL_EVENT_ID(group, channel, event) \
    ((uint32_t [1][LEDC_CHANNEL_ETM_EVENT_MAX]){{ \
        [LEDC_CHANNEL_ETM_EVENT_FADE_END]          = LEDC_EVT_DUTY_CHNG_END_CH0, \
        [LEDC_CHANNEL_ETM_EVENT_REACH_MAX_OVF_CNT] = LEDC_EVT_OVF_CNT_PLS_CH0, \
    }}[(group)][(event)] + (channel))

#define LEDC_LL_ETM_CHANNEL_EVENT_EN_REG(group, event) \
    ((volatile uint32_t *[1][LEDC_CHANNEL_ETM_EVENT_MAX]){{ \
        [LEDC_CHANNEL_ETM_EVENT_FADE_END]          = (volatile uint32_t *)LEDC_EVT_TASK_EN0_REG, \
        [LEDC_CHANNEL_ETM_EVENT_REACH_MAX_OVF_CNT] = (volatile uint32_t *)LEDC_EVT_TASK_EN0_REG, \
    }}[(group)][(event)])

#define LEDC_LL_ETM_CHANNEL_EVENT_EN_BIT(group, channel, event) \
    ((uint32_t [1][LEDC_CHANNEL_ETM_EVENT_MAX]){{ \
        [LEDC_CHANNEL_ETM_EVENT_FADE_END]          = BIT(LEDC_EVT_DUTY_CHNG_END_CH0_EN_S), \
        [LEDC_CHANNEL_ETM_EVENT_REACH_MAX_OVF_CNT] = BIT(LEDC_EVT_OVF_CNT_PLS_CH0_EN_S), \
    }}[(group)][(event)] << (channel))

// Timer tasks: ID, enable register and bit
#define LEDC_LL_ETM_TIMER_TASK_ID(group, timer, task) \
    ((uint32_t [1][LEDC_TIMER_ETM_TASK_MAX]){{ \
        [LEDC_TIMER_ETM_TASK_RST]    = LEDC_TASK_TIMER0_RST, \
        [LEDC_TIMER_ETM_TASK_RESUME] = LEDC_TASK_TIMER0_RESUME, \
        [LEDC_TIMER_ETM_TASK_PAUSE]  = LEDC_TASK_TIMER0_PAUSE, \
    }}[(group)][(task)] + (timer))

#define LEDC_LL_ETM_TIMER_TASK_EN_REG(group, task) \
    ((volatile uint32_t *[1][LEDC_TIMER_ETM_TASK_MAX]){{ \
        [LEDC_TIMER_ETM_TASK_RST]    = (volatile uint32_t *)LEDC_EVT_TASK_EN1_REG, \
        [LEDC_TIMER_ETM_TASK_RESUME] = (volatile uint32_t *)LEDC_EVT_TASK_EN1_REG, \
        [LEDC_TIMER_ETM_TASK_PAUSE]  = (volatile uint32_t *)LEDC_EVT_TASK_EN1_REG, \
    }}[(group)][(task)])

#define LEDC_LL_ETM_TIMER_TASK_EN_BIT(group, timer, task) \
    ((uint32_t [1][LEDC_TIMER_ETM_TASK_MAX]){{ \
        [LEDC_TIMER_ETM_TASK_RST]    = BIT(LEDC_TASK_TIMER0_RST_EN_S), \
        [LEDC_TIMER_ETM_TASK_RESUME] = BIT(LEDC_TASK_TIMER0_PAUSE_RESUME_EN_S), \
        [LEDC_TIMER_ETM_TASK_PAUSE]  = BIT(LEDC_TASK_TIMER0_PAUSE_RESUME_EN_S), \
    }}[(group)][(task)] << (timer))

// Timer events: ID, enable register and bit
#define LEDC_LL_ETM_TIMER_EVENT_ID(group, timer, event) \
    ((uint32_t [1][LEDC_TIMER_ETM_EVENT_MAX]){{ \
        [LEDC_TIMER_ETM_EVENT_OVF] = LEDC_EVT_TIME_OVF_TIMER0, \
    }}[(group)][(event)] + (timer))

#define LEDC_LL_ETM_TIMER_EVENT_EN_REG(group, event) \
    ((volatile uint32_t *[1][LEDC_TIMER_ETM_EVENT_MAX]){{ \
        [LEDC_TIMER_ETM_EVENT_OVF] = (volatile uint32_t *)LEDC_EVT_TASK_EN0_REG, \
    }}[(group)][(event)])

#define LEDC_LL_ETM_TIMER_EVENT_EN_BIT(group, timer, event) \
    ((uint32_t [1][LEDC_TIMER_ETM_EVENT_MAX]){{ \
        [LEDC_TIMER_ETM_EVENT_OVF] = BIT(LEDC_EVT_TIME_OVF_TIMER0_EN_S), \
    }}[(group)][(event)] << (timer))

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable peripheral register clock
 *
 * @param enable    Enable/Disable
 */
static inline void ledc_ll_enable_bus_clock(bool enable)
{
    PCR.ledc_conf.ledc_clk_en = enable;
}

/**
 * @brief Reset whole peripheral register to init value defined by HW design
 */
static inline void ledc_ll_enable_reset_reg(bool enable)
{
    PCR.ledc_conf.ledc_rst_en = enable;
}

/**
 * @brief Enable the power for LEDC memory block
 */
static inline void ledc_ll_enable_mem_power(bool enable)
{
    // No LEDC memory block on H4
}

/**
 * @brief Enable LEDC function clock
 *
 * @param hw Beginning address of the peripheral registers
 * @param en True to enable, false to disable
 *
 * @return None
 */
static inline void ledc_ll_enable_clock(ledc_dev_t *hw, bool en)
{
    (void)hw;
    PCR.ledc_sclk_conf.ledc_sclk_en = en;
}

/**
 * @brief Enable the power for LEDC channel
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param en True to enable, false to disable
 */
static inline void ledc_ll_enable_channel_power(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, bool en)
{
    if (en) {
        hw->ch_power_up_conf.val |= BIT(speed_mode * SOC_LEDC_CHANNEL_NUM + channel_num);
    } else {
        hw->ch_power_up_conf.val &= ~BIT(speed_mode * SOC_LEDC_CHANNEL_NUM + channel_num);
    }
}

/**
 * @brief Enable the power for LEDC timer
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 * @param en True to enable, false to disable
 */
static inline void ledc_ll_enable_timer_power(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_timer_t timer_sel, bool en)
{
    if (en) {
        hw->timer_power_up_conf.val |= BIT(speed_mode * SOC_LEDC_TIMER_NUM + timer_sel);
    } else {
        hw->timer_power_up_conf.val &= ~BIT(speed_mode * SOC_LEDC_TIMER_NUM + timer_sel);
    }
}

/**
 * @brief Set LEDC low speed timer clock
 *
 * @param hw Beginning address of the peripheral registers
 * @param slow_clk_sel LEDC low speed timer clock source
 *
 * @return None
 */
static inline void ledc_ll_set_slow_clk_sel(ledc_dev_t *hw, ledc_slow_clk_sel_t slow_clk_sel)
{
    (void) hw;
    uint32_t clk_sel_val = 3;
    switch (slow_clk_sel) {
    case LEDC_SLOW_CLK_XTAL:
        clk_sel_val = 0;
        break;
    case LEDC_SLOW_CLK_RC_FAST:
        clk_sel_val = 1;
        break;
    case LEDC_SLOW_CLK_PLL_DIV:
        clk_sel_val = 2;
        break;
    default:
        abort();
    }

    PCR.ledc_sclk_conf.ledc_sclk_sel = clk_sel_val;
}

/**
 * @brief Get LEDC low speed timer clock
 *
 * @param hw Beginning address of the peripheral registers
 * @param slow_clk_sel LEDC low speed timer clock source
 *
 * @return None
 */
static inline void ledc_ll_get_slow_clk_sel(ledc_dev_t *hw, ledc_slow_clk_sel_t *slow_clk_sel)
{
    (void) hw;
    switch (PCR.ledc_sclk_conf.ledc_sclk_sel) {
    case 0:
        *slow_clk_sel = LEDC_SLOW_CLK_XTAL;
        break;
    case 1:
        *slow_clk_sel = LEDC_SLOW_CLK_RC_FAST;
        break;
    case 2:
        *slow_clk_sel = LEDC_SLOW_CLK_PLL_DIV;
        break;
    default:
        abort();
    }
}

/**
 * @brief Update LEDC low speed timer
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 *
 * @return None
 */
static inline void ledc_ll_ls_timer_update(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_timer_t timer_sel)
{
    hw->timer_group[speed_mode].timer[timer_sel].conf.para_up = 1;
    // Here, we don't wait for the bit gets cleared since it can take quite long depends on the pwm frequency
}

/**
 * @brief Reset LEDC timer
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 *
 * @return None
 */
static inline void ledc_ll_timer_rst(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_timer_t timer_sel)
{
    hw->timer_group[speed_mode].timer[timer_sel].conf.rst = 1;
    hw->timer_group[speed_mode].timer[timer_sel].conf.rst = 0;
}

/**
 * @brief Pause LEDC timer
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 *
 * @return None
 */
static inline void ledc_ll_timer_pause(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_timer_t timer_sel)
{
    hw->timer_group[speed_mode].timer[timer_sel].conf.pause = 1;
}

/**
 * @brief Resume LEDC timer
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 *
 * @return None
 */
static inline void ledc_ll_timer_resume(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_timer_t timer_sel)
{
    hw->timer_group[speed_mode].timer[timer_sel].conf.pause = 0;
}

/**
 * @brief Set LEDC timer clock divider
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 * @param clock_divider Timer clock divide value, the timer clock is divided from the selected clock source
 *
 * @return None
 */
static inline void ledc_ll_set_clock_divider(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_timer_t timer_sel, uint32_t clock_divider)
{
    hw->timer_group[speed_mode].timer[timer_sel].conf.clk_div = clock_divider;
}

/**
 * @brief Get LEDC timer clock divider
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 * @param clock_divider Timer clock divide value, the timer clock is divided from the selected clock source
 *
 * @return None
 */
static inline void ledc_ll_get_clock_divider(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_timer_t timer_sel, uint32_t *clock_divider)
{
    *clock_divider = hw->timer_group[speed_mode].timer[timer_sel].conf.clk_div;
}

/**
 * @brief Get LEDC timer clock source
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 * @param clk_src Pointer to accept the timer clock source
 *
 * @return None
 */
static inline void ledc_ll_get_clock_source(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_timer_t timer_sel, ledc_clk_src_t *clk_src)
{
    // The target has no timer-specific clock source option
    *clk_src = LEDC_SCLK;
}

/**
 * @brief Set LEDC duty resolution
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 * @param duty_resolution Resolution of duty setting in number of bits. The range of duty values is [0, (2**duty_resolution)]
 *
 * @return None
 */
static inline void ledc_ll_set_duty_resolution(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_timer_t timer_sel, uint32_t duty_resolution)
{
    hw->timer_group[speed_mode].timer[timer_sel].conf.duty_res = duty_resolution;
}

/**
 * @brief Get LEDC duty resolution
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 * @param duty_resolution Pointer to accept the resolution of duty setting in number of bits.
 *
 * @return None
 */
static inline void ledc_ll_get_duty_resolution(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_timer_t timer_sel, uint32_t *duty_resolution)
{
    *duty_resolution = hw->timer_group[speed_mode].timer[timer_sel].conf.duty_res;
}

/**
 * @brief Get LEDC max duty
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 * @param max_duty Pointer to accept the max duty
 *
 * @return None
 */
static inline void ledc_ll_get_max_duty(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_timer_t timer_sel, uint32_t *max_duty)
{
    *max_duty = (1 << (hw->timer_group[speed_mode].timer[timer_sel].conf.duty_res));
}

/**
 * @brief Update channel configure when select low speed mode
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 *
 * @return None
 */
static inline void ledc_ll_ls_channel_update(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num)
{
    hw->channel_group[speed_mode].channel[channel_num].conf0.para_up = 1;
}

/**
 * @brief Set LEDC hpoint value
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param hpoint_val LEDC hpoint value(max: 0xfffff)
 *
 * @return None
 */
static inline void ledc_ll_set_hpoint(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, uint32_t hpoint_val)
{
    hw->channel_group[speed_mode].channel[channel_num].hpoint.hpoint = hpoint_val;
}

/**
 * @brief Get LEDC hpoint value
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param hpoint_val Pointer to accept the LEDC hpoint value(max: 0xfffff)
 *
 * @return None
 */
static inline void ledc_ll_get_hpoint(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, uint32_t *hpoint_val)
{
    *hpoint_val = hw->channel_group[speed_mode].channel[channel_num].hpoint.hpoint;
}

/**
 * @brief Set LEDC the integer part of duty value
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param duty_val LEDC duty value, the range of duty setting is [0, (2**duty_resolution)]
 *
 * @return None
 */
static inline void ledc_ll_set_duty_int_part(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, uint32_t duty_val)
{
    hw->channel_group[speed_mode].channel[channel_num].duty_init.duty = duty_val << 4;
}

/**
 * @brief Get LEDC duty value
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param duty_val Pointer to accept the LEDC duty value
 *
 * @return None
 */
static inline void ledc_ll_get_duty(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, uint32_t *duty_val)
{
    *duty_val = (hw->channel_group[speed_mode].channel[channel_num].duty_r.duty_r >> 4);
}

/**
 * @brief Function to set fade parameters all-in-one
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param dir LEDC duty change direction, increase or decrease
 * @param cycle The duty cycles
 * @param scale The step scale
 * @param step The number of increased or decreased times
 *
 * @return None
 */
static inline void ledc_ll_set_fade_param(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, uint32_t dir, uint32_t cycle, uint32_t scale, uint32_t step)
{
    ledc_chn_fade_param_reg_t fade_param = {
        .duty_inc = dir,
        .duty_cycle = cycle,
        .scale = scale,
        .duty_num = step,
    };
    hw->channel_fade_param_group[speed_mode].channel[channel_num].fade_param.val = fade_param.val;
}

/**
 * @brief Set the output enable
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param sig_out_en The output enable status
 *
 * @return None
 */
__attribute__((always_inline))
static inline void ledc_ll_set_sig_out_en(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, bool sig_out_en)
{
    hw->channel_group[speed_mode].channel[channel_num].conf0.sig_out_en = sig_out_en;
}

/**
 * @brief Set the duty start
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 *
 * @return None
 */
static inline void ledc_ll_set_duty_start(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num)
{
    hw->channel_group[speed_mode].channel[channel_num].conf1.duty_start = 1;
}

/**
 * @brief Set output idle level
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param idle_level The output idle level
 *
 * @return None
 */
__attribute__((always_inline))
static inline void ledc_ll_set_idle_level(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, uint32_t idle_level)
{
    hw->channel_group[speed_mode].channel[channel_num].conf0.idle_lv = idle_level & 0x1;
}

/**
 * @brief Set fade end interrupt enable
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param fade_end_intr_en The fade end interrupt enable status
 *
 * @return None
 */
static inline void ledc_ll_set_fade_end_intr(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, bool fade_end_intr_en)
{
    uint32_t value = hw->int_ena.val;
    uint32_t int_en_base = LEDC_DUTY_CHNG_END_CH0_INT_ENA_S;
    hw->int_ena.val = fade_end_intr_en ? (value | BIT(int_en_base + channel_num)) : (value & (~(BIT(int_en_base + channel_num))));
}

/**
 * @brief Get fade end interrupt status
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param intr_status The fade end interrupt status
 *
 * @return None
 */
static inline void ledc_ll_get_fade_end_intr_status(ledc_dev_t *hw, ledc_mode_t speed_mode, uint32_t *intr_status)
{
    uint32_t value = hw->int_st.val;
    uint32_t int_en_base = LEDC_DUTY_CHNG_END_CH0_INT_ENA_S;
    *intr_status = (value >> int_en_base) & 0xff;
}

/**
 * @brief Get the address of the fade end interrupt status register.
 *
 * @param hw Beginning address of the peripheral registers
 * @return Pointer to the fade end interrupt status register.
 */
static inline volatile void* ledc_ll_get_fade_end_intr_addr(ledc_dev_t *hw)
{
    return &hw->int_st.val;
}

/**
 * @brief Clear fade end interrupt status
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 *
 * @return None
 */
static inline void ledc_ll_clear_fade_end_intr_status(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num)
{
    uint32_t int_en_base = LEDC_DUTY_CHNG_END_CH0_INT_ENA_S;
    hw->int_clr.val = BIT(int_en_base + channel_num);
}

/**
 * @brief Set timer index of the specified channel
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 *
 * @return None
 */
static inline void ledc_ll_bind_channel_timer(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, ledc_timer_t timer_sel)
{
    hw->channel_group[speed_mode].channel[channel_num].conf0.timer_sel = timer_sel;
}

/**
 * @brief Get timer index of the specified channel
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param timer_sel Pointer to accept the LEDC timer index
 *
 * @return None
 */
static inline void ledc_ll_get_channel_timer(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, ledc_timer_t *timer_sel)
{
    *timer_sel = (ledc_timer_t)(hw->channel_group[speed_mode].channel[channel_num].conf0.timer_sel);
}

/**
 * @brief Enable or disable ETM event/task
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param reg_addr Register address to control the ETM event/task
 * @param bit Bit position in the register
 * @param enable Enable or disable the ETM event/task
 *
 * @return None
 */
static inline void ledc_ll_etm_enable_evt_task(ledc_dev_t *hw, ledc_mode_t speed_mode, volatile uint32_t *reg_addr, uint32_t bit, bool enable)
{
    if (enable) {
        REG_SET_BIT(reg_addr, bit);
    } else {
        REG_CLR_BIT(reg_addr, bit);
    }
}

/**
 * @brief Enable or disable the timer overflow counter for the specified channel
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel LEDC channel index (0-5), select from ledc_channel_t
 * @param enable True to enable; false to disable
 *
 * @return None
 */
static inline void ledc_ll_channel_enable_timer_ovt_cnt(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel, bool enable)
{
    hw->channel_group[speed_mode].channel[channel].conf0.ovf_cnt_en = enable;
}

/**
 * @brief Set the maximum timer overflow count for the specified channel
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel LEDC channel index (0-5), select from ledc_channel_t
 * @param max_ovf_cnt The maximum timer overflow count
 *
 * @return None
 */
static inline void ledc_ll_channel_set_maximum_timer_ovf_cnt(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t max_ovf_cnt)
{
    HAL_ASSERT(max_ovf_cnt > 0 && max_ovf_cnt <= LEDC_LL_OVF_CNT_MAX);
    hw->channel_group[speed_mode].channel[channel].conf0.ovf_num = max_ovf_cnt - 1;
}

/**
 * @brief Reset the timer overflow counter for the specified channel
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel LEDC channel index (0-5), select from ledc_channel_t
 *
 * @return None
 */
static inline void ledc_ll_channel_reset_timer_ovf_cnt(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel)
{
    hw->channel_group[speed_mode].channel[channel].conf0.ovf_cnt_reset = 1;
}

#ifdef __cplusplus
}
#endif
