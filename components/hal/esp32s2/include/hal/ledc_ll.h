/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for LEDC register operations.
// Note that most of the register operations in this layer are non-atomic operations.

#pragma once

#include "hal/ledc_types.h"
#include "soc/ledc_periph.h"
#include "soc/ledc_struct.h"
#include "soc/dport_reg.h"
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LEDC_LL_GET_HW()           &LEDC

#define LEDC_LL_DUTY_NUM_MAX       (LEDC_DUTY_NUM_LSCH0_V)
#define LEDC_LL_DUTY_CYCLE_MAX     (LEDC_DUTY_CYCLE_LSCH0_V)
#define LEDC_LL_DUTY_SCALE_MAX     (LEDC_DUTY_SCALE_LSCH0_V)
#define LEDC_LL_HPOINT_VAL_MAX     (LEDC_HPOINT_LSCH0_V)
#define LEDC_LL_FRACTIONAL_BITS    (8)
#define LEDC_LL_FRACTIONAL_MAX     ((1 << LEDC_LL_FRACTIONAL_BITS) - 1)

#define LEDC_LL_GLOBAL_CLOCKS { \
                                LEDC_SLOW_CLK_APB, \
                                LEDC_SLOW_CLK_XTAL, \
                                LEDC_SLOW_CLK_RC_FAST, \
                              }
#define LEDC_LL_TIMER_SPECIFIC_CLOCKS {\
                                        LEDC_REF_TICK, \
                                      }

#define LEDC_LL_IS_TIMER_SPECIFIC_CLOCK(SPEED, CLK) ((CLK) == LEDC_USE_REF_TICK)

#define LEDC_LL_GLOBAL_CLK_DEFAULT LEDC_SLOW_CLK_RC_FAST

/**
 * @brief Enable peripheral register clock
 *
 * @param enable    Enable/Disable
 */
static inline void ledc_ll_enable_bus_clock(bool enable) {
    if (enable) {
        DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_LEDC_CLK_EN);
    } else {
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_LEDC_CLK_EN);
    }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define ledc_ll_enable_bus_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; ledc_ll_enable_bus_clock(__VA_ARGS__)

/**
 * @brief Reset whole peripheral register to init value defined by HW design
 */
static inline void ledc_ll_enable_reset_reg(bool enable) {
    if (enable) {
        DPORT_SET_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_LEDC_RST);
    } else {
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_LEDC_RST);
    }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define ledc_ll_enable_reset_reg(...) (void)__DECLARE_RCC_ATOMIC_ENV; ledc_ll_enable_reset_reg(__VA_ARGS__)

/**
 * @brief Enable LEDC function clock
 *
 * @param hw Beginning address of the peripheral registers
 * @param en True to enable, false to disable
 *
 * @return None
 */
static inline void ledc_ll_enable_clock(ledc_dev_t *hw, bool en) {
    //resolve for compatibility
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
    uint32_t clk_sel_val = 0;
    if (slow_clk_sel == LEDC_SLOW_CLK_APB) {
        clk_sel_val = 1;
    } else if (slow_clk_sel == LEDC_SLOW_CLK_RC_FAST) {
        clk_sel_val = 2;
    } else if (slow_clk_sel == LEDC_SLOW_CLK_XTAL) {
        clk_sel_val = 3;
    }
    hw->conf.apb_clk_sel = clk_sel_val;
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
    uint32_t clk_sel_val = hw->conf.apb_clk_sel;
    if (clk_sel_val == 1) {
        *slow_clk_sel = LEDC_SLOW_CLK_APB;
    } else if (clk_sel_val == 2) {
        *slow_clk_sel = LEDC_SLOW_CLK_RC_FAST;
    } else if (clk_sel_val == 3) {
        *slow_clk_sel = LEDC_SLOW_CLK_XTAL;
    } else {
        abort();
    }
}

/**
 * @brief Update LEDC low speed timer
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, high-speed mode or low-speed mode
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 *
 * @return None
 */
static inline void ledc_ll_ls_timer_update(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_timer_t timer_sel)
{
    hw->timer_group[speed_mode].timer[timer_sel].conf.low_speed_update = 1;
}

/**
 * @brief Reset LEDC timer
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, high-speed mode or low-speed mode
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
 * @param speed_mode LEDC speed_mode, high-speed mode or low-speed mode
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
 * @param speed_mode LEDC speed_mode, high-speed mode or low-speed mode
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
 * @param speed_mode LEDC speed_mode, high-speed mode or low-speed mode
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 * @param clock_divider Timer clock divide value, the timer clock is divided from the selected clock source
 *
 * @return None
 */
static inline void ledc_ll_set_clock_divider(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_timer_t timer_sel, uint32_t clock_divider)
{
    hw->timer_group[speed_mode].timer[timer_sel].conf.clock_divider = clock_divider;
}

/**
 * @brief Get LEDC timer clock divider
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, high-speed mode or low-speed mode
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 * @param clock_divider Timer clock divide value, the timer clock is divided from the selected clock source
 *
 * @return None
 */
static inline void ledc_ll_get_clock_divider(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_timer_t timer_sel, uint32_t *clock_divider)
{
    *clock_divider = hw->timer_group[speed_mode].timer[timer_sel].conf.clock_divider;
}

/**
 * @brief Set LEDC timer clock source
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, high-speed mode or low-speed mode
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 * @param clk_src Timer clock source
 *
 * @note  REF_TICK can only be used when hw->conf.apb_clk_sel is set to 1 (through ledc_ll_set_slow_clk_sel()).
 *        This is ensured in the LEDC driver layer.
 *
 * @return None
 */
static inline void ledc_ll_set_clock_source(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_timer_t timer_sel, ledc_clk_src_t clk_src)
{
    if (clk_src == LEDC_REF_TICK) {
        hw->timer_group[speed_mode].timer[timer_sel].conf.tick_sel = 1;
    } else {
        hw->timer_group[speed_mode].timer[timer_sel].conf.tick_sel = 0;
    }
}

/**
 * @brief Get LEDC timer clock source
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, high-speed mode or low-speed mode
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 * @param clk_src Pointer to accept the timer clock source
 *
 * @return None
 */
static inline void ledc_ll_get_clock_source(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_timer_t timer_sel, ledc_clk_src_t *clk_src)
{
    if (hw->timer_group[speed_mode].timer[timer_sel].conf.tick_sel == 1) {
        *clk_src = LEDC_REF_TICK;
    } else {
        *clk_src = LEDC_APB_CLK;
    }
}

/**
 * @brief Set LEDC duty resolution
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, high-speed mode or low-speed mode
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 * @param duty_resolution Resolution of duty setting in number of bits. The range of duty values is [0, (2**duty_resolution)]
 *
 * @return None
 */
static inline void ledc_ll_set_duty_resolution(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_timer_t timer_sel, uint32_t duty_resolution)
{
    hw->timer_group[speed_mode].timer[timer_sel].conf.duty_resolution = duty_resolution;
}

/**
 * @brief Get LEDC duty resolution
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, high-speed mode or low-speed mode
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 * @param duty_resolution Pointer to accept the resolution of duty setting in number of bits.
 *
 * @return None
 */
static inline void ledc_ll_get_duty_resolution(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_timer_t timer_sel, uint32_t *duty_resolution)
{
    *duty_resolution = hw->timer_group[speed_mode].timer[timer_sel].conf.duty_resolution;
}

/**
 * @brief Get LEDC max duty
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, high-speed mode or low-speed mode
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 * @param max_duty Pointer to accept the max duty
 *
 * @return None
 */
static inline void ledc_ll_get_max_duty(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_timer_t timer_sel, uint32_t *max_duty)
{
    *max_duty = (1 << (LEDC.timer_group[speed_mode].timer[timer_sel].conf.duty_resolution));
}

/**
 * @brief Update channel configure when select low speed mode
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, high-speed mode or low-speed mode
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 *
 * @return None
 */
static inline void ledc_ll_ls_channel_update(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num)
{
    hw->channel_group[speed_mode].channel[channel_num].conf0.low_speed_update = 1;
}

/**
 * @brief Set LEDC hpoint value
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, high-speed mode or low-speed mode
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
 * @param speed_mode LEDC speed_mode, high-speed mode or low-speed mode
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
 * @param speed_mode LEDC speed_mode, high-speed mode or low-speed mode
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param duty_val LEDC duty value, the range of duty setting is [0, (2**duty_resolution)]
 *
 * @return None
 */
static inline void ledc_ll_set_duty_int_part(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, uint32_t duty_val)
{
    hw->channel_group[speed_mode].channel[channel_num].duty.duty = duty_val << 4;
}

/**
 * @brief Get LEDC duty value
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, high-speed mode or low-speed mode
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param duty_val Pointer to accept the LEDC duty value
 *
 * @return None
 */
static inline void ledc_ll_get_duty(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, uint32_t *duty_val)
{
    *duty_val = (hw->channel_group[speed_mode].channel[channel_num].duty_rd.duty_read >> 4);
}

/**
 * @brief Set LEDC duty change direction
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, high-speed mode or low-speed mode
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param duty_direction LEDC duty change direction, increase or decrease
 *
 * @return None
 */
static inline void ledc_ll_set_duty_direction(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, ledc_duty_direction_t duty_direction)
{
    hw->channel_group[speed_mode].channel[channel_num].conf1.duty_inc = duty_direction;
}

/**
 * @brief Set the number of increased or decreased times
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, high-speed mode or low-speed mode
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param duty_num The number of increased or decreased times
 *
 * @return None
 */
static inline void ledc_ll_set_duty_num(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, uint32_t duty_num)
{
    hw->channel_group[speed_mode].channel[channel_num].conf1.duty_num = duty_num;
}

/**
 * @brief Set the duty cycles of increase or decrease
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, high-speed mode or low-speed mode
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param duty_cycle The duty cycles
 *
 * @return None
 */
static inline void ledc_ll_set_duty_cycle(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, uint32_t duty_cycle)
{
    hw->channel_group[speed_mode].channel[channel_num].conf1.duty_cycle = duty_cycle;
}

/**
 * @brief Set the step scale of increase or decrease
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, high-speed mode or low-speed mode
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param duty_scale The step scale
 *
 * @return None
 */
static inline void ledc_ll_set_duty_scale(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, uint32_t duty_scale)
{
    hw->channel_group[speed_mode].channel[channel_num].conf1.duty_scale = duty_scale;
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
    typeof(hw->channel_group[speed_mode].channel[channel_num].conf1) conf1_reg;
    conf1_reg.val = hw->channel_group[speed_mode].channel[channel_num].conf1.val;
    conf1_reg.duty_inc = dir;
    conf1_reg.duty_num = step;
    conf1_reg.duty_cycle = cycle;
    conf1_reg.duty_scale = scale;
    hw->channel_group[speed_mode].channel[channel_num].conf1.val = conf1_reg.val;
}

/**
 * @brief Set the output enable
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, high-speed mode or low-speed mode
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
 * @param speed_mode LEDC speed_mode, high-speed mode or low-speed mode
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param duty_start The duty start
 *
 * @return None
 */
static inline void ledc_ll_set_duty_start(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, bool duty_start)
{
    hw->channel_group[speed_mode].channel[channel_num].conf1.duty_start = duty_start;
}

/**
 * @brief Set output idle level
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, high-speed mode or low-speed mode
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
 * @param speed_mode LEDC speed_mode, high-speed mode or low-speed mode
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param fade_end_intr_en The fade end interrupt enable status
 *
 * @return None
 */
static inline void ledc_ll_set_fade_end_intr(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, bool fade_end_intr_en)
{
    uint32_t value = hw->int_ena.val;
    uint32_t int_en_base = LEDC_DUTY_CHNG_END_LSCH0_INT_ENA_S;
    hw->int_ena.val = fade_end_intr_en ? (value | BIT(int_en_base + channel_num)) : (value & (~(BIT(int_en_base + channel_num))));
}

/**
 * @brief Get fade end interrupt status
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, high-speed mode or low-speed mode
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param intr_status The fade end interrupt status
 *
 * @return None
 */
static inline void ledc_ll_get_fade_end_intr_status(ledc_dev_t *hw, ledc_mode_t speed_mode, uint32_t *intr_status)
{
    uint32_t value = hw->int_st.val;
    uint32_t int_en_base = LEDC_DUTY_CHNG_END_LSCH0_INT_ENA_S;
    *intr_status = (value >> int_en_base) & 0xff;
}

/**
 * @brief Clear fade end interrupt status
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, high-speed mode or low-speed mode
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 *
 * @return None
 */
static inline void ledc_ll_clear_fade_end_intr_status(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num)
{
    uint32_t int_en_base = LEDC_DUTY_CHNG_END_LSCH0_INT_ENA_S;
    hw->int_clr.val = BIT(int_en_base + channel_num);
}

/**
 * @brief Set timer index of the specified channel
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, high-speed mode or low-speed mode
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
 * @param speed_mode LEDC speed_mode, high-speed mode or low-speed mode
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param timer_sel Pointer to accept the LEDC timer index
 *
 * @return None
 */
static inline void ledc_ll_get_channel_timer(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, ledc_timer_t *timer_sel)
{
    *timer_sel = (ledc_timer_t)(hw->channel_group[speed_mode].channel[channel_num].conf0.timer_sel);
}

#ifdef __cplusplus
}
#endif
